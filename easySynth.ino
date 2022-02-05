/*
 * Copyright (c) 2022 Marcel Licence
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
 * veröffentlichten Version, weiter verteilen und/oder modifizieren.
 *
 * Dieses Programm wird in der Hoffnung bereitgestellt, dass es nützlich sein wird, jedoch
 * OHNE JEDE GEWÄHR,; sogar ohne die implizite
 * Gewähr der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Einzelheiten.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <https://www.gnu.org/licenses/>.
 */

/**
 * @file easySynth.ino
 * @author Marcel Licence
 * @date 17.12.2021
 *
 * @brief   This file contains a simple implementation for a polyphonic synthesizer
 */


#ifdef __CDT_PARSER__
#include "cdt.h"
#endif


#include <ml_osc.h>
#include <ml_env.h>
#include <ml_filter.h>
#include <ml_waveform.h>


static struct notePlayerT *getFreeVoice(void);


/*
 * Param indices for Synth_SetParam function
 */
#define SYNTH_PARAM_VEL_ENV_ATTACK  0
#define SYNTH_PARAM_VEL_ENV_DECAY   1
#define SYNTH_PARAM_VEL_ENV_SUSTAIN 2
#define SYNTH_PARAM_VEL_ENV_RELEASE 3
#define SYNTH_PARAM_FIL_ENV_ATTACK  4
#define SYNTH_PARAM_FIL_ENV_DECAY   5
#define SYNTH_PARAM_FIL_ENV_SUSTAIN 6
#define SYNTH_PARAM_FIL_ENV_RELEASE 7
#define SYNTH_PARAM_MAIN_FILT_CUTOFF    10
#define SYNTH_PARAM_MAIN_FILT_RESO      11
#define SYNTH_PARAM_VOICE_FILT_RESO     12
#define SYNTH_PARAM_VOICE_NOISE_LEVEL   13

#define SYNTH_PARAM_VOICE_PORT_TIME     14

#define SYNTH_PARAM_PITCH_BEND_RANGE    15
#define SYNTH_PARAM_MODULATION_SPEED    16
#define SYNTH_PARAM_MODULATION_PITCH    17

#define SYNTH_PARAM_OSCILLATOR_SEL      18

#define SYNTH_PARAM_VOICE_MORPH         19
#define SYNTH_PARAM_VOICE_MORPH_WAVEFORM    20
#define SYNTH_PARAM_VOICE_MORPH_LFO 21

#define SYNTH_OSC_PARAM_VOLUME      0
#define SYNTH_OSC_PARAM_PITCH      1
#define SYNTH_OSC_PARAM_WAVEFORM      2

/*
 * following are the envelope indexes for easier selection
 */
#define ENV_IDX_VOLUME  0 /* envelope for the voice volume */
#define ENV_IDX_FILTER  1 /* envelope for the voice filter */
#define ENV_IDX_PITCH  2 /* envelope for the voice pitch */
#define ENV_IDX_MOD_PITCH  3 /* envelope for the voice modulation depth of pitch */
#define ENV_IDX_MOD_FILTER  4 /* envelope for the voice modulation depth of filter */
#define ENV_IDX_MOD_MORPH  5 /* envelope for the voice modulation depth of filter */

/*
 * Following defines can be changed for different puprposes
 */
#ifdef ARDUINO_DISCO_F407VG
#define MAX_POLY_VOICE  6  /* max single voices, can use multiple osc */
#else
#define MAX_POLY_VOICE  8  /* max single voices, can use multiple osc */
#endif
#define MAX_POLY_OSC    (3*MAX_POLY_VOICE) /* osc polyphony, always active reduces single voices max poly */

#define MIDI_NOTE_CNT 128
static uint32_t midi_note_to_add[MIDI_NOTE_CNT]; /* lookup to playback waveforms with correct frequency */

#define CHANNEL_MAX 16

/*
 * set the correct count of available waveforms
 */
#define WAVEFORM_TYPE_COUNT 7

/*
 * add here your waveforms
 */
//float *sine = NULL;
float sine[WAVEFORM_CNT];
//float *saw = NULL;
float saw[WAVEFORM_CNT];
//float *square = NULL;
float square[WAVEFORM_CNT];
#ifdef ESP32
float *pulse = NULL;
float *tri = NULL;
float *crappy_noise = NULL;
float *silence = NULL;
#else
float pulse[WAVEFORM_CNT];
float tri[WAVEFORM_CNT];
float crappy_noise[WAVEFORM_CNT];
float silence[WAVEFORM_CNT];
#endif

/*
 * do not forget to enter the waveform pointer addresses here
 */
float *waveFormLookUp[WAVEFORM_TYPE_COUNT];

struct synth_ctrl_s
{
    uint8_t sel_osc_mask;
} synthCtrl =
{
    1,
};

static uint8_t lastCh = 0;


struct synth_channel_cfg_s
{
    struct synth_osc_cfg_s oscCfg[3];

    float glideTime;
    float cutoffFreq;

    struct adsrT filEnv;
    struct adsrT volEnv;
};

static struct filterCoeffT filterGlobalC;
static struct filterProcT mainFilterL, mainFilterR;


#define NOTE_STACK_MAX  8


struct channelSetting_s
{
    float soundFiltReso;
    float soundNoiseLevel;

    struct adsrT adsr_vol;
    struct adsrT adsr_fil;
    struct adsrT adsr_pit;
    struct adsrT adsr_mod;
    struct adsrT adsr_mof;
    struct adsrT adsr_mph;

    float pitchEnv;

    /* modulation */
    float modulationDepth;
    float modulationSpeed;
    float modulationPitch;

    /* pitchbend */
    float pitchBendValue;
    float pitchMultiplier;

    /* morph */
    float morph;
    float morph_lfo;

    /* mono mode variables */
    bool mono;

    float portAdd;
    float port;
    float noteA;
    float noteB;

    float modulation;

    uint32_t noteCnt;
    uint32_t noteStack[NOTE_STACK_MAX];

    /*
     * store configuration for three different oscillators
     */
    struct synth_osc_cfg_s oscCfg[3];
};

static struct channelSetting_s chCfg[CHANNEL_MAX];
static struct channelSetting_s *curChCfg = &chCfg[0];

static uint8_t selectedOsc = 0;

float voiceSink[SAMPLE_BUFFER_SIZE];
struct oscillatorT oscPlayer[MAX_POLY_OSC];

static uint32_t osc_act = 0;



struct notePlayerT
{
    float lastSample[2][SAMPLE_BUFFER_SIZE];

    float velocity;
    bool active;
    //  adsr_phaseT phase;

    uint8_t midiCh;
    uint8_t midiNote;

    // float control_sign;
    float out_level;

    float pitch_mod;

    float morph; /* current morph value per voice */


    struct filterCoeffT filterC;
    struct filterProcT filterL;
    struct filterProcT filterR;

    float f_control_sign_slow;

    struct adsr_ctrl_t adsr_volume;
    struct adsr_ctrl_t adsr_filter;
    struct adsr_ctrl_t adsr_pitch;
    struct adsr_ctrl_t adsr_mod;
    struct adsr_ctrl_t adsr_mod_f;
    struct adsr_ctrl_t adsr_mph;

    struct channelSetting_s *cfg;
};


struct notePlayerT voicePlayer[MAX_POLY_VOICE];

uint32_t voc_act = 0;

static float justOne = 1.0f;

void Synth_Init()
{
#ifdef ESP32
    randomSeed(34547379);
#endif

    /*
     * we do not check if malloc was successful
     * if there is not enough memory left the application will crash
     */
    //sine = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
    //saw = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
    //square = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
#ifdef ESP32
    pulse = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
    tri = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
    crappy_noise = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
    silence = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
#endif

    if (sine == NULL)
    {
        Serial.printf("Not enough memory (1)\n");
    }
    if (saw == NULL)
    {
        Serial.printf("Not enough memory (2)\n");
    }
    else
    {
        Serial.printf("saw ready\n");
    }
    if (square == NULL)
    {
        Serial.printf("Not enough memory (3)\n");
    }
    if (pulse == NULL)
    {
        Serial.printf("Not enough memory (4)\n");
    }
    if (tri == NULL)
    {
        Serial.printf("Not enough memory (5)\n");
    }
    if (crappy_noise == NULL)
    {
        Serial.printf("Not enough memory (6)\n");
    }
    if (silence == NULL)
    {
        Serial.printf("Not enough memory (7)\n");
    }



    /*
     * let us calculate some waveforms
     * - using lookup tables can save a lot of processing power later
     * - but it does consume memory
     */
    for (int i = 0; i < WAVEFORM_CNT; i++)
    {
        float val = (float)sin(i * 2.0 * PI / WAVEFORM_CNT);
        sine[i] = val;
        saw[i] = (2.0f * ((float)i) / ((float)WAVEFORM_CNT)) - 1.0f;
        square[i] = (i > (WAVEFORM_CNT / 2)) ? 1 : -1;
        pulse[i] = (i > (WAVEFORM_CNT / 4)) ? 1.0f / 4.0f : -3.0f / 4.0f;
        tri[i] = ((i > (WAVEFORM_CNT / 2)) ? (((4.0f * (float)i) / ((float)WAVEFORM_CNT)) - 1.0f) : (3.0f - ((4.0f * (float)i) / ((float)WAVEFORM_CNT)))) - 2.0f;
        crappy_noise[i] = (random(1024) / 512.0f) - 1.0f;
        silence[i] = 0;
    }

    waveFormLookUp[0] = sine;
    waveFormLookUp[1] = saw;
    waveFormLookUp[2] = square;
    waveFormLookUp[3] = pulse;
    waveFormLookUp[4] = tri;
    waveFormLookUp[5] = crappy_noise;
    waveFormLookUp[6] = silence;

    /*
     * initialize all oscillators
     */
    for (int i = 0; i < MAX_POLY_OSC; i++)
    {
        oscillatorT *osc = &oscPlayer[i];
        osc->waveForm = &waveFormLookUp[WAVEFORM_TYPE_COUNT - 1];
        osc->dest[0] = voiceSink;
        osc->dest[1] = voiceSink;

        osc->cfg = &chCfg[0].oscCfg[0]; /* link to a configuration */
        osc->pitchMod = &justOne;
    }

    /*
     * initialize all voices
     */
    for (int i = 0; i < MAX_POLY_VOICE; i++)
    {
        notePlayerT *voice = &voicePlayer[i];
        voice->active = false;
        for (int n = 0; n < SAMPLE_BUFFER_SIZE; n++)
        {
            voice->lastSample[0][n] = 0.0f;
            voice->lastSample[1][n] = 0.0f;
        }
        voice->filterL.filterCoeff = &voice->filterC;
        voice->filterR.filterCoeff = &voice->filterC;
        voice->cfg = &chCfg[0];
    }

    /*
     * prepare lookup for constants to drive oscillators
     */
    for (int i = 0; i < MIDI_NOTE_CNT; i++)
    {
        float f = ((pow(2.0f, (float)(i - 69) / 12.0f) * 440.0f));
        uint32_t add = (uint32_t)(f * ((float)(1ULL << 32ULL) / ((float)SAMPLE_RATE)));
        midi_note_to_add[i] = add;
    }

    /*
     * assign main filter
     */
    mainFilterL.filterCoeff = &filterGlobalC;
    mainFilterR.filterCoeff = &filterGlobalC;

    Filter_Proc_Init(&mainFilterL);
    Filter_Proc_Init(&mainFilterR);
    Filter_Coeff_Init(mainFilterL.filterCoeff);

    Filter_Calculate(1.0f, 1.0f, &filterGlobalC);

    for (int i = 0; i < CHANNEL_MAX; i++)
    {
        Synth_ChannelSettingInit(&chCfg[i]);
    }
}

static struct filterCoeffT mainFilt;

static float filtCutoff = 1.0f;
static float filtReso = 0.5f;
static float constZero = 0.0f;

static void Synth_OscCfgInit(struct synth_osc_cfg_s *cfg)
{
    cfg->pitch = 1.0f;
    cfg->pitchOctave = 1;
    cfg->volume = 0.0f;
    cfg->selectedWaveForm = saw;
    cfg->morphWaveForm = sine;
    cfg->morph = &constZero;
}

static void Synth_ChannelSettingInit(struct channelSetting_s *setting)
{
    setting->soundFiltReso = 0.5f;
    setting->soundNoiseLevel = 0.0f;

    struct adsrT adsr_def = {1.0f, 0.25f, 1.0f, 0.01f};

    memcpy(&setting->adsr_vol, &adsr_def, sizeof(adsr_def));
    memcpy(&setting->adsr_fil, &adsr_def, sizeof(adsr_def));
    memcpy(&setting->adsr_pit, &adsr_def, sizeof(adsr_def));
    memcpy(&setting->adsr_mod, &adsr_def, sizeof(adsr_def));
    memcpy(&setting->adsr_mof, &adsr_def, sizeof(adsr_def));
    memcpy(&setting->adsr_mph, &adsr_def, sizeof(adsr_def));


    setting->pitchEnv = 0.5f;

    setting->modulationDepth = 0.0f;
    setting->modulationSpeed = 5.0f;
    setting->modulationPitch = 1.0f;

    setting->pitchBendValue = 0.0f;
    setting->pitchMultiplier = 1.0f;

    setting->morph = 0.0f;
    setting->morph_lfo = 0.0f;

    setting->mono = false;
    setting->portAdd = 0.01f; /*!< speed of portamento */
    setting->port = 1.0f;
    setting->noteA = 0;
    setting->noteB = 0;

    setting->noteCnt = 0;
    /* setting->noteStack[NOTE_STACK_MAX]; can be left uninitialized */

    for (int i = 0; i < 3; i++)
    {
        Synth_OscCfgInit(&setting->oscCfg[i]);
        setting->oscCfg->pitchMultiplier = &setting->pitchMultiplier; /* link pitch multiplier */
    }
    setting->oscCfg[0].volume = 1.0f;
}
void Voice_Off(uint32_t i)
{
    notePlayerT *voice = &voicePlayer[i];
    for (int f = 0; f < MAX_POLY_OSC; f++)
    {
        oscillatorT *osc = &oscPlayer[f];
        if (osc->dest[0] == voice->lastSample[0])
        {
            osc->dest[0] = voiceSink;
            osc->dest[1] = voiceSink;
            osc_act -= 1;
        }
    }
    voc_act -= 1;
}

inline
float SineNorm(float alpha_div2pi)
{
    uint32_t index = ((uint32_t)(alpha_div2pi * ((float)WAVEFORM_CNT))) % WAVEFORM_CNT;
    return sine[index];
}

inline
float GetModulation(uint8_t ch)
{
    float modSpeed = chCfg[ch].modulationSpeed;
    return chCfg[ch].modulationPitch * (SineNorm((modSpeed * ((float)millis()) / 1000.0f)));
}

//[[gnu::noinline, gnu::optimize ("fast-math")]]
inline void Synth_Process(float *left, float *right, uint32_t len)
{
    /*
     * update pitch bending / modulation
     */
    {

        for (int i = 0; i < CHANNEL_MAX; i++)
        {
            chCfg[i].modulation = GetModulation(i) ;

            chCfg[i].port += chCfg[i].portAdd; /* active portamento */
            chCfg[i].port = chCfg[i].port > 1.0f ? 1.0f : chCfg[i].port; /* limit value to max of 1.0f */

            float portVal = (((float)(chCfg[i].noteA)) * (1.0f - chCfg[i].port) + ((float)(chCfg[i].noteB)) * chCfg[i].port);

            float pitchVar = chCfg[i].pitchBendValue + chCfg[i].modulationDepth * chCfg[i].modulation + portVal;
            chCfg[i].pitchMultiplier = pow(2.0f, pitchVar / 12.0f);
        }
    }

    /*
     * prepare all voices
     */
    for (int i = 0; i < MAX_POLY_VOICE; i++) /* one loop is faster than two loops */
    {
        notePlayerT *voice = &voicePlayer[i];
        for (uint32_t n = 0; n < len; n++)
        {
            voice->lastSample[0][n] = 0;
            voice->lastSample[1][n] = 0;
        }
        voice->pitch_mod = 1.0f + (voice->adsr_pitch.ctrl * voice->cfg->adsr_pit.w);
        voice->pitch_mod *= pow(2.0f, voice->cfg->modulation * voice->adsr_mod.ctrl * voice->cfg->adsr_mod.s);

        voice->morph = voice->cfg->morph;
        voice->morph += voice->adsr_mph.ctrl * voice->cfg->adsr_mph.w;
        voice->morph += voice->cfg->morph_lfo * voice->cfg->modulation * 2;
    }

    /*
     * oscillator processing -> mix to voice
     */
    OscProcess(oscPlayer, MAX_POLY_OSC, len);

#ifdef NOISE_ENABLED
    float noise_signal[len];

    for (uint32_t n = 0; n < len; n++)
    {
        /* generate a noise signal */
        noise_signal[n] = ((random(1024) / 512.0f) - 1.0f) * soundNoiseLevel;
    }
#endif

    for (int i = 0; i < MAX_POLY_VOICE; i++) /* one loop is faster than two loops */
    {
        notePlayerT *voice = &voicePlayer[i];
        if (voice->active)
        {
#ifdef NOISE_ENABLED
            for (uint32_t n = 0; n < len; n++)
            {
                /* add some noise to the voice */
                voice->lastSample[0][n] += noise_signal[n];
                voice->lastSample[1][n] += noise_signal[n];
            }
#endif

            for (uint32_t n = 0; n < len; n++)
            {
                if (n % 4 == 0)
                {
                    voice->active = ADSR_Process(&voice->cfg->adsr_vol, &voice->adsr_volume);
                    if (voice->active == false)
                    {
                        Voice_Off(i);
                    }
                    /*
                     * make is slow to avoid bad things .. or crying ears
                     */
                    (void)ADSR_Process(&voice->cfg->adsr_fil, &voice->adsr_filter);

                    (void)ADSR_Process(&voice->cfg->adsr_pit, &voice->adsr_pitch);
                    (void)ASRM_Process(&voice->cfg->adsr_mod, &voice->adsr_mod);
                    (void)ASRM_Process(&voice->cfg->adsr_mof, &voice->adsr_mod_f);
                    (void)ASRM_Process(&voice->cfg->adsr_mph, &voice->adsr_mph);
                }

                voice->lastSample[0][n] *= voice->adsr_volume.ctrl * voice->velocity;
                voice->lastSample[1][n] *= voice->adsr_volume.ctrl * voice->velocity;

                if (n % 32 == 0)
                {
                    float ff = voice->adsr_filter.ctrl;
                    ff += voice->cfg->modulation * voice->adsr_mod_f.ctrl * voice->cfg->adsr_mof.s ;
                    if (ff > 1.0f)
                    {
                        ff = 1.0f;
                    }
                    if (ff < 0.0f)
                    {
                        ff = 0.0;
                    }
                    voice->f_control_sign_slow = 0.05 * ff + 0.95 * voice->f_control_sign_slow;
                    Filter_Calculate(voice->f_control_sign_slow, voice->cfg->soundFiltReso, &voice->filterC);
                }
            }

            Filter_Process_Buffer(voice->lastSample[0], &voice->filterL, len);
            Filter_Process_Buffer(voice->lastSample[1], &voice->filterR, len);

            /*
             * push voice audio to main signal
             */
            for (uint32_t n = 0; n < len; n++)
            {
                left[n] += voice->lastSample[0][n] * 0.4f * 0.25f * (1.0f / 3.0f) ;
                right[n] += voice->lastSample[1][n] * 0.4f * 0.25f * (1.0f / 3.0f);
            }
        }
    }

    /*
     * process main filter
     */
    Filter_Process_Buffer(left, &mainFilterL, len);
    Filter_Process_Buffer(right, &mainFilterR, len);
}

struct oscillatorT *getFreeOsc()
{
    for (int i = 0; i < MAX_POLY_OSC ; i++)
    {
        //if (oscPlayer[i].isFree())
        if (oscPlayer[i].dest[0] == voiceSink)
        {
            return &oscPlayer[i];
        }
    }
    return NULL;
}

static struct notePlayerT *getFreeVoice(void)
{
    for (int i = 0; i < MAX_POLY_VOICE ; i++)
    {
        if (voicePlayer[i].active == false)
        {
            return &voicePlayer[i];
        }
    }
    return NULL;
}

inline void Synth_NoteOn(uint8_t ch, uint8_t note, float vel __attribute__((unused)))
{
    struct notePlayerT *voice = getFreeVoice();
    struct oscillatorT *osc = getFreeOsc();

    lastCh = ch;

    /* put note onto stack */
    if (chCfg[ch].mono)
    {
        if (chCfg[ch].noteCnt < (NOTE_STACK_MAX - 1))
        {
            chCfg[ch].noteStack[chCfg[ch].noteCnt] = note;
            chCfg[ch].noteCnt++;
            //Status_ValueChangedIntArr("noteCnt", chCfg[ch].noteCnt, ch);
        }

        if (chCfg[ch].noteCnt > 1)
        {
            for (int i = 0; i < MAX_POLY_VOICE ; i++)
            {
                if ((voicePlayer[i].active) && (voicePlayer[i].midiCh == ch))
                {
                    float diff = note - voicePlayer[i].midiNote;

                    voicePlayer[i].cfg->noteA = voicePlayer[i].cfg->port * ((float)voicePlayer[i].cfg->noteB) + (1.0f - voicePlayer[i].cfg->port) * voicePlayer[i].cfg->noteA;
                    voicePlayer[i].cfg->port = 0.0f;

                    voicePlayer[i].cfg->noteB += diff;
                    voicePlayer[i].midiNote = note;

                    return;
                }
            }
        }
    }

    /*
     * No free voice found, return otherwise crash xD
     */
    if ((voice == NULL) || (osc == NULL))
    {
        Serial.printf("NotFree, voc: %d, osc: %d\n", voc_act, osc_act);
        return ;
    }

    voice->cfg = &chCfg[ch];
    voice->midiCh = ch;
    voice->midiNote = note;
#ifdef MIDI_USE_CONST_VELOCITY
    voice->velocity = 1.0f;
#else
    voice->velocity = vel;
#endif
    for (int n = 0; n < SAMPLE_BUFFER_SIZE; n++)
    {
        voice->lastSample[0][n] = 0.0f;
        voice->lastSample[1][n] = 0.0f;
    }

    /* default values to avoid portamento */
    voice->cfg->port = 1.0f;
    voice->cfg->noteB = 0;

    /* pitch mod */
    voice->pitch_mod = 1.0f;
    voice->morph = 0.0f;

    voice->active = true;

    ADSR_Start(&voice->cfg->adsr_vol, &voice->adsr_volume);
    ADSR_Start(&voice->cfg->adsr_fil, &voice->adsr_filter);
    ADSR_Start(&voice->cfg->adsr_pit, &voice->adsr_pitch);
    ADSR_Start(&voice->cfg->adsr_mod, &voice->adsr_mod);
    ADSR_Start(&voice->cfg->adsr_mof, &voice->adsr_mod_f);
    ADSR_Start(&voice->cfg->adsr_mph, &voice->adsr_mph);

    voice->f_control_sign_slow = voice->adsr_filter.ctrl;

    /* update all values to avoid audible artifacts */
    ADSR_Process(&voice->cfg->adsr_vol, &voice->adsr_volume);
    ADSR_Process(&voice->cfg->adsr_fil, &voice->adsr_filter);
    ADSR_Process(&voice->cfg->adsr_pit, &voice->adsr_pitch);
    ASRM_Process(&voice->cfg->adsr_mod, &voice->adsr_mod);
    ASRM_Process(&voice->cfg->adsr_mof, &voice->adsr_mod_f);
    ASRM_Process(&voice->cfg->adsr_mph, &voice->adsr_mph);

    Filter_Calculate(voice->f_control_sign_slow, voice->cfg->soundFiltReso, &voice->filterC);

    voc_act += 1;

    /*
     * add oscillator
     */
    for (int  i = 0; i < 3; i++)
    {
        osc = getFreeOsc();
        if (osc != NULL)
        {

            {
                osc->addVal = midi_note_to_add[note];
            }
            osc->samplePos = 0;
            osc->dest[0] = voice->lastSample[0];
            osc->dest[1] = voice->lastSample[1];
            osc->pan_l = 1;
            osc->pan_r = 1;
            osc->cfg = &chCfg[ch].oscCfg[i];
            osc->cfg->pitchMultiplier = &chCfg[ch].pitchMultiplier;

            osc->pitchMod = &voice->pitch_mod;
            osc->cfg->morph = &voice->morph;

            osc_act += 1;
        }
        else
        {
            Serial.printf("not enough free osc!\n");
            break;
        }
    }

    /*
     * trying to avoid audible suprises
     */
    Filter_Reset(&voice->filterL);
    Filter_Reset(&voice->filterR);
    Filter_Process(&voice->lastSample[0][0], &voice->filterL);
    Filter_Process(&voice->lastSample[0][0], &voice->filterL);
    Filter_Process(&voice->lastSample[0][0], &voice->filterL);

    Filter_Process(&voice->lastSample[1][0], &voice->filterR);
    Filter_Process(&voice->lastSample[1][0], &voice->filterR);
    Filter_Process(&voice->lastSample[1][0], &voice->filterR);
}

inline void Synth_NoteOff(uint8_t ch, uint8_t note)
{
    for (int j = 0; j < chCfg[ch].noteCnt; j++)
    {
        if (chCfg[ch].noteStack[j] == note)
        {
            for (int k = j; k < NOTE_STACK_MAX - 1; k++)
            {
                chCfg[ch].noteStack[k] = chCfg[ch].noteStack[k + 1];
            }
            chCfg[ch].noteCnt = (chCfg[ch].noteCnt > 0) ? (chCfg[ch].noteCnt - 1) : 0;
            //Status_ValueChangedIntArr("noteCnt-", chCfg[ch].noteCnt, ch);
        }
    }

    for (int i = 0; i < MAX_POLY_VOICE ; i++)
    {
        if ((voicePlayer[i].active) && (voicePlayer[i].midiNote == note) && (voicePlayer[i].midiCh == ch))
        {
            if ((voicePlayer[i].cfg->noteCnt > 0) && (voicePlayer[i].cfg->mono))
            {
                uint8_t midiNote = voicePlayer[i].cfg->noteStack[voicePlayer[i].cfg->noteCnt - 1];

                float diff = midiNote - voicePlayer[i].midiNote;

                voicePlayer[i].cfg->noteA = voicePlayer[i].cfg->port * ((float)voicePlayer[i].cfg->noteB) + (1.0f - voicePlayer[i].cfg->port) * voicePlayer[i].cfg->noteA;
                voicePlayer[i].cfg->port = 0.0f;

                voicePlayer[i].cfg->noteB += diff;
                voicePlayer[i].midiNote = midiNote;
            }
            else
            {
                voicePlayer[i].adsr_volume.phase = release;
            }
        }
    }
}

void Synth_ModulationWheel(uint8_t ch, float value)
{
    chCfg[ch].modulationDepth = value;
}

void Synth_ModulationSpeed(uint8_t ch, float value)
{
    chCfg[ch].modulationSpeed = value * 10;
    //Status_ValueChangedFloat("ModulationSpeed", modulationSpeed);
}

void Synth_ModulationPitch(uint8_t ch, float value)
{
    chCfg[ch].modulationPitch = value * 5;
    //Status_ValueChangedFloat("ModulationDepth", modulationPitch);
}

void Synth_PitchBend(uint8_t ch, float bend)
{
    chCfg[ch].pitchBendValue = bend;
    //Serial.printf("pitchBendValue: %0.3f\n", chCfg[ch].pitchBendValue);
}

void Synth_PortTime(float value)
{
    float min = 0.02f; /* 1/(0.02 * 1000) -> 0.05s */
    float max = 0.0002f; /* 1/(0.0002 * 1000) -> 5s */

    curChCfg->portAdd = (pow(2.0f, value) - 1.0f) * (max - min) + min;
}

void Synth_SetCurCh(uint8_t ch, float value)
{
    if (value > 0)
    {
        if (ch < 16)
        {
            curChCfg = &chCfg[ch];
            //Status_ValueChangedInt("Current ch", ch);
        }
    }
}

void Synth_ToggleMono(uint8_t ch, float value)
{
    if (value > 0)
    {
        curChCfg->mono = !curChCfg->mono;
        //Status_LogMessage(curChCfg->mono ? "Mono" : "Poly");
    }
}

void Synth_SetCfgOsc(uint8_t oscIdx, float value)
{
    if (value > 0)
    {
        if (oscIdx < 3)
        {
            selectedOsc = oscIdx;
        }
    }
}

void Synth_SetOscParam(uint8_t slider, float value)
{
    struct synth_osc_cfg_s *oscCfg = &curChCfg->oscCfg[selectedOsc];
    switch (slider)
    {
    case SYNTH_OSC_PARAM_WAVEFORM:
        {
            uint8_t selWaveForm = (value) * (WAVEFORM_TYPE_COUNT);
            oscCfg->selectedWaveForm = waveFormLookUp[selWaveForm];
            Serial.printf("osc[%d].selWaveForm: %d\n", selectedOsc, selWaveForm);
        }
        break;
    case SYNTH_OSC_PARAM_VOLUME:
        oscCfg->volume = value;
        break;
    case SYNTH_OSC_PARAM_PITCH:
        oscCfg->pitch =  pow(2, (14.0f / 12.0f) * (value - 0.5f));
        break;

    case SYNTH_PARAM_VOICE_MORPH_WAVEFORM:
        {
            uint8_t selWaveForm = (value) * (WAVEFORM_TYPE_COUNT);
            oscCfg->morphWaveForm = waveFormLookUp[selWaveForm];
            Serial.printf("osc[%d].morphWaveForm: %d\n", selectedOsc, selWaveForm);
        }
        break;
    }
}

void Synth_SetOctave(uint8_t octave, float value)
{
    struct synth_osc_cfg_s *oscCfg = &curChCfg->oscCfg[selectedOsc];
    if (value > 0)
    {
        oscCfg->pitchOctave = pow(2, octave);
    }
}

struct adsrT *adsrCfg1 = &chCfg[0].adsr_mph;
struct adsrT *adsrCfg2 = &chCfg[0].adsr_fil;
struct adsrT **adsrCfg = &adsrCfg1;

void Synth_Sel_Env(uint8_t env, float value)
{
    if (value > 0)
    {
        switch (env)
        {
        case ENV_IDX_VOLUME:
            (*adsrCfg) = &curChCfg->adsr_vol;
            break;
        case ENV_IDX_FILTER:
            (*adsrCfg) = &curChCfg->adsr_fil;
            break;
        case ENV_IDX_PITCH:
            (*adsrCfg) = &curChCfg->adsr_pit;
            break;
        case ENV_IDX_MOD_PITCH:
            (*adsrCfg) = &curChCfg->adsr_mod;
            break;
        case ENV_IDX_MOD_FILTER:
            (*adsrCfg) = &curChCfg->adsr_mof;
            break;
        case ENV_IDX_MOD_MORPH:
            (*adsrCfg) = &curChCfg->adsr_mph;
            break;
        }
    }
}

void Synth_SetParam(uint8_t slider, float value)
{
    switch (slider)
    {
    case SYNTH_PARAM_VEL_ENV_ATTACK:
        if (value == 0)
        {
            adsrCfg1->a = 1.0f;
        }
        else if (value >= 0.999f)
        {
            adsrCfg1->a = 0.0f;
        }
        else
        {
            adsrCfg1->a = (0.00005 * pow(5000, 1.0f - value));
        }
        Serial.printf("voice volume attack: %0.6f\n", adsrCfg1->a);
        break;
    case SYNTH_PARAM_VEL_ENV_DECAY:
        if (value == 0)
        {
            adsrCfg1->d = 1.0f;
        }
        else if (value >= 0.999f)
        {
            adsrCfg1->d = 0.0f;
        }
        else
        {
            adsrCfg1->d = (0.00005 * pow(5000, 1.0f - value));
        }
        Serial.printf("voice volume decay: %0.6f\n", adsrCfg1->d);
        break;
    case SYNTH_PARAM_VEL_ENV_SUSTAIN:
        adsrCfg1->s = (0.01 * pow(100, value));
        adsrCfg1->w = ((2.0f * value) - 1.0f);
        Serial.printf("voice volume sustain: %0.6f\n", adsrCfg1->s);
        break;
    case SYNTH_PARAM_VEL_ENV_RELEASE:
        adsrCfg1->r = (0.0001 * pow(100, 1.0f - value));
        Serial.printf("voice volume release: %0.6f\n", adsrCfg1->r);
        break;

    case SYNTH_PARAM_FIL_ENV_ATTACK:
        if (value == 0)
        {
            adsrCfg2->a = 1.0f;
        }
        else
        {
            adsrCfg2->a = (0.00005 * pow(5000, 1.0f - value));
        }
        Serial.printf("voice filter attack: %0.6f\n", adsrCfg2->a);
        break;
    case SYNTH_PARAM_FIL_ENV_DECAY:
        adsrCfg2->d = (0.00005 * pow(5000, 1.0f - value));
        Serial.printf("voice filter decay: %0.6f\n", adsrCfg2->d);
        break;
    case SYNTH_PARAM_FIL_ENV_SUSTAIN:
        adsrCfg2->s = value;
        Serial.printf("voice filter sustain: %0.6f\n", adsrCfg2->s);
        break;
    case SYNTH_PARAM_FIL_ENV_RELEASE:
        adsrCfg2->r = (0.0001 * pow(100, 1.0f - value));
        Serial.printf("voice filter release: %0.6f\n", adsrCfg2->r);
        break;

    case SYNTH_PARAM_MAIN_FILT_CUTOFF:
        filtCutoff = value;
        Serial.printf("main filter cutoff: %0.3f\n", filtCutoff);
        Filter_Calculate(filtCutoff, filtReso, &filterGlobalC);
        break;
    case SYNTH_PARAM_MAIN_FILT_RESO:
        filtReso =  0.5f + 10 * value * value * value; /* min q is 0.5 here */
        Serial.printf("main filter reso: %0.3f\n", filtReso);
        Filter_Calculate(filtCutoff, filtReso, &filterGlobalC);
        break;

    case SYNTH_PARAM_VOICE_FILT_RESO:
        curChCfg->soundFiltReso = 0.5f + 10 * value * value * value; /* min q is 0.5 here */
        Serial.printf("voice filter reso: %0.3f\n", curChCfg->soundFiltReso);
        break;

    case SYNTH_PARAM_VOICE_NOISE_LEVEL:
        curChCfg->soundNoiseLevel = value;
        Serial.printf("voice noise level: %0.3f\n", curChCfg->soundNoiseLevel);
        break;

    case SYNTH_PARAM_VOICE_PORT_TIME:
        curChCfg->portAdd = value * value * value * value;
        Serial.printf("voice port time: %0.3f\n", curChCfg->portAdd);
        break;

    case SYNTH_PARAM_VOICE_MORPH:
        curChCfg->morph = value;
        Serial.printf("morph: %0.3f\n", curChCfg->morph);
        break;

    case SYNTH_PARAM_VOICE_MORPH_LFO:
        curChCfg->morph_lfo = value;
        Serial.printf("morph_lfo: %0.3f\n", curChCfg->morph_lfo);
        break;


    default:
        /* not connected */
        break;
    }
}

void Synth_SelectLastChannel(uint8_t unused, float value)
{
    if (value > 0)
    {
        curChCfg = &chCfg[lastCh];
    }
}

