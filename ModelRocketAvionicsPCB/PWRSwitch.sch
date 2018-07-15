EESchema Schematic File Version 2
LIBS:AvionicsBoard-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:tm4c123gh6pm
LIBS:lis331
LIBS:l3gd20htr
LIBS:mpl3115a2
LIBS:css-1210tb
LIBS:AvionicsBoard-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L DMG1029SV-7 U10
U 1 1 5B4A3342
P 5900 4275
F 0 "U10" H 5900 4025 60  0000 C CNN
F 1 "DMG1029SV-7" H 5900 4525 60  0000 C CNN
F 2 "AvionicsIC:SOT563" H 5900 4275 60  0001 C CNN
F 3 "" H 5900 4275 60  0001 C CNN
F 4 "DMP2004VKDICT-ND" H 5900 4275 60  0001 C CNN "Digi-Key Part Number"
	1    5900 4275
	1    0    0    -1  
$EndComp
$Comp
L CSS-1210TB U11
U 1 1 5B4A3383
P 6025 2075
F 0 "U11" H 6075 2075 60  0000 C CNN
F 1 "CSS-1210TB" H 6025 2325 60  0000 C CNN
F 2 "Avionics:CSS-1210TB" H 6025 2075 60  0001 C CNN
F 3 "" H 6025 2075 60  0001 C CNN
F 4 "563-1091-1-ND" H 6025 2075 60  0001 C CNN "Digi-Key Part Number"
	1    6025 2075
	1    0    0    -1  
$EndComp
Text HLabel 6300 4125 2    60   Input ~ 0
+VBAT
Text HLabel 5500 4125 0    60   Output ~ 0
+VBAT_ON
Text GLabel 5500 4275 0    60   Input ~ 0
PWR_SWITCH
Text GLabel 5625 2225 0    60   Input ~ 0
PWR_SWITCH
Text GLabel 5625 1925 0    60   Input ~ 0
+VBAT
$Comp
L GND #PWR053
U 1 1 5B4A341F
P 5625 2075
F 0 "#PWR053" H 5625 1825 50  0001 C CNN
F 1 "GND" H 5625 1925 50  0000 C CNN
F 2 "" H 5625 2075 50  0001 C CNN
F 3 "" H 5625 2075 50  0001 C CNN
	1    5625 2075
	0    1    1    0   
$EndComp
Wire Wire Line
	5500 4125 5550 4125
Wire Wire Line
	5500 4275 5550 4275
Wire Wire Line
	6300 4125 6250 4125
Wire Wire Line
	5625 1925 5675 1925
Wire Wire Line
	5675 2075 5625 2075
Wire Wire Line
	5625 2225 5675 2225
Wire Notes Line
	525  675  11150 675 
Wire Notes Line
	11150 675  11150 3225
Wire Notes Line
	11150 3225 525  3225
Wire Notes Line
	525  3225 525  675 
Wire Notes Line
	525  3250 11150 3250
Wire Notes Line
	11150 3250 11150 5600
Wire Notes Line
	11150 5600 525  5600
Wire Notes Line
	525  5600 525  3250
Text Notes 5225 825  0    60   ~ 0
Power Switch - Slide Switch
Text Notes 5050 3450 0    60   ~ 0
High Power Switching P-MOSFET
$Comp
L GND #PWR054
U 1 1 5B4A8B23
P 5500 4425
F 0 "#PWR054" H 5500 4175 50  0001 C CNN
F 1 "GND" H 5500 4275 50  0000 C CNN
F 2 "" H 5500 4425 50  0001 C CNN
F 3 "" H 5500 4425 50  0001 C CNN
	1    5500 4425
	0    1    1    0   
$EndComp
Wire Wire Line
	5550 4425 5500 4425
$Comp
L GND #PWR055
U 1 1 5B4A8B63
P 6325 4350
F 0 "#PWR055" H 6325 4100 50  0001 C CNN
F 1 "GND" H 6325 4200 50  0000 C CNN
F 2 "" H 6325 4350 50  0001 C CNN
F 3 "" H 6325 4350 50  0001 C CNN
	1    6325 4350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6250 4275 6325 4275
Wire Wire Line
	6325 4275 6325 4425
Wire Wire Line
	6325 4425 6250 4425
Connection ~ 6325 4350
$EndSCHEMATC
