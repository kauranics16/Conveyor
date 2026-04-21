void mqttReconnectCheck(){
  static unsigned long lastMqttCheck = 0;
  //Every 2 Seconds Try to reconnect
  if (!mqttClient.connected() && millis()- lastMqttCheck > 2000) {
    lastMqttCheck = millis();
    Serial.print("Attempting MQTT connection...");
    const char* mqttClientId = "ESP32_Kinjal_99214po";
    // Does not require the username and Password
    if (mqttClient.connect(mqttClientId)) { 
      Serial.println("connected");
      // Subscribe to a topic
      //client.subscribe("esp32/test/kinjal_mqtt_topic");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      // Wait 5 seconds before retrying
      Serial.println(" try again in 2 seconds");
    }
  }
  return;
}



void mqttPublishData(bool *shiftCount,bool *jsonCounterOn ){//Publish Mqtt Data
   //long now = millis();
  static unsigned long lastMqttDataPub = 0;
  while (millis() - lastMqttDataPub > 2000) {
    lastMqttDataPub = millis();
    mqttDataCount++;
    Serial.println("----------------------------------------------------------------+");
     Serial.println(storedTimeBwObject);
    // DynamicJsonDocument doc(1024);or                             
    /*JsonDocument doc;
    doc[storedInput1] = 0;
    doc[storedInput2] = 0;
    serializeJson(doc,Serial);//first convert the JSON object into a string (serialization)
    char buffer[256];
    serializeJson(doc, buffer);
    mqtt_client.publish("kinjal/esp32/countertype", buffer);*/


    if (*jsonCounterOn == true){
      /// if both sensors are on
      if (storedSensor1OnChoice == true && storedSensor2OnChoice == 1){
      
      String counterMsg = "{";
      counterMsg+= "\"" + String(storedSensor1Name) + "\":" + String(sensor1Count) + ",";
      counterMsg += "\"" + String(storedSensor2Name) + "\":" + String(sensor2Count);
      counterMsg += "}";
      mqttClient.publish("kinjal/esp32/counter1", counterMsg.c_str());
      Serial.println(counterMsg);
      }
      //if sensor1 is off and sensor2 is on
      else if (storedSensor1OnChoice== false && storedSensor2OnChoice == 1){
        String counterMsg = "{";
        counterMsg += "\"" + String(storedSensor2Name) + "\":" + String(sensor2Count);
        counterMsg += "}";
        mqttClient.publish("kinjal/esp32/counter1", counterMsg.c_str());
        Serial.println(counterMsg);
      }
      //if sensor1 is on and sensor2 off
      else if (storedSensor1OnChoice == true && storedSensor2OnChoice == 2){
        String counterMsg = "{";
        counterMsg += "\"" + String(storedSensor1Name) + "\":" + String(sensor1Count);
        counterMsg += "}";
        mqttClient.publish("kinjal/esp32/counter1", counterMsg.c_str());
        Serial.println(counterMsg);
      }
      //if sensor1 is on and sensor2 is reset switch for sensor1 counter
      else if (storedSensor1OnChoice == true && storedSensor2OnChoice == 3){
        String counterMsg = "{";
        counterMsg+= "\"" + String(storedSensor1Name) + "\":" + String(sensor1Count) + ",";
        counterMsg += "\"" + String(storedSensor2Name) + "\":" + String(sensor2Count);
        counterMsg += "}";
        mqttClient.publish("kinjal/esp32/counter1", counterMsg.c_str());
        Serial.println(counterMsg);
      }
      //If no Sensor On
      else{
        String counterMsg = "No Sensor Found";
        mqttClient.publish("kinjal/esp32/counter1", counterMsg.c_str());
        Serial.println(counterMsg);
       }
    }


    //If Right shift Choosen
    if (storedSensorShiftChoise == true){
      rightShiftStatus = true;
      sensorShiftRight();
       
    }
    //If Left Shift Choosen
    else{
      sensorShiftLeft();
    }
    
    //if User Need Counter on Left and Right Shift
    //For both proximity detects one object than counter increments
    if (*shiftCount == true){
       if(rightShiftStatus == true){
       
       String counterShift;
       counterShift += "ShiftRightCounter:" +  String(sensorCount);
       mqttClient.publish("kinjal/esp32/counter", counterShift.c_str());
       Serial.println(counterShift);
       }
       else{
       String counterShift;
       counterShift += "ShiftLeftCounter:" + String(sensorCount);
       mqttClient.publish("kinjal/esp32/counter", counterShift.c_str());
       Serial.println(counterShift);
       }
    }

    //If User Want Mqtt message of Time Difference Alert
    if (storedMqttPubSensorTimeDiff==true){
       acceptableTimeAlert();
    }
    
    //serializeJson(doc, mqtt_client);
    //char buffer[256];
    //size_t n = serializeJson(doc, buffer);
    //serializeJson(doc, buffer, n);
    //mqtt_client.publish("kinjal/esp32/countertype", doc);
    //mqtt_client.beginMessage("kinjal/esp32/countertype");
    //serializeJson(doc, mqtt_client);
    //mqtt_client.endMessage();

  }
  return;
}