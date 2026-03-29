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
 * @file config_esp8266.h
 * @author Marcel Licence
 *
 * @brief Configuration for
 *          Board: "LOLIN(WEMOS) D1 R2 & mini 2 or similar
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
