void mqttReconnectCheck(){
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
      Serial.println(" try again in 5 seconds");
    }
  }
  return;
}



void mqttPublishData(bool *shiftCount,bool *jsonCounterOn ){//Publish Mqtt Data
   //long now = millis();
  while (millis() - lastMqttDataPub > 2000) {
    lastMqttDataPub = millis();
    mqttDataCount++;
    Serial.println("----------------------------------------------------------------+");
    char msg[50];
    snprintf (msg, 50, "hello world #%d", mqttDataCount);
    // Publish a message to a topic
    mqttClient.publish("esp32/test/kinjal_mqtt_topic", msg);
    Serial.print("Publishing message: ");
    Serial.println(msg);
    
     Serial.println(storedsensor1OnTime);
     
     
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
      char msg2[50]; 
      snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedSensor1Name ,sensor1Count, storedSensor2Name, sensor2Count);
      mqttClient.publish("kinjal/esp32/counter1", msg2);
      Serial.println(msg2);
      }
      //if sensor1 is off and sensor2 is on
      else if (storedSensor1OnChoice== false && storedSensor2OnChoice == 1){
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", storedSensor2Name, sensor2Count);
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      //if sensor1 is on and sensor2 off
      else if (storedSensor1OnChoice == true && storedSensor2OnChoice == 2){
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", storedSensor1Name, sensor1Count);
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      //if sensor1 is on and sensor2 is reset switch for sensor1 counter
      else if (storedSensor1OnChoice == true && storedSensor2OnChoice == 3){
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedSensor1Name ,sensor1Count, storedSensor2Name, sensor2Count);
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      //If no Sensor On
      else{
        char msg2[50]; 
        snprintf ( msg2, 50, " {no sensor on %d}", mqttDataCount );
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
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
       char msg1[50];
       snprintf(msg1, 50, "Proxi Counter: %d", sensorCount);  
       mqttClient.publish("kinjal/esp32/counter", msg1);
       Serial.println(msg1);
       }
       else{
        char msg1[50];
        snprintf(msg1, 50, "Proxi Counter: %d", sensorCount );  
        mqttClient.publish("kinjal/esp32/counter", msg1);
        Serial.println(msg1);
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