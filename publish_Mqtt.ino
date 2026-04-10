void mqttReconnectCheck(){
  // Loop until we're reconnected
  
  if (!mqttClient.connected() && millis()- lastMqttCheck > 2000) {
    lastMqttCheck = millis();
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // test.mosquitto.org does not require username/password
    const char* mqttClientId = "ESP32_Kinjal_99214po";
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
      //delay(5000);
    }
  }
  return;
}



void mqttPublishData(uint8_t *shiftCount,uint8_t *jsonCounterOn ){
   //long now = millis();
     // Serial.println("----------------------------------------------------------------+");
  while (millis() - lastMqttDataPublish > 2000) {
    lastMqttDataPublish = millis();
    commomMqttDataCountValue++;
    Serial.println("----------------------------------------------------------------+");
    char msg[50];
    snprintf (msg, 50, "hello world #%d", commomMqttDataCountValue);
     //Serial.println("----------------------------------------------------------------+");
    // Publish a message to a topic
    mqttClient.publish("esp32/test/kinjal_mqtt_topic", msg);
    Serial.print("Publishing message: ");
    Serial.println(msg);
    
  

    if (sensor1Triggered == true ) {//sensor detects input
      Serial.print("metal detected");
      Serial.print(sensor1ProxiCount);
      Serial.println(" times.");
      //detected_proxi1 = false;
    }

    if (sensor2Triggered == true) {//sensor detects
      Serial.print("metal detected");
      Serial.print(sensor2ProxiCount);
      Serial.println(" times.");
      //detected_proxi1 = false;
    }
    

    



   // DynamicJsonDocument doc(1024);or
    Serial.println(storedSensor1Name);
    Serial.println(storedSensor2Name);
   // Serial.println(storedInput1Mode);
    //Serial.println(storedInput2Mode);
    Serial.println(storedSensor1OnChoice);
    Serial.println(storedSensor2OnChoice);
    Serial.println(storedSensorShiftChoise);
    Serial.println(*jsonCounterOn);
    Serial.println(storedUserInputSensorsTimeDifferenceInSeconds);
    Serial.println(storedOutputONSetupChoise);
    Serial.println(storedSensorIndividualAcceptTimeSelectAndOutputOn);
    Serial.println(storedSensorIndividualAcceptTimeUserInputInSeconds);
    Serial.println(storedSensorDifferenceOutputAlerton);
    /*JsonDocument doc;
    doc[storedInput1] = 0;
    doc[storedInput2] = 0;
    serializeJson(doc,Serial);//first convert the JSON object into a string (serialization)
    char buffer[256];
    serializeJson(doc, buffer);
    mqtt_client.publish("kinjal/esp32/countertype", buffer);*/

    
    

    if (*jsonCounterOn == 1){

      if (storedSensor1OnChoice == 1 && storedSensor2OnChoice == 1){// if both sensors are on
      char msg2[50]; 
      snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedSensor1Name ,sensor1ProxiCount, storedSensor2Name, sensor2ProxiCount);
      mqttClient.publish("kinjal/esp32/counter1", msg2);
      Serial.println(msg2);
      }
      else if (storedSensor1OnChoice== 2 && storedSensor2OnChoice == 1){//if sensor1 is off and sensor2 is on
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", storedSensor2Name, sensor2ProxiCount);
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      else if (storedSensor1OnChoice == 1 && storedSensor2OnChoice == 2){//if sensor1 is on and sensor2 off
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", storedSensor1Name, sensor1ProxiCount);
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      else if (storedSensor1OnChoice == 1 && storedSensor2OnChoice == 3){//if sensor1 is on and sensor2 is reset switch for sensor1
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedSensor1Name ,sensor1ProxiCount, storedSensor2Name, sensor2ProxiCount);
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }

      else{
        char msg2[50]; 
        snprintf ( msg2, 50, " {no sensor on %d}", commomMqttDataCountValue );
        mqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);

       }
    }



    if (storedSensorShiftChoise == 1){
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
       snprintf(msg1, 50, "Proxi Counter: %d", proxiCounterAfterBothSensorDetects);  //for both proximity detects one object than counter increments
       mqttClient.publish("kinjal/esp32/counter", msg1);
       Serial.println(msg1);
       }
       else{
        char msg1[50];
        snprintf(msg1, 50, "Proxi Counter: %d", proxiCounterAfterBothSensorDetects );  //for both proximity detects one object than counter increments
        volatile int32_t sensor1ProxiCount = 0;
        volatile int32_t sensor2ProxiCount = 0;
        volatile bool sensor1Triggered = false;
        volatile bool sensor2Triggered = false;
        mqttClient.publish("kinjal/esp32/counter", msg1);
        Serial.println(msg1);
       }
    }


    if (storedSensorDifferenceAlertMqttPublish==1){
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