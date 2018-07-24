 #include <SPI.h>
#include <SD.h> 
#include <dht.h>          // For Humidity Sensor library DHT
#include "MQ135.h"        // For MQ 135 Sensor (CO2)
#include<string.h>       // For dust Sensor Use

#include<SoftwareSerial.h>  //GPS MODULE
#include<TinyGPS++.h>       //GPS MODULE
#define DEBUG true
#define ESP_BRATE=9600
                                                                                                      //WIFI Module
//******************************************************************************************************************************************************************************************
#include<Timer.h>
Timer t;
static char postUrl[150];
int humi,tem;
void httpGet(String ip, String path, int port=5000);
String GET = "GET /goto?";
String cmd="";                       
     String _timestamp;                                                                                                        // Ublox Neo 6mv1  Defination
//******************************************************************************************************************************************************************************************
static const uint32_t GPSBaud = 9600;
//static const int RXPin=10, TXPin=11; //GPS TX goes into D4 and GPS RX goes into 3
//SoftwareSerial gpsSerial(RXPin, TXPin); // GPS Module Connection
TinyGPSPlus gps;                  // GPS Module Object
//
//
//                                                                      // End OF Ublox Neo 6mv1  Defination
//******************************************************************************************************************************************************************************************
                                 
                                                                      // Dust Sensor Dsm501A Defination
//******************************************************************************************************************************************************************************************
byte buff[2];
int dustpin =44;//DSM501A input D8
unsigned long duration;
unsigned long starttime;
unsigned long endtime;
unsigned long sampletime_ms = 1000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;                      // Size Of Particles 
float concentration = 0;              // Particles Amount
//int i=0;
                                                                        
                                                                      
                                                                      // End Of Dust Sensor Dsm501A Defination
//******************************************************************************************************************************************************************************************

                                                                        
                                                                        //MQ 135 Sensor Defination
//******************************************************************************************************************************************************************************************
                                                                        
#define PIN_MQ135 A5
MQ135 mq135_sensor = MQ135(PIN_MQ135);

float temperature = 21.0; // assume current temperature. Recommended to measure with DHT22
float humidity = 25.0; // assume current humidity. Recommended to measure with DHT22

                                                                        //End OF MQ 135 Sensor Defination
//******************************************************************************************************************************************************************************************



                                                                      // Humidity Sensor Defination (DHT11)
//******************************************************************************************************************************************************************************************
                                                                      
dht DHT;

// if you require to change the pin number, Edit the pin with your arduino pin.
                                                                                    //pind mode D3
#define DHT11_PIN 4  //(Digital Pin)



                                                                      // End of Humidity sensor Code
//******************************************************************************************************************************************************************************************



                                                                                  // MQ 2 Sensor Code Defination of Variables
//******************************************************************************************************************************************************************************************


#define  MQ_PIN                               (0)     // 
    //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (9.86)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO, define which analog input channel you are going to use
#define         RL_VALUE                     (5)
                                                     //which is derived from the chart in datasheet
 #define        CALIBARAION_SAMPLE_TIMES     (500)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (50)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define          READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 
                                                     //normal operation
 
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)
 
float           COCurve[3]  =  {2.3,0.72,-0.34};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15) 
float           SmokeCurve[3] ={2.3,0.53,-0.44};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)                                                     
float           Ro           =  10.98;                 //Ro is initialized to 10 kilo ohms


                                                                          // MQ 2 Sensor End
//******************************************************************************************************************************************************************************************
                                               // MQ 131 Sensor Start
//******************************************************************************************************************************************************************************************

#define  MQ131_PIN                               (8)     // 
    //define the load resistance on the board, in kilo ohms
#define         RO_O3_CLEAN_AIR_FACTOR          (41.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO, define which analog input channel you are going to use
#define         RL_O3_VALUE                     (990)
                                                   //Samples Time
//******************************************************************************************************************************************************************************************
 
#define         GAS_O3                       (3)
 
float           O3Curve[2] ={42.84561841, -1.043297135};    //two points are taken from the curve. 
                                                            //with these two points, a line is formed which is "approximately equivalent" 
                                                            //to the original curve.
                                                            //data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)                                                     
float           RoO           =  10000;                 //Ro is initialized to 10 kilo ohms





  // MQ 131 Sensor End
//******************************************************************************************************************************************************************************************

/*
 * File Handling Code
 */
File dataFile;

//*******************************For Setting Up ALL the Communication between Arduino And GPS Module*******************************************************************************************************************************************

                                                                          
void setup()                // Setting Up the Modules And Sensors
{
  
  Serial.begin(9600);         // Baud Rate is 9600 for serial communication
//  gpsSerial.begin(9600);      // GPS Sensor Baud Rate
 Serial1.begin(GPSBaud);/////GPS Baud rate

 Serial2.begin(9600); ///////ESP Baud rate
  
//*************************************AT COMMANDS HERE FOR MODULE SETUP (NO SERVER COMMANDS HERE)****************************************************************

//  sendAT("AT",2000,DEBUG); //Debug for console
//  sendAT("AT+CWMODE=1",1000,DEBUG); // configure as access point
//  sendAT("AT+CWQAP",1000,DEBUG); // get ip address
//  sendAT("AT+RST",2000,DEBUG); // restart module
//  sendAT("AT+CWJAP=\"ptcl .basement\",\"basement1122\"",1000,DEBUG); // configure for multiple connections
Serial.println("Connecting Wifi....");
 connect_wifi("AT",1000);
 connect_wifi("AT+CWMODE=1",1000);
 connect_wifi("AT+CWQAP",1000);  
 connect_wifi("AT+RST",3000);
 connect_wifi("AT+CWJAP=\"android\",\"12345678\"",2000);
 
  
  Serial.println("Wifi Connected"); // turn on server on port 80
   //t.every(20000, send2server);
//****************************************************************************************************************************************************************
 
  
//*********************************************************Dust Sensor Setup**************************************************************************************************************************

  pinMode(8,INPUT);
  starttime = millis(); 
 
//*********************************************************End Of Dust Sensor Setup*****************************************************************************************************************************
// 
//
  Serial.print("Initializing SD card...");
//
  if (!SD.begin(53)) 
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
//  
//
//                                                       // MQ 2 Sensor
  //******************************************************************************************************************************************************************************************

  Serial.print("Calibrating...\n");                
  Ro = MQCalibration(MQ_PIN);                       //Calibrating the sensor. Please make sure the sensor is in clean air 
                                                    //when you perform the calibration                    
  Serial.print("Calibration is done...\n"); 
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\n");

                                                        // MQ 2 Sensor
 //******************************************************************************************************************************************************************************************
              // MQ 131 Sensor
  //******************************************************************************************************************************************************************************************

  Serial.print("Calibrating...\n");                
  RoO = MQCalibrationO3(MQ131_PIN);                       //Calibrating the sensor. Please make sure the sensor is in clean air 
                                                    //when you perform the calibration                    
  Serial.print("Calibration is done...\n"); 
  Serial.print("Ro=");
  Serial.print(RoO);
  Serial.print("kohm");
  Serial.print("\n");

                                                        // MQ 131 Sensor
 //******************************************************************************************************************************************************************************************
                                
                                
                                                        // MQ135
//******************************************************************************************************************************************************************************************

 /*
  delay(3000);
 
     */ 
                                                  
//  float rzero = gasSensor.getRZero();
Serial.println("MQ135 RZERO Calibration Value : ");
//  Serial.println(rzero);
float rzero = mq135_sensor.getRZero();
// float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
//float resistance = mq135_sensor.getResistance();

                                                        //End Of MQ 135
//******************************************************************************************************************************************************************************************


                                                        //File Initialization
//******************************************************************************************************************************************************************************************

   dataFile= SD.open("datab.csv",FILE_WRITE);
  if(dataFile)
    {
   //     myFile.println(",,"); //If previous Data
      String header="Latitude,Longitude,TimeStamp,CO,SMOKE,O3,Humidity,Temperature,CO2,Dust";
     
//      dataFile.println(header);
//      dataFile.close();
//     Serial.println(header);
    }
    else
    {
//    
// 
         Serial.println("File Exists So writin into it");
// 
      }
                                                          // End Of Initialization
//******************************************************************************************************************************************************************************************

}
 

void loop()
{
int chk = DHT.read11(DHT11_PIN);

  // if the file opened okay, write to it:
float _lat,_lng;
int _month,_day,_year;
int _hour,_min,_sec; 
File myFile = SD.open("datab.csv", FILE_WRITE);
                                                //Start Of GPS availaibility
//******************************************************************************************************************************************************************************************
if(myFile){
while (Serial1.available() > 0)
 {   if (gps.encode(Serial1.read()))
      {
     //   Serial.print(F("Location: "));
  Serial.print(F("\nLocation: ")); 
  //if (gps.location.isValid())
 // {

    _lat=gps.location.lat();
    Serial.print(gps.location.lat(), 6);
    myFile.print(gps.location.lat(),6);
    myFile.print(",");
    Serial.print(F(","));
   
    _lng=gps.location.lng();
    Serial.print(gps.location.lng(), 6);
    myFile.print(gps.location.lng(),6);
    myFile.print(",");
  
  //}
  //else
 // {
  //  Serial.print(F("********"));
  //} 
   Serial.print("   ");   
  Serial.print(F("TimeStamp: "));
  //if (gps.date.isValid())
  //{
  
    _month=gps.date.month();
    Serial.print(gps.date.month());
    myFile.print(gps.date.month());
    myFile.print("/");
    Serial.print(F("/"));
    
    _day=gps.date.day();
    Serial.print(gps.date.day());
    myFile.print(gps.date.day());
    myFile.print("/");
    Serial.print(F("/"));
    
    _year=gps.date.year();
    Serial.print(gps.date.year());
    myFile.print(gps.date.year());
    myFile.print("  ");
 // }
  //else
  //{
   // Serial.print(F("INVALID"));
  //}

  Serial.print(F(" "));
 // if (gps.time.isValid())
  //{
    if (gps.time.hour() < 10) Serial.print(F("0"));
    _hour=gps.time.hour();
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    myFile.print(gps.time.hour());
    myFile.print(":");
    
    if (gps.time.minute() < 10) Serial.print(F("0"));
    _min=gps.time.minute();
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    myFile.print(gps.time.minute());
    myFile.print(":");
    
    if (gps.time.second() < 10) Serial.print(F("0"));
    _sec=gps.time.second();
    Serial.print(gps.time.second());
    myFile.print(gps.time.second());
    myFile.print(",");
    
  //  Serial.print(F("."));
    
//  }
 
  Serial.print("   ");

  
//******************************************************************************************************************************************************************************************
// myFile = SD.open("data.csv", FILE_WRITE);
  
 // if (myFile) 
 // {

//
// Serial.print("Latitude:");
// Serial.print(gps.location.lat());
 //myFile.print(gps.location.lat());
// Serial.print("   ");
 //myFile.print(",");
//
// Serial.print("Longitude:");
// Serial.print(gps.location.lng());
 //myFile.println(gps.location.lng());
// Serial.print("   ");
// myFile.print(",");
// 
// 
//  
//
//
//
                                                //END Of GPS Write CSV
//******************************************************************************************************************************************************************************************


    
                                                //Start Of MQ 2 Write CSV
//******************************************************************************************************************************************************************************************
  float MQ2_VALUE_CO;
  MQ2_VALUE_CO =  MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO);
  Serial.print("CO:"); 
   Serial.print(MQ2_VALUE_CO);
   myFile.print(MQ2_VALUE_CO);
   myFile.print(",");  
   
  Serial.print( "ppm" );
   Serial.print("   "); 
   
  float MQ2_VALUE_SMOKE;
  MQ2_VALUE_SMOKE=MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE);
   
   Serial.print("SMOKE:");  
   Serial.print(MQ2_VALUE_SMOKE );
   myFile.print(MQ2_VALUE_SMOKE );
   myFile.print(","); 
 
  Serial.print( "ppm" );
  Serial.print("   ");
 // myFile.print(",");
 //  Serial.print("\n");
 //myFile.print("\n");
                                                //End Of MQ 2 Write CSV
//******************************************************************************************************************************************************************************************

                                                //Start Of MQ 131 Write CSV
//******************************************************************************************************************************************************************************************

  float  MQ131_VALUE_O3=0;
  MQ131_VALUE_O3=MQGetGasPercentage(MQReadO3(MQ131_PIN)/RoO,GAS_O3);
  Serial.print("O3:"); 
  Serial.print(MQ131_VALUE_O3);
  myFile.print(MQ131_VALUE_O3);
  myFile.print(",");
  
  Serial.print( "ppm" );
   Serial.print("   "); 
  // myFile.print(",");  
                                      //End Of MQ 131 Write CSV
//******************************************************************************************************************************************************************************************




                                                 //Start Of Humidity Write CSV
//******************************************************************************************************************************************************************************************

Serial.print(" Humi:" );
Serial.print(DHT.humidity, 1);
myFile.print(DHT.humidity, 1);
Serial.print("%");
Serial.print("   ");
myFile.print(",");

Serial.print(" Temp:");
Serial.print(DHT.temperature, 1);
myFile.print(DHT.temperature, 1);
Serial.print(" C ");
Serial.print("   ");
myFile.print(",");
//Serial.print("\n");
//myFile.print("\n");

                                                // End Of Humidity Write CSV
//******************************************************************************************************************************************************************************************


                                                //Start Of MQ135 Write CSV
//******************************************************************************************************************************************************************************************

// float ppm = gasSensor.getPPM();
//  delay(1000);
//  digitalWrite(13,HIGH);
 float MQ135_VALUE=0;
  MQ135_VALUE= mq135_sensor.getPPM();
  Serial.print(" CO2:"); 
  Serial.print(MQ135_VALUE);
  myFile.print(MQ135_VALUE);
 
  Serial.print("ppm");
  Serial.print("   ");
  myFile.print(",");
 // Serial.print("\n");
                                            
                                                //End Of MQ135 Write CSV
//******************************************************************************************************************************************************************************************

                                                
                                                //Start Of Dust Sensor Code
//******************************************************************************************************************************************************************************************
float _dust;
 duration = pulseIn(dustpin, HIGH);
 
 lowpulseoccupancy += duration;
 endtime = millis();
//  if ((endtime-starttime) > sampletime_ms)
//  {
    ratio = (lowpulseoccupancy-endtime+starttime + sampletime_ms)/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
//    Serial.print("lowpulseoccupancy:");
//    Serial.print(lowpulseoccupancy);
//    Serial.print("    ratio:");
//    Serial.print(ratio);
//    Serial.print("    DSM501A:");
    
    Serial.print("Concent: ");
    Serial.print(concentration);
    if(concentration)
    myFile.print(concentration);
    myFile.print("\n");
    Serial.print("pcs/0.01cf");
    Serial.print("   ");
//    Serial.print("\n");
    lowpulseoccupancy = 0;
    starttime = millis();
//  } 
_dust=concentration;

//delay(2000);
                                                // END OF Dust Sensor Code CSV
//******************************************************************************************************************************************************************************************

   
//myFile.close();  
//  }
 // else
 // {
    // if the file didn't open, print an error:
  //  Serial.println("error opening data.csv");
  //}

//   if(Serial2.available())
//  {
//    if(Serial2.find("+IPD,"))
//      {
//
////      delay(1000);
//     
//    //  int con_id=wifi_mod.read()-48; //BECAUSE WE NEED TO CONVERT DECIMAL ASCII STARTING FROM 0 INTO READABLE ASCII CHARACTER AT 48
//       int con_id=Serial2.read()-48; //BECAUSE WE NEED TO CONVERT DECIMAL ASCII STARTING FROM 0 INTO READABLE ASCII CHARACTER AT 48
//      String web="<header><title>ECOINSPECT</title></header><body><h1>By Canary_Developers</h1>";
//      
//     web+= "<p><h2>";
//     web+= "Current Value Of CO2 is : ";
//     web+= MQ135_VALUE;
//     web+= "PPM<br/></h2>";
//     web+= "</p>";
//    //_____________________________
//    web+= "<p><h2>";
//     web+= "Current Value Of CO is : ";
//     web+= MQ2_VALUE_CO;
//     web+= "PPM<br/></h2>";
//     web+= "</p>";
//    //_____________________________
//    web+= "<p><h2>";
//     web+= "Current Value Of SMOKE is : ";
//     web+= MQ2_VALUE_SMOKE;
//     web+= "PPM<br/></h2>";
//   //  web+= "</p>";
////    //_____________________________
//    web+= "<p><h2>";
//     web+= "Current Value Of Ozone is : ";
//     web+= MQ131_VALUE_O3;
//     web+= "PPM<br/></h2>";
//   //  web+= "</p>";
////    //_____________________________
//
//
//  //  web+= "<p><h2>";
//   //  web+= "Current Value Of DUST is : ";
//   //  web+= _dust;
//   //  web+= "pcs/0.01cf<br/></h2>";
//  //   web+= "</p>";   
//    //_____________________________
//    web+= "<p><h2>";
//     web+= "Current Value Of TEMPERATURE is : ";
//     web+= DHT.temperature;
//     web+= "C<br/></h2>";
////     web+= "</p>";   
////    //_____________________________
//      web+= "<p><h2>";
//       web+= "Current Value Of humidity is : ";
//       web+= DHT.humidity;
//       web+= "%<br/></h2>";
//     web+= "</p>";
////    //_____________________________
//      web+= "<p><h2>";
//       web+= "Current Location is: ";
//       web+= _lat;
//       web+=",";
//       web+=_lng;
//       web+= "<br/></h2>";
//     web+= "</p>";
////    //_____________________________
//      web+= "<p><h2>";
//       web+= "Current Date is : ";
//       web+= _month;
//       web+="/";
//       web+=_day;
//       web+="/";
//       web+=_year;
//       web+= "<br/></h2>";
//       web+= "</p>";
//
////    //_____________________________
//         web+= "<p><h2>";
//       web+= "Current Time is : ";
//       web+= _hour;
//       web+=":";
//       web+=_min;
//       web+=":";
//       web+=_sec;
//       web+= "</p></body>";
////-----------------------------------------------------------------------------      
//      String cipsend="AT+CIPSEND="; //For Transmitting data over TCP
//      cipsend+=con_id;
//      cipsend+=",";
//      cipsend+=web.length();
//
//      sendAT(cipsend,1000,DEBUG);
//      sendAT(web,1000,DEBUG);
//      
////      cipsend="AT+CIPSEND="; //For Transmitting data over TCP
////      cipsend+=con_id;
////      cipsend+=",";
////      cipsend+=web.length();
//
//      String close_con="AT+CIPCLOSE=";
//      close_con+=con_id;    // APPENDING COMMAND WITH CONN ID
//      sendAT(close_con,3000,DEBUG);
//      }
//  }

Serial.print(concentration);
//_timestamp+=_month;
//_timestamp+="/";
//_timestamp+=_day;
//_timestamp+="/";
//_timestamp+=_year;
//_timestamp+=" ";
//_timestamp+=_hour;
//_timestamp+=":";
//_timestamp+=_min;
//_timestamp+=":";
//_timestamp+=_sec;
cmd = GET + "_latitude=" + String(_lat) +"&_longitude="+ String(_lng)
+"&co="+String(MQ2_VALUE_CO)+"&smoke="+String(MQ2_VALUE_SMOKE)
+"&O3="+String(MQ131_VALUE_O3)+"&humidity="+String(DHT.humidity)+"&temperature="
+String(DHT.temperature)+"&co2="+String(MQ135_VALUE)+"&dust="+String(_dust);
httpGet("192.168.43.213", cmd, 5000);  

delay(3000);
}

 }
 
 myFile.close();  
 
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening data.csv");
  }
  //delay(2000);
}
//******************************************************************************************************************************************************************************************

//void send2server()
//{
//  
//  char tempStr[8];
//  char humidStr[8];
//  Serial.println(humidStr);
//  dtostrf(tem, 5, 3, tempStr);
//  dtostrf(humi, 5, 3, humidStr);
//  //sprintf(postUrl, "/?name=%s&humi=%s","Ime",humidStr);
//  cmd = GET + "name=" + "sad" +"&humi="+ "sa";
// // sprintf(postUrl, "/",api_key,humidStr,tempStr);
//  httpGet("192.168.10.2", cmd, 5000);
//}

//GET http://ecoinspect.com/goto?latitude=0&longitude=0......

void httpGet(String ip, String path, int port)
{
  int resp;
  String atHttpGetCmd =path+" HTTP/1.0\r\n\r\n";
  //AT+CIPSTART="TCP","192.168.20.200",80
  String atTcpPortConnectCmd = "AT+CIPSTART=\"TCP\",\""+ip+"\","+port+"";
  connect_wifi(atTcpPortConnectCmd,1000);
  int len = atHttpGetCmd.length();
  String atSendCmd = "AT+CIPSEND=";
  atSendCmd+=len;
  connect_wifi(atSendCmd,1000);
  connect_wifi(atHttpGetCmd,1000);
}

void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    
    Serial.println(cmd);
    Serial2.println(cmd); 
    while(Serial2.available())
    {
      if(Serial2.find("OK"))

      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  {
   Serial.println("OK");
        
  }
  else
  {
   Serial.println("Error");
        
  }
}








//******************************************************************************************************************************************************************************************

String sendAT(String command, const int timeout, boolean debug)
{
    String response = ""; //For READING THE RESPONSE
//    wifi_mod.println(command); // TRANSMITTING THE RECEIVED CAMMAND TO ESP
    
    Serial2.println(command); // TRANSMITTING THE RECEIVED CAMMAND TO ESP  
    long int time = millis(); // FOR DELAY BETWEEN COMMAND AND RESPONSE
    
    while( (time+timeout) > millis())
    {
//      while(wifi_mod.available())
//      {
      while(Serial2.available())
      {

        
        // AS THE SERIAL PRINT CH BY CH SO READING THE RESPONSE IN SAME WAY 
//        char read_esp = wifi_mod.read(); //ITERATING THROUGH THE OUTPUT.
          
          char read_esp = Serial2.read(); //ITERATING THROUGH THE OUTPUT.
          response+=read_esp;
      }  
    }
    if(debug)
    {
      Serial.print(response);
    }
    return response;
}
















                                                  // MQ 2 Sensor Defination Of Functions

//******************************************************************************************************************************************************************************************


 
/*MQResistanceCalculation 
Input:   raw_adc - raw value read from adc, which represents the voltage
Output:  the calculated sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
*/ 
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}
float MQResistanceCalculationO3(int raw_adc)
{
  return ( ((float)RL_O3_VALUE*(1023-raw_adc)/raw_adc));
}
 
/* MQCalibration 
Input:   mq_pin - analog channel
Output:  Ro of the sensor
Remarks: This function assumes that the sensor is in clean air. It use  
         MQResistanceCalculation to calculates the sensor resistance in clean air 
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about 
         10, which differs slightly between different sensors.
*/ 
float MQCalibration(int mq_pin)
{
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   //calculate the average value
 
  val = val/RO_CLEAN_AIR_FACTOR;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                        //according to the chart in the datasheet 
 
  return val; 
}

float MQCalibrationO3(int mq_pin)
{
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculationO3(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   //calculate the average value
 
  val = val/RO_O3_CLEAN_AIR_FACTOR;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                        //according to the chart in the datasheet 
 
  return val; 
}
/*  MQRead 
Input:   mq_pin - analog channel
Output:  Rs of the sensor
Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
*/ 
float MQRead(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  
}
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}
float MQReadO3(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculationO3(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  
}
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
} 
/*  MQGetGasPercentage 
Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
Output:  ppm of the target gas
Remarks: This function passes different curves to the MQGetPercentage function which 
         calculates the ppm (parts per million) of the target gas.
*/ 
float MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  
   if ( gas_id == GAS_CO ) {
     return MQGetPercentage(rs_ro_ratio,COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
     return MQGetPercentage(rs_ro_ratio,SmokeCurve);
  } else  if ( gas_id == GAS_O3 ) 
   {
     return MQGetPercentageO3(rs_ro_ratio,O3Curve);
  }    
 
  return 0;
}
 
/*  MQGetPercentage 

Input:   rs_ro_ratio - Rs divided by Ro
         pcurve      - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(rs_ro_ratio) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
*/ 
float  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}

float  MQGetPercentageO3(float rs_ro_ratio, float *pcurve)
{
   return (double)(pcurve[0] * pow(((double)rs_ro_ratio ), pcurve[1]));
}
    
                                                                              
                                                                              //MQ 2 End Of Functions
//******************************************************************************************************************************************************************************************

