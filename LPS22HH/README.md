## LPS22HH Pressure Sensor Driver

This is a generic driver for atsamv71 SPI peripheral based on the [LPS22HH datasheet](https://gr.mouser.com/datasheet/2/389/lps22hh-1395924.pdf).

To test the driver SPI0 from atsamv71q21b dev board can used, as shown below

| SPI0        | LPS22H | 
|-------------|:------:|
| MOSI (PD21) |   SDI   |
| MISO (PD20) |   SDO   | 
| CS0 (PA05)   |  SSN   |
| SPCK (PD22) |  CLK   |

where NPCS0 (PA05) was used as a GPIO pin.

All the pins are present on the J500 connector.

The [STEVAL-MKI192V1](https://gr.mouser.com/ProductDetail/STMicroelectronics/STEVAL-MKI192V1?qs=qSfuJ%252Bfl%2Fd7Y3a%2FNbcbDRg%3D%3D) evaluation was used to develop the driver.

Basic **LPS22HH** Class functions
- `LPS22HH()`: Set the chip select pin.
- `getStatus()`: Get the status of the sensor.    
- `getTemperature()`: Get the sensor's temperature value.
- `getPressure()`: Get the calculated pressure value.
- `temperatureDataAvailableCheck()`: Check if new temperature data are ready.
- `pressureDataAvailableCheck()`: Check if new pressure data are ready.




