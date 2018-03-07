/*
 * BeagleScopeJoystic.cpp
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL I
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include "SimpleGPIO.h"
using namespace std;

// controller_pins: pinmux_controller_pins{ //pin mode settings
//    pinctrl-single,pins = <
//    0x040 0x37 // P9_15 = Controller UP     = GPIO1_16  = pin  48  = 1 * 32 + 16
//    0x15C 0x37 // P9_17 = Controller DOWN   = GPIO0_5   = pin  5   = 0 * 32 + 5
//    0x154 0x37 // P9_21 = Controller LEFT   = GPIO0_3   = pin  3   = 0 * 32 + 3
//    0x1A4 0x37 // P9_27 = Controller RIGHT  = GPIO3_19  = pin  115 = 3 * 32 + 19
//    0x180 0x37 // P9_26 = Controller BUTTON = GPIO0_14  = pin  14  = 0 * 32 + 14
//
//    // OUTPUT GPIO(mode7) 0x07 pulldown, 0x17 pullup, 0x?f no pullup/down
//    // INPUT  GPIO(mode7) 0x27 pulldown, 0x37 pullup, 0x?f no pullup/down
//    >;

unsigned int joystick_Up     = 48 ;   // GPIO1_16 = ( 1 * 32 ) + 16 = 48
unsigned int joystick_Down   = 5  ;   // GPIO0_5  = ( 0 * 32 ) + 5  = 5
unsigned int joystick_Left   = 3  ;   // GPIO0_3  = ( 0 * 32 ) + 3  = 3
// unsigned int joystick_Right  = 115;   // GPIO3_19 = ( 3 * 32 ) + 19 = 115  // this pin did not work
unsigned int joystick_Right  = 66 ;   // GPIO2_2 = ( 2 * 32 ) + 2 = 66
unsigned int joystick_Button = 14 ;   // GPIO0_14 = ( 0 * 32 ) + 14 = 14

int main(int argc, char *argv[]){

   // Start message
   printf ("==========================================================================\n");
   printf ("\tThe BeagleScopeJoystic STARTS NOW!!!!\n");
   printf ("--------------------------------------------------------------------------\n");

   // Unexport the pins, necessary if pins are already exported
   gpio_unexport(joystick_Up    );             /*gpio_unexport(48 );*/
   gpio_unexport(joystick_Down  );             /*gpio_unexport(5  );*/
   gpio_unexport(joystick_Left  );             /*gpio_unexport(3  );*/
   gpio_unexport(joystick_Right );             /*gpio_unexport(115);*/
   gpio_unexport(joystick_Button);             /*gpio_unexport(14 );*/

   // Now export the pins, if they are already exported there will be a problem
   gpio_export(joystick_Up    );     // Export the pin to the file structure
   gpio_export(joystick_Down  );     //
   gpio_export(joystick_Left  );     //
   gpio_export(joystick_Right );     //
   gpio_export(joystick_Button);     //
   gpio_set_dir(joystick_Up,     INPUT_PIN);  // Set direction to input
   gpio_set_dir(joystick_Down,   INPUT_PIN);  //
   gpio_set_dir(joystick_Left,   INPUT_PIN);  //
   gpio_set_dir(joystick_Right,  INPUT_PIN);  //
   gpio_set_dir(joystick_Button, INPUT_PIN);  //

   // Values for joystic and button
   unsigned int joystick_value_Up          = HIGH;
   unsigned int joystick_value_Down        = HIGH;
   unsigned int joystick_value_Left        = HIGH;
   unsigned int joystick_value_Right       = HIGH;
   unsigned int joystick_value_Button      = HIGH;

   // Flags for joystic and button
   unsigned int joystick_value_Up_Flag     = LOW;
   unsigned int joystick_value_Down_Flag   = LOW;
   unsigned int joystick_value_Left_Flag   = LOW;
   unsigned int joystick_value_Right_Flag  = LOW;
   unsigned int joystick_value_Button_Flag = LOW;
   unsigned int loopLock = HIGH;

   int systemRet = 0;

   // Program loop start
   while(loopLock != LOW ){
      // Get value of joystic and button
      gpio_get_value(joystick_Up    , &joystick_value_Up    );
      gpio_get_value(joystick_Down  , &joystick_value_Down  );
      gpio_get_value(joystick_Left  , &joystick_value_Left  );
      gpio_get_value(joystick_Right , &joystick_value_Right );
      gpio_get_value(joystick_Button, &joystick_value_Button);

      // Up controls Y+ and Z+ if button toggled.
      if (joystick_value_Up == LOW || joystick_value_Up_Flag == HIGH) {
         if (joystick_value_Button_Flag == LOW){ // If button toggled, move the Z
            if (joystick_value_Up_Flag == LOW){
               joystick_value_Up_Flag = HIGH;
               printf ("joystick_value_Up + Start\tY+\n");
               systemRet = system("echo G91 > /dev/octoprint_1; echo G1 Y5 F3000 > /dev/octoprint_1;");
               if(systemRet == -1){
                  // The system method failed
               }
               usleep(100000);
            } else {
               joystick_value_Up_Flag = LOW;
               printf ("joystick_value_Up + Stop\n");
               usleep(100000);
            }
            if (joystick_value_Up == LOW && joystick_value_Up_Flag == HIGH) {
               printf ("joystick_value_Up + Repeat\n");
               usleep(100000);
            }
         }
         if (joystick_value_Button_Flag == HIGH){
            if (joystick_value_Up_Flag == LOW){
               joystick_value_Up_Flag = HIGH;
               printf ("joystick_value_Up + Start\tZ+\n");
               systemRet = system("echo G91 > /dev/octoprint_1; echo G1 Z5 F3000 > /dev/octoprint_1;");
               if(systemRet == -1){
                  // The system method failed
               }
               usleep(100000);
            } else {
               joystick_value_Up_Flag = LOW;
               printf ("joystick_value_Up + Stop\n");
               usleep(100000);
            }
            if (joystick_value_Up == LOW && joystick_value_Up_Flag == HIGH) {
               printf ("joystick_value_Up + Repeat\n");
               usleep(100000);
            }
         }
      }

      // Down controls Y- and Z- if button toggled.
      if (joystick_value_Down == LOW || joystick_value_Down_Flag == HIGH) {
         if (joystick_value_Button_Flag == LOW){
            if (joystick_value_Down_Flag == LOW){
               joystick_value_Down_Flag = HIGH;
               printf ("joystick_value_Down + Start\tY-\n");
               systemRet = system("echo G91 > /dev/octoprint_1; echo G1 Y-5 F3000 > /dev/octoprint_1; ");
               if(systemRet == -1){
                  // The system method failed
               }
               usleep(100000);
            } else {
               joystick_value_Down_Flag = LOW;
               printf ("joystick_value_Down - Stop\n");
               usleep(100000);
            }
            if (joystick_value_Down == LOW && joystick_value_Down_Flag == HIGH) {
               printf ("joystick_value_Down - Repeat\n");
               usleep(100000);
            }
         }
         if (joystick_value_Button_Flag == HIGH){
            if (joystick_value_Down == LOW){
               joystick_value_Down_Flag = HIGH;
               printf ("joystick_value_Down + Start\tZ-\n");
               systemRet = system("echo G91 > /dev/octoprint_1; echo G1 Z-5 F3000 > /dev/octoprint_1;");
               if(systemRet == -1){
                  // The system method failed
               }
               usleep(100000);
            } else {
               joystick_value_Down_Flag = LOW;
               printf ("joystick_value_Down + Stop\n");
               usleep(100000);
            }
            if (joystick_value_Down == LOW && joystick_value_Down_Flag == HIGH) {
               printf ("joystick_value_Down + Repeat\n");
               usleep(100000);
            }
         }
      }

      // Left controls X- and E- if button toggled.
      if (joystick_value_Left == LOW || joystick_value_Left_Flag == HIGH) {
         if (joystick_value_Button_Flag == LOW){
            if (joystick_value_Left_Flag == LOW){
               joystick_value_Left_Flag = HIGH;
               printf ("joystick_value_Left + Start\tX-\n");
               systemRet = system("echo G91 > /dev/octoprint_1; echo G1 X-5 F3000 > /dev/octoprint_1; ");
               if(systemRet == -1){
                  // The system method failed
               }
               usleep(100000);
            } else {
               joystick_value_Left_Flag = LOW;
               printf ("joystick_value_Left - Stop\n");
               usleep(100000);
            }
            if (joystick_value_Left == LOW && joystick_value_Left_Flag == HIGH) {
               printf ("joystick_value_Left - Repeat\n");
               usleep(100000);
            }
         }
         if (joystick_value_Button_Flag == HIGH){
            if (joystick_value_Left_Flag == LOW){
               joystick_value_Left_Flag = HIGH;
               printf ("joystick_value_Left + Start\tE-\n");
               systemRet = system("echo G91 > /dev/octoprint_1; echo G1 E-5 F3000 > /dev/octoprint_1;");
               if(systemRet == -1){
                  // The system method failed
               }
               usleep(100000);
            } else {
               joystick_value_Left_Flag = LOW;
               printf ("joystick_value_Left + Stop\n");
               usleep(100000);
            }
            if (joystick_value_Left == LOW && joystick_value_Left_Flag == HIGH) {
               printf ("joystick_value_Left + Repeat\n");
               usleep(100000);
            }
         }
      }

      // Left controls X+ and E+ if button toggled.
      if (joystick_value_Right == LOW || joystick_value_Right_Flag == HIGH) {
         if (joystick_value_Button_Flag == LOW){
            if (joystick_value_Right_Flag == LOW){
               joystick_value_Right_Flag = HIGH;
               printf ("joystick_value_Right + Start\tX+\n");
               systemRet = system("echo G91 > /dev/octoprint_1; echo G1 X5 F3000 > /dev/octoprint_1; ");
               if(systemRet == -1){
                  // The system method failed
               }
               usleep(100000);
            } else {
               joystick_value_Right_Flag = LOW;
               printf ("joystick_value_Right - Stop\n");
               usleep(100000);
            }
            if (joystick_value_Right == LOW && joystick_value_Right_Flag == HIGH) {
               printf ("joystick_value_Right - Repeat\n");
               usleep(100000);
            }
         }
         if (joystick_value_Button_Flag == HIGH){
            if (joystick_value_Right_Flag == LOW){
               joystick_value_Right_Flag = HIGH;
               printf ("joystick_value_Right + Start\tE+\n");
               systemRet = system("echo G91 > /dev/octoprint_1; echo G1 E5 F3000 > /dev/octoprint_1;");
               if(systemRet == -1){
                  // The system method failed
               }
               usleep(100000);
            } else {
               joystick_value_Right_Flag = LOW;
               printf ("joystick_value_Right + Stop\n");
               usleep(100000);
            }
            if (joystick_value_Right == LOW && joystick_value_Right_Flag == HIGH) {
               printf ("joystick_value_Right + Repeat\n");
               usleep(100000);
            }
         }
      }

      // Button toggles joystick flag to switch control
      if (joystick_value_Button == LOW){
         joystick_value_Button_Flag = !joystick_value_Button_Flag;
         printf ("joystick_value_Button_Flag = %d and stop motors\n", joystick_value_Button_Flag);
         systemRet = system("echo M18 > /dev/octoprint_1;");
         if(systemRet == -1){
            // The system method failed
         }
         if (joystick_value_Button_Flag == LOW){
            printf ("joystick_Up/Down = control Z\n");
         }
         if (joystick_value_Button_Flag == HIGH){
            printf ("joystick_Up/Down = control Y\n");
         }
         usleep(1000000);
      }
   }

   // The while loop never breaks, but to properly exit the program this needs to happen
   printf ("--------------------------------------------------------------------------\n");
   printf ("\tTime to unexport the GPIO Pins\n");
   gpio_unexport(joystick_Up    );             /*gpio_unexport(48 );*/
   gpio_unexport(joystick_Down  );             /*gpio_unexport(5  );*/
   gpio_unexport(joystick_Left  );             /*gpio_unexport(3  );*/
   gpio_unexport(joystick_Right );             /*gpio_unexport(115);*/
   gpio_unexport(joystick_Button);             /*gpio_unexport(14 );*/

   // Start message
   printf ("--------------------------------------------------------------------------\n");
   printf ("\tThe BeagleScopeJoystic ENDS NOW!!!!\n");
   printf ("==========================================================================\n");

   return 0;
}



// notes:
//    echo M18 > /dev/octoprint_1
//    echo G91 > /dev/octoprint_1
//    echo M18 > /dev/octoprint_1
//    echo G91 > /dev/octoprint_1
//    echo G1 X-1 F6000 > /dev/octoprint_1
//    echo G0 > /dev/octoprint_1