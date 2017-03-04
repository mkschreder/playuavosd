EESchema Schematic File Version 2
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
LIBS:special
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
LIBS:74xgxx
LIBS:ac-dc
LIBS:actel
LIBS:allegro
LIBS:Altera
LIBS:analog_devices
LIBS:battery_management
LIBS:bbd
LIBS:bosch
LIBS:brooktre
LIBS:cmos_ieee
LIBS:dc-dc
LIBS:diode
LIBS:elec-unifil
LIBS:ESD_Protection
LIBS:ftdi
LIBS:gennum
LIBS:graphic
LIBS:hc11
LIBS:ir
LIBS:Lattice
LIBS:leds
LIBS:logo
LIBS:maxim
LIBS:mechanical
LIBS:microchip_dspic33dsc
LIBS:microchip_pic10mcu
LIBS:microchip_pic12mcu
LIBS:microchip_pic16mcu
LIBS:microchip_pic18mcu
LIBS:microchip_pic24mcu
LIBS:microchip_pic32mcu
LIBS:modules
LIBS:motor_drivers
LIBS:motors
LIBS:msp430
LIBS:nordicsemi
LIBS:nxp
LIBS:nxp_armmcu
LIBS:onsemi
LIBS:Oscillators
LIBS:powerint
LIBS:Power_Management
LIBS:pspice
LIBS:references
LIBS:relays
LIBS:rfcom
LIBS:sensors
LIBS:silabs
LIBS:stm8
LIBS:stm32
LIBS:supertex
LIBS:switches
LIBS:transf
LIBS:triac_thyristor
LIBS:ttl_ieee
LIBS:video
LIBS:wiznet
LIBS:Worldsemi
LIBS:Xicor
LIBS:zetex
LIBS:Zilog
LIBS:crystal
LIBS:martin.schroder
LIBS:playuav-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 4
Title ""
Date "4 mar 2017"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L SPX3819 U6
U 1 1 58BB2EBC
P 5900 4250
F 0 "U6" H 5900 4050 60  0000 C CNN
F 1 "SPX3819" H 5900 3950 60  0000 C CNN
F 2 "SOIC8" H 5900 3850 60  0000 C CNN
F 3 "~" H 6050 4000 60  0000 C CNN
	1    5900 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 4250 5000 4250
Wire Wire Line
	4500 4150 5350 4150
Wire Wire Line
	5350 4350 4650 4350
Connection ~ 4650 4350
Wire Wire Line
	4650 3500 4650 4400
$Comp
L C C21
U 1 1 58BB2EDF
P 4650 4600
F 0 "C21" H 4650 4700 40  0000 L CNN
F 1 "10uF" H 4656 4515 40  0000 L CNN
F 2 "~" H 4688 4450 30  0000 C CNN
F 3 "~" H 4650 4600 60  0000 C CNN
	1    4650 4600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR016
U 1 1 58BB2EF8
P 4650 5000
F 0 "#PWR016" H 4650 5000 30  0001 C CNN
F 1 "GND" H 4650 4930 30  0001 C CNN
F 2 "" H 4650 5000 60  0000 C CNN
F 3 "" H 4650 5000 60  0000 C CNN
	1    4650 5000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR017
U 1 1 58BB2F07
P 5000 4250
F 0 "#PWR017" H 5000 4250 30  0001 C CNN
F 1 "GND" H 5000 4180 30  0001 C CNN
F 2 "" H 5000 4250 60  0000 C CNN
F 3 "" H 5000 4250 60  0000 C CNN
	1    5000 4250
	0    1    1    0   
$EndComp
Connection ~ 4650 4150
Text Label 4500 4150 0    60   ~ 0
5-16V
Text HLabel 4500 4150 0    60   Input ~ 0
5-16V
Wire Wire Line
	6450 4150 6800 4150
NoConn ~ 6450 4350
Text Label 6800 4150 0    60   ~ 0
3.3V
$Comp
L SPX3819 U5
U 1 1 58BB3053
P 5900 3600
F 0 "U5" H 5900 3400 60  0000 C CNN
F 1 "SPX3819" H 5900 3300 60  0000 C CNN
F 2 "SOIC8" H 5900 3850 60  0000 C CNN
F 3 "~" H 6050 3350 60  0000 C CNN
	1    5900 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 3500 6800 3500
NoConn ~ 6450 3700
$Comp
L C C22
U 1 1 58BB3064
P 6650 3750
F 0 "C22" H 6650 3850 40  0000 L CNN
F 1 "10uF" H 6656 3665 40  0000 L CNN
F 2 "~" H 6688 3600 30  0000 C CNN
F 3 "~" H 6650 3750 60  0000 C CNN
	1    6650 3750
	1    0    0    -1  
$EndComp
$Comp
L C C23
U 1 1 58BB3073
P 6650 4400
F 0 "C23" H 6650 4500 40  0000 L CNN
F 1 "10uF" H 6656 4315 40  0000 L CNN
F 2 "~" H 6688 4250 30  0000 C CNN
F 3 "~" H 6650 4400 60  0000 C CNN
	1    6650 4400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 58BB3082
P 6650 4000
F 0 "#PWR018" H 6650 4000 30  0001 C CNN
F 1 "GND" H 6650 3930 30  0001 C CNN
F 2 "" H 6650 4000 60  0000 C CNN
F 3 "" H 6650 4000 60  0000 C CNN
	1    6650 4000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR019
U 1 1 58BB3091
P 6650 4650
F 0 "#PWR019" H 6650 4650 30  0001 C CNN
F 1 "GND" H 6650 4580 30  0001 C CNN
F 2 "" H 6650 4650 60  0000 C CNN
F 3 "" H 6650 4650 60  0000 C CNN
	1    6650 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 3500 5350 3500
Wire Wire Line
	4650 3700 5350 3700
Connection ~ 4650 3700
Wire Wire Line
	5350 3600 5000 3600
$Comp
L GND #PWR020
U 1 1 58BB30CF
P 5000 3600
F 0 "#PWR020" H 5000 3600 30  0001 C CNN
F 1 "GND" H 5000 3530 30  0001 C CNN
F 2 "" H 5000 3600 60  0000 C CNN
F 3 "" H 5000 3600 60  0000 C CNN
	1    5000 3600
	0    1    1    0   
$EndComp
Text HLabel 6800 3500 2    60   Input ~ 0
5V_OUT
Text HLabel 6800 4150 2    60   Input ~ 0
3.3V_OUT
Wire Wire Line
	4650 4800 4650 5000
Wire Wire Line
	4650 4900 4500 4900
Connection ~ 4650 4900
Text HLabel 4500 4900 0    60   Input ~ 0
GND
Wire Wire Line
	6650 3550 6650 3500
Connection ~ 6650 3500
Wire Wire Line
	6650 3950 6650 4000
Wire Wire Line
	6650 4600 6650 4650
Wire Wire Line
	6650 4200 6650 4150
Connection ~ 6650 4150
$EndSCHEMATC
