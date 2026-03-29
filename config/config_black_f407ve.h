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
 * @file config_black_f407ve.h
 * @author Marcel Licence
 *
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
 *
 * @see http://wiki.stm32duino.com/images/5/5c/STM32_F4VE_SCHEMATIC.PDF
 */


#ifdef ARDUINO_BLACK_F407VE
#define BLINK_LED_PIN LED_BUILTIN /* PA6 */
#define LED_PIN LED_BUILTIN

#define SAMPLE_BUFFER_SIZE  48
#define SAMPLE_RATE  44100

/*
 * define your I2S interface here!
 * values are just example values and will not work
 */
#if 1
#define I2S_I2SN    SPI2 // only SPI2 and SPI3 supports I2S
#define I2S_MCLK    PC6 // I2S2_MCK
#define I2S_SCLK    PB10 // or PB13 I2S2_CK
#define I2S_SDIN    PC2 // I2S2_SD or PB14 mcu out -> dac in
#define I2S_LRCK    PB12 // I2S2_WS
#else
#define I2S_I2SN    SPI1 // Using SPI1 for I2S
#define I2S_MCLK    PC7 // I2S1_MCK
#define I2S_SCLK    PC10 // I2S1_CK
#define I2S_SDIN    PC12 // I2S1_SD mcu out -> dac in
#define I2S_LRCK    PA4 // I2S1_WS
#endif


#endif /* DARDUINO_BLACK_F407VE */

