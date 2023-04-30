/*
 * notes:
 * - acc calibration should also be done static while system is level (offset x and y)
 * 
 * 
*/
#include <mbed.h>

#include "Param.h"
#include "DebounceIn.h"
#include "SDCardThread.h"
#include "IMUThread.h"
#include "GNSSThread.h"

DebounceIn user_button(PC_13);
void user_button_pressed_fcn();
bool do_close_sd_file = false;
DigitalOut user_led(LED1);

int main(){

    user_button.fall(&user_button_pressed_fcn);

    Data data;
    printf("program Start\n");

    

    SDCardThread sdCardThread(data);
    IMUThread imuThread(data);
    GNSSThread GNSSThread(data);
    
    GNSSThread.StartThread();
    imuThread.StartThread();
    //thread_sleep_for(20);
    //sdCardThread.StartThread();
    
    

    while(true) {

        if (do_close_sd_file) {

            //sdCardThread.CloseFile();
        }
        user_led = !user_led;
        
        thread_sleep_for(500);
    }
}

void user_button_pressed_fcn()
{
    if (!do_close_sd_file) do_close_sd_file = !do_close_sd_file;
}
