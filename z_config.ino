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
 * @file z_config.ino
 * @author Marcel Licence
 * @date 12.05.2021
 *
 * @brief This file contains the mapping configuration
 * Put all your project configuration here (no defines etc)
 * This file will be included at the and can access all
 * declarations and type definitions
 *
 * @see ESP32 Arduino DIY Synthesizer Projects - Little startup guide to get your MIDI synth working - https://youtu.be/ZNxGCB-d68g
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef AUDIO_KIT_BUTTON_ANALOG
audioKitButtonCb audioKitButtonCallback = App_ButtonCb;
#endif

/*
 * adc to midi mapping
 */
#ifdef _ADC_TO_MIDI_ENABLED
struct adc_to_midi_s adcToMidiLookUp[ADC_TO_MIDI_LOOKUP_SIZE] =
{
    {0, 0x10},
    {1, 0x10},
    {2, 0x10},
    {3, 0x10},
    {4, 0x10},
    {5, 0x10},
    {6, 0x10},
    {7, 0x10},
};


struct adc_to_midi_mapping_s adcToMidiMapping =
{
    adcToMidiLookUp,
    sizeof(adcToMidiLookUp) / sizeof(adcToMidiLookUp[0]),
#ifdef MIDI_VIA_USB_ENABLED
    UsbMidi_SendControlChange,
#else
    Midi_ControlChange,
#endif
};

#endif

/*
 * this mapping is used for the edirol pcr-800
 * this should be changed when using another controller
 *
 * Make your own mapping:
 * The MIDI controller might send different control change messages when moving knobs,
 * sliders and pressing buttons (if available)
 *
 * Usually the controller sends message on a selected channel (configured channel).
 * The channel is represented by the first hex value.
 * In the mapping for the PCR-800 the first hex value is different because the controller is configured a bit weird.
 * MIDI channel 1 to 16 are represented by the values 0x0 to 0xF
 *
 * The second value represents the control change number.
 * Some are defined by the MIDI specification like 0x40 - "sustain"
 *
 * The third value is a free text. This might be used in future for display purposes.
 * You can just enter some text to identify the knob etc.
 *
 * The last three values are for linking a function + user parameter
 *
 * For example your controller is sending a control change message
 * - channel 0x2, control number: 0x11, value: 0x32
 * following entry will be identified as linked:
 * { 0x2, 0x11, "S3", NULL, Synth_SetParam, SYNTH_PARAM_VEL_ENV_SUSTAIN},
 *
 * The function Synth_SetParam will be called with following parameters:
 * Synth_SetParam(SYNTH_PARAM_VEL_ENV_SUSTAIN, 0x32);
 *
 * Some functions ignore the additional parameter in that case the last value can be left as zero.
 */

/*
 * this mapping will be always active and can not be changed during runtime
 */
struct midiControllerMapping edirolMapping[] =
{
    { 0x0, 0x51, "B1", NULL, Midi_SetMidiMapByIndex, 0},
    { 0x1, 0x51, "B2", NULL, Midi_SetMidiMapByIndex, 1},
    { 0x2, 0x51, "B3", NULL, Midi_SetMidiMapByIndex, 2},
    { 0x3, 0x51, "B4", NULL, Midi_SetMidiMapByIndex, 3},

    { 0x4, 0x51, "B5", NULL, Midi_SetMidiMapByIndex, 4},
    { 0x5, 0x51, "B6", NULL, Midi_SetMidiMapByIndex, 5},
    { 0x6, 0x51, "B7", NULL, Midi_SetMidiMapByIndex, 6},
    { 0x7, 0x51, "B8", NULL, Midi_SetMidiMapByIndex, 7},

    { 0x1, 0x53, "B9", NULL, Synth_SelectLastChannel, 8},

    /* rotary */
    { 0x5, 0x10, "R6", NULL, Synth_SetParam, SYNTH_PARAM_MAIN_FILT_CUTOFF},
    { 0x6, 0x10, "R7", NULL, Synth_SetParam, SYNTH_PARAM_MAIN_FILT_RESO},
};

/*
 * the other mappings can be selected during runtime
 */
struct midiControllerMapping edirolMapping_arp[] =
{
    /* upper row of buttons */
    { 0x0, 0x50, "A1", NULL, Arp_SelectSequence, 0},
    { 0x1, 0x50, "A2", NULL, Arp_SelectSequence, 1},
    { 0x2, 0x50, "A3", NULL, Arp_SelectSequence, 2},
    { 0x3, 0x50, "A4", NULL, Arp_SelectSequence, 3},

    { 0x4, 0x50, "A5", NULL, Arp_SelectSequence, 4},
    { 0x5, 0x50, "A6", NULL, Arp_SelectSequence, 5},
    { 0x6, 0x50, "A7", NULL, Arp_SelectSequence, 6},
    { 0x7, 0x50, "A8", NULL, Arp_SelectSequence, 7},

    { 0x0, 0x53, "A9", NULL, Arp_StartRecord, 8},

    /* slider */
    { 0x0, 0x11, "S1", NULL, Arp_Tempo, SYNTH_PARAM_VEL_ENV_ATTACK},
    { 0x1, 0x11, "S2", NULL, Arp_GateTime, SYNTH_PARAM_VEL_ENV_DECAY},
};

struct midiControllerMapping edirolMapping_osc[] =
{
    /* upper row of buttons */
    { 0x0, 0x50, "A1", NULL, Synth_SetOctave, 0},
    { 0x1, 0x50, "A2", NULL, Synth_SetOctave, 1},
    { 0x2, 0x50, "A3", NULL, Synth_SetOctave, 2},
    { 0x3, 0x50, "A4", NULL, Synth_SetOctave, 3},

    { 0x4, 0x50, "A5", NULL, Synth_SetCfgOsc, 0},
    { 0x5, 0x50, "A6", NULL, Synth_SetCfgOsc, 1},
    { 0x6, 0x50, "A7", NULL, Synth_SetCfgOsc, 2},
    { 0x7, 0x50, "A8", NULL, NULL, 7},

    { 0x0, 0x53, "A9", NULL, Synth_ToggleMono, 8},

    /* rotary */
    { 0x0, 0x10, "R1", NULL, Synth_SetOscParam, SYNTH_OSC_PARAM_WAVEFORM},
    { 0x1, 0x10, "R2", NULL, Synth_SetOscParam, SYNTH_OSC_PARAM_VOLUME},
    { 0x2, 0x10, "R3", NULL, Synth_SetOscParam, SYNTH_OSC_PARAM_PITCH},

    { 0x3, 0x10, "R4", NULL, Synth_SetParam, SYNTH_PARAM_VOICE_MORPH},

};

struct midiControllerMapping edirolMapping_voc[] =
{
    /* upper row of buttons */
    { 0x0, 0x50, "A1", NULL, Synth_Sel_Env, ENV_IDX_VOLUME},
    { 0x1, 0x50, "A2", NULL, Synth_Sel_Env, ENV_IDX_FILTER},
    { 0x2, 0x50, "A3", NULL, Synth_Sel_Env, ENV_IDX_PITCH},
    { 0x3, 0x50, "A4", NULL, Synth_Sel_Env, ENV_IDX_MOD_PITCH},

    { 0x4, 0x50, "A5", NULL, Synth_Sel_Env, ENV_IDX_MOD_FILTER},
    { 0x5, 0x50, "A6", NULL, Synth_Sel_Env, ENV_IDX_MOD_MORPH},
    { 0x6, 0x50, "A7", NULL, NULL, 2},
    { 0x7, 0x50, "A8", NULL, NULL, 7},

    { 0x0, 0x53, "A9", NULL, Synth_ToggleMono, 8},

    /* rotary */
    { 0x0, 0x10, "R1", NULL, Synth_SetParam, SYNTH_PARAM_VOICE_PORT_TIME},
    { 0x1, 0x10, "R2", NULL, Synth_SetParam, SYNTH_PARAM_VOICE_MORPH_LFO},
    { 0x2, 0x10, "R3", NULL, Synth_SetParam, SYNTH_PARAM_VOICE_MORPH},
    { 0x3, 0x10, "R4", NULL, Synth_SetParam, SYNTH_PARAM_VOICE_MORPH_WAVEFORM},

    { 0x0, 0x12, "R9", NULL, Synth_SetParam, SYNTH_PARAM_VOICE_FILT_RESO},

    /* slider */
    { 0x0, 0x11, "S1", NULL, Synth_SetParam, SYNTH_PARAM_VEL_ENV_ATTACK},
    { 0x1, 0x11, "S2", NULL, Synth_SetParam, SYNTH_PARAM_VEL_ENV_DECAY},
    { 0x2, 0x11, "S3", NULL, Synth_SetParam, SYNTH_PARAM_VEL_ENV_SUSTAIN},
    { 0x3, 0x11, "S4", NULL, Synth_SetParam, SYNTH_PARAM_VEL_ENV_RELEASE},

    { 0x4, 0x11, "S5", NULL, Synth_SetParam, SYNTH_PARAM_FIL_ENV_ATTACK},
    { 0x5, 0x11, "S6", NULL, Synth_SetParam, SYNTH_PARAM_FIL_ENV_DECAY},
    { 0x6, 0x11, "S7", NULL, Synth_SetParam, SYNTH_PARAM_FIL_ENV_SUSTAIN},
    { 0x7, 0x11, "S8", NULL, Synth_SetParam, SYNTH_PARAM_FIL_ENV_RELEASE},
};

struct midiControllerMapping edirolMapping_fx[] =
{
#ifdef MAX_DELAY
    { 0x0, 0x10, "R1", NULL, Delay_SetLength, 2},
    { 0x1, 0x10, "R2", NULL, Delay_SetOutputLevel, 3},
    { 0x2, 0x10, "R3", NULL, Delay_SetFeedback, 4},
#endif

#ifdef REVERB_ENABLED
    { 0x0, 0x12, "R9", NULL, Reverb_SetLevel, 0},
#endif
};

/*
 * this lookup table shall contain all selectable MIDI maps
 */
struct midiMapLookUpEntry midiMapLookUp[] =
{
    {"Oscillator", edirolMapping_osc, sizeof(edirolMapping_osc) / sizeof(edirolMapping_osc[0])},
    {"Voice", edirolMapping_voc, sizeof(edirolMapping_voc) / sizeof(edirolMapping_voc[0])},
    {"Arpeggio", edirolMapping_arp, sizeof(edirolMapping_arp) / sizeof(edirolMapping_arp[0])},
    {"Effects", edirolMapping_fx, sizeof(edirolMapping_fx) / sizeof(edirolMapping_fx[0])},
};

int midiMapLookUpCnt = sizeof(midiMapLookUp) / sizeof(midiMapLookUp[0]);

struct midiMapping_s midiMapping =
{
    NULL,
#ifdef ARP_MODULE_ENABLED
    Arp_NoteOn,
    Arp_NoteOff,
#else
    Synth_NoteOn,
    Synth_NoteOff,
#endif
    Synth_PitchBend,
    Synth_ModulationWheel,
    Synth_RealTimeMsg,
    Synth_SongPosition,
    edirolMapping,
    sizeof(edirolMapping) / sizeof(edirolMapping[0]),
    edirolMapping_osc,
    sizeof(edirolMapping_osc) / sizeof(edirolMapping_osc[0]),
};

#ifdef MIDI_VIA_USB_ENABLED
struct usbMidiMappingEntry_s usbMidiMappingEntries[] =
{
    {
        NULL,
        App_UsbMidiShortMsgReceived,
        NULL,
        NULL,
        0xFF,
    },
};

struct usbMidiMapping_s usbMidiMapping =
{
    NULL,
    NULL,
    usbMidiMappingEntries,
    sizeof(usbMidiMappingEntries) / sizeof(usbMidiMappingEntries[0]),
};
#endif /* MIDI_VIA_USB_ENABLED */

