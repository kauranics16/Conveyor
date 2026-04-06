


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
 Left Right Shift Count Choice: <input type="radio"  name="Shiftcount" value=1> ON
 <input type="radio"  name="Shiftcount" value=2>OFF<br>
 Proximity Individual Counter: <input type="radio"  name="proxicounteronoff" value=1>ON
 <input type="radio"  name="proxicounteronoff" value=2>OFF<br>
 Input Acceptable Time in Seconds :<br><input type="number" name="seconds"><br>
 <label for="cars">Choose Output on or off:</label>
 <select name="onoff" id="onoff">
    <option value="ON">ON</option>
    <option value="OFF">OFF</option>
 </select><br>
 output ON on sensor trigger?<input type="radio"  name="outputtrig" value=1>ON
 <input type="radio"  name="outputtrig" value=2>OFF<br>
 Output Alert on gap between sensors larger than inputed above<input type="radio"  name="outputalert" value=1>ON
 <input type="radio"  name="outputalert" value=2>OFF<br>
 Individual sensor output on <input type="radio"  name="individual" value=1> Acceptable time sensor1
 <input type="radio"  name="individual" value=2> Acceptable time sensor2
 <input type="radio"  name="individual" value=3> on reset
 <input type="radio"  name="individual" value=4> offff<br>
 Input Acceptable Time in Seconds for above selected option :<br><input type="number" name="accepttime"><br>
 SSID:<br><input type="text" name="ssid"><br>
 Password:<br><input type="password" name="pass"><br><br>
 <input type="submit" value="Save">
 </form>
</body></html>)rawliteral";




void WebServerConfig(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){// "/" means the host ip address
  request->send(200, "text/html", htmlForm);
  });
 
  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      String s = request->getParam("ssid", true)->value();
      String p = request->getParam("pass", true)->value();
      String InputSensor1 = request->getParam("input1", true)->value();
      String InputSensor2 = request->getParam("input2", true)->value();
      String Sensor1Mode = request->getParam("sensormode1", true)->value();
      String Sensor2Mode = request->getParam("sensormode2", true)->value();
      uint8_t Sensor1onoff = request->getParam("myGroup", true)->value().toInt();
      uint8_t Sensor2onoff = request->getParam("myGroup1", true)->value().toInt();
      uint8_t Sensor1nonc = request->getParam("myGroup2", true)->value().toInt();
      uint8_t Sensor2nonc = request->getParam("myGroup3", true)->value().toInt();
      uint8_t SensorShiftchoise = request->getParam("Shifting", true)->value().toInt();//from sensor1 to sensor2 or sensor2 to sensor1
      uint8_t proxicounteronoff = request->getParam("proxicounteronoff", true)->value().toInt();// choise to wherether to publish sensor individual count i.e sensor1 : 1 , sensor2 :2..
      uint8_t proxishiftcounteronoff = request->getParam("Shiftcount", true)->value().toInt();
      uint8_t Sensorusertimedifference= request->getParam("seconds", true)->value().toInt();
      String Outputonoff = request->getParam("onoff", true)->value();
      uint8_t SensorIndividualAcceptTimeselect = request->getParam("individual", true)->value().toInt();
      uint8_t SensorIndividualAcceptTimeinput = request->getParam("accepttime", true)->value().toInt();
      uint8_t SensorIndividualTriggerOutputon = request->getParam("outputtrig", true)->value().toInt();
      uint8_t SensorDiffOutputAlerton = request->getParam("outputalert", true)->value().toInt();

      Serial.println("input by user:");
      Serial.println(InputSensor1);
      Serial.println(InputSensor2);
      Serial.println(Sensor1onoff);
      Serial.println(Sensor2onoff);
      Serial.println(Sensor1nonc);
      Serial.println(Sensor2nonc);
      Serial.println(SensorShiftchoise);
      Serial.println(proxicounteronoff);
      Serial.println(Outputonoff);
      Serial.println(StoredSensorIndividualAcceptTimeUserInputInSeconds);


      //Serial.println(Sensor1_Mode);
      //Serial.println(Sensor2_Mode);
      
      preferences.begin("sensor", false);//store in preference
      preferences.putString("InputSensor_1", InputSensor1);
      preferences.putString("InputSensor_2",InputSensor2);
      preferences.putString("Sensor_1mode", Sensor1Mode);
      preferences.putString("Sensor_2mode", Sensor2Mode);
      preferences.putInt("Sensor1onoff", Sensor1onoff);
      preferences.putInt("Sensor2onoff", Sensor2onoff);
      preferences.putInt("Sensor1nonc", Sensor1nonc);
      preferences.putInt("Sensor2nonc", Sensor2nonc);
      preferences.putInt("Sensorshift", SensorShiftchoise);
      preferences.putInt("proxionoff", proxicounteronoff);
      preferences.putInt("Shiftcount", proxishiftcounteronoff);
      
      preferences.putInt("proxi_time",Sensorusertimedifference );
      preferences.putString("Outputonoff", Outputonoff);
      preferences.putInt("individual",SensorIndividualAcceptTimeselect);
      preferences.putInt("accepttime",SensorIndividualAcceptTimeinput);
      preferences.putInt("outputtrig",SensorIndividualTriggerOutputon);
      preferences.putInt("outputalert",SensorDiffOutputAlerton);
      preferences.end();

      
      preferences.begin("mywifi", false);
      preferences.putString("ssid", s);
      preferences.putString("password", p);
      preferences.end();
 
      request->send(200, "text/html", "<h3>Credentials Saved. Restarting...</h3>");
      Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++");
     
      ShouldRestartESP = true; 
      //delay(5000);
      //ESP.restart();
    }
  });
 
  server.begin();
  return;
}