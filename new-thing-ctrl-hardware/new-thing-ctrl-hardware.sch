EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L MCU_Module:WeMos_D1_mini U2
U 1 1 616DF51F
P 5100 3250
F 0 "U2" H 4800 4000 50  0000 C CNN
F 1 "WeMos_D1_mini" H 5400 2500 50  0000 C CNN
F 2 "Module:WEMOS_D1_mini_light" H 5100 2100 50  0001 C CNN
F 3 "https://wiki.wemos.cc/products:d1:d1_mini#documentation" H 3250 2100 50  0001 C CNN
	1    5100 3250
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:L7805 U1
U 1 1 616E06D7
P 3450 1850
F 0 "U1" H 3450 2092 50  0000 C CNN
F 1 "L7805" H 3450 2001 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Horizontal_TabDown" H 3475 1700 50  0001 L CIN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/41/4f/b3/b0/12/d4/47/88/CD00000444.pdf/files/CD00000444.pdf/jcr:content/translations/en.CD00000444.pdf" H 3450 1800 50  0001 C CNN
	1    3450 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 1850 5000 2450
Wire Wire Line
	3450 2300 3450 2150
$Comp
L power:GND #PWR0101
U 1 1 616E42BC
P 3450 2350
F 0 "#PWR0101" H 3450 2100 50  0001 C CNN
F 1 "GND" H 3455 2177 50  0000 C CNN
F 2 "" H 3450 2350 50  0001 C CNN
F 3 "" H 3450 2350 50  0001 C CNN
	1    3450 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 2350 3450 2300
Connection ~ 3450 2300
Wire Wire Line
	3750 1850 3800 1850
$Comp
L Connector:Conn_01x08_Male J4
U 1 1 616E576D
P 8550 3300
F 0 "J4" H 8522 3182 50  0000 R CNN
F 1 "MFRC522" H 8522 3273 50  0000 R CNN
F 2 "Connector_JST:JST_XH_B8B-XH-A_1x08_P2.50mm_Vertical" H 8550 3300 50  0001 C CNN
F 3 "~" H 8550 3300 50  0001 C CNN
	1    8550 3300
	-1   0    0    1   
$EndComp
Text Label 8150 2900 2    50   ~ 0
SS
Text Label 8150 3000 2    50   ~ 0
SCK
Text Label 8150 3100 2    50   ~ 0
MOSI
Text Label 8150 3200 2    50   ~ 0
MISO
Text Label 8150 3300 2    50   ~ 0
IRQ
Text Label 8150 3500 2    50   ~ 0
RST
Wire Wire Line
	8150 2900 8350 2900
Wire Wire Line
	8150 3000 8350 3000
Wire Wire Line
	8350 3100 8150 3100
Wire Wire Line
	8150 3200 8350 3200
Wire Wire Line
	8350 3300 8150 3300
Wire Wire Line
	8350 3400 8300 3400
Wire Wire Line
	8300 3400 8300 3900
Wire Wire Line
	8150 3500 8350 3500
$Comp
L power:+3V3 #PWR0102
U 1 1 616ED42B
P 8200 2850
F 0 "#PWR0102" H 8200 2700 50  0001 C CNN
F 1 "+3V3" H 8215 3023 50  0000 C CNN
F 2 "" H 8200 2850 50  0001 C CNN
F 3 "" H 8200 2850 50  0001 C CNN
	1    8200 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 2850 8200 3600
Wire Wire Line
	8200 3600 8350 3600
$Comp
L power:+3V3 #PWR0103
U 1 1 616EEA7F
P 5200 2350
F 0 "#PWR0103" H 5200 2200 50  0001 C CNN
F 1 "+3V3" H 5215 2523 50  0000 C CNN
F 2 "" H 5200 2350 50  0001 C CNN
F 3 "" H 5200 2350 50  0001 C CNN
	1    5200 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 2350 5200 2400
$Comp
L power:GND #PWR0104
U 1 1 616EF5C8
P 5100 4200
F 0 "#PWR0104" H 5100 3950 50  0001 C CNN
F 1 "GND" H 5105 4027 50  0000 C CNN
F 2 "" H 5100 4200 50  0001 C CNN
F 3 "" H 5100 4200 50  0001 C CNN
	1    5100 4200
	1    0    0    -1  
$EndComp
Text Label 5550 3250 0    50   ~ 0
SS
Text Label 5550 3350 0    50   ~ 0
SCK
Text Label 5550 3550 0    50   ~ 0
MOSI
Text Label 5550 3450 0    50   ~ 0
MISO
Wire Wire Line
	5550 3250 5500 3250
Wire Wire Line
	5500 3550 5550 3550
Wire Wire Line
	5500 3450 5550 3450
Wire Wire Line
	5500 3350 5550 3350
Text Label 5850 2850 0    50   ~ 0
RST
Wire Wire Line
	5850 2850 5500 2850
$Comp
L Transistor_BJT:TIP120 Q1
U 1 1 61704962
P 6800 3650
F 0 "Q1" H 7007 3696 50  0000 L CNN
F 1 "TIP120" H 7007 3605 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Horizontal_TabDown" H 7000 3575 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/TIP120-D.PDF" H 6800 3650 50  0001 L CNN
	1    6800 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R7
U 1 1 617057B9
P 6400 3650
F 0 "R7" V 6204 3650 50  0000 C CNN
F 1 "1K" V 6295 3650 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 6400 3650 50  0001 C CNN
F 3 "~" H 6400 3650 50  0001 C CNN
	1    6400 3650
	0    1    1    0   
$EndComp
Wire Wire Line
	6500 3650 6600 3650
$Comp
L power:GND #PWR0105
U 1 1 61706D42
P 6900 3900
F 0 "#PWR0105" H 6900 3650 50  0001 C CNN
F 1 "GND" H 6905 3727 50  0000 C CNN
F 2 "" H 6900 3900 50  0001 C CNN
F 3 "" H 6900 3900 50  0001 C CNN
	1    6900 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 3900 6900 3850
Wire Wire Line
	6300 3650 5500 3650
$Comp
L Device:LED_Small D4
U 1 1 6171C2D3
P 6600 3200
F 0 "D4" V 6646 3130 50  0000 R CNN
F 1 "RED" V 6555 3130 50  0000 R CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" V 6600 3200 50  0001 C CNN
F 3 "~" V 6600 3200 50  0001 C CNN
	1    6600 3200
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R8
U 1 1 6171D54C
P 6600 2900
F 0 "R8" H 6541 2854 50  0000 R CNN
F 1 "1K" H 6541 2945 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 6600 2900 50  0001 C CNN
F 3 "~" H 6600 2900 50  0001 C CNN
	1    6600 2900
	-1   0    0    1   
$EndComp
Wire Wire Line
	6600 3000 6600 3100
Wire Wire Line
	6600 3300 6600 3350
Wire Wire Line
	6600 3350 6900 3350
Wire Wire Line
	6900 3350 6900 3250
Wire Wire Line
	6900 3250 7000 3250
Wire Wire Line
	6900 3450 6900 3350
Connection ~ 6900 3350
Wire Wire Line
	7000 3150 6900 3150
Wire Wire Line
	6900 3150 6900 2575
Wire Wire Line
	6900 2575 6600 2575
Wire Wire Line
	6600 2575 6600 2800
$Comp
L power:+24V #PWR0106
U 1 1 617207CC
P 6900 2550
F 0 "#PWR0106" H 6900 2400 50  0001 C CNN
F 1 "+24V" H 6915 2723 50  0000 C CNN
F 2 "" H 6900 2550 50  0001 C CNN
F 3 "" H 6900 2550 50  0001 C CNN
	1    6900 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 2550 6900 2575
Connection ~ 6900 2575
$Comp
L power:+24V #PWR0107
U 1 1 61723D63
P 2800 1600
F 0 "#PWR0107" H 2800 1450 50  0001 C CNN
F 1 "+24V" H 2815 1773 50  0000 C CNN
F 2 "" H 2800 1600 50  0001 C CNN
F 3 "" H 2800 1600 50  0001 C CNN
	1    2800 1600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J1
U 1 1 61724165
P 1700 1950
F 0 "J1" H 1673 1832 50  0000 R CNN
F 1 "MSTBAish" H 1673 1923 50  0000 R CNN
F 2 "Connector_Phoenix_MSTB:PhoenixContact_MSTBA_2,5_2-G-5,08_1x02_P5.08mm_Horizontal" H 1700 1950 50  0001 C CNN
F 3 "~" H 1700 1950 50  0001 C CNN
	1    1700 1950
	1    0    0    1   
$EndComp
$Comp
L Device:Fuse F1
U 1 1 61729099
P 2550 1850
F 0 "F1" V 2353 1850 50  0000 C CNN
F 1 "2AT" V 2444 1850 50  0000 C CNN
F 2 "Fuse:Fuseholder_Cylinder-5x20mm_Schurter_0031_8201_Horizontal_Open" V 2480 1850 50  0001 C CNN
F 3 "~" H 2550 1850 50  0001 C CNN
	1    2550 1850
	0    1    1    0   
$EndComp
Wire Wire Line
	2800 1600 2800 1850
Wire Wire Line
	2800 1850 2700 1850
$Comp
L Device:CP_Small C1
U 1 1 61739AC7
P 2800 2000
F 0 "C1" H 2600 2050 50  0000 L CNN
F 1 "350u" H 2500 1950 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 2800 2000 50  0001 C CNN
F 3 "~" H 2800 2000 50  0001 C CNN
	1    2800 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 2300 2800 2300
Wire Wire Line
	2000 2300 2000 1950
Wire Wire Line
	2000 1950 1900 1950
Wire Wire Line
	2800 1850 3150 1850
Connection ~ 2800 1850
Wire Wire Line
	2800 1850 2800 1900
Wire Wire Line
	2800 2100 2800 2300
Connection ~ 2800 2300
Wire Wire Line
	2800 2300 2000 2300
Text Notes 750  2100 0    50   ~ 0
Ferrite choke on input wires
Wire Wire Line
	3800 2000 3800 1850
Wire Wire Line
	3800 2200 3800 2300
Connection ~ 3800 2300
Wire Wire Line
	3800 2300 3450 2300
$Comp
L Device:C_Small C2
U 1 1 6174DD5C
P 3800 2100
F 0 "C2" H 3892 2146 50  0000 L CNN
F 1 "10n" H 3892 2055 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D7.0mm_W2.5mm_P5.00mm" H 3800 2100 50  0001 C CNN
F 3 "~" H 3800 2100 50  0001 C CNN
	1    3800 2100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3950 2000 3950 1850
Wire Wire Line
	3950 2200 3950 2300
$Comp
L Connector:Conn_01x04_Male J3
U 1 1 6175B598
P 8500 2150
F 0 "J3" H 8472 2032 50  0000 R CNN
F 1 "LCD" H 8472 2123 50  0000 R CNN
F 2 "Connector_JST:JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical" H 8500 2150 50  0001 C CNN
F 3 "~" H 8500 2150 50  0001 C CNN
	1    8500 2150
	-1   0    0    1   
$EndComp
$Comp
L power:+3V3 #PWR0108
U 1 1 6175CDF1
P 7950 1850
F 0 "#PWR0108" H 7950 1700 50  0001 C CNN
F 1 "+3V3" H 7965 2023 50  0000 C CNN
F 2 "" H 7950 1850 50  0001 C CNN
F 3 "" H 7950 1850 50  0001 C CNN
	1    7950 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 1850 7950 1900
Wire Wire Line
	7950 2150 8300 2150
$Comp
L power:GND #PWR0109
U 1 1 616EA573
P 8300 3950
F 0 "#PWR0109" H 8300 3700 50  0001 C CNN
F 1 "GND" H 8305 3777 50  0000 C CNN
F 2 "" H 8300 3950 50  0001 C CNN
F 3 "" H 8300 3950 50  0001 C CNN
	1    8300 3950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 61762B5C
P 8200 2300
F 0 "#PWR0110" H 8200 2050 50  0001 C CNN
F 1 "GND" H 8205 2127 50  0000 C CNN
F 2 "" H 8200 2300 50  0001 C CNN
F 3 "" H 8200 2300 50  0001 C CNN
	1    8200 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 2300 8200 2250
Wire Wire Line
	8200 2250 8300 2250
$Comp
L Device:C_Small C5
U 1 1 61766DFD
P 7850 2100
F 0 "C5" H 7650 2150 50  0000 L CNN
F 1 "10u" H 7600 2050 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric" H 7850 2100 50  0001 C CNN
F 3 "~" H 7850 2100 50  0001 C CNN
	1    7850 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 2200 7850 2250
Wire Wire Line
	7850 2250 8200 2250
Connection ~ 8200 2250
Wire Wire Line
	7850 2000 7850 1900
Wire Wire Line
	7850 1900 7950 1900
Connection ~ 7950 1900
Wire Wire Line
	7950 1900 7950 2150
Text Label 8250 2050 2    50   ~ 0
SDA
Text Label 8250 1950 2    50   ~ 0
SCL
Wire Wire Line
	8250 1950 8300 1950
Wire Wire Line
	8250 2050 8300 2050
Text Label 5850 3050 0    50   ~ 0
SDA
Text Label 5850 2950 0    50   ~ 0
SCL
$Comp
L Device:R_Small R4
U 1 1 6178367E
P 5550 2550
F 0 "R4" H 5491 2596 50  0000 R CNN
F 1 "10K" H 5491 2505 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 5550 2550 50  0001 C CNN
F 3 "~" H 5550 2550 50  0001 C CNN
	1    5550 2550
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_Small R5
U 1 1 61785AC9
P 5800 2550
F 0 "R5" H 5741 2596 50  0000 R CNN
F 1 "10K" H 5741 2505 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 5800 2550 50  0001 C CNN
F 3 "~" H 5800 2550 50  0001 C CNN
	1    5800 2550
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5550 2650 5550 2950
Wire Wire Line
	5550 2950 5500 2950
Wire Wire Line
	5800 2650 5800 3050
Wire Wire Line
	5800 3050 5500 3050
Wire Wire Line
	5850 3050 5800 3050
Connection ~ 5800 3050
Wire Wire Line
	5850 2950 5550 2950
Connection ~ 5550 2950
Wire Wire Line
	5200 2400 5550 2400
Wire Wire Line
	5800 2400 5800 2450
Connection ~ 5200 2400
Wire Wire Line
	5200 2400 5200 2450
Wire Wire Line
	5550 2450 5550 2400
Connection ~ 5550 2400
Wire Wire Line
	5550 2400 5800 2400
$Comp
L Device:C_Small C6
U 1 1 617BE69D
P 8200 3750
F 0 "C6" H 8000 3800 50  0000 L CNN
F 1 "10u" H 7950 3700 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric" H 8200 3750 50  0001 C CNN
F 3 "~" H 8200 3750 50  0001 C CNN
	1    8200 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 3850 8200 3900
Wire Wire Line
	8200 3900 8300 3900
Connection ~ 8300 3900
Wire Wire Line
	8300 3900 8300 3950
Wire Wire Line
	8200 3650 8200 3600
Connection ~ 8200 3600
$Comp
L Connector:Conn_01x02_Male J2
U 1 1 617D3635
P 7200 3250
F 0 "J2" H 7172 3132 50  0000 R CNN
F 1 "MSTBAish" H 7172 3223 50  0000 R CNN
F 2 "Connector_Phoenix_MSTB:PhoenixContact_MSTBA_2,5_2-G-5,08_1x02_P5.08mm_Horizontal" H 7200 3250 50  0001 C CNN
F 3 "~" H 7200 3250 50  0001 C CNN
	1    7200 3250
	-1   0    0    1   
$EndComp
Text Label 5550 3150 0    50   ~ 0
NEOPIXEL
Wire Wire Line
	5550 3150 5500 3150
$Comp
L LED:WS2812B D5
U 1 1 617D93B5
P 8250 1000
F 0 "D5" H 8594 1046 50  0000 L CNN
F 1 "WS2812B" H 8594 955 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 8300 700 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 8350 625 50  0001 L TNN
	1    8250 1000
	1    0    0    -1  
$EndComp
Text Label 7900 1000 2    50   ~ 0
NEOPIXEL
Wire Wire Line
	7900 1000 7950 1000
$Comp
L power:+3V3 #PWR0111
U 1 1 617E3DD3
P 8250 650
F 0 "#PWR0111" H 8250 500 50  0001 C CNN
F 1 "+3V3" H 8265 823 50  0000 C CNN
F 2 "" H 8250 650 50  0001 C CNN
F 3 "" H 8250 650 50  0001 C CNN
	1    8250 650 
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 650  8250 700 
$Comp
L power:GND #PWR0112
U 1 1 617E7465
P 8250 1350
F 0 "#PWR0112" H 8250 1100 50  0001 C CNN
F 1 "GND" H 8255 1177 50  0000 C CNN
F 2 "" H 8250 1350 50  0001 C CNN
F 3 "" H 8250 1350 50  0001 C CNN
	1    8250 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 1350 8250 1300
Text Label 5850 2750 0    50   ~ 0
BUTTONS
Wire Wire Line
	5850 2750 5500 2750
$Comp
L Switch:SW_SPST SW1
U 1 1 618390ED
P 4400 3400
F 0 "SW1" H 4400 3175 50  0000 C CNN
F 1 "SW_SPST" H 4400 3266 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 4400 3400 50  0001 C CNN
F 3 "~" H 4400 3400 50  0001 C CNN
	1    4400 3400
	0    1    1    0   
$EndComp
Wire Wire Line
	4400 3200 4400 2850
Wire Wire Line
	4400 2850 4700 2850
Wire Wire Line
	5100 4200 5100 4100
Wire Wire Line
	5100 4100 4400 4100
Wire Wire Line
	4400 4100 4400 3600
Connection ~ 5100 4100
Wire Wire Line
	5100 4100 5100 4050
Connection ~ 6600 2575
Connection ~ 6600 3350
$Comp
L Switch:SW_SPST SW2
U 1 1 617594D8
P 5000 5700
F 0 "SW2" H 5000 5475 50  0000 C CNN
F 1 "SW_SPST" H 5000 5566 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 5000 5700 50  0001 C CNN
F 3 "~" H 5000 5700 50  0001 C CNN
	1    5000 5700
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_SPST SW3
U 1 1 6175976E
P 5350 5700
F 0 "SW3" H 5350 5475 50  0000 C CNN
F 1 "SW_SPST" H 5350 5566 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 5350 5700 50  0001 C CNN
F 3 "~" H 5350 5700 50  0001 C CNN
	1    5350 5700
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_SPST SW4
U 1 1 61759D08
P 5700 5700
F 0 "SW4" H 5700 5475 50  0000 C CNN
F 1 "SW_SPST" H 5700 5566 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 5700 5700 50  0001 C CNN
F 3 "~" H 5700 5700 50  0001 C CNN
	1    5700 5700
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_SPST SW5
U 1 1 6175A06C
P 6050 5700
F 0 "SW5" H 6050 5475 50  0000 C CNN
F 1 "SW_SPST" H 6050 5566 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 6050 5700 50  0001 C CNN
F 3 "~" H 6050 5700 50  0001 C CNN
	1    6050 5700
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R2
U 1 1 6175AF8B
P 5175 5425
F 0 "R2" V 5250 5475 50  0000 R CNN
F 1 "1K" V 5325 5475 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 5175 5425 50  0001 C CNN
F 3 "~" H 5175 5425 50  0001 C CNN
	1    5175 5425
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R3
U 1 1 6175BAD8
P 5525 5425
F 0 "R3" V 5600 5475 50  0000 R CNN
F 1 "1K" V 5675 5475 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 5525 5425 50  0001 C CNN
F 3 "~" H 5525 5425 50  0001 C CNN
	1    5525 5425
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R6
U 1 1 6175BEB8
P 5875 5425
F 0 "R6" V 5950 5475 50  0000 R CNN
F 1 "1K" V 6025 5475 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 5875 5425 50  0001 C CNN
F 3 "~" H 5875 5425 50  0001 C CNN
	1    5875 5425
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5975 5425 6050 5425
Wire Wire Line
	6050 5425 6050 5500
Wire Wire Line
	5775 5425 5700 5425
Wire Wire Line
	5700 5500 5700 5425
Connection ~ 5700 5425
Wire Wire Line
	5700 5425 5625 5425
Wire Wire Line
	5350 5500 5350 5425
Wire Wire Line
	5350 5425 5275 5425
Wire Wire Line
	5350 5425 5425 5425
Connection ~ 5350 5425
Wire Wire Line
	5075 5425 5000 5425
Wire Wire Line
	5000 5425 5000 5500
$Comp
L Device:R_Small R1
U 1 1 61772CB5
P 4850 5425
F 0 "R1" V 4925 5475 50  0000 R CNN
F 1 "1K" V 5000 5475 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 4850 5425 50  0001 C CNN
F 3 "~" H 4850 5425 50  0001 C CNN
	1    4850 5425
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4950 5425 5000 5425
Connection ~ 5000 5425
$Comp
L power:+3V3 #PWR0113
U 1 1 617777CD
P 4675 5325
F 0 "#PWR0113" H 4675 5175 50  0001 C CNN
F 1 "+3V3" H 4690 5498 50  0000 C CNN
F 2 "" H 4675 5325 50  0001 C CNN
F 3 "" H 4675 5325 50  0001 C CNN
	1    4675 5325
	1    0    0    -1  
$EndComp
Wire Wire Line
	4675 5325 4675 5425
Wire Wire Line
	4675 5425 4750 5425
$Comp
L power:GND #PWR0114
U 1 1 6177C2DA
P 5000 5975
F 0 "#PWR0114" H 5000 5725 50  0001 C CNN
F 1 "GND" H 5005 5802 50  0000 C CNN
F 2 "" H 5000 5975 50  0001 C CNN
F 3 "" H 5000 5975 50  0001 C CNN
	1    5000 5975
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 5900 5000 5925
Wire Wire Line
	6050 5900 6050 5925
Wire Wire Line
	6050 5925 5700 5925
Connection ~ 5000 5925
Wire Wire Line
	5000 5925 5000 5975
Wire Wire Line
	5350 5900 5350 5925
Connection ~ 5350 5925
Wire Wire Line
	5350 5925 5000 5925
Wire Wire Line
	5700 5900 5700 5925
Connection ~ 5700 5925
Wire Wire Line
	5700 5925 5350 5925
Text Label 5000 5175 0    50   ~ 0
BUTTONS
Wire Wire Line
	5000 5175 5000 5425
$Comp
L Diode:1N5401 D1
U 1 1 6179EDE8
P 2175 1850
F 0 "D1" H 2175 1633 50  0000 C CNN
F 1 "1N5401" H 2175 1724 50  0000 C CNN
F 2 "Diode_THT:D_DO-201AD_P15.24mm_Horizontal" H 2175 1675 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88516/1n5400.pdf" H 2175 1850 50  0001 C CNN
	1    2175 1850
	-1   0    0    1   
$EndComp
Wire Wire Line
	1900 1850 2025 1850
Wire Wire Line
	2325 1850 2400 1850
$Comp
L Diode:1N4001 D3
U 1 1 617AD75B
P 6375 3050
F 0 "D3" V 6375 3175 50  0000 R CNN
F 1 "1N4001" V 6475 3375 50  0000 R CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 6375 2875 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 6375 3050 50  0001 C CNN
	1    6375 3050
	0    -1   1    0   
$EndComp
Wire Wire Line
	6375 2900 6375 2575
Wire Wire Line
	6375 2575 6600 2575
Wire Wire Line
	6375 3200 6375 3350
Wire Wire Line
	6375 3350 6600 3350
Wire Wire Line
	3800 2300 3950 2300
$Comp
L Diode:1N4001 D2
U 1 1 617BDB06
P 4150 1850
F 0 "D2" H 4275 2050 50  0000 R CNN
F 1 "1N4001" H 4300 1975 50  0000 R CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 4150 1675 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 4150 1850 50  0001 C CNN
	1    4150 1850
	-1   0    0    -1  
$EndComp
$Comp
L Device:CP_Small C3
U 1 1 617C3592
P 3950 2100
F 0 "C3" H 3750 2150 50  0000 L CNN
F 1 "350u" H 3650 2050 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 3950 2100 50  0001 C CNN
F 3 "~" H 3950 2100 50  0001 C CNN
	1    3950 2100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3800 1850 3950 1850
Connection ~ 3800 1850
Wire Wire Line
	4000 1850 3950 1850
Connection ~ 3950 1850
Wire Wire Line
	4300 1850 5000 1850
$Comp
L Device:CP_Small C4
U 1 1 617DAFC0
P 7025 2725
F 0 "C4" H 6825 2775 50  0000 L CNN
F 1 "350u" H 6725 2675 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 7025 2725 50  0001 C CNN
F 3 "~" H 7025 2725 50  0001 C CNN
	1    7025 2725
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0115
U 1 1 617E43FE
P 7025 2850
F 0 "#PWR0115" H 7025 2600 50  0001 C CNN
F 1 "GND" H 7030 2677 50  0000 C CNN
F 2 "" H 7025 2850 50  0001 C CNN
F 3 "" H 7025 2850 50  0001 C CNN
	1    7025 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7025 2825 7025 2850
Wire Wire Line
	7025 2625 7025 2575
Wire Wire Line
	7025 2575 6900 2575
$Comp
L Mechanical:DIN_Rail_Adapter DRA1
U 1 1 617FBA99
P 1325 3475
F 0 "DRA1" H 1325 3740 50  0000 C CNN
F 1 "DIN_Rail_Adapter" H 1325 3649 50  0000 C CNN
F 2 "MountingEquipment:DINRailAdapter_3xM3_PhoenixContact_1201578" H 1325 3275 50  0001 C CNN
F 3 "~" H 1325 3625 50  0001 C CNN
	1    1325 3475
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:DIN_Rail_Adapter DRA2
U 1 1 617FBEA9
P 1325 3850
F 0 "DRA2" H 1325 4115 50  0000 C CNN
F 1 "DIN_Rail_Adapter" H 1325 4024 50  0000 C CNN
F 2 "MountingEquipment:DINRailAdapter_3xM3_PhoenixContact_1201578" H 1325 3650 50  0001 C CNN
F 3 "~" H 1325 4000 50  0001 C CNN
	1    1325 3850
	1    0    0    -1  
$EndComp
$EndSCHEMATC