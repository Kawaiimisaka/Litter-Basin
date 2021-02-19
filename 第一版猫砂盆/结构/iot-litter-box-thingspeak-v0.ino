// DIY IoT Cat Litter Box V0.0 by Igor Fonseca @2019
// Saves weights and status online on Thingspeak.com
// Reads and displays current time from an internet server
//
// based on Example using the SparkFun HX711 breakout board with a scale by Nathan Seidle 2017
//
// All text above must be included in any redistribution.

/************************ Thingspeak  Configuration ************************/

// visit thingspeak.com if you need to create an account or if you need your write key.
#include "ThingSpeak.h"
unsigned long myChannelNumber = XXXXX;
const char * apiKey = "XXXXX";                   //  Enter your Write API key from ThingSpeak
const char * myReadAPIKey = "XXXXX";
const char* server = "api.thingspeak.com";                  //   "184.106.153.149" or api.thingspeak.com

/************************ WIFI Configuration ************************/

#include <WiFi.h>                                           // Wi-Fi
#include <WiFiClient.h>                                     // Wi-Fi client
const char *ssid =  "XXXXX";                              // replace with your wifi ssid and wpa2 key
const char *pass =  "XXXXX";
WiFiClient client;


/************************ Define IO pins ************************/

#include <HX711.h>
#define calibration_factor -23750.0                         //-7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DOUT 5                                              // Pin connected to HX711 data output pin
#define CLK  2                                              // Pin connected to HX711 clk pin
HX711 scale;                                                // Scale initialization

#define NUM_MEASUREMENTS 10 // Number of measurements
#define THRESHOLD 0.2  // Restart averaging if the weight changes more than 0.2 kg.

float weight = 0;
float sandWeight = 0;
float weightbox = 16.35;  // Weight of empty box
float cat1MinWeight = 3.0;  // expected minimum weight for cat #1
float cat1MaxWeight = 4.3;  // expected maximum weight for cat #1
float cat2MinWeight = 4.4;  // expected minimum weight for cat #2
long cat1Uses = 0;          // number of uses of the box in a day
long cat2Uses = 0;          // number of uses of the box in a day

#define reed_switch_PIN 25
int reed_switch_value = 1;
int prev_reed_switch_value = 1;

/************************ Clock settings ************************/

String TimeDate = ""; // Variable used to store the data received from the server
int hours;    // Current hour
int minutes;  // Current minute
int seconds;  // Current second
int seconds0; // Last time the clock was updated
#define TIMEZONE  3 // Current time zone. Used for correction of server time


void setup() {

  // Start the serial connection
  Serial.begin(115200);
  Serial.println("Testing serial communication...");

  pinMode(reed_switch_PIN, INPUT_PULLUP);

  scale.begin(DOUT, CLK);  // Scale initialization
  // Scale calibration
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch

  // Connect to WiFi router
  Serial.println("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

  ThingSpeak.begin(client); // Initialize ThingSpeak

  // Read saved values
  cat1Uses = ThingSpeak.readLongField(myChannelNumber, 5, myReadAPIKey);  
  cat2Uses = ThingSpeak.readLongField(myChannelNumber, 6, myReadAPIKey);  
  sandWeight = ThingSpeak.readLongField(myChannelNumber, 4, myReadAPIKey);  
  
}

void loop() {
  
  getTime();  // get current time

  // restart daily parameters
  if ((hours == 0) && (minutes == 0)) {
    cat1Uses = 0;
    ThingSpeak.setField(5, cat1Uses);
    cat2Uses = 0;
    ThingSpeak.setField(6, cat2Uses);
  }
  
  // read Reed Switch
  reed_switch_value = digitalRead(reed_switch_PIN);
  
  // prepare to updated status of the box on Thingspeak
  if (reed_switch_value == 0) {
    ThingSpeak.setField(3, 1);
  }
  else {
    ThingSpeak.setField(3, 0);
  }

  // check if something changed
  if ((reed_switch_value == 1) && (prev_reed_switch_value == 1)) {
    Serial.println("Cleaning mode");                                  // On cleaning mode the controller will stop measuring the weight (do nothing)
  }
  if ((reed_switch_value == 0) && (prev_reed_switch_value == 1)) {
    Serial.println("Leaving cleaning mode");                          // When it leaves cleaning mode, it shall wait some seconds and measure the weight of sand in the litter box
    delay (5000);
    float weight0 = scale.get_units() - weightbox;
    float avgweight = 0;
    for (int i = 0; i <= NUM_MEASUREMENTS; i++) {  // Takes several measurements
          weight = scale.get_units() - weightbox;
          delay(100);
          avgweight = avgweight + weight;
          if (abs(weight - weight0) > THRESHOLD) {
            avgweight = 0;
            i = 0;
          }
          weight0 = weight;
        }
        sandWeight = avgweight / NUM_MEASUREMENTS;
    Serial.print("Measured sand weight: ");
    Serial.print(sandWeight, 2);
    Serial.println(" kg");
    ThingSpeak.setField(4, sandWeight);
  }
  if ((reed_switch_value == 0) && (prev_reed_switch_value == 0)) {
    Serial.println("Running mode");                                   // On running mode the controller will measure the weight of the cats
    weight = scale.get_units() - weightbox - sandWeight;
    if (weight > cat1MinWeight) { // cat detected
      Serial.println("Cat detected!");
      float weight0 = scale.get_units() - weightbox - sandWeight;
      float avgweight = 0;
      for (int i = 0; i <= NUM_MEASUREMENTS; i++) {  // Takes several measurements
          weight = scale.get_units() - weightbox - sandWeight;
          delay(100);
          avgweight = avgweight + weight;
          if (abs(weight - weight0) > THRESHOLD) {
            avgweight = 0;
            i = 0;
          }
          weight0 = weight;
        }
        avgweight = avgweight / NUM_MEASUREMENTS; // Calculate average weight
        Serial.print("Measured weight: ");
        Serial.print(avgweight, 2);
        Serial.println(" kg");
        if ((avgweight >= cat1MinWeight) && (avgweight <= cat1MaxWeight)) { // cat #1 detected
          Serial.println("Cat #1");
          ThingSpeak.setField(1, avgweight); // set measured value to cat #1
          cat1Uses += 1;
          ThingSpeak.setField(5, cat1Uses);
        }
        if (avgweight >= cat2MinWeight) { // cat #2 detected
          Serial.println("Cat #2");
          ThingSpeak.setField(2, avgweight); // set measured value to cat #2  
          cat2Uses += 1;
          ThingSpeak.setField(6, cat2Uses);
        }

      // wait while there's someone on the scale
      while ((scale.get_units() - weightbox - sandWeight) > cat1MinWeight) {
        delay(1000);
      }
    }
  }
  
  if ((reed_switch_value == 1) && (prev_reed_switch_value == 0)) {
    Serial.println("Leaving running mode");                           // When it leaves running mode, it will stop measurint the weight (do nothing)
  }

  // save values do Thingspeak
  if (client.connect(server,80))                 
  {  
    int x = ThingSpeak.writeFields(myChannelNumber, apiKey);
  }
  client.stop();

  prev_reed_switch_value = reed_switch_value; // prepare for next scan

  Serial.println("Waiting...");
  delay(5000);    
}



//Get current time Google server
void getTime() {

  // connect server
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) {
    Serial.println("connection failed, retrying...");
  }
  client.print("HEAD / HTTP/1.1\r\n\r\n");

  delay(500); //wait a few milisseconds for incoming message

  //if there is an incoming message
  if(client.available()){
    while(client.available()){
      if (client.read() == '\n') {    
        if (client.read() == 'D') {    
          if (client.read() == 'a') {    
            if (client.read() == 't') {    
              if (client.read() == 'e') {    
                if (client.read() == ':') {    
                  client.read();
                  String theDate = client.readStringUntil('\r');
                  client.stop();

                  TimeDate = theDate.substring(7);
                  String strCurrentHour = theDate.substring(17,19);
                  String strCurrentMinute = theDate.substring(20,22);
                  String strCurrentSecond = theDate.substring(23,25);
                  // store received data on global variables
                  hours = strCurrentHour.toInt();
                  hours = hours - TIMEZONE; // compensate for TIMEZONE
                  if (hours < 0) {
                    hours = hours + 24;
                  }
                  minutes = strCurrentMinute.toInt();
                  seconds = strCurrentSecond.toInt();
                }
              }
            }
          }
        }
      }
    }
  }
  //if no message was received (an issue with the Wi-Fi connection, for instance)
  else{
    seconds = 0;
    minutes += 1;
    if (minutes > 59) {
      minutes = 0;
      hours += 1;
      if (hours > 23) {
        hours = 0;
      }
    }
  }

  // serial output current time
  Serial.print("Current time: ");
  if (hours < 10) {
   Serial.print("0"); 
  }
  Serial.print(hours);
  Serial.print(":");
  if (minutes < 10) {
   Serial.print("0"); 
  }
  Serial.println(minutes);
  
}
