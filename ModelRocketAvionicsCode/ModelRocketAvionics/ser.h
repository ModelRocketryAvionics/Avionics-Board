/*
 * ser.h
 *
 *  Created on: Jul 26, 2018
 *      Author: Michael Graves
 */

#ifndef SER_H_
#define SER_H_

//#############################################//
//                Serial Functions
//#############################################//
void SerialWrite(char character);
void SerialPrint(char string[]);
void SerialPrintln(char string[]);
void SerialPrintInt(int integer);
void SerialPrintlnInt(int integer);

uint8_t InitSerial(void (*serialOnCharReceived)(char), void (*serialOnLineReceived)(volatile char*, volatile char*));
void SerialInterruptHandler(void);

#endif /* SER_H_ */
