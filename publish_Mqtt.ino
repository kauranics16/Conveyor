void mqttReconnectCheck(){
  // Loop until we're reconnected
  
  if (!mqttClient.connected() && millis()- lastMqttCheck > 2000) {//Every 2 seconds
    lastMqttCheck = millis();
    Serial.print("Attempting MQTT connection...");
    const char* mqttClientId = "ESP32_Kinjal_99214po";// Does not require the username and Password
    if (mqttClient.connect(mqttClientId)) { 
      Serial.println("connected");
      // Subscribe to a topic
      //client.subscribe("esp32/test/kinjal_mqtt_topic");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      
    }
  }
  return;
}



void mqttPublishData(uint8_t *shiftCount,uint8_t *jsonCounterOn ){//Publish Mqtt Data
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

    
    

    if (*jsonCounterOn == 1){

      if (storedSensor1OnChoice == true && storedSensor2OnChoice == 1){// if both sensors are on
      char msg2[50]; 
      snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedSensor1Name ,sensor1Count, storedSensor2Name, sensor2Count);
      mqttClient.publish("kinjal/esp32/counter1", msg2);
      Serial.println(msg2);
      }
      else if (storedSensor1OnChoice== false && storedSensor2OnChoice == 1){//if sensor1 is off and sensor2 is on
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", storedSensor2Name, sensor2Count);
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      else if (storedSensor1OnChoice == true && storedSensor2OnChoice == 2){//if sensor1 is on and sensor2 off
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", storedSensor1Name, sensor1Count);
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      else if (storedSensor1OnChoice == true && storedSensor2OnChoice == 3){//if sensor1 is on and sensor2 is reset switch for sensor1
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedSensor1Name ,sensor1Count, storedSensor2Name, sensor2Count);
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }

      else{
        char msg2[50]; 
        snprintf ( msg2, 50, " {no sensor on %d}", mqttDataCount );
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);

       }
    }



    if (storedSensorShiftChoise == true){//If Right shift Choosen
      rightShiftStatus = true;
      sensorShiftRight();
       
    }
    else{
      //leftshiftcount = true;
      sensorShiftLeft();
    }

    if (*shiftCount == 1){
       if(rightShiftStatus == true){
       char msg1[50];
       snprintf(msg1, 50, "Proxi Counter: %d", sensorCount);  //for both proximity detects one object than counter increments
       mqttClient.publish("kinjal/esp32/counter", msg1);
       Serial.println(msg1);
       }
       else{
        char msg1[50];
        snprintf(msg1, 50, "Proxi Counter: %d", sensorCount );  //for both proximity detects one object than counter increments
        //volatile int32_t sensor1Count = 0;
        //volatile int32_t sensor2Count = 0;
        //volatile bool sensor1Triggered = false;
        //volatile bool sensor2Triggered = false;
        mqttClient.publish("kinjal/esp32/counter", msg1);
        Serial.println(msg1);
       }
    }


    if (storedMqttPubSensorTimeDiff==1){
       acceptableTimeAlert();
    }
    

  
   // serializeJson(doc, mqtt_client);
    //char buffer[256];
    //size_t n = serializeJson(doc, buffer);
    //serializeJson(doc, buffer, n);
    //mqtt_client.publish("kinjal/esp32/countertype", doc);
   // mqtt_client.beginMessage("kinjal/esp32/countertype");
    //serializeJson(doc, mqtt_client);
   // mqtt_client.endMessage();

  }
  return;
}