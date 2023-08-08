# Flow Valve Driver

Eight LHLA0542311H flow valves are utilized, which are 2-port latching solenoid valves manufactured by the Lee Co.

Each valves is driven using an H-bridge and 2 GPIOs of the MCU to permit or block the fluid from going through the valve. The H-bridge switches the polarity of its output to control the electromagnet inside the valve. The valves, thus the H-bridges, are connected to 5V but can be controlled with either a 5V or 3V3 MCU output.
