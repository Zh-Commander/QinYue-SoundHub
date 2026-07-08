#include "midi.h"
#include "debug.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_usart.h"
#include <string.h>

#define MIDI_USART                  USART3
#define MIDI_USART_TX_PIN           GPIO_Pin_10
#define MIDI_USART_GPIO             GPIOB
#define MIDI_READY_GPIO             GPIOD
#define MIDI_READY_PIN              GPIO_Pin_11
#define MIDI_SELECT_GPIO            GPIOD
#define MIDI_SELECT_PIN             GPIO_Pin_12
#define MIDI_BAUDRATE               31250U

static uint8_t g_midi_initialized = 0U;
static midi_runtime_state_t g_midi_runtime = {0};

static void midi_hw_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, DISABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_USART3, DISABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, DISABLE);

    gpio_init.GPIO_Pin = MIDI_USART_TX_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(MIDI_USART_GPIO, &gpio_init);

    gpio_init.GPIO_Pin = MIDI_READY_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(MIDI_READY_GPIO, &gpio_init);

    gpio_init.GPIO_Pin = MIDI_SELECT_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MIDI_SELECT_GPIO, &gpio_init);
    GPIO_ResetBits(MIDI_SELECT_GPIO, MIDI_SELECT_PIN);
}

static void midi_hw_usart_init(void)
{
    USART_InitTypeDef usart_init = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    usart_init.USART_BaudRate = MIDI_BAUDRATE;
    usart_init.USART_WordLength = USART_WordLength_8b;
    usart_init.USART_StopBits = USART_StopBits_1;
    usart_init.USART_Parity = USART_Parity_No;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode = USART_Mode_Tx;
    USART_Init(MIDI_USART, &usart_init);
    USART_Cmd(MIDI_USART, ENABLE);
}

static int midi_send_bytes(const uint8_t *data, uint8_t len)
{
    uint8_t index;

    if ((data == NULL) || (len == 0U)) {
        return -1;
    }

    if (midi_is_ready() == 0U) {
        g_midi_runtime.ready = 0U;
        g_midi_runtime.send_error_count++;
        return -1;
    }

    g_midi_runtime.ready = 1U;

    for (index = 0U; index < len; index++) {
        USART_SendData(MIDI_USART, data[index]);
        while (USART_GetFlagStatus(MIDI_USART, USART_FLAG_TXE) == RESET) {
        }
    }

    while (USART_GetFlagStatus(MIDI_USART, USART_FLAG_TC) == RESET) {
    }
    return 0;
}

int midi_init(void)
{
    if (g_midi_initialized != 0U) {
        return 0;
    }

    memset(&g_midi_runtime, 0, sizeof(g_midi_runtime));
    midi_hw_gpio_init();
    midi_hw_usart_init();
    g_midi_initialized = 1U;
    g_midi_runtime.ready = midi_is_ready();
    Debug_Printf("[MIDI] USART3 PB10 initialized at 31250bps\r\n");
    return 0;
}

void midi_deinit(void)
{
    if (g_midi_initialized == 0U) {
        return;
    }

    while (USART_GetFlagStatus(MIDI_USART, USART_FLAG_TC) == RESET) {
    }
    USART_Cmd(MIDI_USART, DISABLE);
    g_midi_initialized = 0U;
    g_midi_runtime.ready = 0U;
}

uint8_t midi_is_ready(void)
{
    return (GPIO_ReadInputDataBit(MIDI_READY_GPIO, MIDI_READY_PIN) == Bit_RESET) ? 1U : 0U;
}

uint8_t midi_map_velocity_127(uint8_t velocity_255)
{
    uint32_t mapped;

    if (velocity_255 == 0U) {
        return 1U;
    }

    mapped = ((uint32_t)velocity_255 * 126U + 127U) / 255U;
    return (uint8_t)(mapped + 1U);
}

int midi_send_program_change(uint8_t channel, uint8_t program)
{
    uint8_t packet[2];

    packet[0] = (uint8_t)(0xC0U | (channel & 0x0FU));
    packet[1] = (uint8_t)(program & 0x7FU);
    return midi_send_bytes(packet, (uint8_t)sizeof(packet));
}

int midi_send_note_on(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t packet[3];
    int result;

    packet[0] = (uint8_t)(0x90U | (channel & 0x0FU));
    packet[1] = (uint8_t)(note & 0x7FU);
    packet[2] = (uint8_t)(velocity & 0x7FU);
    result = midi_send_bytes(packet, (uint8_t)sizeof(packet));
    if (result == 0) {
        g_midi_runtime.last_note = packet[1];
        g_midi_runtime.last_velocity = packet[2];
        g_midi_runtime.note_on_count++;
    }
    return result;
}

int midi_send_note_off(uint8_t channel, uint8_t note)
{
    uint8_t packet[3];
    int result;

    packet[0] = (uint8_t)(0x80U | (channel & 0x0FU));
    packet[1] = (uint8_t)(note & 0x7FU);
    packet[2] = 0U;
    result = midi_send_bytes(packet, (uint8_t)sizeof(packet));
    if (result == 0) {
        g_midi_runtime.last_note = packet[1];
        g_midi_runtime.last_velocity = 0U;
        g_midi_runtime.note_off_count++;
    }
    return result;
}

int midi_send_all_notes_off(uint8_t channel)
{
    uint8_t packet[3];

    packet[0] = (uint8_t)(0xB0U | (channel & 0x0FU));
    packet[1] = 0x7BU;
    packet[2] = 0U;
    return midi_send_bytes(packet, (uint8_t)sizeof(packet));
}

void midi_get_runtime_state(midi_runtime_state_t *state)
{
    if (state == NULL) {
        return;
    }

    g_midi_runtime.ready = midi_is_ready();
    *state = g_midi_runtime;
}
