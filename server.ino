
const char *htmlForm = R"rawliteral(
<!DOCTYPE HTML><html><head>
 <title>Wi-Fi Setup</title>
 <meta name="viewport" content="width=device-width, initial-scale=1">
</head><body>
 <h2>Configure Wi-Fi</h2>
 <form action="/save" method="POST">
 InputSensor1 Name:<br><input type="text" name="input1"><br>
 <input type="radio"  name="myGroup" value=1> ON Sensor1
 <input type="radio"  name="myGroup" value=2> OFF Sensor1<br>
 <input type="radio"  name="myGroup2" value=1> NO
 <input type="radio"  name="myGroup2" value=2> NC<br>
 Sensor1 Mode:<input type="radio"  name="sensormode1" value=1>FALLING
 <input type="radio"  name="sensormode1" value=2> RISING<br>
 InputSensor2 Name:<br><input type="text" name="input2"><br>
 <input type="radio"  name="myGroup1" value=1> ON Sensor2
 <input type="radio"  name="myGroup1" value=2> OFF Sensor2
 <input type="radio"  name="myGroup1" value=3> As Reset Button<br>
 <input type="radio"  name="myGroup3" value=1> NO
 <input type="radio"  name="myGroup3" value=2> NC<br>
 Sensor2 Mode:<input type="radio"  name="sensormode2" value=1>FALLING
 <input type="radio"  name="sensormode2" value=2> RISING<br>
 Shift Choice: <input type="radio"  name="Shifting" value=1> Sensor1 to Sensor2 Right Shift
 <input type="radio"  name="Shifting" value=2> Sensor2 to Sensor1 Left Shift<br>
 Left Right Shift Count Choice: <input type="radio"  name="Shiftcount" value=1> ON
 <input type="radio"  name="Shiftcount" value=2>OFF<br>
 Proximity Individual Counter: <input type="radio"  name="proxicounteronoff" value=1>ON
 <input type="radio"  name="proxicounteronoff" value=2>OFF<br>
 Input Acceptable Time in Seconds :<br><input type="number" name="seconds"><br>
 Alert On Gap Between Sensors Mqtt publish?<input type="radio"  name="outputpublish" value=1>ON
 <input type="radio"  name="outputpublish" value=2>OFF<br>
 Choose Output on or off:<input type="radio"  name="onoff" value=1>ON
 <input type="radio"  name="onoff" value=2>OFF<br>
 Output ON on Sensor Trigger Respectively?<input type="radio"  name="outputtrig" value=1>ON
 <input type="radio"  name="outputtrig" value=2>OFF<br>
 Output On Alert Gap Between Sensors Larger Than Inputed Above<input type="radio"  name="outputalert" value=1>ON
 <input type="radio"  name="outputalert" value=2>OFF<br>
 Individual Sensor Output On <input type="radio"  name="individual" value=1> Acceptable time sensor1
 <input type="radio"  name="individual" value=2> Acceptable time sensor2
 <input type="radio"  name="individual" value=3> on reset
 <input type="radio"  name="individual" value=4> offff<br>
 Input Acceptable Time in Seconds For Above Selected Option :<br><input type="number" name="accepttime"><br>
 Mqtt Publish Time Between Object detected:<input type="radio"  name="objectdetect" value=1> Sensor1 Only
 <input type="radio"  name="objectdetect" value=2>Sensor2 Only
 <input type="radio"  name="objectdetect" value=3>Both Sensors
 <input type="radio"  name="objectdetect" value=4>Off<br>
 Input Mqtt publish the ON--OFF sensor1 time Mqtt Publish(Change mode only):<input type="radio"  name="onofftime" value=1>Sensor1 Only
 <input type="radio"  name="onofftime" value=2>Sensor2 Only
 <input type="radio"  name="onofftime" value=3>Both Sensors
 <input type="radio"  name="onofftime" value=4>Off<br>
 SSID:<br><input type="text" name="ssid"><br>
 Password:<br><input type="password" name="pass"><br><br>
 <input type="submit" value="Save">
 </form>
</body></html>)rawliteral";




void webServerConfig() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {  // "/" means the host ip address
    request->send(200, "text/html", htmlForm);
  });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      String s = request->getParam("ssid", true)->value();
      String p = request->getParam("pass", true)->value();
      String inputSensor1Name = request->getParam("input1", true)->value();
      String inputSensor2Name = request->getParam("input2", true)->value();
      bool sensor1Modew = (request->getParam("sensormode1", true)->value() == "1") ? true : false;
      bool sensor2Modew = (request->getParam("sensormode2", true)->value() == "1") ? true : false;
      bool sensor1OnChoice = (request->getParam("myGroup", true)->value() == "1") ? true : false;
      uint8_t sensor2OnChoice = request->getParam("myGroup1", true)->value().toInt();
      bool sensor1NoNcChoice = (request->getParam("myGroup2", true)->value() == "1") ? true : false;
      bool sensor2NoNcChoice = (request->getParam("myGroup3", true)->value() == "1") ? true : false;
      bool sensorShiftChoice = (request->getParam("Shifting", true)->value() == "1") ? true : false;             
      bool jsonCountOnChoice = (request->getParam("proxicounteronoff", true)->value() == "1") ? true : false;  
      bool ShiftCountOnChoice = (request->getParam("Shiftcount", true)->value() == "1") ? true : false;
      uint8_t inputTimeDifference = request->getParam("seconds", true)->value().toInt();
      bool outputOnChoice = (request->getParam("onoff", true)->value() == "1") ? true : false;
      bool sensorTriggerOutputOn = (request->getParam("outputtrig", true)->value() == "1") ? true : false;
      bool sensorTimeDiffPublish = (request->getParam("outputpublish", true)->value() == "1") ? true : false;
      bool sensorTimeDiffOutOn = (request->getParam("outputalert", true)->value() == "1") ? true : false;
      uint8_t sensorAcceptTimeSelect = request->getParam("individual", true)->value().toInt();
      uint8_t sensorAcceptTimeSeconds = request->getParam("accepttime", true)->value().toInt();
      uint8_t timeBetweenObject = request->getParam("objectdetect", true)->value().toInt();
      uint8_t sensorOnOffTime = request->getParam("onofftime", true)->value().toInt();

      if (s == "" && p == "") {
        request->send(200, "text/html", "<h3>SSID And Password not Provided</h3>");
      } 
      else {
        if (inputSensor1Name == "") { inputSensor1Name = "sensor1"; }
        if (inputSensor2Name == "") { inputSensor2Name = "sensor2"; }


        //store in preference
        preferences.begin("sensor", false);
        preferences.putString("sensor1Name", inputSensor1Name);
        preferences.putString("sensor2Name", inputSensor2Name);
        preferences.putBool("sensor1Mode", sensor1Modew);
        preferences.putBool("sensor2Mode", sensor2Modew);
        preferences.putBool("sensor1On", sensor1OnChoice);
        preferences.putInt("sensor2On", sensor2OnChoice);
        preferences.putBool("sensor1Type", sensor1NoNcChoice);
        preferences.putBool("sensor2Type", sensor2NoNcChoice);
        preferences.putBool("shiftChoice", sensorShiftChoice);
        preferences.putBool("shiftCount", ShiftCountOnChoice);
        preferences.putBool("jsonCountOn", jsonCountOnChoice);
        preferences.putInt("inputTimeDiff", inputTimeDifference);
        preferences.putBool("timeDiffPub", sensorTimeDiffPublish);
        preferences.putBool("outputOn", outputOnChoice);
        preferences.putBool("outputOnTrig", sensorTriggerOutputOn);
        preferences.putBool("timeDiffOut", sensorTimeDiffOutOn);
        preferences.putInt("acceptSelect", sensorAcceptTimeSelect);
        preferences.putInt("acceptTime", sensorAcceptTimeSeconds);
        preferences.putInt("timeBwObject", timeBetweenObject);
        preferences.putInt("sensorOnOffTime", sensorOnOffTime);
        preferences.end();


        preferences.begin("mywifi", false);
        preferences.putString("ssid", s);
        preferences.putString("password", p);
        preferences.end();

        request->send(200, "text/html", "<h3>Credentials Saved. Restarting...</h3>");
        Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++");

        shouldRestartESP = true;
      }
    }
  });

  server.begin();
  return;
}