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
PubSubClient mqttClient(espClient);  //pack wifi client in mqtt client container

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
void mqttPublishData();
void TimeBetweenObject1();
void TimeBetweenObject2();
void onOffTimeSensor1();
void onOffTimeSensor2();

void sensor1IsrFlags();
void sensor2IsrFlags();

// --------- Enum defination --------
typedef enum SensorState : uint8_t {
  STATE_ON,
  STATE_OFF,
  STATE_RESET
} sensorState;

typedef enum SensorType : bool {
  TYPE_NO,
  TYPE_NC
} sensorType;

typedef enum SensorMode : bool {
  MODE_RISING,
  MODE_FALLING
} sensorMode;

typedef enum ShiftMode : bool {
  RIGHT_SHIFT,
  LEFT_SHIFT
} shiftChoice;

typedef enum MqttChoice : uint8_t {
  SENSOR1_ONLY,
  SENSOR2_ONLY,
  BOTH_SENSORS,
  OFF_STATE
} mqttChoice;

typedef enum OutputChoice : uint8_t {
  OUTPUT_ON,
  OUTPUT_OFF,
  RESET_OUTPUT
} outputChoice;

typedef enum OutputInAcceptTime : uint8_t {
  SENSOR1,
  SENSOR2,
  RESET,
  OFF
} outputInAcceptTime;


// --------- Enum declaration--------
sensorState sensor1State = STATE_OFF;
sensorState sensor2State = STATE_OFF;
shiftChoice shiftMode;
mqttChoice pubTimeBwObject = OFF_STATE;
mqttChoice pubOnOffCycle = OFF_STATE;
outputChoice onSensorTrigger = OUTPUT_OFF;
outputChoice onSensorsTimeDiff = OUTPUT_OFF;
outputInAcceptTime outputOnAcceptTime = OFF;


// -- -- -- -- -Global variables-- -- -- --
String storedSensor1Name;       //Sensor at Input1 Name i.e sensor1 Name
String storedSensor2Name;       //Sensor at Input2 Name i.e sensor2 Name
bool storedShiftCounterOn;      //Right and Left shift counter Choice
bool storedJsonCounterOn;       //Sensors Individual counters in Json form Choice
uint8_t storedTimeDiffSeconds;  //User Input of Acceptable time Difference of User
bool storedPubTimeDiffAlert;    //Mqtt Publish Time Difference Between Sensors


//Counters
unsigned int mqttDataCount = 0;      //common count for every mqtt publish data
volatile int32_t sensorCount = 0;    //for both proximity detects one object than counter increments
volatile int32_t sensor1Count = 0;   //Sensor1 Count to Publish when needed
volatile int32_t sensor2Count = 0;   //Sensor2 Count to Publish when needed
volatile bool sensor1Shift = false;  //Sensor1 Detects Status for shift operation
volatile bool sensor2Shift = false;  //Sensor2 Detects Status for shift operation

//Time Difference Isr Variables
volatile bool timeDiffIsrOut1 = false;  //Sensor1 Detects Status for Alert
volatile bool timeDiffIsrOut2 = false;  //Sensor2 Detects Status for Alert
unsigned long timeDiffIsrOutTime1 = 0;  //Time in millis when sensor detects
unsigned long timeDiffIsrOutTime2 = 0;  //Time in millis when sensor detects

//For Mqtt Publish
volatile bool timeDiffPubIsr1 = false;  //sensor mqtt publish Alert message
volatile bool timeDiffPubIsr2 = false;  //sensor mqtt publish Alert message
unsigned long timeDiffPubIsrTime1 = 0;  //sensor 1 time when triggered
unsigned long timeDiffPubIsrTime2 = 0;  //sensor 2 time when triggered

//Individual acceptable times of sensor1 and sensor2 and output integration Respectively
uint8_t storedAcceptTimeSeconds = 0;   // input from user the seconds they want for individual accept time
unsigned long outTimeSensor1 = 0;      //for Acceptable time sensor1
volatile bool acceptTimeIsr1 = false;  //object detected status
unsigned long outTimeSensor2 = 0;      //for Acceptable time sensor2
volatile bool acceptTimeIsr2 = false;  //object detected status

//On and Off Cycle
volatile bool currentStateSensor1;
volatile bool isStateChanged1 = false;
volatile bool currentStateSensor2;
volatile bool isStateChanged2 = false;

//ISR Main flags
volatile bool sensor1Isr = false;
volatile bool sensor2Isr = false;

volatile bool waitingForResetIsr = false;  // for reset output on function
bool shouldRestartESP = false;             //used for esp restart input uis given through webserver


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
  //Check Wifi Status
  wifiCheck();

  //If Mqtt Not Connected
  if (!mqttClient.connected()) {
    mqttReconnectCheck();
  } else {
    //Make MQTT live Continuously
    mqttClient.loop();
    //Publish Mqtt Data
    mqttPublishData();
  }

  //Handles the ExtraFlags Isr Variables for Function
  if (sensor1Isr == true) {
    //if User Want Time taken by Sensor to Detect object Continuosly(Time between Object Detection)
    if (pubTimeBwObject == SENSOR1_ONLY) {
      TimeBetweenObject1();
    } else if (pubTimeBwObject == BOTH_SENSORS) {
      TimeBetweenObject1();
    }
    sensor1IsrFlags();
    sensor1Isr = false;
  }
  if (sensor2Isr == true) {
    sensor2IsrFlags();
    if (pubTimeBwObject == SENSOR2_ONLY) {
      TimeBetweenObject2();
    } else if (pubTimeBwObject == BOTH_SENSORS) {
      TimeBetweenObject2();
    }
    sensor2Isr = false;
  }

  //If User Wants the Ouput when Sensor Sense Something
  if (onSensorTrigger == OUTPUT_ON) {
    sensorTriggerOutputOn();
  }

  //User Selection of Acceptable Time Sensor1, Acceptable Time sensor2, Reset Mode
  if (outputOnAcceptTime == SENSOR1) {
    sensor1AcceptableTime();
  } else if (outputOnAcceptTime == SENSOR2) {
    sensor2AcceptableTime();
  } else if (outputOnAcceptTime == RESET) {
    outputOnReset();
  }


  //If User Want Ouput On For Seconds For Time Difference Between 2 Sensors
  if (onSensorsTimeDiff == OUTPUT_ON) {
    timeDiffAlertBetweenSensors();
  }

  //If User Want On and Off Cycle Time of Sensor1, Sensor2, Both (Mqtt Publish)
  if (pubOnOffCycle == SENSOR1_ONLY) {
    onOffTimeSensor1();
  } else if (pubOnOffCycle == SENSOR2_ONLY) {
    onOffTimeSensor2();
  } else if (pubOnOffCycle == BOTH_SENSORS) {
    onOffTimeSensor1();
    onOffTimeSensor2();
  }
}
