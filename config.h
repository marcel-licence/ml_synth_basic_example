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


#define ARP_MODULE_ENABLED /* allow using arp module */
#define MIDI_SYNC_MASTER /* turn this off to use external midi clock signal */


#ifdef TEENSYDUINO
#include <Audio.h> /* required to access teensy audio defines */
#endif



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

/*
 * Configuration for
 * Board: "LOLIN(WEMOS) D1 R2 & mini 2 or similar
 */
#ifdef ESP8266

#define SWAP_SERIAL
#define I2S_NODAC /* RX pin will be used for audio output */
#define LED_PIN     LED_BUILTIN

#define MIDI_PORT_ACTIVE

#ifndef SWAP_SERIAL
#define RXD2 13 /* U2RRXD, D7 */
#define TXD2 15 /* U2RRXD, D0 */
#include <SoftwareSerial.h>
SoftwareSerial Serial2(RXD2, TXD2);
#define MIDI_PORT2_ACTIVE
#endif

#define SAMPLE_RATE 44100
#define SAMPLE_BUFFER_SIZE 48

#endif /* ESP8266 */

/*
 * Configuration for
 * Board: "ESP32 Dev Module" or similar
 */
#ifdef ESP32

#define MEMORY_FROM_HEAP



//#define BOARD_ML_V1 /* activate this when using the ML PCB V1 */
//#define BOARD_ESP32_AUDIO_KIT_AC101 /* activate this when using the ESP32 Audio Kit v2.2 with the AC101 codec */
#define BOARD_ESP32_AUDIO_KIT_ES8388 /* activate this when using the ESP32 Audio Kit v2.2 with the ES8388 codec */
//#define BOARD_ESP32_DOIT /* activate this when using the DOIT ESP32 DEVKIT V1 board */

#define LED_PIN     BLINK_LED_PIN

#define REVERB_ENABLED

#define MAX_DELAY   (SAMPLE_RATE/2)

/* use this to display a scope on the oled display */
//#define OLED_OSC_DISP_ENABLED

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

#endif /* ESP32 */

/*
 * Configuration for
 * Board: "Teensy 4.1"
 *
 * BCK: 21
 * DIN: 7
 * LCK: 20
 */
#ifdef TEENSYDUINO // CORE_TEENSY

#define LED_PIN 13 /* led pin on teensy 4.1 */
#define MIDI_PORT1_ACTIVE
#define MIDI_SERIAL1_BAUDRATE   31250
#define SAMPLE_BUFFER_SIZE AUDIO_BLOCK_SAMPLES
#define SAMPLE_RATE AUDIO_SAMPLE_RATE

#endif /* TEENSYDUINO */

/*
 * Configuration for
 * Board: "Generic STM32H7 Series"
 * Board part number: "Daisy Seed"
 */
#ifdef ARDUINO_DAISY_SEED

#define LED_PIN LED_BUILTIN
#define SAMPLE_BUFFER_SIZE  48
#define SAMPLE_RATE 48000

#define MIDI_PORT2_ACTIVE
#define MIDI_BAUDRATE   31250

#endif /* ARDUINO_DAISY_SEED */

/*
 * Configuration for
 * Board: "Seeeduino XIAO"
 */
#ifdef ARDUINO_SEEED_XIAO_M0

#define LED_PIN LED_BUILTIN
#define SAMPLE_BUFFER_SIZE  48
#define SAMPLE_RATE  22050

#define MIDI_PORT1_ACTIVE

#endif /* ARDUINO_SEEED_XIAO_M0 */

/*
 * Configuration for
 * Board: "Rapsberry Pi Pico"
 *
 * BCK: 26
 * DIN: 28
 * LCK: 27  (always BCK + 1)
 *
 * MIDI_RX: 12 (GP9)
 *
 * Pinout @see https://www.raspberrypi-spy.co.uk/2021/01/pi-pico-pinout-and-power-pins/#prettyPhoto
 */
#ifdef ARDUINO_RASPBERRY_PI_PICO

#define LED_PIN LED_BUILTIN
#define SAMPLE_BUFFER_SIZE  48
#define SAMPLE_RATE  44100

#define MIDI_PORT2_ACTIVE

#endif /* ARDUINO_RASPBERRY_PI_PICO */

/*
 * Configuration for
 * Board: "Generic STM32F4 Series"
 * Board part number: "Generic F407VGTx"
 *
 * does not work at the moment
 */
#ifdef ARDUINO_GENERIC_F407VGTX

#include <ml_boards.h> /* requires the ML_Synth library:  https://github.com/marcel-licence/ML_SynthTools */

#define BLINK_LED_PIN LED_USER_RED
#define LED_PIN LED_USER_GREEN

#define SAMPLE_BUFFER_SIZE  48
#define SAMPLE_RATE  44100

#define MIDI_PORT2_ACTIVE

#endif /* ARDUINO_GENERIC_F407VGTX */


#ifdef ARDUINO_DISCO_F407VG

/*
 * max poly tested: 8 voices
 */

#include <ml_boards.h> /* requires the ML_Synth library:  https://github.com/marcel-licence/ML_SynthTools */

#define BLINK_LED_PIN LED_USER_RED
#define LED_PIN LED_USER_GREEN

#define SAMPLE_BUFFER_SIZE  48
#define SAMPLE_RATE  44100

//#define MIDI_PORT1_ACTIVE /* MIDI in : PIN A10  not tested*/
#define MIDI_PORT2_ACTIVE /* MIDI in : PIN A3 */


#define REVERB_ENABLED
#define REVERB_STATIC

#define MAX_DELAY (44100/10) /* very short but not enough memory for more - mono delay or lo-fi might help */
#define DELAY_STATIC


#endif /* ARDUINO_DISCO_F407VG */


#ifdef ARDUINO_BLACK_F407VE
/*
 * from variant.h
 * LED_BUILTIN: PA6
 * LED1_BUILTIN: PA7
 *
 * USER_BUTTON0: PA0
 * USER_BUTTON1: PE3
 * USER_BUTTON2: PE4
 *
 * SDA: PB7
 * SCL: PB6
 */
#define BLINK_LED_PIN LED_BUILTIN /* PA6 */
#define LED_PIN LED_BUILTIN

#define SAMPLE_BUFFER_SIZE  48
#define SAMPLE_RATE  44100

/*
 * define your I2S interface here!
 * values are just example values and will not work
 */
#define I2S_I2SN    SPI2 // only SPI2 and SPI3 supports I2S
#define I2S_MCLK    PC6 // I2S2_MCK
#define I2S_SCLK    PB10 // or PB13 I2S2_CK
#define I2S_SDIN    PC2 // I2S2_SD or PB14 mcu out -> dac in
#define I2S_LRCK    PB12 // I2S2_WS

#endif /* DARDUINO_BLACK_F407VE */


//#define MIDI_FMT_INT
#ifndef MIDI_BAUDRATE
#define MIDI_BAUDRATE   31250
#endif

#endif /* CONFIG_H_ */

