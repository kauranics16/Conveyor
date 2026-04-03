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
      String Sensor1_Mode = request->getParam("sensormode1", true)->value();
      String Sensor2_Mode = request->getParam("sensormode2", true)->value();
      uint8_t Sensor1onoff = request->getParam("myGroup", true)->value().toInt();
      uint8_t Sensor2onoff = request->getParam("myGroup1", true)->value().toInt();
      uint8_t Sensor1nonc = request->getParam("myGroup2", true)->value().toInt();
      uint8_t Sensor2nonc = request->getParam("myGroup3", true)->value().toInt();
      uint8_t SensorShiftchoise = request->getParam("Shifting", true)->value().toInt();//from sensor1 to sensor2 or sensor2 to sensor1
      uint8_t proxicounteronoff = request->getParam("proxicounteronoff", true)->value().toInt();// choise to wherether to publish sensor individual count i.e sensor1 : 1 , sensor2 :2..
      uint8_t Sensor_timedifference= request->getParam("seconds", true)->value().toInt();
      String Outputonoff = request->getParam("onoff", true)->value();

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


      //Serial.println(Sensor1_Mode);
      //Serial.println(Sensor2_Mode);
      
      preferences.begin("sensor", false);//store in preference
      preferences.putString("InputSensor_1", InputSensor1);
      preferences.putString("InputSensor_2",InputSensor2);
      preferences.putString("Sensor_1mode", Sensor1_Mode);
      preferences.putString("Sensor_2mode", Sensor2_Mode);
      preferences.putInt("Sensor1onoff", Sensor1onoff);
      preferences.putInt("Sensor2onoff", Sensor2onoff);
      preferences.putInt("Sensor1nonc", Sensor1nonc);
      preferences.putInt("Sensor2nonc", Sensor2nonc);
      preferences.putInt("Sensorshift", SensorShiftchoise);
      preferences.putInt("proxionoff", proxicounteronoff);
      preferences.putInt("proxi_time",Sensor_timedifference );
      preferences.putString("Outputonoff", Outputonoff);
      preferences.end();

      
      preferences.begin("mywifi", false);
      preferences.putString("ssid", s);
      preferences.putString("password", p);
      preferences.end();
 
      request->send(200, "text/html", "<h3>Credentials Saved. Restarting...</h3>");
      Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++");
     
      shouldRestartESP = true; 
      //delay(5000);
      //ESP.restart();
    }
  });
 
  server.begin();
  return;
}