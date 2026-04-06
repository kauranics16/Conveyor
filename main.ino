#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <PubSubClient.h>
//#include <ArduinoJson.h>

#define PROX_SENSOR 6
#define OUTPUT_PIN 10

Preferences preferences;
AsyncWebServer server(80);

#define SSID_AP "ESP32-Setup"
#define PASSWORD_AP "12345678"


WiFiClient espClient;
PubSubClient MqttClient(espClient);



//sensor Global variables
String StoredSensor1Name;
String StoredSensor2Name;
//String storedInput1Mode;
//String storedInput1Mode;
uint8_t StoredSensor1Offon;
uint8_t StoredSensor2Offon;
//uint8_t storedSensor1nonc;
//uint8_t storedSensor2nonc;
uint8_t StoredSensorShiftChoise;
uint8_t StoredUserInputSensorsTimeDifferenceInSeconds;
uint8_t StoredSensorIndividualTriggerOutputon;
uint8_t StoredSensorDiffOutputAlerton;
String StoredOutputONSetupChoise;

bool RightShiftStatus = false;
//bool leftshiftcount = false;

long LastMqttDataPublish = 0;
long LastWifiCheck = 0;
long LastMqttCheck = 0;

int CommomMqttDataCountValue = 0;//common count for every mqtt publish data
volatile int32_t ProxiCounterAfterBothSensorDetects = 0;  //for both proximity detects one object than counter increments
volatile int32_t Sensor1ProxiCount = 0;
volatile int32_t Sensor2ProxiCount = 0; 
volatile bool Senson1Triggered = false;
volatile bool Sensor2Triggered = false;
//volatile bool detected_proxi3 = false;// counter reset mode
unsigned long Sensor1TimeWhenTriggered = 0;
unsigned long Sensor2TimeWhenTriggered = 0;
unsigned long TimeDifferenceBetweenSensors = 0;
unsigned long lasttimedifftime = 0;
//volatile bool WaitingForObjectDetectedSensor1=false;


unsigned long OutputStartTimeOnSensorTriggering = 0;//output on for few seconds when sensor detects 
unsigned long OutputStartTimeOnReset = 0;//output for reset
unsigned long OutputStartTimeSensorDifferenceAlert = 0;  //buzzer on timing for negative output
bool OutputIsActiveForTriggering = false;
bool OutputIsActiveForReset = false;
bool OutputIsActiveForSensorDifferenceAlert = false;
//unsigned long outputStartResetTime = 0;
//bool outputIsReset = false;

// Individual acceptable times of sensor1 and sensor2 and outpput integration
uint8_t StoredSensorIndividualAcceptTimeSelectAndOutputOn = 0;//select for whome they want to enter acceptable time and want output ON sensor1,sensor2 or reset 
uint8_t StoredSensorIndividualAcceptTimeUserInputInSeconds = 0;// input from user the seconds they want for individual accept time
unsigned long CurrentTimeForOutputSensor1 = 0;
volatile bool WaitingForObjectDetectedSensor1 = false;
long LastDetectedOutoutTimeSensor1 = 0;
bool OutputStateForSensor1 = false;
unsigned long TimeDifferenceSensor1 = 0;


unsigned long CurrentTimeForOutputSensor2 = 0;
volatile bool WaitingForObjectDetectedSensor2 = false;
long LastDetectedOutoutTimeSensor2 = 0;
bool OutputStateForSensor2 = false;
unsigned long TimeDifferenceSensor2 = 0;

volatile bool WaitingForReset = false;// for reset output on function


bool ShouldRestartESP = false;  //used for esp restart input uis given through webserver





void setup() {
  Serial.begin(115200);

  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);


  ProximityConfig();  // configure the proximity and output ONOFF condition by user
  Outputonoff();


  // 2. Decide Connection Strategy
  WifiConfig();

  MqttClient.setServer("broker.emqx.io", 1883);

  WebServerConfig();
}

void loop() {
  // proximity_config();// configure the proximity
  if (ShouldRestartESP == true) {

    //delay(5000);
    Serial.println("Restart!!!!, Disconnection.........................................................................");
    ESP.restart();  // This avoids the "wifi:sta is connecting" error
    //delay(2000);
    return;
  }

  static bool IsFirstRun = true;
  static uint8_t StoredUserInputProxiShiftCounterOn; // This stays "local" to the loop function
  //If you used a regular int, the variable would be reset/deleted every single time the loop finishes and restarts.
  static uint8_t StoredUserInputSensorProxiJsonCounterOn;
  if (IsFirstRun == true) {
    preferences.begin("sensor", true);
    StoredUserInputProxiShiftCounterOn = preferences.getInt("Shiftcount", 0);
    StoredUserInputSensorProxiJsonCounterOn=preferences.getInt("proxionoff", 0);
    preferences.end();
    Serial.print("beiiiiiiiii ");
    IsFirstRun = false;
  }

    // Now send that local value to other functions using a pointer
   //MqttPublish_data(&storedproxishiftcounteronoff);

  if (millis() - LastWifiCheck > 5000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("wifi Diconneted");
      WiFi.reconnect();// make wifi previously connected
      WiFi.mode(WIFI_AP_STA);
      WiFi.softAP(SSID_AP, PASSWORD_AP);

    } else {
      Serial.println("wifi connected");

      // Serial.println("try connecting the wifi connection again and ap mode on");
      //Optional: keep AP running even when wifi disconnected
      //WiFi.mode(WIFI_AP_STA); //station ap mMode on

      //WiFi.begin(storedSSID.c_str(), storedPass.c_str());
    }
    LastWifiCheck = millis();
  }


  if (!MqttClient.connected()) {
    MqttReconnectCheck();
    //Serial.println("mqttttttffffff");
  } else {
    MqttClient.loop();
    // Serial.println("mqttttttffffff");
    //MqttPublish_data();
    MqttPublish_data(&StoredUserInputProxiShiftCounterOn,&StoredUserInputSensorProxiJsonCounterOn);
    //Serial.println("----------------------------------------------------------------+");
  }

  if(StoredSensorIndividualTriggerOutputon==1){
    SensorTriggerOutputon();
  }

 /*if (outputIsActiveNeg && (micros() - outputStartTimeNeg >= 500000)) {//nbuzzer 500 mili seconds
      digitalWrite(10, LOW);
      outputIsActiveNeg = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }*/
  if (StoredSensorIndividualAcceptTimeSelectAndOutputOn == 1) {  // for input of acceptable time for sensor 1
    Sensor1AcceptableTime();
  } 
  else if (StoredSensorIndividualAcceptTimeSelectAndOutputOn == 2) {
    Sensor2AcceptableTime();
  }
  else if (StoredSensorIndividualAcceptTimeSelectAndOutputOn == 3) {
    OutputOnReset();
  }
  else{
    return;
  }

  //if (storedSensorDiffOutputAlerton == 1){
  //  Serial.println("hhhhhhhhhhhhdddddddddd");
  //  TimeDiffAlertBetweenSensors();
 // }
  
  /*if (outputIsReset && (micros() - outputStartResetTime >= 500000)) {//nbuzzer 500 mili seconds
      digitalWrite(10, LOW);
      outputIsActiveNeg = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }*/
}
