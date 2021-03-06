EESchema Schematic File Version 4
LIBS:controlBoard-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 6
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
L rofi:ESP32-WROVER U6
U 1 1 5E084D17
P 2200 2000
F 0 "U6" H 2200 3375 50  0000 C CNN
F 1 "ESP32-WROVER" H 2200 3284 50  0000 C CNN
F 2 "rofi:ESP32-WROVER" H 2200 2950 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf" H 2200 2950 50  0001 C CNN
F 4 "356-ESP32-WROB(16MB)" H 2200 2000 50  0001 C CNN "#manf"
	1    2200 2000
	1    0    0    -1  
$EndComp
Text GLabel 1550 1900 0    50   Input ~ 0
ESP_EN
Text GLabel 3800 2250 2    50   Input ~ 0
IO0
Text Notes 4000 2300 0    50   ~ 0
IO0: LOW => Bootloader
Text Notes 4000 2200 0    50   ~ 0
IO2: Floating/LOW => bootloader
Text Notes 4000 2100 0    50   ~ 0
IO15: LOW => Silence boot message
Text Notes 4000 2000 0    50   ~ 0
IO12: HIGH => 1.8V flash
Text GLabel 3450 2950 2    50   Input ~ 0
TXD0
Text GLabel 3450 2850 2    50   Input ~ 0
RXD0
NoConn ~ 1550 1300
NoConn ~ 1550 1400
NoConn ~ 1550 1500
NoConn ~ 1550 1600
NoConn ~ 1550 1700
NoConn ~ 1550 1200
Wire Wire Line
	1550 2850 1450 2850
Wire Wire Line
	1450 2850 1450 2950
Wire Wire Line
	1450 2950 1550 2950
Wire Wire Line
	1550 3050 1450 3050
Wire Wire Line
	1450 3050 1450 2950
Connection ~ 1450 2950
Wire Wire Line
	1550 3150 1450 3150
Wire Wire Line
	1450 3150 1450 3050
Connection ~ 1450 3050
Wire Wire Line
	1450 3150 1450 3250
Connection ~ 1450 3150
$Comp
L Device:C C40
U 1 1 5E136009
P 1250 2550
F 0 "C40" H 1365 2596 50  0000 L CNN
F 1 "470n" H 1365 2505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 1288 2400 50  0001 C CNN
F 3 "~" H 1250 2550 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 1250 2550 50  0001 C CNN "#manf"
	1    1250 2550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C39
U 1 1 5E1367F8
P 800 2550
F 0 "C39" H 915 2596 50  0000 L CNN
F 1 "22u" H 915 2505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 838 2400 50  0001 C CNN
F 3 "~" H 800 2550 50  0001 C CNN
F 4 "GRM21BR61C226ME44L" H 800 2550 50  0001 C CNN "#manf"
	1    800  2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 2400 1250 2400
Wire Wire Line
	1250 2400 800  2400
Connection ~ 1250 2400
Text GLabel 2850 2550 2    50   Input ~ 0
SPI_MISO_MOSI
Text GLabel 2850 1650 2    50   Input ~ 0
DOCK_1_CE
Text GLabel 2850 1450 2    50   Input ~ 0
DOCK_2_CE
Text GLabel 2850 1250 2    50   Input ~ 0
DOCK_3_CE
Text GLabel 2850 1150 2    50   Input ~ 0
DOCK_4_CE
Text GLabel 2850 1350 2    50   Input ~ 0
DOCK_5_CE
Text GLabel 2850 1550 2    50   Input ~ 0
DOCK_6_CE
Text GLabel 2850 2150 2    50   Input ~ 0
MOTORS_RX
Wire Wire Line
	2850 3150 2950 3150
Wire Wire Line
	2950 3150 2950 3700
Wire Wire Line
	2850 3050 3050 3050
Wire Wire Line
	3050 3050 3050 3550
Wire Wire Line
	4200 3700 4200 3800
Wire Wire Line
	4200 3450 4200 3550
Text GLabel 4300 2750 2    50   Input ~ 0
BIOS_TX
Text GLabel 4300 2650 2    50   Input ~ 0
BIOS_RX
Wire Wire Line
	3800 2250 2850 2250
Text GLabel 2850 2450 2    50   Input ~ 0
SPI_SCK
$Comp
L Connector_Generic:Conn_02x05_Odd_Even J1
U 1 1 5E6930A0
P 4400 1100
F 0 "J1" H 4450 1517 50  0000 C CNN
F 1 "ESP_JTAG" H 4450 1426 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x05_P2.54mm_Vertical" H 4400 1100 50  0001 C CNN
F 3 "~" H 4400 1100 50  0001 C CNN
	1    4400 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 1000 4100 1000
Wire Wire Line
	4100 1000 4100 1100
Wire Wire Line
	4100 1100 4200 1100
Wire Wire Line
	4100 1100 4100 1200
Wire Wire Line
	4100 1200 4200 1200
Connection ~ 4100 1100
Wire Wire Line
	4100 1200 4100 1300
Wire Wire Line
	4100 1300 4200 1300
Connection ~ 4100 1200
Text GLabel 4700 900  2    50   Input ~ 0
ESP_TMS
Text GLabel 4700 1000 2    50   Input ~ 0
ESP_TCK
Text GLabel 4700 1100 2    50   Input ~ 0
ESP_TDO
Text GLabel 4700 1200 2    50   Input ~ 0
ESP_TDI
NoConn ~ 4700 1300
Text GLabel 2850 1750 2    50   Input ~ 0
ESP_TMS
Text GLabel 2850 2050 2    50   Input ~ 0
ESP_TDO
Text GLabel 2850 1850 2    50   Input ~ 0
ESP_TDI
Text GLabel 2850 1950 2    50   Input ~ 0
ESP_TCK
$Comp
L Sensor_Motion:ICM-20948 U7
U 1 1 5E6C3FF7
P 2100 5950
F 0 "U7" H 2450 6650 50  0000 C CNN
F 1 "ICM-20948" H 2400 5300 50  0000 C CNN
F 2 "Sensor_Motion:InvenSense_QFN-24_3x3mm_P0.4mm" H 2100 4950 50  0001 C CNN
F 3 "http://www.invensense.com/wp-content/uploads/2016/06/DS-000189-ICM-20948-v1.3.pdf" H 2100 5800 50  0001 C CNN
F 4 "ICM-20948" H 2100 5950 50  0001 C CNN "#manf"
	1    2100 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 3450 4400 3450
Wire Wire Line
	4200 3550 4400 3550
Connection ~ 4200 3550
Wire Wire Line
	4200 3700 4400 3700
Connection ~ 4200 3700
Wire Wire Line
	4200 3800 4400 3800
Connection ~ 3950 3550
Wire Wire Line
	3950 3550 4200 3550
Connection ~ 3950 3700
Wire Wire Line
	3950 3700 4200 3700
Connection ~ 5900 3050
Text GLabel 5750 3050 0    50   Input ~ 0
DTR
$Comp
L Device:R R30
U 1 1 5E09ADFB
P 6100 3050
F 0 "R30" V 5893 3050 50  0000 C CNN
F 1 "10k" V 5984 3050 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6030 3050 50  0001 C CNN
F 3 "~" H 6100 3050 50  0001 C CNN
F 4 "RT0402FRE0710KL" H 6100 3050 50  0001 C CNN "#manf"
	1    6100 3050
	0    1    1    0   
$EndComp
$Comp
L Device:R R31
U 1 1 5E09B50E
P 6100 3800
F 0 "R31" V 5893 3800 50  0000 C CNN
F 1 "10k" V 5984 3800 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6030 3800 50  0001 C CNN
F 3 "~" H 6100 3800 50  0001 C CNN
F 4 "RT0402FRE0710KL" H 6100 3800 50  0001 C CNN "#manf"
	1    6100 3800
	0    1    1    0   
$EndComp
Wire Wire Line
	5950 3050 5900 3050
Wire Wire Line
	5950 3800 5900 3800
Wire Wire Line
	5900 3050 5900 3300
Wire Wire Line
	5900 3800 5900 3550
Connection ~ 5900 3800
Wire Wire Line
	5900 3300 6700 3550
Wire Wire Line
	5900 3550 6700 3300
Wire Wire Line
	6700 3300 6700 3250
Wire Wire Line
	6700 3550 6700 3600
Wire Wire Line
	6400 3050 6250 3050
Wire Wire Line
	6400 3800 6350 3800
$Comp
L Transistor_BJT:BC817 Q4
U 1 1 5E0B3938
P 6600 3050
F 0 "Q4" H 6791 3096 50  0000 L CNN
F 1 "BC817" H 6791 3005 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 6800 2975 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC817.pdf" H 6600 3050 50  0001 L CNN
F 4 "BC817-40,235" H 6600 3050 50  0001 C CNN "#manf"
	1    6600 3050
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC817 Q5
U 1 1 5E0B5752
P 6600 3800
F 0 "Q5" H 6791 3754 50  0000 L CNN
F 1 "BC817" H 6791 3845 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 6800 3725 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC817.pdf" H 6600 3800 50  0001 L CNN
F 4 "BC817-40,235" H 6600 3800 50  0001 C CNN "#manf"
	1    6600 3800
	1    0    0    1   
$EndComp
Wire Wire Line
	6700 4000 6700 4100
Wire Wire Line
	6700 4100 6900 4100
Wire Wire Line
	6700 2850 6700 2700
Wire Wire Line
	6700 2700 7150 2700
$Comp
L Device:R R32
U 1 1 5E0B9694
P 6100 4100
F 0 "R32" V 5893 4100 50  0000 C CNN
F 1 "10k" V 5984 4100 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6030 4100 50  0001 C CNN
F 3 "~" H 6100 4100 50  0001 C CNN
F 4 "RT0402FRE0710KL" H 6100 4100 50  0001 C CNN "#manf"
	1    6100 4100
	0    1    1    0   
$EndComp
Wire Wire Line
	6250 4100 6350 4100
Wire Wire Line
	6350 4100 6350 3800
Connection ~ 6350 3800
Wire Wire Line
	6350 3800 6250 3800
$Comp
L Device:C C44
U 1 1 5E0BA74F
P 7150 2850
F 0 "C44" H 7265 2896 50  0000 L CNN
F 1 "470n" H 7265 2805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 7188 2700 50  0001 C CNN
F 3 "~" H 7150 2850 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 7150 2850 50  0001 C CNN "#manf"
	1    7150 2850
	1    0    0    -1  
$EndComp
Connection ~ 7150 2700
$Comp
L Device:R R33
U 1 1 5E0BBB47
P 7150 2550
F 0 "R33" H 7220 2596 50  0000 L CNN
F 1 "10k" H 7220 2505 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7080 2550 50  0001 C CNN
F 3 "~" H 7150 2550 50  0001 C CNN
F 4 "RT0402FRE0710KL" H 7150 2550 50  0001 C CNN "#manf"
	1    7150 2550
	1    0    0    -1  
$EndComp
Text GLabel 5750 3800 0    50   Input ~ 0
RTS
Text GLabel 6900 4100 2    50   Input ~ 0
IO0
Text GLabel 7500 2700 2    50   Input ~ 0
ESP_EN
Text Notes 5800 2500 0    50   ~ 0
ESP32 PROGRAMMING INTERFACE
$Comp
L Interface_USB:CP2102N-A01-GQFN28 U8
U 1 1 5E086C4A
P 9400 2450
F 0 "U8" H 9700 1200 50  0000 C CNN
F 1 "CP2102N-A01-GQFN28" H 9950 1100 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-28-1EP_5x5mm_P0.5mm_EP3.35x3.35mm" H 9850 1250 50  0001 L CNN
F 3 "https://www.silabs.com/documents/public/data-sheets/cp2102n-datasheet.pdf" H 9450 1700 50  0001 C CNN
F 4 "CP2102N-A01-GQFN28" H 9400 2450 50  0001 C CNN "#manf"
	1    9400 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 1150 9300 1050
Wire Wire Line
	9300 1050 9400 1050
Wire Wire Line
	9400 1050 9400 1150
Wire Wire Line
	8900 1550 8650 1550
$Comp
L Device:R R35
U 1 1 5E08C943
P 8650 1300
F 0 "R35" H 8720 1346 50  0000 L CNN
F 1 "100k" H 8720 1255 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 8580 1300 50  0001 C CNN
F 3 "~" H 8650 1300 50  0001 C CNN
F 4 "RT0402FRE07100KL" H 8650 1300 50  0001 C CNN "#manf"
	1    8650 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 1450 8650 1550
Text GLabel 8800 1850 0    50   Input ~ 0
USB_C_D+
Text GLabel 8800 1950 0    50   Input ~ 0
USB_C_D-
Wire Wire Line
	8800 1850 8900 1850
Wire Wire Line
	8900 1950 8800 1950
Wire Wire Line
	8550 1550 8650 1550
Connection ~ 8650 1550
Text GLabel 8550 1550 0    50   Input ~ 0
USB_BRIDGE_EN
Text GLabel 9900 1550 2    50   Input ~ 0
TXD0
Text GLabel 9900 1650 2    50   Input ~ 0
RXD0
Text GLabel 9900 1850 2    50   Input ~ 0
DTR
Text GLabel 9900 1450 2    50   Input ~ 0
RTS
NoConn ~ 9900 1350
NoConn ~ 9900 1750
NoConn ~ 9900 1950
NoConn ~ 9900 2250
$Comp
L Device:C C50
U 1 1 5E0EEFEF
P 10500 1200
F 0 "C50" H 10615 1246 50  0000 L CNN
F 1 "470n" H 10615 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 10538 1050 50  0001 C CNN
F 3 "~" H 10500 1200 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 10500 1200 50  0001 C CNN "#manf"
	1    10500 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	10500 1050 9400 1050
Connection ~ 9400 1050
$Comp
L Device:LED D9
U 1 1 5E0F2F94
P 10200 2950
F 0 "D9" H 10193 2695 50  0000 C CNN
F 1 "RED" H 10193 2786 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 10200 2950 50  0001 C CNN
F 3 "~" H 10200 2950 50  0001 C CNN
	1    10200 2950
	1    0    0    1   
$EndComp
$Comp
L Device:LED D10
U 1 1 5E0F3CF7
P 10550 3050
F 0 "D10" H 10543 3266 50  0000 C CNN
F 1 "YELLOW" H 10543 3175 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 10550 3050 50  0001 C CNN
F 3 "~" H 10550 3050 50  0001 C CNN
	1    10550 3050
	1    0    0    -1  
$EndComp
NoConn ~ 9900 2550
NoConn ~ 9900 2650
NoConn ~ 9900 2750
$Comp
L Device:R R36
U 1 1 5E0F551E
P 10350 2650
F 0 "R36" H 10420 2696 50  0000 L CNN
F 1 "330R" H 10420 2605 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 10280 2650 50  0001 C CNN
F 3 "~" H 10350 2650 50  0001 C CNN
F 4 "RR0510P-331-D" H 10350 2650 50  0001 C CNN "#manf"
	1    10350 2650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R37
U 1 1 5E0F5A3A
P 10700 2650
F 0 "R37" H 10770 2696 50  0000 L CNN
F 1 "330R" H 10770 2605 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 10630 2650 50  0001 C CNN
F 3 "~" H 10700 2650 50  0001 C CNN
F 4 "RR0510P-331-D" H 10700 2650 50  0001 C CNN "#manf"
	1    10700 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	10350 2800 10350 2950
Wire Wire Line
	10700 2800 10700 3050
Wire Wire Line
	10400 3050 9900 3050
Wire Wire Line
	10050 2950 9900 2950
NoConn ~ 9900 3550
NoConn ~ 9900 3450
NoConn ~ 9900 3350
NoConn ~ 9900 3250
NoConn ~ 9900 3150
$Comp
L Device:D_Schottky D7
U 1 1 5E2FC764
P 7150 1750
F 0 "D7" H 7150 1534 50  0000 C CNN
F 1 "BAT54J,115" H 7150 1625 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-323" H 7150 1750 50  0001 C CNN
F 3 "~" H 7150 1750 50  0001 C CNN
F 4 "BAT54J,115" H 7150 1750 50  0001 C CNN "#manf"
	1    7150 1750
	-1   0    0    1   
$EndComp
$Comp
L Device:D_Zener D8
U 1 1 5E2FD068
P 7900 1900
F 0 "D8" V 7854 1979 50  0000 L CNN
F 1 "5V1" V 7945 1979 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-323" H 7900 1900 50  0001 C CNN
F 3 "~" H 7900 1900 50  0001 C CNN
F 4 "MM3Z5V1ST1G" V 7900 1900 50  0001 C CNN "#manf"
	1    7900 1900
	0    1    1    0   
$EndComp
$Comp
L Device:R R34
U 1 1 5E30339E
P 7600 1750
F 0 "R34" V 7800 1750 50  0000 C CNN
F 1 "100k" V 7700 1750 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7530 1750 50  0001 C CNN
F 3 "~" H 7600 1750 50  0001 C CNN
F 4 "RT0402FRE07100KL" H 7600 1750 50  0001 C CNN "#manf"
	1    7600 1750
	0    1    1    0   
$EndComp
Wire Wire Line
	7750 1750 7900 1750
Wire Wire Line
	7900 1750 8900 1750
Connection ~ 7900 1750
Wire Wire Line
	7450 1750 7300 1750
Text Notes 7450 1050 0    50   ~ 0
USB TO UART BRIDGE
NoConn ~ 2600 5850
NoConn ~ 2600 5950
Wire Wire Line
	2200 5250 2200 5050
Wire Wire Line
	2000 5050 2000 5250
Wire Wire Line
	2200 5050 2700 5050
$Comp
L Device:C C43
U 1 1 5E86A657
P 2800 6300
F 0 "C43" H 2685 6254 50  0000 R CNN
F 1 "470n" H 2685 6345 50  0000 R CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2838 6150 50  0001 C CNN
F 3 "~" H 2800 6300 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 2800 6300 50  0001 C CNN "#manf"
	1    2800 6300
	-1   0    0    1   
$EndComp
Wire Wire Line
	2800 6150 2600 6150
$Comp
L Device:C C42
U 1 1 5E86746B
P 2700 5200
F 0 "C42" H 2815 5246 50  0000 L CNN
F 1 "470n" H 2815 5155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2738 5050 50  0001 C CNN
F 3 "~" H 2700 5200 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 2700 5200 50  0001 C CNN "#manf"
	1    2700 5200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C41
U 1 1 5E86F45B
P 1450 5250
F 0 "C41" H 1565 5296 50  0000 L CNN
F 1 "470n" H 1565 5205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 1488 5100 50  0001 C CNN
F 3 "~" H 1450 5250 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 1450 5250 50  0001 C CNN "#manf"
	1    1450 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 5050 1450 5050
Wire Wire Line
	1450 5050 1450 5100
Wire Wire Line
	1000 5950 1600 5950
Wire Wire Line
	1000 6150 1600 6150
NoConn ~ 2850 850 
NoConn ~ 2850 950 
NoConn ~ 2850 1050
NoConn ~ 1600 6250
$Comp
L Device:R R51
U 1 1 5E9E00D9
P 1150 5650
F 0 "R51" V 943 5650 50  0000 C CNN
F 1 "100k" V 1034 5650 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 1080 5650 50  0001 C CNN
F 3 "~" H 1150 5650 50  0001 C CNN
F 4 "RT0402FRE07100KL" H 1150 5650 50  0001 C CNN "#manf"
	1    1150 5650
	0    1    1    0   
$EndComp
Wire Wire Line
	1300 5650 1600 5650
Text GLabel 10100 2050 2    50   Input ~ 0
USB_CLK
Text GLabel 10100 2150 2    50   Input ~ 0
USB_SUSPEND
Wire Wire Line
	10100 2150 10100 2350
Wire Wire Line
	10100 2350 9900 2350
Wire Wire Line
	10100 2050 9900 2050
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 5DFCF9E0
P 4950 4750
AR Path="/5E8D0C73/5DFCF9E0" Ref="J?"  Part="1" 
AR Path="/5E080FD6/5DFCF9E0" Ref="J5"  Part="1" 
F 0 "J5" H 5030 4742 50  0000 L CNN
F 1 "MOTOR_BUS" H 5030 4651 50  0000 L CNN
F 2 "Connector_PinHeader_2.00mm:PinHeader_1x04_P2.00mm_Vertical" H 4950 4750 50  0001 C CNN
F 3 "~" H 4950 4750 50  0001 C CNN
	1    4950 4750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 5DFCF9E6
P 4950 5300
AR Path="/5E8D0C73/5DFCF9E6" Ref="J?"  Part="1" 
AR Path="/5E080FD6/5DFCF9E6" Ref="J6"  Part="1" 
F 0 "J6" H 5030 5342 50  0000 L CNN
F 1 "MOTOR_BUS_ALT" H 5030 5251 50  0000 L CNN
F 2 "Connector_PinHeader_2.00mm:PinHeader_1x03_P2.00mm_Vertical" H 4950 5300 50  0001 C CNN
F 3 "~" H 4950 5300 50  0001 C CNN
	1    4950 5300
	1    0    0    -1  
$EndComp
Text GLabel 4750 4950 0    50   Input ~ 0
MOTORS_TX
Text GLabel 4750 5400 0    50   Input ~ 0
MOTORS_TX
$Comp
L Connector_Generic:Conn_02x05_Odd_Even J?
U 1 1 5DFCF9FC
P 4300 6100
AR Path="/5E8D0C73/5DFCF9FC" Ref="J?"  Part="1" 
AR Path="/5E080FD6/5DFCF9FC" Ref="J4"  Part="1" 
F 0 "J4" H 4350 6517 50  0000 C CNN
F 1 "ShoeAConnectors" H 4350 6426 50  0000 C CNN
F 2 "Connector_PinHeader_2.00mm:PinHeader_2x05_P2.00mm_Vertical" H 4300 6100 50  0001 C CNN
F 3 "~" H 4300 6100 50  0001 C CNN
	1    4300 6100
	1    0    0    -1  
$EndComp
Text GLabel 4100 6200 0    50   Input ~ 0
SPI_SCK
Text GLabel 4600 6200 2    50   Input ~ 0
SPI_MISO_MOSI
Text GLabel 4600 6100 2    50   Input ~ 0
DOCK_1_CE
Text GLabel 4600 6000 2    50   Input ~ 0
DOCK_2_CE
Text GLabel 4600 5900 2    50   Input ~ 0
DOCK_3_CE
$Comp
L Connector_Generic:Conn_01x12 J?
U 1 1 5DFCFA20
P 6150 6450
AR Path="/5E8D0C73/5DFCFA20" Ref="J?"  Part="1" 
AR Path="/5E080FD6/5DFCFA20" Ref="J7"  Part="1" 
F 0 "J7" H 6230 6492 50  0000 L CNN
F 1 "ShoeBConnectors" H 6230 6401 50  0000 L CNN
F 2 "rofi:slipRingBrush" H 6150 6450 50  0001 C CNN
F 3 "~" H 6150 6450 50  0001 C CNN
	1    6150 6450
	1    0    0    -1  
$EndComp
Text GLabel 4750 4850 0    50   Input ~ 0
MOTORS_RX
Text GLabel 5950 6150 0    50   Input ~ 0
MOTORS_RX
Text GLabel 5950 6250 0    50   Input ~ 0
MOTORS_TX
Text GLabel 5950 6550 0    50   Input ~ 0
SPI_SCK
Text GLabel 5950 6650 0    50   Input ~ 0
SPI_MISO_MOSI
Text GLabel 5950 6750 0    50   Input ~ 0
DOCK_4_CE
Text GLabel 5950 6850 0    50   Input ~ 0
DOCK_5_CE
Text GLabel 5950 6950 0    50   Input ~ 0
DOCK_6_CE
Wire Wire Line
	5950 7050 5850 7050
Wire Wire Line
	5850 7050 5850 7150
Text HLabel 4100 6000 0    50   Input ~ 0
INT
Wire Wire Line
	4100 6100 3750 6100
Text HLabel 4750 4750 0    50   Input ~ 0
BATT_VDD
Text HLabel 4100 5900 0    50   Input ~ 0
BATT_VDD
Text HLabel 4750 5300 0    50   Input ~ 0
BATT_VDD
Text HLabel 5950 6050 0    50   Input ~ 0
BATT_VDD
Text HLabel 4100 6300 0    50   Input ~ 0
GND
Text HLabel 4600 6300 2    50   Input ~ 0
GND
Text HLabel 5850 7150 3    50   Input ~ 0
GND
Text HLabel 4750 5200 0    50   Input ~ 0
GND
Text HLabel 4750 4650 0    50   Input ~ 0
GND
Text HLabel 5950 5950 0    50   Input ~ 0
GND
Text HLabel 10350 2500 1    50   Input ~ 0
3V3
Text HLabel 10700 2500 1    50   Input ~ 0
3V3
Text HLabel 9400 1050 1    50   Input ~ 0
3V3
Text HLabel 8650 1150 1    50   Input ~ 0
3V3
Text HLabel 7900 2050 3    50   Input ~ 0
GND
Text HLabel 9400 3750 3    50   Input ~ 0
GND
Text HLabel 10500 1350 3    50   Input ~ 0
GND
Text HLabel 5950 4100 0    50   Input ~ 0
3V3
Text HLabel 7150 3000 3    50   Input ~ 0
GND
Text HLabel 7150 2400 1    50   Input ~ 0
3V3
Text HLabel 4100 1300 3    50   Input ~ 0
GND
Text HLabel 4200 900  0    50   Input ~ 0
3V3
Text HLabel 3950 3250 1    50   Input ~ 0
3V3
Text HLabel 3950 4000 3    50   Input ~ 0
3V3
Text HLabel 1450 3250 3    50   Input ~ 0
GND
Text HLabel 1250 2700 3    50   Input ~ 0
GND
Text HLabel 800  2700 3    50   Input ~ 0
GND
Text HLabel 800  2400 1    50   Input ~ 0
3V3
Text HLabel 1450 5050 0    50   Input ~ 0
3V3
Text HLabel 2700 5050 2    50   Input ~ 0
3V3
Text HLabel 2700 5350 3    50   Input ~ 0
GND
Text HLabel 1450 5400 3    50   Input ~ 0
GND
Text HLabel 1000 5650 0    50   Input ~ 0
GND
Text HLabel 1000 6150 0    50   Input ~ 0
GND
Text HLabel 1000 5950 0    50   Input ~ 0
3V3
Text HLabel 2800 6450 3    50   Input ~ 0
GND
Text HLabel 2100 6650 3    50   Input ~ 0
GND
Text HLabel 6850 1750 0    50   Input ~ 0
USB_C_VDD
Text HLabel 1600 5750 0    50   Input ~ 0
SDA
Text HLabel 1600 5850 0    50   Input ~ 0
SCL
Wire Wire Line
	5950 6450 5600 6450
Text HLabel 5950 6350 0    50   Input ~ 0
INT
Text Label 5600 6450 0    50   ~ 0
EXT
Text Label 3750 6100 0    50   ~ 0
EXT
Wire Wire Line
	7000 1750 6850 1750
Wire Wire Line
	7150 2700 7500 2700
Wire Wire Line
	5750 3050 5900 3050
Wire Wire Line
	5750 3800 5900 3800
Wire Wire Line
	2850 2950 3450 2950
Wire Wire Line
	2850 2850 3450 2850
Wire Wire Line
	4300 2650 3900 2650
Wire Wire Line
	4300 2750 4150 2750
$Comp
L Connector:TestPoint TP1
U 1 1 5DEF79BA
P 3900 2550
F 0 "TP1" H 3958 2668 50  0000 L CNN
F 1 "TP_BRX" H 3958 2577 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D2.0mm" H 4100 2550 50  0001 C CNN
F 3 "~" H 4100 2550 50  0001 C CNN
	1    3900 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2550 3900 2650
Connection ~ 3900 2650
Wire Wire Line
	3900 2650 2850 2650
$Comp
L Connector:TestPoint TP2
U 1 1 5DEFA86F
P 4150 2800
F 0 "TP2" H 4208 2918 50  0000 L CNN
F 1 "TP_BTX" H 4208 2827 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D2.0mm" H 4350 2800 50  0001 C CNN
F 3 "~" H 4350 2800 50  0001 C CNN
	1    4150 2800
	-1   0    0    1   
$EndComp
Wire Wire Line
	4150 2800 4150 2750
Connection ~ 4150 2750
Wire Wire Line
	4150 2750 2850 2750
Wire Wire Line
	3400 3550 3950 3550
Wire Wire Line
	3050 3550 3400 3550
Connection ~ 3400 3550
$Comp
L Connector:TestPoint TP3
U 1 1 5E8B6CC3
P 3400 3450
F 0 "TP3" H 3458 3568 50  0000 L CNN
F 1 "TP_SCL" H 3458 3477 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D2.0mm" H 3600 3450 50  0001 C CNN
F 3 "~" H 3600 3450 50  0001 C CNN
	1    3400 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 3450 3400 3550
Wire Wire Line
	3400 3700 3950 3700
Connection ~ 3400 3700
Wire Wire Line
	2950 3700 3400 3700
Wire Wire Line
	3400 3800 3400 3700
$Comp
L Connector:TestPoint TP4
U 1 1 5E8BA514
P 3400 3800
F 0 "TP4" H 3342 3826 50  0000 R CNN
F 1 "TP_SDA" H 3342 3917 50  0000 R CNN
F 2 "TestPoint:TestPoint_Pad_D2.0mm" H 3600 3800 50  0001 C CNN
F 3 "~" H 3600 3800 50  0001 C CNN
	1    3400 3800
	-1   0    0    1   
$EndComp
$Comp
L Device:R R29
U 1 1 5E6E28E0
P 3950 3850
F 0 "R29" H 4020 3896 50  0000 L CNN
F 1 "3k3" H 4020 3805 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3880 3850 50  0001 C CNN
F 3 "~" H 3950 3850 50  0001 C CNN
F 4 "RR0510P-332-D" H 3950 3850 50  0001 C CNN "#manf"
	1    3950 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R28
U 1 1 5E6E0D40
P 3950 3400
F 0 "R28" H 4020 3446 50  0000 L CNN
F 1 "3k3" H 4020 3355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3880 3400 50  0001 C CNN
F 3 "~" H 3950 3400 50  0001 C CNN
F 4 "RR0510P-332-D" H 3950 3400 50  0001 C CNN "#manf"
	1    3950 3400
	1    0    0    -1  
$EndComp
Text HLabel 4400 3700 2    50   Input ~ 0
SCL
Text GLabel 4400 3800 2    50   Input ~ 0
BIOS_SCL
Text HLabel 4400 3550 2    50   Input ~ 0
SDA
Text GLabel 4400 3450 2    50   Input ~ 0
BIOS_SDA
Text GLabel 2850 2350 2    50   Input ~ 0
MOTORS_TX
$EndSCHEMATC
