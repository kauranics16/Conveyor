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


WiFiClient espClient;
PubSubClient mqtt_client(espClient);



//sensor Global variables
String storedInput1;
String storedInput2;
//String storedInput1Mode;
//String storedInput1Mode;
uint8_t storedsensor1offon;
uint8_t storedsensor2offon;
//uint8_t storedSensor1nonc;
//uint8_t storedSensor2nonc;
uint8_t storedSensorShiftchoise;
uint8_t storedSensor_timedifference;
uint8_t storedSensorIndividualTriggerOutputon;
uint8_t storedSensorDiffOutputAlerton;
String storedOutputonoff;

bool rightshiftcount = false;
//bool leftshiftcount = false;

long lastMsg = 0;
long LastWifiCheck = 0;
long LastMqttCheck = 0;

int CommomCountvalue = 0;//common count for every mqtt publish data
volatile int32_t proxi_counter = 0;  //for both proximity
volatile int32_t proxi_counter1 = 0;
volatile int32_t proxi_counter2 = 0;
volatile bool detected_proxi1 = false;
volatile bool detected_proxi2 = false;
//volatile bool detected_proxi3 = false;// counter reset mode
unsigned long time1 = 0;
unsigned long time2 = 0;
unsigned long TimeDifference = 0;
unsigned long lasttimedifftime = 0;
//volatile bool WaitingForObjectDetectedSensor1=false;


unsigned long outputStartTime = 0;
unsigned long outputStartTimeNeg = 0;  //buzzer on timing for negative output
bool outputIsActive = false;
bool outputIsActiveNeg = false;
//unsigned long outputStartResetTime = 0;
//bool outputIsReset = false;

// Individual acceptable times of sensor1 and sensor2 and outpput integration
uint8_t storedSensorIndividualAcceptTimeselect = 0;
uint8_t storedSensorIndividualAcceptTimeinput = 0;// input from user the seconds they want for individual accept time
unsigned long CurrentTimesensor1 = 0;
volatile bool WaitingForObjectDetectedSensor1 = false;
long lastdetectectedtimeSensor1 = 0;
bool outputstatesensor1 = false;
unsigned long timedifferenceSensor1 = 0;


unsigned long CurrentTimesensor2 = 0;
volatile bool WaitingForObjectDetectedSensor2 = false;
long lastdetectectedtimeSensor2 = 0;
bool outputstatesensor2 = false;
unsigned long timedifferenceSensor2 = 0;

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

  mqtt_client.setServer("broker.emqx.io", 1883);

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

  static bool isFirstRun = true;
  static uint8_t storedproxishiftcounteronoff; // This stays "local" to the loop function
  //If you used a regular int, the variable would be reset/deleted every single time the loop finishes and restarts.
  static uint8_t storedSensorproxicounteronoff;
  if (isFirstRun == true) {
    preferences.begin("sensor", true);
    storedproxishiftcounteronoff = preferences.getInt("Shiftcount", 0);
    storedSensorproxicounteronoff=preferences.getInt("proxionoff", 0);
    preferences.end();
    Serial.print("beiiiiiiiii ");
    isFirstRun = false;
  }

    // Now send that local value to other functions using a pointer
   //MqttPublish_data(&storedproxishiftcounteronoff);

  if (millis() - LastWifiCheck > 5000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("wifi Diconneted");
      WiFi.reconnect();// make wifi previously connected
      WiFi.mode(WIFI_AP_STA);
      WiFi.softAP(ssid_ap, password_ap);

    } else {
      Serial.println("wifi connected");

      // Serial.println("try connecting the wifi connection again and ap mode on");
      //Optional: keep AP running even when wifi disconnected
      //WiFi.mode(WIFI_AP_STA); //station ap mMode on

      //WiFi.begin(storedSSID.c_str(), storedPass.c_str());
    }
    LastWifiCheck = millis();
  }


  if (!mqtt_client.connected()) {
    MqttReconnectCheck();
    //Serial.println("mqttttttffffff");
  } else {
    mqtt_client.loop();
    // Serial.println("mqttttttffffff");
    //MqttPublish_data();
    MqttPublish_data(&storedproxishiftcounteronoff,&storedSensorproxicounteronoff);
    //Serial.println("----------------------------------------------------------------+");
  }

  if(storedSensorIndividualTriggerOutputon==1){
    SensorTriggerOutputon();
  }

 /*if (outputIsActiveNeg && (micros() - outputStartTimeNeg >= 500000)) {//nbuzzer 500 mili seconds
      digitalWrite(10, LOW);
      outputIsActiveNeg = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }*/
  if (storedSensorIndividualAcceptTimeselect == 1) {  // for input of acceptable time for sensor 1
    Sensor1AcceptableTime();
  } 
  else if (storedSensorIndividualAcceptTimeselect == 2) {
    Sensor2AcceptableTime();
  }
  else if (storedSensorIndividualAcceptTimeselect == 3) {
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
