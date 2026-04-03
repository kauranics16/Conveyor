#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <PubSubClient.h>
//#include <ArduinoJson.h>

#define PROX_SENSOR 6
Preferences preferences;
AsyncWebServer server(80);

#define ssid_ap "ESP32-Setup"
#define password_ap "12345678"

String storedSSID;
String storedPass;

WiFiClient espClient;
PubSubClient mqtt_client(espClient);



//sensor Global variables
uint8_t storedsensor1offon;
uint8_t storedsensor2offon;
uint8_t storedSensor1nonc;
uint8_t storedSensor2nonc;
uint8_t storedSensorShiftchoise;




long lastMsg = 0;
long lastwificheck = 0;
long lastmqttcheck = 0;

int value = 0;
volatile int32_t proxi_counter = 0;//for both proximity
volatile int32_t proxi_counter1 = 0;
volatile int32_t proxi_counter2 = 0;
volatile bool detected_proxi1 = false;
volatile bool detected_proxi2 = false;
//volatile bool detected_proxi3 = false;// counter reset mode
unsigned long time1=0;
unsigned long time2=0;
unsigned long TimeDifference=0;
volatile bool WaitingForObjectDetectedSensor1=false;


unsigned long outputStartTime = 0;
unsigned long outputStartTimeNeg = 0;//buzzer on timing for negative output
bool outputIsActive = false;
bool outputIsActiveNeg = false;
//unsigned long outputStartResetTime = 0;
//bool outputIsReset = false;


bool shouldRestartESP = false;//used for esp restart input uis given through webserver

volatile bool IfNeedAcceptableTime = false;


const char* htmlForm = R"rawliteral(
<!DOCTYPE HTML><html><head>
 <title>Wi-Fi Setup</title>
 <meta name="viewport" content="width=device-width, initial-scale=1">
</head><body>
 <h2>Configure Wi-Fi</h2>
 <form action="/save" method="POST">
 InputSensor1:<br><input type="text" name="input1"><br>
 <input type="radio"  name="myGroup" value=1> ON Sensor1
 <input type="radio"  name="myGroup" value=2> OFF Sensor1<br>
 <input type="radio"  name="myGroup2" value=1> NO
 <input type="radio"  name="myGroup2" value=2> NC<br>
 Sensor1 Mode:<br><input type="text" name="sensormode1"><br>
 InputSensor2:<br><input type="text" name="input2"><br>
 <input type="radio"  name="myGroup1" value=1> ON Sensor2
 <input type="radio"  name="myGroup1" value=2> OFF Sensor2
 <input type="radio"  name="myGroup1" value=3> As Reset Button<br>
 <input type="radio"  name="myGroup3" value=1> NO
 <input type="radio"  name="myGroup3" value=2> NC<br>
 Sensor2 Mode:<br><input type="text" name="sensormode2"><br>
 Shift Choice: <input type="radio"  name="Shifting" value=1> Sensor1 to Sensor2 Right Shift
 <input type="radio"  name="Shifting" value=2> Sensor2 to Sensor1 Left Shift<br>
 Proximity Individual Counter: <input type="radio"  name="proxicounteronoff" value=1>ON
 <input type="radio"  name="proxicounteronoff" value=2>OFF<br>
 Input Acceptable Time in Seconds:<br><input type="number" name="seconds"><br>
 <label for="cars">Choose Output on or off:</label>
 <select name="onoff" id="onoff">
    <option value="ON">ON</option>
    <option value="OFF">"OFF"</option>
    
 </select><br>
 SSID:<br><input type="text" name="ssid"><br>
 Password:<br><input type="password" name="pass"><br><br>
 <input type="submit" value="Save">
 </form>
</body></html>)rawliteral";
 
void setup() {
  Serial.begin(115200);
  
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

   
  ProximityConfig();// configure the proximity and output ONOFF condition by user
  Outputonoff();
  

  // 2. Decide Connection Strategy
  WifiConfig();
  
  mqtt_client.setServer("broker.emqx.io", 1883);
  
  WebServerConfig();
 

}

void loop() {
 // proximity_config();// configure the proximity
  if (shouldRestartESP == true) {

    //delay(5000);
    Serial.println("Restart!!!!, Disconnection.........................................................................");
    ESP.restart(); // This avoids the "wifi:sta is connecting" error
    //delay(2000);
    return;
  }
  
  if (millis() - lastwificheck > 5000) {
    if (WiFi.status() != WL_CONNECTED) {
       Serial.println("wifi Diconneted");
       WiFi.mode(WIFI_AP_STA);
       WiFi.softAP(ssid_ap, password_ap);
       
    }
    else{
      Serial.println("wifi connected");
      
      // Serial.println("try connecting the wifi connection again and ap mode on");
       //Optional: keep AP running even when wifi disconnected 
       //WiFi.mode(WIFI_AP_STA); //station ap mMode on
       
       //WiFi.begin(storedSSID.c_str(), storedPass.c_str());
      
    }
    lastwificheck = millis();
    
      
  }
  
 
  if (!mqtt_client.connected()) {
    MqttReconnectCheck();
    //Serial.println("mqttttttffffff");
  }
  else{
    mqtt_client.loop();
    // Serial.println("mqttttttffffff");
    MqttPublish_data();
   //Serial.println("----------------------------------------------------------------+");
     
  }

  if (outputIsActive && (micros() - outputStartTime >= 100000)) {//nbuzzer on for 100 mili seconds
      digitalWrite(10, LOW);
      outputIsActive = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }

  if (outputIsActiveNeg && (micros() - outputStartTimeNeg >= 500000)) {//nbuzzer 500 mili seconds
      digitalWrite(10, LOW);
      outputIsActiveNeg = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }
  
  
  /*if (outputIsReset && (micros() - outputStartResetTime >= 500000)) {//nbuzzer 500 mili seconds
      digitalWrite(10, LOW);
      outputIsActiveNeg = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }*/


}
