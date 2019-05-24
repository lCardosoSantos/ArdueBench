/*
Written by Luan Cardoso dos Santos. May 2019 - Luxembourg

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>

#include <sam3x8e.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "cycleCount.h"

//Init is defined in sam3x8e.h, and we need to call it in main for booting-up the Cortex-M3 device.
extern void init(void);

void setup(){
    //use this code if you need to change the operating frequency and waitstates of the arduino due board. Be advised, though, that this may introduce incompatibilities and the board may outright not work.
    //#define WAITSTATE0
    #ifdef WAITSTATE0

    #warning "You are compiling code with different wait states. Might be unstable! Baudrate changes!"

    #define SYS_BOARD_OSCOUNT   (CKGR_MOR_MOSCXTST(0x8UL))

    // System internal clock = 6 * MULA + 6. baudrate will scale from 115200 @ 84 MHz. Change it accordingly on arm_serial_terminal.py
    /*
    | R_PLLAR_MULA | Clock value | Baudrate |
    | ------------:| -----------:| --------:|
    |            1 |      12 MHz |    16457 |
    |            2 |      18 MHz |    24686 |
    |            3 |      24 MHz |    32914 |
    |            4 |      30 MHz |    41143 |
    |            5 |      36 MHz |    49371 |
    |            6 |      42 MHz |    57600 |
    |            7 |      48 MHz |    65829 |
    |            8 |      54 MHz |    74057 |
    |            9 |      60 MHz |    82286 |
    |           10 |      66 MHz |    90514 |
    |           11 |      72 MHz |    98743 |
    |           12 |      78 MHz |   106971 |
    |           13 |      84 MHz |   115200 |
    |           14 |      90 MHz |   123429 |
    |           15 |      96 MHz |   131657 |
    |           16 |     102 MHz |   139886 |
    |           17 |     108 MHz |   148114 |
    |           18 |     114 MHz |   156343 |
    */
    #define SYS_BOARD_PLLAR     (CKGR_PLLAR_ONE \
                                 | CKGR_PLLAR_MULA(1ul) \
                                 | CKGR_PLLAR_PLLACOUNT(0x3ful) \
                                 | CKGR_PLLAR_DIVA(0x1UL))
    #define SYS_BOARD_MCKR      ( PMC_MCKR_PRES_CLK_2|PMC_MCKR_CSS_PLLA_CLK)

      /* Set FWS according to SYS_BOARD_MCKR configuration */
      EFC0->EEFC_FMR = EEFC_FMR_FWS(3);
      EFC1->EEFC_FMR = EEFC_FMR_FWS(3);

      /* Initialize PLLA*/
      PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
      while (!(PMC->PMC_SR & PMC_SR_LOCKA));

      PMC->PMC_MCKR = SYS_BOARD_MCKR;
      while (!(PMC->PMC_SR & PMC_SR_MCKRDY));

      SystemCoreClockUpdate(); // re-synchronize the USB with the new clock speed

    #endif
}

uint32_t cycleCount(uint32_t n){
    uint32_t t=1;

    CYCLE_COUNT_INIT;
    CYCLE_COUNT_START;

    for(uint32_t i=0; i<n; i++)
        t*=(t+i);

    CYCLE_COUNT_STOP;

    return CYCLE_COUNT_ELAPSED;

}

int main(){
    init();
    //setup();

    //parameters
    printf("This is a very simple demonstration on how to upload code to the arduino due. \n");
    printf("cycleCount.h can be used for timing functions, but beware of the processor's waitstates. \n");
    //all printf statements should finish with a \n character, or else the terminal wont show it.
    printf("Here is an exemple of cycle counter:\n");
    printf("%d cycles to multiply all positive integers less than 10000\n", cycleCount(10000));
    printf("But remember that the Systick value is corrected for the measument overhead");
    return 0;
}
