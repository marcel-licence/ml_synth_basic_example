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
 * @file ml_synth_organ_example.ino
 * @author Marcel Licence
 * @date 21.03.2026
 *
 * @brief This is the main project file which handles the app startup and loop calls
 *        You will find the main code in app.cpp
 */


#include "app.h"


#if (defined ESP32) && (SOC_CPU_CORES_NUM > 1)
void Core0TaskInit(void);
void Core0Task(void *parameter);
#endif

#ifdef ARDUINO_RP2040
volatile bool g_setup_done = false;
#endif


void setup()
{
    Serial.begin(115200);
    App_Setup();

#ifdef ARDUINO_RP2040
    g_setup_done = true;
#endif

#if (defined ESP32) && (SOC_CPU_CORES_NUM > 1)
    Core0TaskInit();
#endif
}

void loop()
{
    App_Loop();
}

#ifdef ARDUINO_RP2040

void wait_until_setup_finished(void)
{
    while (!g_setup_done)
    {
        delay(1);
    }
}

void setup1()
{
    wait_until_setup_finished();
    App_Setup1();
}

void loop1()
{
    App_Loop1();
}
#endif


#if (defined ESP32) && (SOC_CPU_CORES_NUM > 1)
/*
 * Core 0
 */
/* this is used to add a task to core 0 */
TaskHandle_t Core0TaskHnd;

void Core0TaskInit(void)
{
    /* we need a second task for the terminal output */
    xTaskCreatePinnedToCore(Core0Task, "CoreTask0", 8000, NULL, 4, &Core0TaskHnd, 0);
}

void Core0TaskSetup(void)
{
    /*
     * init your stuff for core0 here
     */
    App_Setup1();
}

void Core0TaskLoop(void)
{
    App_Loop1();
}

void Core0Task(void *parameter)
{
    Core0TaskSetup();

    while (true)
    {
        Core0TaskLoop();

        /* this seems necessary to trigger the watchdog */
        delay(1);
        yield();
    }
}
#endif
