#include "dsp_main.h"
#include "config.h"

#include "dsp_base.h"
#include "dsp_audio_process_base.h"

#include "dsp_fuzz.h"
#include "dsp_overdrive.h"
#include "dsp_eq.h"
#include "dsp_distortion.h"
#include "dsp_compressor.h"
#include "dsp_speakersim.h"
#include "dsp_tremolo.h"
#include "dsp_phaser.h"
#include "dsp_flanger.h"
#include "dsp_chorus.h"
#include "dsp_delay.h"
#include "dsp_vibrato.h"

#define V5F_EVENT_APPLY_PER_BLOCK 4u

static inline uint8_t DSP_EventQueue_Pop(
    volatile dsp_event_queue_t *q,
    dsp_event_t *event)
{
    uint32_t rd = q->rd;
    uint32_t wr = q->wr;

    if (rd == wr)
        return 0;

    CPU_FENCE();

    event->seq = q->event[rd].seq;
    event->cmd = q->event[rd].cmd;
    event->slot = q->event[rd].slot;
    event->effect_type = q->event[rd].effect_type;
    event->param_id = q->event[rd].param_id;
    event->value = q->event[rd].value;

    CPU_FENCE();

    q->rd = (rd + 1u) & DSP_EVENT_QUEUE_MASK;

    CPU_FENCE();

    return 1;
}

void DSP_LoadDefaultParamsByEffect(uint8_t effect_type)
{
    switch (effect_type)
    {
        case DSP_Chorus_enum:
            DSP_Chorus_LoadDefaultParams();
            break;

        case DSP_Flanger_enum:
            DSP_Flanger_LoadDefaultParams();
            break;

        case DSP_Phaser_enum:
            DSP_Phaser_LoadDefaultParams();
            break;

        case DSP_Tremolo_enum:
            DSP_Tremolo_LoadDefaultParams();
            break;

        case DSP_Vibrato_enum:
            DSP_Vibrato_LoadDefaultParams();
            break;

        case DSP_Delay_enum:
            DSP_Delay_LoadDefaultParams();
            break;

        case DSP_EQ_enum:
            DSP_EQ_LoadDefaultParams();
            break;

        case DSP_Compressor_enum:
            DSP_Compressor_LoadDefaultParams();
            break;

        case DSP_Distortion_enum:
            DSP_Distortion_LoadDefaultParams();
            break;

        case DSP_Overdrive_enum:
            DSP_Overdrive_LoadDefaultParams();
            break;

        case DSP_Fuzz_enum:
            DSP_Fuzz_LoadDefaultParams();
            break;

        case DSP_SpeakerSim_enum:
            DSP_SpeakerSim_LoadDefaultParams();
            break;

        default:
            break;
    }
}

void DSP_ClearStateByEffect(uint8_t effect_type)
{
    switch (effect_type)
    {
        case DSP_Chorus_enum:
            DSP_Chorus_ClearState();
            break;

        case DSP_Flanger_enum:
            DSP_Flanger_ClearState();
            break;

        case DSP_Phaser_enum:
            DSP_Phaser_ClearState();
            break;

        case DSP_Tremolo_enum:
            DSP_Tremolo_ClearState();
            break;

        case DSP_Vibrato_enum:
            DSP_Vibrato_ClearState();
            break;

        case DSP_Delay_enum:
            DSP_Delay_ClearState();
            break;

        case DSP_EQ_enum:
            DSP_EQ_ClearState();
            break;

        case DSP_Compressor_enum:
            DSP_Compressor_ClearState();
            break;

        case DSP_Distortion_enum:
            DSP_Distortion_ClearState();
            break;

        case DSP_Overdrive_enum:
            DSP_Overdrive_ClearState();
            break;

        case DSP_Fuzz_enum:
            DSP_Fuzz_ClearState();
            break;

        case DSP_SpeakerSim_enum:
            DSP_SpeakerSim_ClearState();
            break;

        default:
            break;
    }
}

void DSP_UpdateParamFromPot(
    uint8_t effect_type,
    uint8_t param_id,
    int32_t value)
{
    switch (effect_type)
    {
        case DSP_Chorus_enum:
            DSP_Chorus_UpdateParamFromPot(param_id, value);
            break;

        case DSP_Flanger_enum:
            DSP_Flanger_UpdateParamFromPot(param_id, value);
            break;

        case DSP_Phaser_enum:
            DSP_Phaser_UpdateParamFromPot(param_id, value);
            break;

        case DSP_Tremolo_enum:
            DSP_Tremolo_UpdateParamFromPot(param_id, value);
            break;

        case DSP_Vibrato_enum:
            DSP_Vibrato_UpdateParamFromPot(param_id, value);
            break;

        case DSP_Delay_enum:
            DSP_Delay_UpdateParamFromPot(param_id, value);
            break;

        case DSP_EQ_enum:
            DSP_EQ_UpdateParamFromPot(param_id, value);
            break;

        case DSP_Compressor_enum:
            DSP_Compressor_UpdateParamFromPot(param_id, value);
            break;

        case DSP_Distortion_enum:
            DSP_Distortion_UpdateParamFromPot(param_id, value);
            break;

        case DSP_Overdrive_enum:
            DSP_Overdrive_UpdateParamFromPot(param_id, value);
            break;

        case DSP_Fuzz_enum:
            DSP_Fuzz_UpdateParamFromPot(param_id, value);
            break;

        case DSP_SpeakerSim_enum:
            DSP_SpeakerSim_UpdateParamFromPot(param_id, value);
            break;

        default:
            break;
    }
}

static inline uint8_t DSP_IsEffectAlreadyUsedInOtherSlot(
    uint8_t slot,
    uint8_t effect_type)
{
    if (effect_type == DSP_NULL_enum)
        return 0;

    for (uint8_t i = 0; i < MAX_DSP_NUM; i++)
    {
        if (i == slot)
            continue;

        if (Effect_Algorithm_TransPkg->Audio_DSP_Type[i] == effect_type)
            return 1;
    }

    return 0;
}

static inline void V5F_ApplySetSlotEffectEvent(const dsp_event_t *event)
{
    uint8_t slot = (uint8_t)event->slot;
    uint8_t effect_type = (uint8_t)event->effect_type;

    if (slot >= MAX_DSP_NUM)
    {
        Effect_Algorithm_TransPkg->EventQueue.error_count++;
        return;
    }

    if (effect_type >= DSP_EFFECT_TYPE_MAX)
    {
        Effect_Algorithm_TransPkg->EventQueue.error_count++;
        return;
    }

    /*
     * ��ǰ Reverb / Preamp ��û�������룬�����һ�治�����л���ȥ��?
     * ��������������ǽ��뵫����������������ɾ����� switch ���?
     */
    switch (effect_type)
    {
        case DSP_NULL_enum:
        case DSP_Chorus_enum:
        case DSP_Flanger_enum:
        case DSP_Phaser_enum:
        case DSP_Tremolo_enum:
        case DSP_Vibrato_enum:
        case DSP_Delay_enum:
        case DSP_EQ_enum:
        case DSP_Compressor_enum:
        case DSP_Distortion_enum:
        case DSP_Overdrive_enum:
        case DSP_Fuzz_enum:
        case DSP_SpeakerSim_enum:
            break;

        default:
            Effect_Algorithm_TransPkg->EventQueue.error_count++;
            return;
    }

    if (DSP_IsEffectAlreadyUsedInOtherSlot(slot, effect_type))
    {
        Effect_Algorithm_TransPkg->EventQueue.error_count++;
        return;
    }

    if (effect_type != DSP_NULL_enum)
    {
        DSP_ClearStateByEffect(effect_type);
    }

    CPU_FENCE();

    Effect_Algorithm_TransPkg->Audio_DSP_Type[slot] = effect_type;

    CPU_FENCE();
}

static inline void V5F_ApplySetEffectParamEvent(const dsp_event_t *event)
{
    uint8_t effect_type = (uint8_t)event->effect_type;
    uint8_t param_id = (uint8_t)event->param_id;

    if (effect_type >= DSP_EFFECT_TYPE_MAX)
    {
        Effect_Algorithm_TransPkg->EventQueue.error_count++;
        return;
    }

    if (param_id >= DSP_PARAM_ID_MAX)
    {
        Effect_Algorithm_TransPkg->EventQueue.error_count++;
        return;
    }

    DSP_UpdateParamFromPot(
        effect_type,
        param_id,
        event->value);
}

static inline void V5F_ApplyLoadDefaultParamsEvent(const dsp_event_t *event)
{
    uint8_t effect_type = (uint8_t)event->effect_type;

    if (effect_type >= DSP_EFFECT_TYPE_MAX)
    {
        Effect_Algorithm_TransPkg->EventQueue.error_count++;
        return;
    }

    DSP_LoadDefaultParamsByEffect(effect_type);
    DSP_ClearStateByEffect(effect_type);
}

static inline void V5F_ApplyEvent(const dsp_event_t *event)
{
    switch ((dsp_event_cmd_t)event->cmd)
    {
        case DSP_SetSlotEffect_CMD:
        {
            V5F_ApplySetSlotEffectEvent(event);
            break;
        }

        case DSP_SetEffectParam_CMD:
        {
            V5F_ApplySetEffectParamEvent(event);
            break;
        }

        case DSP_LoadDefaultParams_CMD:
        {
            V5F_ApplyLoadDefaultParamsEvent(event);
            break;
        }

        default:
        {
            Effect_Algorithm_TransPkg->EventQueue.error_count++;
            break;
        }
    }
}

void V5F_ProcessEventQueue(uint8_t max_event_count)
{
    dsp_event_t event;

    while (max_event_count--)
    {
        if (!DSP_EventQueue_Pop(
                &Effect_Algorithm_TransPkg->EventQueue,
                &event))
        {
            break;
        }

        V5F_ApplyEvent(&event);

        Effect_Algorithm_TransPkg->EventQueue.last_applied_seq = event.seq;
    }
}

void DSP_Init(void)
{
    DSP_Fuzz_Init();
    DSP_Overdrive_Init();
    DSP_EQ_Init();
    DSP_Distortion_Init();
    DSP_Compressor_Init();
    DSP_SpeakerSim_Init();
    DSP_Tremolo_Init();
    DSP_Phaser_Init();
    DSP_Flanger_Init();
    DSP_Chorus_Init();
    DSP_Delay_Init();
    DSP_Vibrato_Init();
}

void Audio_DSP_Main(volatile uint16_t *I2S_SourceBuffer, volatile uint16_t *I2S_ObjectBuffer)
{
    uint32_t input_volume_q16;
    uint32_t output_volume_q16;

    V5F_ProcessEventQueue(V5F_EVENT_APPLY_PER_BLOCK);

    CPU_FENCE();
    input_volume_q16 = Effect_Algorithm_TransPkg->InputVolume_Q16;
    output_volume_q16 = Effect_Algorithm_TransPkg->OutputVolume_Q16;
    CPU_FENCE();

    if (input_volume_q16 > DSP_GLOBAL_VOLUME_Q16_MAX)
        input_volume_q16 = DSP_GLOBAL_VOLUME_Q16_MAX;

    if (output_volume_q16 > DSP_GLOBAL_VOLUME_Q16_MAX)
        output_volume_q16 = DSP_GLOBAL_VOLUME_Q16_MAX;

    // ��Ƶ����
    for (uint16_t frame = 0; frame < I2S_BUFFER_HALF_FRAME; frame++)
    {
        int32_t sample_r =
            (int32_t)I2S_32_UnpackSample(I2S_SourceBuffer + frame * 4);
        int32_t sample_l =
            (int32_t)I2S_32_UnpackSample(I2S_SourceBuffer + frame * 4 + 2);

        if (input_volume_q16 == 0u)
        {
            sample_r = 0;
            sample_l = 0;
        }
        else if (input_volume_q16 != DSP_GLOBAL_VOLUME_Q16_ONE)
        {
            sample_r = multiply_q16_clamp24(sample_r, input_volume_q16);
            sample_l = multiply_q16_clamp24(sample_l, input_volume_q16);
        }

        I2S_RightDecode_Buffer[frame] = sample_r;
        I2S_LeftDecode_Buffer[frame] = sample_l;
    }

    // ��Ƶ����
    for (uint8_t DSP_i = 0; DSP_i < MAX_DSP_NUM; DSP_i++)
    {
        uint8_t effect_type = Effect_Algorithm_TransPkg->Audio_DSP_Type[DSP_i];
        switch (effect_type)
        {
            case DSP_NULL_enum:
            {
                break;
            }

            case DSP_Delay_enum:
            {
                delay_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_Reverb_enum:
            {
                break;
            }

            case DSP_Chorus_enum:
            {
                chorus_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_Flanger_enum:
            {
                flanger_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_Phaser_enum:
            {
                phaser_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_Tremolo_enum:
            {
                tremolo_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_Vibrato_enum:
            {
                vibrato_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_EQ_enum:
            {
                eq_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_Compressor_enum:
            {
                compressor_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_Distortion_enum:
            {
                distortion_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_Overdrive_enum:
            {
                overdrive_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_Fuzz_enum:
            {
                fuzz_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_SpeakerSim_enum:
            {
                speaker_sim_process_block(
                    I2S_LeftDecode_Buffer,
                    I2S_RightDecode_Buffer,
                    I2S_BUFFER_HALF_FRAME);
                break;
            }

            case DSP_PreampMarshall_enum:
            {
                break;
            }

            case DSP_PreampVox_enum:
            {
                break;
            }

            case DSP_PreampFender_enum:
            {
                break;
            }

            case DSP_PreampSoldano_enum:
            {
                break;
            }

            default:
            {
                break;
            }
        }
    }

    // ��Ƶ����
    for (uint16_t frame = 0; frame < I2S_BUFFER_HALF_FRAME; frame++)
    {
        int32_t sample_r = I2S_RightDecode_Buffer[frame];
        int32_t sample_l = I2S_LeftDecode_Buffer[frame];

        if (output_volume_q16 == 0u)
        {
            sample_r = 0;
            sample_l = 0;
        }
        else if (output_volume_q16 != DSP_GLOBAL_VOLUME_Q16_ONE)
        {
            sample_r = multiply_q16_clamp24(sample_r, output_volume_q16);
            sample_l = multiply_q16_clamp24(sample_l, output_volume_q16);
        }
        else
        {
            sample_r = clamp24(sample_r);
            sample_l = clamp24(sample_l);
        }

        I2S_32_PackSample(
            I2S_ObjectBuffer + frame * 4,
            (uint32_t)sample_r);
        I2S_32_PackSample(
            I2S_ObjectBuffer + frame * 4 + 2,
            (uint32_t)sample_l);
    }
}