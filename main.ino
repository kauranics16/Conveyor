#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <PubSubClient.h>
//#include <ArduinoJson.h>

#define PROX_SENSOR1 6
#define PROX_SENSOR2 7
#define OUTPUT_PIN 10

Preferences preferences;
AsyncWebServer server(80);

#define SSID_AP "ESP32-Setup"
#define PASSWORD_AP "12345678"

#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883

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
void wifiCheck();
void webServerConfig();
void mqttReconnectCheck();
void mqttPublishData(bool *shiftCounter,bool *jsonCountOn);
void TimeBetweenObject1();
void TimeBetweenObject2();
void onOffTimeSensor1();
void onOffTimeSensor2();

// --------- Enum defination And Declaration--------
typedef enum SensorState : uint8_t{
  STATE_ON,
  STATE_OFF,
  STATE_RESET
}sensorState;
sensorState sensor1State = STATE_OFF;
sensorState sensor2State = STATE_OFF;

typedef enum SensorType : bool{
  TYPE_NO,
  TYPE_NC
}sensorType;

typedef enum SensorMode : bool{
  MODE_RISING,
  MODE_FALLING
}sensorMode;


// --------- Global variables --------
String storedSensor1Name;
String storedSensor2Name;
bool storedSensorShiftChoise;
uint8_t storedSensorTimeDiffSeconds;

bool storedSensorTriggerOut;//User input of whether the user want the output on for few seconds when sensor detectes something
bool storedSensorDiffOut;//User input of ALERT Output "ON" when the sensor Difference exceeds thresold given by user
bool storedMqttPubSensorTimeDiff ;//User input if user want to Publish the Alert message on Mqtt or Not
uint8_t storedOnTimeChoice;
uint8_t storedTimeBwObject;

bool rightShiftStatus = false;//Whether User Inputed RightShift or LeftShift Choice Flag For Shift Count Publish 

unsigned int mqttDataCount = 0;//common count for every mqtt publish data
volatile int32_t sensorCount = 0; //for both proximity detects one object than counter increments
volatile int32_t sensor1Count = 0;//Sensor1 Count to Publish when needed
volatile int32_t sensor2Count = 0;//Sensor2 Count to Publish when needed
volatile bool sensor1Shift = false;//Sensor1 Detects Status for shift operation 
volatile bool sensor2Shift = false;//Sensor2 Detects Status for shift operation

//For timeDiffAlertBetweenSensors()
volatile bool sensor1TriggeredAlert = false;//Sensor1 Detects Status for Alert
volatile bool sensor2TriggeredAlert = false;//Sensor2 Detects Status for Alert
unsigned long sensor1TimeTriggered = 0;//Time in millis for timeDiffAlertBetweenSensors()
unsigned long sensor2TimeTriggered = 0;//Time in millis for timeDiffAlertBetweenSensors()

//For acceptableTimeAlert()
volatile bool sensor1MqttPub = false;//sensor mqtt publish Alert message
volatile bool sensor2MqttPub = false;//sensor mqtt publish Alert message
unsigned long sensor1TimeMqttPub = 0;//sensor 1 time when triggered
unsigned long sensor2TimeMqttPub = 0;//sensor 2 time when triggered

// Individual acceptable times of sensor1 and sensor2 and output integration Respectively
uint8_t storedAcceptTimeSelect = 0;//select for whom they want to enter acceptable time and want output ON sensor1,sensor2 or reset 
uint8_t storedAcceptTimeSeconds = 0;// input from user the seconds they want for individual accept time
unsigned long outTimeSensor1 = 0;//for Acceptable time sensor1
volatile bool objectDetectedSensor1 = false;//object detected status
unsigned long outTimeSensor2 = 0;////for Acceptable time sensor2
volatile bool objectDetectedSensor2 = false;//object detected status

volatile bool currentStateSensor1;//or 
volatile bool isStateChanged1 = false;
volatile bool currentStateSensor2;//or 
volatile bool isStateChanged2 = false;

volatile bool objectPresent1=false;
volatile bool objectPresent2=false;

volatile bool waitingForReset = false;// for reset output on function
bool shouldRestartESP = false;  //used for esp restart input uis given through webserver



// =============================================================
//                             SETUP
// =============================================================
void setup() {
  Serial.begin(115200);

  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  
  //Configure Input And Output As Per User Input and Need
  configInputOutput();
  
  //Configure Wifi Ap or Station Mode
  wifiConfig();
  
  //Set Mqtt Server Name And Port To Begin Mqtt
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  //Configure Webserver 
  webServerConfig();
}


// =============================================================
//                           MAIN LOOP
// =============================================================
void loop() {
  
  if (shouldRestartESP == true) {
    Serial.println("Restart!!!!, Disconnection.........................................................................");
    ESP.restart();  
    return;
  }
  
  
  static bool isFirstRun = true;
  static bool storedUserInputProxiShiftCounterOn; 
  static bool storedUserInputSensorProxiJsonCounterOn;

  //Runns When Loop Runs First Time
  if (isFirstRun == true) {
    preferences.begin("sensor", true);
    storedUserInputProxiShiftCounterOn = preferences.getBool("Shiftcount", false);
    storedUserInputSensorProxiJsonCounterOn=preferences.getBool("proxionoff", false);
    preferences.end();
    Serial.print("beiiiiiiiii ");
    isFirstRun = false;
  }// Now send that local value to other functions using a pointer                                              

    
  //Check Wifi Status
  wifiCheck();

  //If Mqtt Not Connected  
  if (!mqttClient.connected()) {
    mqttReconnectCheck();
  } 
  else {
    //Make MQTT live Continuously
    mqttClient.loop();
    
    //Publish Mqtt Data
    mqttPublishData(&storedUserInputProxiShiftCounterOn,&storedUserInputSensorProxiJsonCounterOn);
  }

  //If User Wants the Ouput when Sensor Sense Something
  if(storedSensorTriggerOut==true){
    sensorTriggerOutputOn();
  }

  //User Selection of Acceptable Time Sensor1, Acceptable Time sensor2, Reset Mode 
  if (storedAcceptTimeSelect == 1) {  
    sensor1AcceptableTime();
  } 
  else if (storedAcceptTimeSelect == 2) {
    sensor2AcceptableTime();
  }
  else if (storedAcceptTimeSelect == 3) {
    outputOnReset();
  }
  
  //If User Want Ouput On For Seconds For Time Difference Between 2 Sensors   
  if (storedSensorDiffOut == true){
    timeDiffAlertBetweenSensors();
  }

  //If User Want On and Off Cycle Time of Sensor1, Sensor2, Both (Mqtt Publish)
  if (storedOnTimeChoice==1){
    onOffTimeSensor1();
  }
  else if (storedOnTimeChoice==2){
    onOffTimeSensor2();
  }
  else if (storedOnTimeChoice==3){
    onOffTimeSensor1();
    onOffTimeSensor2();
  }
  
  //if User Want Time taken by Sensor to Detect object Continuosly(Time between Object Detection)
  if(storedTimeBwObject==1){
    TimeBetweenObject1();
  }
  else if(storedTimeBwObject==2){
    TimeBetweenObject2();
  }
  else if(storedTimeBwObject==3){
    TimeBetweenObject1();
    TimeBetweenObject2();
  }

}
