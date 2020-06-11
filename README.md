# Arduino_Air_Pollution_Monitoring_Device

## Overview
This sketch contains the code for enabling the arduino mega to gather the air pollution
data from the environment, tag the data with GPS cordinates,timestamp. Save the incoming data into the SD card attached and at the
same time transfer the incoming data on real time basis to my web server i.e **Node.js REST server**. For the visualization of data 
**GOOGLE Maps API** is used  **React SPA**. For visualization of data heatmap layer, Data points with Info box and circle plot was used.
You can view the demo Web site at [Environment Inspector](https://ecinspect.herokuapp.com). The visualized data is of **Peshawar,Pakistan**.
This is currently the **beta version**. Also as  hardware devices are now offline so you can't view the real time data now.
Complete code of Web APP will be shared .

The code was written for arduino mega but it was also tested on __arduino uno__.
For __Arduino Uno__ You need to change the pins of __SD-Module,GPS module,ESP8266E__ with respect to Arduino Uno.
Sensors And Modules used are following.
1. Arduino Mega
2. MQ-131 (For Ozone)
3. MQ-2   (Gas and Smoke)
4. MQ-135 ( For CO2)
5. DHT-11 (Temperature and Humidity)
6. DSM- 501 A (For Dust)
6. Ublox Neo 6m v2 GPS Module
7. Micro SD Card Module
8. ESP8266-12-E Module (For Wifi)


Their are two sketch files in this repository one contains the code for saving data on SD Card and the other one doesn't.

**The formula for calculating the ppm values might require improvements**

The libraries used are listed below.

[GeorgeK](https://github.com/GeorgK/MQ135)

[JChristensen](https://github.com/JChristensen/Timer)

[mikalhart](https://github.com/mikalhart/TinyGPSPlus)
