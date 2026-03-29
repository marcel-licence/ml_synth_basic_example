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
 * @file config.h
 * @author Marcel Licence
 * @date 12.05.2021
 *
 * @brief This file contains the project configuration
 *
 * All definitions are visible in the entire project
 *
 * Put all your project settings here (defines, numbers, etc.)
 * configurations which are requiring knowledge of types etc.
 * shall be placed in z_config.ino (will be included at the end)
 */


#ifndef CONFIG_H_
#define CONFIG_H_


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#define NOTE_ON_AFTER_SETUP /* used to get a test tone without MIDI input. Can be deactivated */

//#define USE_ML_SYNTH_PRO

//#define AUDIO_PASS_THROUGH

/* use the following to test the output / codec */
//#define OUTPUT_SAW_TEST
//#define OUTPUT_SINE_TEST


#define ARP_MODULE_ENABLED /* allow using arp module */
#define MIDI_SYNC_MASTER /* turn this off to use external midi clock signal */


#define MIDI_MAP_FLEX_ENABLED

/* this will force using const velocity for all notes, remove this to get dynamic velocity */
#define MIDI_USE_CONST_VELOCITY

/* you can receive MIDI messages via serial-USB connection */
/*
 * you could use for example https://projectgus.github.io/hairless-midiserial/
 * to connect your MIDI device via computer to the serial port
 */
//#define MIDI_RECV_FROM_SERIAL

/* MIDI_VIA_USB_ENABLED activates MIDI via USB (please look into usbMidiHost.ino for more information) */
//#define MIDI_VIA_USB_ENABLED


//#define MIDI_FMT_INT


#include "config/config_black_f407ve.h"
#include "config/config_blackpill_f411ce.h"
#include "config/config_blue_f103ve.h"
#include "config/config_bluepill_f103c8.h"
#include "config/config_daisy_seed.h"
#include "config/config_disco_f407vg.h"
#include "config/config_esp32.h"
#include "config/config_esp32c3.h"
#include "config/config_esp32s2.h"
#include "config/config_esp8266.h"
#include "config/config_generic_f407vgtx.h"
#include "config/config_rp2040.h"
#include "config/config_rp2350.h"
#include "config/config_seeed_xioa_m0.h"
#include "config/config_stm32f407vgtx.h"
#include "config/config_teensy.h"


/*
 * include the board configuration
 * there you will find the most hardware depending pin settings
 */
#include <ml_boards.h> /* requires library ML_SynthTools: https://github.com/marcel-licence/ML_SynthTools */


#ifndef MIDI_BAUDRATE
#define MIDI_BAUDRATE   31250
#endif


#endif /* CONFIG_H_ */

