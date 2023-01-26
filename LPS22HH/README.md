## LPS22HH Pressure Sensor Driver

This is a generic driver for atsamv71 SPI0 and SPI1 peripherals based on [LPS22HH datasheet](https://gr.mouser.com/datasheet/2/389/lps22hh-1395924.pdf).

To test the driver SPI0 from atsamv71q21b dev board can used, as shown below

| SPI0        | LPS22H | 
|-------------|:------:|
| MOSI (PD21) |   SI   |
| MISO (PD20) |   SO   | 
| CS0 (PB2)   |  SSN   |
| NPCL (PD22) |  CLK   |

where NPCS0 (PB2) was used as a GPIO pin.

Basic **LPS22HH** Class functions
- `LPS22HH()`: Set the chip select pin.
- `getStatus()`: Get the status of the sensor.    
- `getTemperature()`: Get the sensor's temperature value.
- `getPressure()`: Get the calculated pressure value.
- `temperatureDataAvailableCheck()`: Check if new temperature data are ready.
- `pressureDataAvailableCheck()`: Check if new pressure data are ready.




