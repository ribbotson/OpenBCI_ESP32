# OpenBCI_ESP32
Clone of OpenBCI Brain Computer Interface Hardware using ESP32 Processor

>OpenBCI Cyton is a well performing Open source hardware and software design for a Brain Computer Interface. It was designed and developed over the last 7 years and used in many experimants and applications.
>This clone is designed because some of the parts used in the Cyton hardware are now obsolete and difficult to source. Also it is hoped that a new design can offer higher throughput. 
The design will continue to use the TI ADS1299 Analog Front end so perfomance should be the same as Cyton. The option to have either 8 or 16 channels will be retained.

## Changes for ESP Version

**Replace existing PIC32MX processor with ESP32 processor with integral WiFi and Bluetooth**

>The ESP32 was chosen because it has integral WiFi and bluetooth and so replaces the obsolete RFD22301 bluetooth module and the WiFi board. The ESP32 is better supported under Arduino environment than PIC and RFD device. Raspberry Pi Zero W was considered as it has more processing power, but was excluded due to much higher power consumption so not suitable for battery operation.

**The design is split into two plugable modules**

>The base module supports the Processor, Micro SD Card, Accelerometer, and Power Supplies. One or two input modules are the stacked below to add 8 or 16 electrode inputs. The first base module will use a prebuilt ESP32 DEVkit C module to make firmware development easier. A later version could place as smaller ESP32 module directly on the base module.

**Boards only have components on one side**

>To simplify construction the new boards only have components mounted on one side of the board. Both boards are 2 layer PCB.

**Some components are to allow use of more common generic devices**

>Use of more common generic components should help with procurement especially if using outsourced PCB Assembly


![image](/OpenBCI_ESP32_BASE/esp32-devkitc.jpg "ESP32 DEVKIT C")

![image](/OpenBCI_ESP32_BASE/OpenBCI_ESP32_BASE.JPG "OpenBCI ESP32 Base")

![image](/OpenBCI_ESP32_INPUT/OpenBCI_ESP32_INPUT.JPG "OpenBCI ESP32 Input")

