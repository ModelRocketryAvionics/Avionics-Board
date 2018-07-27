#include "stdint.h"
#include "stdbool.h"

//#include "inc/hw_types.h"
//#include "inc/hw_memmap.h"

#include "sch.h"
#include "ser.h"

//#############################################//
//                    Defines
//#############################################//



//#############################################//
//                   Variables
//#############################################//

extern uint32_t currentTime;

//#############################################//
//                 User Functions
//#############################################//

uint8_t Task_StatusUpdate_Init(void) {
    //Nothing to initialize
    return 0;
}

_Bool Task_StatusUpdate_Exec(uint8_t taskID){
    //Super basic taskExample, send the current time every second
    SerialPrintlnInt(currentTime);

    // Returns true to set the task back to IDLE state
    return true;
}

/*
 * SerialOnCharReceived(character)
 *  Function to handle received character events over serial
 * This function is called in an interrupt, keep its exec time really short or use..
 *  other non-interrupt functions.
 */
void SerialOnCharReceived(char character) {
    //Echo the character
    SerialWrite(character);
}

/*
 * SerialOnCharReceived(character)
 *  Function to handle received line events over serial configure the line endings..
 *   in the #define section found in ser.c.
 *  This function is called in an interrupt, keep its exec time really short or use..
 *   other non-interrupt functions.
 */
void SerialOnLineReceived(volatile char* stringStart, volatile char* stringEnd) {
    //Do nothing.. Below snippet is provided as an example

    /*
    uint8_t length = stringEnd - stringStart;

    char *character;
    for (character = stringStart; character < stringEnd; character++) {
        //Iterate through each character (useful if you want to do command parsing)..
    }
    */
}

//#############################################//
//                     Setup()
//#############################################//
void Setup(void) {
    // Add peripheral initializations here
    InitScheduler();
    InitSerial(&SerialOnCharReceived, &SerialOnLineReceived);

    // Add Tasks Here
    AddTaskTime(&Task_StatusUpdate_Init, &Task_StatusUpdate_Exec, 1000);

    //
}


//#############################################//
//                     Loop()
//#############################################//
void Loop(void) {
    UpdateScheduler();
}


//#############################################//
//                  Main Function
//#############################################//
/*  You shouldn't need to edit anything here as..
 *   its brought out to Setup() and Loop().
 */
int main(void) {
    Setup();
    while(1){
        Loop();
    }
}
