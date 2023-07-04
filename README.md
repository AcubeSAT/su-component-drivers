# SU Component Drivers for MCU (ATSAMV71Q21B)

This repository will be added as a git-submodule.

SpaceDot members: don't forget to check the guidelines [here](https://mm.spacedot.gr/acubesat/pl/i9qt3t7cyirbxysramwzybk1se)

## Drivers Implemented

- [Camera Driver](https://gitlab.com/acubesat/su/on-board-software/su-component-drivers/-/tree/master/U3VCamDriver) and its [wikipage](https://gitlab.com/groups/acubesat/su/on-board-software/-/wikis/USB3-Vision-Camera-Driver)
- SHT3x-DIS Driver (only the single-shot mode), [Datasheet](https://sensirion.
  com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.
  pdf)

## Playing with Hardware

### SHT3x-DIS
If you are using the SHT3x-DIS evaluation board along with a SAMV71 Xplained Ultra evaluation kit follow, the pinout.
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
