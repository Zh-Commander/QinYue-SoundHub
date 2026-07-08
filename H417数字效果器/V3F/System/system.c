#include "system.h"
#include "hardware_v3f.h"
#include "config.h"

/* V3F shared memory. */
volatile uint16_t I2S_RX_DMA_Buffer_V3F[I2S_BUFFER_SIZE / sizeof(uint16_t)] = {0};
volatile uint16_t I2S_TX_DMA_Buffer_V3F[I2S_BUFFER_SIZE / sizeof(uint16_t)] = {0};

int32_t I2S_RightDecode_Buffer_V3F[I2S_BUFFER_SIZE / sizeof(uint32_t) / 2] = {0};
int32_t I2S_LeftDecode_Buffer_V3F[I2S_BUFFER_SIZE / sizeof(uint32_t) / 2] = {0};

volatile I2S_Buffer_TransPkg_t I2S_Buffer_TransPkg_V3F;
volatile Effect_Algorithm_TransPkg_t Effect_Algorithm_TransPkg_V3F;
volatile Trans_Structure_t Trans_Structure_V3F;

#define V3F_SCREEN_RX_BUFFER_LEN 128u
#define V3F_SCREEN_RX_BUFFER_MASK (V3F_SCREEN_RX_BUFFER_LEN - 1u)
#define V3F_SCREEN_LINE_LEN 64u
#define V3F_PARAM_FLUSH_PERIOD_MS 50u
#define V3F_SCREEN_EFFECT_MAX DSP_SpeakerSim_enum

typedef struct
{
    int32_t value[DSP_PARAM_ID_MAX];
    uint32_t dirty_mask;

} v3f_effect_param_shadow_t;

static v3f_effect_param_shadow_t v3f_effect_param_shadow[DSP_EFFECT_TYPE_MAX];
static uint32_t v3f_event_seq = 0;

static volatile uint8_t v3f_screen_rx_buffer[V3F_SCREEN_RX_BUFFER_LEN];
static volatile uint16_t v3f_screen_rx_wr = 0;
static volatile uint16_t v3f_screen_rx_rd = 0;
static volatile uint32_t v3f_screen_rx_overflow_count = 0;

static volatile uint32_t v3f_system_tick_ms = 0;
static uint32_t v3f_last_param_flush_tick_ms = 0;

static char v3f_screen_line[V3F_SCREEN_LINE_LEN];
static uint8_t v3f_screen_line_len = 0;
static uint8_t v3f_screen_line_cr = 0;
static uint8_t v3f_screen_line_discard = 0;

/* Private functions. */
static void Config_Params_Trans(void);
static uint8_t DSP_EventQueue_Push(
    volatile dsp_event_queue_t *q,
    const dsp_event_t *event);
static uint8_t V3F_ScreenRxPopByte(uint8_t *byte);
static void V3F_ProcessScreenRx(void);
static void V3F_ProcessScreenByte(uint8_t byte);
static void V3F_ProcessScreenLine(const char *line);
static uint8_t V3F_ParseU32(const char **line, uint32_t *value);
static uint8_t V3F_ParseVolumeCode(const char **line, uint32_t *value);
static uint8_t V3F_ExpectChar(const char **line, char ch);
static uint8_t V3F_ExpectEnd(const char *line);
static uint8_t V3F_IsScreenEffectValid(uint32_t effect_type);
static uint32_t V3F_ScreenVolumeCodeToQ16(uint32_t code);

void System_Init(void)
{
    Hardware_V3f();

    Config_Params_Trans();

    CPU_FENCE();
    IPC_WriteMSG(IPC_MSG0, (uint32_t)(Trans_Structure));
    while (IPC->MSG[1] == 0);
    CPU_FENCE();
}

void System_Loop(void)
{
    uint32_t tick;

    V3F_ProcessScreenRx();

    tick = v3f_system_tick_ms;

    if ((uint32_t)(tick - v3f_last_param_flush_tick_ms) >=
        V3F_PARAM_FLUSH_PERIOD_MS)
    {
        v3f_last_param_flush_tick_ms = tick;
        V3F_FlushDirtyParams50ms();
    }
}

void V3F_ScreenRxPushByte(uint8_t byte)
{
    uint16_t wr = v3f_screen_rx_wr;
    uint16_t next = (uint16_t)((wr + 1u) & V3F_SCREEN_RX_BUFFER_MASK);

    if (next == v3f_screen_rx_rd)
    {
        v3f_screen_rx_overflow_count++;
        return;
    }

    v3f_screen_rx_buffer[wr] = byte;
    v3f_screen_rx_wr = next;
}

void V3F_Tick1ms(void)
{
    v3f_system_tick_ms++;
}

static void Config_Params_Trans(void)
{
    /* Global pointer configuration. */
    I2S_RX_DMA_Buffer = I2S_RX_DMA_Buffer_V3F;
    I2S_TX_DMA_Buffer = I2S_TX_DMA_Buffer_V3F;

    I2S_RightDecode_Buffer = I2S_RightDecode_Buffer_V3F;
    I2S_LeftDecode_Buffer = I2S_LeftDecode_Buffer_V3F;

    I2S_Buffer_TransPkg = &I2S_Buffer_TransPkg_V3F;
    Effect_Algorithm_TransPkg = &Effect_Algorithm_TransPkg_V3F;
    Trans_Structure = &Trans_Structure_V3F;

    /* I2S transfer package. */
    I2S_Buffer_TransPkg->I2S_RX_DMA_Buffer = I2S_RX_DMA_Buffer;
    I2S_Buffer_TransPkg->I2S_TX_DMA_Buffer = I2S_TX_DMA_Buffer;

    I2S_Buffer_TransPkg->I2S_RightDecode_Buffer = I2S_RightDecode_Buffer;
    I2S_Buffer_TransPkg->I2S_LeftDecode_Buffer = I2S_LeftDecode_Buffer;

    /* Effect algorithm transfer package. */
    Effect_Algorithm_TransPkg->Audio_DSP_Type[0] = DSP_NULL_enum;
    Effect_Algorithm_TransPkg->Audio_DSP_Type[1] = DSP_NULL_enum;
    Effect_Algorithm_TransPkg->Audio_DSP_Type[2] = DSP_NULL_enum;

    Effect_Algorithm_TransPkg->InputVolume_Q16 = DSP_GLOBAL_VOLUME_Q16_ONE;
    Effect_Algorithm_TransPkg->OutputVolume_Q16 = DSP_GLOBAL_VOLUME_Q16_ONE;

    Effect_Algorithm_TransPkg->EventQueue.wr = 0;
    Effect_Algorithm_TransPkg->EventQueue.rd = 0;
    Effect_Algorithm_TransPkg->EventQueue.overflow_count = 0;
    Effect_Algorithm_TransPkg->EventQueue.last_applied_seq = 0;
    Effect_Algorithm_TransPkg->EventQueue.error_count = 0;

    for (uint8_t effect = 0; effect < DSP_EFFECT_TYPE_MAX; effect++)
    {
        v3f_effect_param_shadow[effect].dirty_mask = 0;

        for (uint8_t param = 0; param < DSP_PARAM_ID_MAX; param++)
        {
            v3f_effect_param_shadow[effect].value[param] = 0;
        }
    }

    Trans_Structure->I2S_Buffer_TransPkg = I2S_Buffer_TransPkg;
    Trans_Structure->Effect_Algorithm_TransPkg = Effect_Algorithm_TransPkg;
}

static uint8_t DSP_EventQueue_Push(
    volatile dsp_event_queue_t *q,
    const dsp_event_t *event)
{
    uint32_t wr = q->wr;
    uint32_t rd = q->rd;
    uint32_t next = (wr + 1u) & DSP_EVENT_QUEUE_MASK;

    if (next == rd)
    {
        q->overflow_count++;
        return 0;
    }

    q->event[wr].seq = event->seq;
    q->event[wr].cmd = event->cmd;
    q->event[wr].slot = event->slot;
    q->event[wr].effect_type = event->effect_type;
    q->event[wr].param_id = event->param_id;
    q->event[wr].value = event->value;

    CPU_FENCE();

    q->wr = next;

    CPU_FENCE();

    return 1;
}

static uint8_t V3F_ScreenRxPopByte(uint8_t *byte)
{
    uint16_t rd = v3f_screen_rx_rd;

    if (rd == v3f_screen_rx_wr)
        return 0;

    *byte = v3f_screen_rx_buffer[rd];
    v3f_screen_rx_rd =
        (uint16_t)((rd + 1u) & V3F_SCREEN_RX_BUFFER_MASK);

    return 1;
}

static void V3F_ProcessScreenRx(void)
{
    uint8_t byte;

    while (V3F_ScreenRxPopByte(&byte))
    {
        V3F_ProcessScreenByte(byte);
    }
}

static void V3F_ProcessScreenByte(uint8_t byte)
{
    if (byte == '\r')
    {
        v3f_screen_line_cr = 1;
        return;
    }

    if (byte == '\n')
    {
        if (v3f_screen_line_cr && !v3f_screen_line_discard)
        {
            v3f_screen_line[v3f_screen_line_len] = '\0';
            V3F_ProcessScreenLine(v3f_screen_line);
        }

        v3f_screen_line_len = 0;
        v3f_screen_line_cr = 0;
        v3f_screen_line_discard = 0;
        return;
    }

    if (v3f_screen_line_cr)
    {
        v3f_screen_line_len = 0;
        v3f_screen_line_cr = 0;
        v3f_screen_line_discard = 0;
    }

    if (v3f_screen_line_discard)
        return;

    if ((byte < 0x20u) || (byte > 0x7eu))
    {
        v3f_screen_line_len = 0;
        v3f_screen_line_discard = 1;
        return;
    }

    if (v3f_screen_line_len >= (V3F_SCREEN_LINE_LEN - 1u))
    {
        v3f_screen_line_len = 0;
        v3f_screen_line_discard = 1;
        return;
    }

    v3f_screen_line[v3f_screen_line_len++] = (char)byte;
}

static void V3F_ProcessScreenLine(const char *line)
{
    const char *p = line;
    uint32_t slot;
    uint32_t effect_type;
    uint32_t param_id;
    uint32_t value;
    uint32_t volume_q16;

    switch (*p)
    {
        case 'S':
            p++;

            if (!V3F_ExpectChar(&p, ','))
                return;

            if (!V3F_ParseU32(&p, &slot))
                return;

            if (!V3F_ExpectChar(&p, ','))
                return;

            if (!V3F_ParseU32(&p, &effect_type))
                return;

            if (!V3F_ExpectEnd(p))
                return;

            if (slot >= MAX_DSP_NUM)
                return;

            if (!V3F_IsScreenEffectValid(effect_type))
                return;

            (void)V3F_SendSetSlotEffectEvent(
                (uint8_t)slot,
                (uint8_t)effect_type);
            break;

        case 'P':
            p++;

            if (!V3F_ExpectChar(&p, ','))
                return;

            if (!V3F_ParseU32(&p, &effect_type))
                return;

            if (!V3F_ExpectChar(&p, ','))
                return;

            if (!V3F_ParseU32(&p, &param_id))
                return;

            if (!V3F_ExpectChar(&p, ','))
                return;

            if (!V3F_ParseU32(&p, &value))
                return;

            if (!V3F_ExpectEnd(p))
                return;

            if (!V3F_IsScreenEffectValid(effect_type))
                return;

            if (param_id >= DSP_PARAM_ID_MAX)
                return;

            if (value > POT_MAX)
                return;

            V3F_OnScreenParamChanged(
                (uint8_t)effect_type,
                (uint8_t)param_id,
                (int32_t)value);
            break;

        case 'D':
            p++;

            if (!V3F_ExpectChar(&p, ','))
                return;

            if (!V3F_ParseU32(&p, &effect_type))
                return;

            if (!V3F_ExpectEnd(p))
                return;

            if (!V3F_IsScreenEffectValid(effect_type))
                return;

            (void)V3F_SendLoadDefaultParamsEvent((uint8_t)effect_type);
            break;

        case 'I':
            p++;

            if (!V3F_ExpectChar(&p, ','))
                return;

            if (!V3F_ParseVolumeCode(&p, &value))
                return;

            if (!V3F_ExpectEnd(p))
                return;

            volume_q16 = V3F_ScreenVolumeCodeToQ16(value);

            CPU_FENCE();
            Effect_Algorithm_TransPkg->InputVolume_Q16 = volume_q16;
            CPU_FENCE();
            break;

        case 'O':
            p++;

            if (!V3F_ExpectChar(&p, ','))
                return;

            if (!V3F_ParseVolumeCode(&p, &value))
                return;

            if (!V3F_ExpectEnd(p))
                return;

            volume_q16 = V3F_ScreenVolumeCodeToQ16(value);

            CPU_FENCE();
            Effect_Algorithm_TransPkg->OutputVolume_Q16 = volume_q16;
            CPU_FENCE();
            break;

        default:
            break;
    }
}

static uint8_t V3F_ParseU32(const char **line, uint32_t *value)
{
    uint32_t result = 0;
    uint8_t digit_found = 0;

    while ((**line >= '0') && (**line <= '9'))
    {
        uint32_t digit = (uint32_t)(**line - '0');

        if (result > ((0xffffffffu - digit) / 10u))
            return 0;

        result = (result * 10u) + digit;
        digit_found = 1;
        (*line)++;
    }

    if (!digit_found)
        return 0;

    *value = result;
    return 1;
}

static uint8_t V3F_ParseVolumeCode(const char **line, uint32_t *value)
{
    uint32_t result = 0;

    for (uint8_t i = 0; i < 3u; i++)
    {
        if ((**line < '0') || (**line > '9'))
            return 0;

        result = (result * 10u) + (uint32_t)(**line - '0');
        (*line)++;
    }

    if (result > DSP_GLOBAL_VOLUME_CODE_MAX)
        return 0;

    *value = result;
    return 1;
}

static uint8_t V3F_ExpectChar(const char **line, char ch)
{
    if (**line != ch)
        return 0;

    (*line)++;
    return 1;
}

static uint8_t V3F_ExpectEnd(const char *line)
{
    return (*line == '\0');
}

static uint8_t V3F_IsScreenEffectValid(uint32_t effect_type)
{
    return (effect_type <= V3F_SCREEN_EFFECT_MAX);
}

static uint32_t V3F_ScreenVolumeCodeToQ16(uint32_t code)
{
    return (uint32_t)((
        ((uint64_t)code * DSP_GLOBAL_VOLUME_Q16_MAX) +
        (DSP_GLOBAL_VOLUME_CODE_MAX / 2u)) /
        DSP_GLOBAL_VOLUME_CODE_MAX);
}

void V3F_OnScreenParamChanged(
    uint8_t effect_type,
    uint8_t param_id,
    int32_t value)
{
    if (effect_type >= DSP_EFFECT_TYPE_MAX)
        return;

    if (param_id >= DSP_PARAM_ID_MAX)
        return;

    if (value < 0)
        value = 0;

    if (value > POT_MAX)
        value = POT_MAX;

    v3f_effect_param_shadow[effect_type].value[param_id] = value;
    v3f_effect_param_shadow[effect_type].dirty_mask |= (1u << param_id);
}

void V3F_FlushDirtyParams50ms(void)
{
    for (uint8_t effect_type = 0;
         effect_type < DSP_EFFECT_TYPE_MAX;
         effect_type++)
    {
        uint32_t mask = v3f_effect_param_shadow[effect_type].dirty_mask;

        if (mask == 0)
            continue;

        for (uint8_t param_id = 0;
             param_id < DSP_PARAM_ID_MAX;
             param_id++)
        {
            uint32_t bit = 1u << param_id;

            if ((mask & bit) == 0)
                continue;

            dsp_event_t event;

            event.seq = ++v3f_event_seq;
            event.cmd = DSP_SetEffectParam_CMD;
            event.slot = 0;
            event.effect_type = effect_type;
            event.param_id = param_id;
            event.value =
                v3f_effect_param_shadow[effect_type].value[param_id];

            if (!DSP_EventQueue_Push(
                    &Effect_Algorithm_TransPkg->EventQueue,
                    &event))
            {
                return;
            }

            v3f_effect_param_shadow[effect_type].dirty_mask &= ~bit;
        }
    }
}

uint8_t V3F_SendSetSlotEffectEvent(
    uint8_t slot,
    uint8_t effect_type)
{
    if (slot >= MAX_DSP_NUM)
        return 0;

    if (effect_type >= DSP_EFFECT_TYPE_MAX)
        return 0;

    dsp_event_t event;

    event.seq = ++v3f_event_seq;
    event.cmd = DSP_SetSlotEffect_CMD;
    event.slot = slot;
    event.effect_type = effect_type;
    event.param_id = 0;
    event.value = 0;

    return DSP_EventQueue_Push(
        &Effect_Algorithm_TransPkg->EventQueue,
        &event);
}

uint8_t V3F_SendLoadDefaultParamsEvent(uint8_t effect_type)
{
    if (effect_type >= DSP_EFFECT_TYPE_MAX)
        return 0;

    v3f_effect_param_shadow[effect_type].dirty_mask = 0;

    dsp_event_t event;

    event.seq = ++v3f_event_seq;
    event.cmd = DSP_LoadDefaultParams_CMD;
    event.slot = 0;
    event.effect_type = effect_type;
    event.param_id = 0;
    event.value = 0;

    return DSP_EventQueue_Push(
        &Effect_Algorithm_TransPkg->EventQueue,
        &event);
}
