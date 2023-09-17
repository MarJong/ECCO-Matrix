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
LIBS:matrix - stromanschluss-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "10 aug 2013"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_10X2 P1
U 1 1 520644D9
P 4050 2150
F 0 "P1" H 4050 2700 60  0000 C CNN
F 1 "CONN_10X2" V 4050 2050 50  0000 C CNN
F 2 "" H 4050 2150 60  0000 C CNN
F 3 "" H 4050 2150 60  0000 C CNN
	1    4050 2150
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR1
U 1 1 520644E8
P 3550 1700
F 0 "#PWR1" H 3550 1650 20  0001 C CNN
F 1 "+12V" H 3550 1800 30  0000 C CNN
F 2 "" H 3550 1700 60  0000 C CNN
F 3 "" H 3550 1700 60  0000 C CNN
	1    3550 1700
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR9
U 1 1 520644F8
P 4550 1700
F 0 "#PWR9" H 4550 1790 20  0001 C CNN
F 1 "+5V" H 4550 1790 30  0000 C CNN
F 2 "" H 4550 1700 60  0000 C CNN
F 3 "" H 4550 1700 60  0000 C CNN
	1    4550 1700
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR10
U 1 1 5206450F
P 4550 1800
F 0 "#PWR10" H 4550 1890 20  0001 C CNN
F 1 "+5V" H 4550 1890 30  0000 C CNN
F 2 "" H 4550 1800 60  0000 C CNN
F 3 "" H 4550 1800 60  0000 C CNN
	1    4550 1800
	0    1    1    0   
$EndComp
NoConn ~ 4450 1900
NoConn ~ 3650 1800
NoConn ~ 3650 1900
$Comp
L +5V #PWR3
U 1 1 52064516
P 3550 2100
F 0 "#PWR3" H 3550 2190 20  0001 C CNN
F 1 "+5V" H 3550 2190 30  0000 C CNN
F 2 "" H 3550 2100 60  0000 C CNN
F 3 "" H 3550 2100 60  0000 C CNN
	1    3550 2100
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR5
U 1 1 5206451C
P 3550 2300
F 0 "#PWR5" H 3550 2390 20  0001 C CNN
F 1 "+5V" H 3550 2390 30  0000 C CNN
F 2 "" H 3550 2300 60  0000 C CNN
F 3 "" H 3550 2300 60  0000 C CNN
	1    3550 2300
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR2
U 1 1 52064524
P 3550 2000
F 0 "#PWR2" H 3550 2000 30  0001 C CNN
F 1 "GND" H 3550 1930 30  0001 C CNN
F 2 "" H 3550 2000 60  0000 C CNN
F 3 "" H 3550 2000 60  0000 C CNN
	1    3550 2000
	0    1    1    0   
$EndComp
$Comp
L GND #PWR4
U 1 1 52064531
P 3550 2200
F 0 "#PWR4" H 3550 2200 30  0001 C CNN
F 1 "GND" H 3550 2130 30  0001 C CNN
F 2 "" H 3550 2200 60  0000 C CNN
F 3 "" H 3550 2200 60  0000 C CNN
	1    3550 2200
	0    1    1    0   
$EndComp
$Comp
L GND #PWR6
U 1 1 52064537
P 3550 2400
F 0 "#PWR6" H 3550 2400 30  0001 C CNN
F 1 "GND" H 3550 2330 30  0001 C CNN
F 2 "" H 3550 2400 60  0000 C CNN
F 3 "" H 3550 2400 60  0000 C CNN
	1    3550 2400
	0    1    1    0   
$EndComp
$Comp
L +3,3V #PWR7
U 1 1 5206453F
P 3550 2500
F 0 "#PWR7" H 3550 2460 30  0001 C CNN
F 1 "+3,3V" H 3550 2610 30  0000 C CNN
F 2 "" H 3550 2500 60  0000 C CNN
F 3 "" H 3550 2500 60  0000 C CNN
	1    3550 2500
	0    -1   -1   0   
$EndComp
$Comp
L +3,3V #PWR8
U 1 1 5206454C
P 3550 2600
F 0 "#PWR8" H 3550 2560 30  0001 C CNN
F 1 "+3,3V" H 3550 2710 30  0000 C CNN
F 2 "" H 3550 2600 60  0000 C CNN
F 3 "" H 3550 2600 60  0000 C CNN
	1    3550 2600
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR11
U 1 1 52064552
P 4550 2000
F 0 "#PWR11" H 4550 2000 30  0001 C CNN
F 1 "GND" H 4550 1930 30  0001 C CNN
F 2 "" H 4550 2000 60  0000 C CNN
F 3 "" H 4550 2000 60  0000 C CNN
	1    4550 2000
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR12
U 1 1 52064558
P 4550 2100
F 0 "#PWR12" H 4550 2100 30  0001 C CNN
F 1 "GND" H 4550 2030 30  0001 C CNN
F 2 "" H 4550 2100 60  0000 C CNN
F 3 "" H 4550 2100 60  0000 C CNN
	1    4550 2100
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR13
U 1 1 5206455E
P 4550 2200
F 0 "#PWR13" H 4550 2200 30  0001 C CNN
F 1 "GND" H 4550 2130 30  0001 C CNN
F 2 "" H 4550 2200 60  0000 C CNN
F 3 "" H 4550 2200 60  0000 C CNN
	1    4550 2200
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR14
U 1 1 52064569
P 4550 2400
F 0 "#PWR14" H 4550 2400 30  0001 C CNN
F 1 "GND" H 4550 2330 30  0001 C CNN
F 2 "" H 4550 2400 60  0000 C CNN
F 3 "" H 4550 2400 60  0000 C CNN
	1    4550 2400
	0    -1   -1   0   
$EndComp
Text Label 4550 2300 0    60   ~ 0
PS_ON
Wire Wire Line
	4550 2300 4450 2300
Wire Wire Line
	4450 2400 4550 2400
Connection ~ 4500 2300
Connection ~ 4500 2400
Wire Wire Line
	4550 2200 4450 2200
Wire Wire Line
	4450 2100 4550 2100
Wire Wire Line
	4550 2000 4450 2000
Wire Wire Line
	4550 1700 4450 1700
Wire Wire Line
	4450 1800 4550 1800
Wire Wire Line
	3650 1700 3550 1700
Wire Wire Line
	3550 2000 3650 2000
Wire Wire Line
	3550 2100 3650 2100
Wire Wire Line
	3550 2200 3650 2200
Wire Wire Line
	3550 2300 3650 2300
Wire Wire Line
	3650 2400 3550 2400
Wire Wire Line
	3550 2500 3650 2500
Wire Wire Line
	3650 2600 3550 2600
$Comp
L -12V #PWR15
U 1 1 52064633
P 4550 2500
F 0 "#PWR15" H 4550 2630 20  0001 C CNN
F 1 "-12V" H 4550 2600 30  0000 C CNN
F 2 "" H 4550 2500 60  0000 C CNN
F 3 "" H 4550 2500 60  0000 C CNN
	1    4550 2500
	0    1    1    0   
$EndComp
Wire Wire Line
	4550 2500 4450 2500
$Comp
L +3,3V #PWR16
U 1 1 52064652
P 4550 2600
F 0 "#PWR16" H 4550 2560 30  0001 C CNN
F 1 "+3,3V" H 4550 2710 30  0000 C CNN
F 2 "" H 4550 2600 60  0000 C CNN
F 3 "" H 4550 2600 60  0000 C CNN
	1    4550 2600
	0    1    1    0   
$EndComp
Wire Wire Line
	4550 2600 4450 2600
$Comp
L CONN_5 P2
U 1 1 520646C8
P 6500 2050
F 0 "P2" V 6450 2050 50  0000 C CNN
F 1 "CONN_5" V 6550 2050 50  0000 C CNN
F 2 "" H 6500 2050 60  0000 C CNN
F 3 "" H 6500 2050 60  0000 C CNN
	1    6500 2050
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 P3
U 1 1 520646D7
P 6500 2550
F 0 "P3" V 6450 2550 40  0000 C CNN
F 1 "Switch" V 6550 2550 40  0000 C CNN
F 2 "" H 6500 2550 60  0000 C CNN
F 3 "" H 6500 2550 60  0000 C CNN
	1    6500 2550
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR19
U 1 1 520646F7
P 6000 2050
F 0 "#PWR19" H 6000 2140 20  0001 C CNN
F 1 "+5V" H 6000 2140 30  0000 C CNN
F 2 "" H 6000 2050 60  0000 C CNN
F 3 "" H 6000 2050 60  0000 C CNN
	1    6000 2050
	0    -1   -1   0   
$EndComp
$Comp
L +12V #PWR17
U 1 1 520646FD
P 6000 1850
F 0 "#PWR17" H 6000 1800 20  0001 C CNN
F 1 "+12V" H 6000 1950 30  0000 C CNN
F 2 "" H 6000 1850 60  0000 C CNN
F 3 "" H 6000 1850 60  0000 C CNN
	1    6000 1850
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR18
U 1 1 52064703
P 6000 1950
F 0 "#PWR18" H 6000 1950 30  0001 C CNN
F 1 "GND" H 6000 1880 30  0001 C CNN
F 2 "" H 6000 1950 60  0000 C CNN
F 3 "" H 6000 1950 60  0000 C CNN
	1    6000 1950
	0    1    1    0   
$EndComp
Wire Wire Line
	6000 1950 6100 1950
Wire Wire Line
	6100 1850 6000 1850
Wire Wire Line
	6000 2050 6100 2050
$Comp
L -12V #PWR20
U 1 1 52064748
P 6000 2150
F 0 "#PWR20" H 6000 2280 20  0001 C CNN
F 1 "-12V" H 6000 2250 30  0000 C CNN
F 2 "" H 6000 2150 60  0000 C CNN
F 3 "" H 6000 2150 60  0000 C CNN
	1    6000 2150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6000 2150 6100 2150
$Comp
L +3,3V #PWR21
U 1 1 52064765
P 6000 2250
F 0 "#PWR21" H 6000 2210 30  0001 C CNN
F 1 "+3,3V" H 6000 2360 30  0000 C CNN
F 2 "" H 6000 2250 60  0000 C CNN
F 3 "" H 6000 2250 60  0000 C CNN
	1    6000 2250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6000 2250 6100 2250
Text Label 6050 2450 2    60   ~ 0
PS_ON
Wire Wire Line
	6050 2450 6150 2450
$Comp
L GND #PWR22
U 1 1 5206479D
P 6050 2650
F 0 "#PWR22" H 6050 2650 30  0001 C CNN
F 1 "GND" H 6050 2580 30  0001 C CNN
F 2 "" H 6050 2650 60  0000 C CNN
F 3 "" H 6050 2650 60  0000 C CNN
	1    6050 2650
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 2650 6150 2650
$Comp
L CONN_1 P4
U 1 1 52064E6D
P 7150 2350
F 0 "P4" H 7230 2350 40  0000 L CNN
F 1 "CONN_1" H 7150 2405 30  0001 C CNN
F 2 "" H 7150 2350 60  0000 C CNN
F 3 "" H 7150 2350 60  0000 C CNN
	1    7150 2350
	1    0    0    -1  
$EndComp
$EndSCHEMATC
