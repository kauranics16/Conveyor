#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>// to store data or user input to esp32 memory for future use to retrieve data when needed
#include <PubSubClient.h>//For Mqtt data publish
//#include <ArduinoJson.h>

#define PROX_SENSOR1 6//pin 6 as Input of Esp32c3
#define PROX_SENSOR2 7//pin 7 as Input of Esp32c3
#define OUTPUT_PIN 10//pin 10 as Output of Esp32c3

Preferences preferences;
AsyncWebServer server(80);

#define SSID_AP "ESP32-Setup"
#define PASSWORD_AP "12345678"


WiFiClient espClient;
PubSubClient mqttClient(espClient);//pack wifi client in mqtt client container

// ----- Forward Declarations Of Function -----
void proximityConfig();
void setupOutputOn();
void sensorShiftRight();
void sensorShiftLeft();
void acceptableTimeAlert();
void timeDiffAlertBetweenSensors();
void sensorTriggerOutputOn();
void sensor1AcceptableTime();
void sensor2AcceptableTime();
void outputOnReset();

void wifiConfig();
void webServerConfig();
void mqttReconnectCheck();
void mqttPublishData(uint8_t *shiftCounter,uint8_t *jsonCountOn);


//sensor Global variables
String storedSensor1Name;//user input Sensor1 name
String storedSensor2Name;//user input Sensor2 name
//String storedInput1Mode;
//String storedInput1Mode;
bool storedSensor1OnChoice;//Whether the input choice is to "ON" the sensor1 or not
uint8_t storedSensor2OnChoice;//Whether the input choice is to "ON" the sensor2 or not
//uint8_t storedSensor1nonc;
//uint8_t storedSensor2nonc;
bool storedSensorShiftChoise;//Whether the user Input is Right Shift or Left Shift
uint8_t storedInputSensorTimeDiff;//User INput Acceptable Time Difference between two sensor in seconds 
uint8_t storedSensorTriggerOut;//User input of whether the user want the output on for few seconds when sensor detectes something
uint8_t storedSensorDiffOut;//User input of ALERT Output "ON" when the sensor Difference exceeds thr 
//String storedOutputChoice;//User Input of user Want Output ON or Not in Setup
uint8_t storedMqttPubSensorTimeDiff ;//User input if user want to Publish the Alert message on Mqtt or Not
uint8_t storedsensor1OnTime;

bool rightShiftStatus = false;//Whether User Inputed RightShift or LeftShift Choice Flag 
//bool leftshiftcount = false;

unsigned long lastMqttDataPub = 0;//Time line for Mqtt Data Publish 
unsigned long lastWifiCheck = 0;//Time line for Wifi Data Publish
unsigned long lastMqttCheck = 0;//Time line for Mqtt Reconnect in millis()

unsigned int mqttDataCount = 0;//common count for every mqtt publish data
volatile int32_t sensorCount = 0;  //for both proximity detects one object than counter increments
volatile int32_t sensor1Count = 0;//Sensor1 Count to Publish when needed
volatile int32_t sensor2Count = 0;//Sensor2 Count to Publish when needed
volatile bool sensor1Shift = false;//Sensor1 Detects Status for shift operation 
volatile bool sensor2Shift = false;//Sensor2 Detects Status for shift operation

volatile bool sensor1TriggeredAlert = false;//Sensor1 Detects Status for Alert
volatile bool sensor2TriggeredAlert = false;//Sensor2 Detects Status for Alert
unsigned long sensor1TimeTriggered = 0;//Time in millis for timeDiffAlertBetweenSensors()
unsigned long sensor2TimeTriggered = 0;//Time in millis for timeDiffAlertBetweenSensors()
unsigned long timeDifferenceAlert=0;//Time Difference between Sensors for timeDiffAlertBetweenSensors()

//volatile bool detected_proxi3 = false;// counter reset mode
volatile bool sensor1MqttPub = false;//sensor mqtt publish Alert message
volatile bool sensor2MqttPub = false;//sensor mqtt publish Alert message
unsigned long sensor1TimeMqttPub = 0;//sensor 1 time when triggered
unsigned long sensor2TimeMqttPub = 0;//sensor 2 time when triggered
unsigned long timeDiffInSensors = 0;//Time difference between Sensors
//unsigned long lasttimedifftime = 0;
//volatile bool waitingForObjectDetectedSensor1=false;


unsigned long outTimeOnSensorTrig = 0;//output on for few seconds when sensor detects for sensorTriggerOutputOn()
unsigned long outTimeOnReset = 0;//output for reset
unsigned long outTimeOnSensorDiff = 0;  //Output on for few senconds on sensor difference 
//unsigned long currentAlert = 0;
bool outActiveOnTrigger = false;
bool outActiveOnReset = false;
bool outActiveOnSensorDiff = false;
//unsigned long outputStartResetTime = 0;
//bool outputIsReset = false;

// Individual acceptable times of sensor1 and sensor2 and outpput integration
uint8_t storedIndividualAcceptTime = 0;//select for whome they want to enter acceptable time and want output ON sensor1,sensor2 or reset 
uint8_t storedUserIndividualAcceptTime = 0;// input from user the seconds they want for individual accept time
unsigned long outTimeSensor1 = 0;
volatile bool objectDetectedSensor1 = false;
long lastOutTimeSensor1 = 0;//for sensor1AcceptableTime()
bool outStateSensor1 = false;
unsigned long timeDifferenceSensor1 = 0;


unsigned long outTimeSensor2 = 0;
volatile bool objectDetectedSensor2 = false;
long lastOutTimeSensor2 = 0; //for sensor2AcceptableTime()
bool outStateSensor2 = false;
unsigned long timeDifferenceSensor2 = 0;

volatile bool waitingForReset = false;// for reset output on function
bool shouldRestartESP = false;  //used for esp restart input uis given through webserver





void setup() {
  Serial.begin(115200);

  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

  configInputOutput();
  //proximityConfig();  // configure the proximity and output ONOFF condition by user
  //setupOutputOn();//


  // 2. Decide Connection Strategy
  wifiConfig();

  mqttClient.setServer("broker.emqx.io", 1883);

  webServerConfig();
}

void loop() {
  
  if (shouldRestartESP == true) {

    //delay(5000);
    Serial.println("Restart!!!!, Disconnection.........................................................................");
    ESP.restart();  // This avoids the "wifi:sta is connecting" error
    //delay(2000);
    return;
  }

  static bool isFirstRun = true;
  static uint8_t storedUserInputProxiShiftCounterOn; // This stays "local" to the loop function
  //If you used a regular int, the variable would be reset/deleted every single time the loop finishes and restarts.
  static uint8_t storedUserInputSensorProxiJsonCounterOn;
  if (isFirstRun == true) {
    preferences.begin("sensor", true);
    storedUserInputProxiShiftCounterOn = preferences.getInt("Shiftcount", 0);
    storedUserInputSensorProxiJsonCounterOn=preferences.getInt("proxionoff", 0);
    preferences.end();
    Serial.print("beiiiiiiiii ");
    isFirstRun = false;
  }

    // Now send that local value to other functions using a pointer
   //MqttPublish_data(&storedproxishiftcounteronoff);

  if (millis() - lastWifiCheck > 5000) {
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
    lastWifiCheck = millis();
  }


  if (!mqttClient.connected()) {
    mqttReconnectCheck();
    //Serial.println("mqttttttffffff");
  } else {
    mqttClient.loop();
    // Serial.println("mqttttttffffff");
    //MqttPublish_data();
    mqttPublishData(&storedUserInputProxiShiftCounterOn,&storedUserInputSensorProxiJsonCounterOn);
    //Serial.println("----------------------------------------------------------------+");
  }

  if(storedSensorTriggerOut==1){
    sensorTriggerOutputOn();
  }

 /*if (outputIsActiveNeg && (micros() - outputStartTimeNeg >= 500000)) {//nbuzzer 500 mili seconds
      digitalWrite(10, LOW);
      outputIsActiveNeg = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }*/
  if (storedIndividualAcceptTime == 1) {  // for input of acceptable time for sensor 1
    sensor1AcceptableTime();
  } 
  else if (storedIndividualAcceptTime == 2) {
    sensor2AcceptableTime();
  }
  else if (storedIndividualAcceptTime == 3) {
    outputOnReset();
  }
  
  if (storedSensorDiffOut == 1){
       timeDiffAlertBetweenSensors();
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
  /*if (OutputIsActiveForSensorDifferenceAlert==true && (millis() - OutputStartTimeSensorDifferenceAlert >= 1000UL)) {//nbuzzer 500 mili seconds
      Serial.println("checkinggggggggggggggg");
      digitalWrite(OUTPUT_PIN, LOW);
      OutputIsActiveForSensorDifferenceAlert = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }*/
}
