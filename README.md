# About the project
This project is an OBD2 simulator. I did this project so I could learn the basics of OBD2 and do something with an Arduino.

# Schematic
![schematic created in fritzing](https://raw.githubusercontent.com/fabiankuffer/arduino-obd2-sim-code/master/images/schematic.png)
In the picture you can see all the components I used. There are 9 potentiometers available to change the PID values. There is a screen to view the PIDs being sent. Furthermore there is an EEPROM which stores the titles and the PID. The multiplexer is used because the Arduino does not have enough analog inputs to connect all potentiometers. The two buttons are available to switch through the values on the display. A switch is provided to switch the backlight on and off. The second switch is for future use only. On top of the Arduino Uno is a CAN-Shield. I used a clone of the SeedStudio CAN-Shield v1.2 from eBay.
