/*
 * Copyright (c) 2026 Marcel Licence
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
 * @file config_esp32.h
 * @author Marcel Licence
 *
 * @brief Configuration for ESP32
 */

#if (defined ESP32) && (!defined ARDUINO_LOLIN_S2_MINI) && (!defined ARDUINO_ESP32S2_DEV) && (!defined ARDUINO_ESP32C3_DEV) && (!defined ARDUINO_SEEED_XIAO_M0)


#define MEMORY_FROM_HEAP

#define BOARD_ML_SYNTH_V2 /* activate this when using the ML PCB V1 */
//#define BOARD_ESP32_AUDIO_KIT_AC101 /* activate this when using the ESP32 Audio Kit v2.2 with the AC101 codec */
//#define BOARD_ESP32_AUDIO_KIT_ES8388 /* activate this when using the ESP32 Audio Kit v2.2 with the ES8388 codec */
//#define BOARD_ESP32_DOIT /* activate this when using the DOIT ESP32 DEVKIT V1 board */
//#define BOARD_WEMOS_D1_MINI_ESP32

//#define INPUT_TO_MIX /* use this to mix the input to the organ signal */

#define LED_PIN     BLINK_LED_PIN

#define REVERB_ENABLED

#define MAX_DELAY   (SAMPLE_RATE/2)

/* use this to display a scope on the oled display */
//#define OLED_OSC_DISP_ENABLED

/*
 * use MIDI_BLE_ENABLED to activate the MIDI BLE functionality
 * you might turn off the delay and reverb due to the high heap consumption
 * MIDI BLE will be set as SERVER if MIDI_BLE_CLIENT is deactivated
 * Turn on MIDI_BLE_DEBUG_ENABLED to get some debug messages.
 * @see https://youtu.be/awurJEY8X10
 */
//#define MIDI_BLE_ENABLED
//#define MIDI_BLE_CLIENT /* configured as client it will start to search for the server to connect to */
//#define MIDI_BLE_DEBUG_ENABLED

/*
 * include the board configuration
 * there you will find the most hardware depending pin settings
 */
#include <ml_boards.h> /* requires the ML_Synth library:  https://github.com/marcel-licence/ML_SynthTools */

#ifdef BOARD_ML_V1
#elif (defined BOARD_ESP32_AUDIO_KIT_AC101)
#elif (defined BOARD_ESP32_AUDIO_KIT_ES8388)
#elif (defined BOARD_ESP32_DOIT)

#define MIDI_PORT2_ACTIVE
#define MIDI_RX2_PIN RXD2

/* you can activate the following lines to get an additional MIDI input */
// MIDI_PORT1_ACTIVE
// #define MIDI_RX1_PIN 13
#endif

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE_16BIT
#define SAMPLE_BUFFER_SIZE  48

//#define MIDI_VIA_USB_ENABLED /* activate this when connected to the USB host breakout board */

#endif /* (defined ESP32) && (!defined ARDUINO_LOLIN_S2_MINI) && (!defined ARDUINO_ESP32S2_DEV) && (!defined ARDUINO_ESP32C3_DEV) && (!defined ARDUINO_SEEED_XIAO_M0) */

