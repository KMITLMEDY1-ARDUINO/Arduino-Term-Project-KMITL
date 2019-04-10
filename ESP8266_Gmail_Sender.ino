#include <ESP8266WiFi.h>
#include "Gsender.h"

#pragma region Globals
const char* ssid = "@KMITL";                           // WIFI network name
const char* username ="61150015";                        // WIFI network username
const char* password = "Nb2lwqfu";                       // WIFI network password
uint8_t connection_state = 0;                    // Connected to WIFI or not
uint16_t reconnect_interval = 10000;             // If not connected wait time to try again
#pragma endregion Globals

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);  
        Serial.println(nSSID);
    } else {
        WiFi.begin(ssid, password);
        Serial.println(ssid);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}

void setup(void)
{
  // start serial port for temperature probe
  Serial.begin(9600); 
  // Start up the temp probe
  sensors.begin();
  // start serial port for WiFi module
    Serial.begin(115200);
    connection_state = WiFiConnect();
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect

    Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
    String subject = "Paitient Temperature Monitor!";
    if(gsender->Subject(subject)->Send("y1kmitlmed.arduino@gmail.com", "Patient Alert")) {
        Serial.println("Patient Temp Alert");
    } else {
        Serial.print("Error sending message: ");
        Serial.println(gsender->getError());
    }
}

void loop(void){}
{ 
  // Get temperature reading
  sensors.requestTemperatures();
  // Get temperature reading in Celsius from first IC on wire
  const float tempC = sensors.getTempCByIndex(0);
  // If temperature goes out of regulation, send alert email
  if (tempC < 23.9 || tempC > 32) {
    if (tempC < 23.9) {
      gsender->Subject(subject)->Send("boris.on@live.com", "Low Temp Alert: " + tempC);
    } else {
      gsender->Subject(subject)->Send("boris.on@live.com", "High Temp Alert: " + tempC);
    }
  }
  // Delay reading by 15 minute
  delay(900000);
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
