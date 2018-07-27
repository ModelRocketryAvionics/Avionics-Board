/*
 * ser.c
 *
 *  Created on: Jul 26, 2018
 *      Author: Michael Graves
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/tm4c123gh6pm.h"

#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

#include "ser.h"

//#############################################//
//                 Serial Defines
//#############################################//
#define SERIAL_BAUDRATE 115200

#define SERIAL_BUFFER_SIZE 16

#define SERIAL_LF 10
#define SERIAL_CR 13

#define SERIAL_IGNORE_CARRIAGE_RETURN 1
#define SERIAL_IGNORE_LINE_FEED 0

//#############################################//
//                Serial Variables
//#############################################//

volatile uint8_t serialBufferIndex = 0;
volatile char SerialBuffer[SERIAL_BUFFER_SIZE];

void (*serialOnCharReceived)(char);
void (*serialOnLineReceived)(volatile char*, volatile char*);

//#############################################//
//                Serial Functions
//#############################################//

/*
 * SerialWrite(character)
 *  puts a character onto the out buffer of UART0
 */
void SerialWrite(char character) {
    UARTCharPut(UART0_BASE, character);
}

/*
 * SerialPrint(string)
 *  Sends a string to UART0 (must be terminated with '\0', default when using "")
 */
void SerialPrint(char string[]) {
    char *character;
    for (character = string; *character != '\0'; character++) {
        SerialWrite(*character);
    }
}

/*
 * SerialPrint(string)
 *  Sends a string to UART0 terminated with \r\n (depending on defines)..
 *   The string must be terminated with '\0', default when using "".
 */
void SerialPrintln(char string[]) {
    SerialPrint(string);
#if SERIAL_IGNORE_CARRIAGE_RETURN == 1
    SerialWrite(SERIAL_CR);
#endif
#if SERIAL_IGNORE_LINE_FEED == 1
    SerialWrite(SERIAL_LF);
#endif
}

void SerialPrintInt(int integer) {
    char buffer[16];
    int length = sprintf(buffer, "%d", integer);

    SerialPrint(buffer);
}

void SerialPrintlnInt(int integer) {
    char buffer[16];
    int length = sprintf(buffer, "%d", integer);

    SerialPrintln(buffer);
}

uint8_t InitSerial(void (*onCharReceived)(char), void (*onLineReceived)(volatile char*, volatile char*)) {
    serialOnCharReceived = onCharReceived;
    serialOnLineReceived = onLineReceived;

    IntMasterDisable();

    //Enable GPIOA Peripheral and wait until it's ready
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

    //Enable UART0 Peripheral and wait until it's ready
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}

    //Configure pins PA0 and PA1 as UART0 pins
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //Configure the Serial Peripheral
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), SERIAL_BAUDRATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //Enable interrupts for Receiving and Receive timeout
    IntMasterEnable();
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    return 0;
}
/*
 * SerialInterruptHandler()
 *  The interrupt handler that receives RX and RT interrupts, ensure this in tm4c1230h6pm_startup_ccs.c
 */
void SerialInterruptHandler(void) {
    //Get the interrupt status
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true);

    //Clear the asserted interrupt
    UARTIntClear(UART0_BASE, ui32Status);

    //Loop while there are characters..
    while(UARTCharsAvail(UART0_BASE)) {
        char character = UARTCharGetNonBlocking(UART0_BASE);

        //Call the OnCharReceived function
        (*serialOnCharReceived)(character);

        //End of Line condition (\r\n or \n\r or \r or \n)
#if (SERIAL_IGNORE_LINE_FEED == 0 && SERIAL_IGNORE_CARRIAGE_RETURN == 0)
        //Line Feed (0xA) + Carriage Return (0xD)
        if (character == SERIAL_LF) {
            if(serialBufferIndex > 0 && SerialBuffer[serialBufferIndex - 1] == SERIAL_CR) {
                // End of Line (\r\n)

                // minus 1 to remove the previous EOL character
                (*serialOnLineReceived)(SerialBuffer, SerialBuffer + (serialBufferIndex - 1));
            } else {
                SerialBuffer[serialBufferIndex] = character;
                serialBufferIndex++;
            }

        } else if(character == SERIAL_CR) {
            if(serialBufferIndex > 0 && SerialBuffer[serialBufferIndex - 1] == SERIAL_LF) {
                // End of Line (\n\r)

                // minus 1 to remove the previous EOL character
                (*serialOnLineReceived)(SerialBuffer, SerialBuffer + (serialBufferIndex - 1));
            } else {
                SerialBuffer[serialBufferIndex] = character;
                serialBufferIndex++;
            }
        }
#elif SERIAL_IGNORE_CARRIAGE_RETURN == 0
        //Carriage Return (0xD)
        if(character == SERIAL_LF) {
            //End Of Line
            (*serialOnLineReceived)(SerialBuffer, SerialBuffer + serialBufferIndex);
            serialBufferIndex = 0;
        } else if (character != SERIAL_CR) {
            SerialBuffer[serialBufferIndex] = character;
            serialBufferIndex++;
        }
#elif SERIAL_IGNORE_LINE_FEED == 0
        //Line Feed (0xA)
        if(character == SERIAL_CR) {
            //End Of Line
            (*serialOnLineReceived)(SerialBuffer, SerialBuffer + serialBufferIndex);
            serialBufferIndex = 0;
        } else if (character != SERIAL_LF) {
            SerialBuffer[serialBufferIndex] = character;
            serialBufferIndex++;
        }
#endif
    }
}
