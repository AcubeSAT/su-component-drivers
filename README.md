# SU Component Drivers for MCU (ATSAMV71Q21B)

This repository will be added as a git-submodule.

SpaceDot members: don't forget to check the guidelines [here](https://mm.spacedot.gr/acubesat/pl/i9qt3t7cyirbxysramwzybk1se)

# Drivers Implemented

## Camera Driver

- [Camera Driver](https://gitlab.com/acubesat/su/on-board-software/su-component-drivers/-/tree/master/U3VCamDriver) and its [wikipage](https://gitlab.com/groups/acubesat/su/on-board-software/-/wikis/USB3-Vision-Camera-Driver)

## Humidity Sensor Driver

SHT3x-DIS Driver (only the single-shot mode)

[Datasheet](https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf)

If you are using the SHT3x-DIS evaluation board along with a SAMV71 Xplained Ultra evaluation kit, follow the pinout below.
You can use the `TW0` pins on the J500 connector or the `TW2` pins on the J505 connector.


| Pin Number | Name   | Comments                                                                                                                              |
| ------ |--------|---------------------------------------------------------------------------------------------------------------------------------------|
| 1 | SDA    | Connect to TWD of ATSAMV71                                                                                                            |
| 2 | ADDR   | Connect to GND pin of ATSAMV71 or use a breadboard for a common GND                                                                   |
| 3 | Alert  | Can be left floating. If required for testing purposes, connect to any GPIO, enable the GPIO as input and activate the GPIO interrupt |
| 4 | SCL    | Connect to TWCK of ATSAMV71                                                                                                           |
| 5 | VDD    | Connect to 3V3 of the ATSAMV71 power headers                                                                                          |
| 6 | nRESET | Can be left floating. If required for testing purposes, connect to any GPIO and activate the GPIO as output                           |
| 7 | R      | Connect to GND pin of ATSAMV71 or to the common GND                                                                                   |
| 8 | VSS    | Connect to GND pin of ATSAMV71 or to the common GND                                                                                                         |

## Flow Valve Driver

Eight LHLA0542311H flow valves are utilized, which are 2-port latching solenoid valves manufactured by the Lee Co.

Each valve is driven using an H-bridge and 2 GPIOs of the MCU to permit or block the fluid from going through the valve. The H-bridge switches the polarity of its output to control the electromagnet inside the valve. The valves, thus the H-bridges, are connected to 5V but can be controlled with either a 5V or 3V3 MCU output.

The flow valve driver breakout board can be found [here](https://gitlab.com/acubesat/su/hardware/breakout-boards/-/tree/main/TLC5940-47?
ref_type=heads) as part of the TLC5940-47 breakout PCB.

| Pin Name     | Comments                                                                                                                                           |
|--------------|----------------------------------------------------------------------------------------------------------------------------------------------------|
| FWD          | Test Point, can be left unconnected                                                                                                                |
| REV          | Test Point, can be left unconnected                                                                                                                |
| Close        | The GPIO used for closing the valve                                                                                                                |
| Open         | The GPIO used for opening the valve                                                                                                                |
| 5V           | The power supply voltage to control the valve, either connect to a power supply or a pin from a MCU evaluation board like the SAMV71 Xplained Ultra |
| GND          | Common ground between the breakout board, the MCU board and the external power supply if applicable                                                |
| Fuse/No Fuse | Select whether the output of the H-bridge should be protected with a fuse or the whether the fuse should be bypassed                               |                                                                                             

**IMPORTANT NOTES**
- The *Positive* output (`VAL+`) of the h-bridge must be connected to the *right* pin of the valve and the *Negative* output (`VAL-`) to the *left* 
  pin of 
  the 
  valve when the pins of the valve are phasing the ground and not the ceiling.
- Driving the Open and Close pins High simultaneously will result in short circuit and damage to the H-bridge.
