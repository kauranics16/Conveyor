void MqttReconnectCheck(){
  // Loop until we're reconnected
  
  if (!MqttClient.connected() && millis()- LastMqttCheck > 2000) {
    LastMqttCheck = millis();
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // test.mosquitto.org does not require username/password
    const char* mqtt_client_id = "ESP32_Kinjal_99214po";
    if (MqttClient.connect(mqtt_client_id)) { 
      Serial.println("connected");
      // Subscribe to a topic
      //client.subscribe("esp32/test/kinjal_mqtt_topic");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(MqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  return;
}



void MqttPublish_data(uint8_t *shiftcount,uint8_t *Jsoncounteron ){
   //long now = millis();
     // Serial.println("----------------------------------------------------------------+");
  while (millis() - LastMqttDataPublish > 2000) {
    LastMqttDataPublish = millis();
    CommomMqttDataCountValue++;
    Serial.println("----------------------------------------------------------------+");
    char msg[50];
    snprintf (msg, 50, "hello world #%d", CommomMqttDataCountValue);
     //Serial.println("----------------------------------------------------------------+");
    // Publish a message to a topic
    MqttClient.publish("esp32/test/kinjal_mqtt_topic", msg);
    Serial.print("Publishing message: ");
    Serial.println(msg);
    
    //char msg1[50];
     /*int sensorState = digitalRead(PROX_SENSOR);
    Serial.println(sensorState);

   if(sensorState == LOW) {
      Serial.println("Metal Detected");
      proxi_counter++;
    } 
    else {
      Serial.println("No Metal Detected");
    }*/

    if (Senson1Triggered == true ) {//sensor detects input
      Serial.print("metal detected");
      Serial.print(Sensor1ProxiCount);
      Serial.println(" times.");
      //detected_proxi1 = false;
    }

    if (Sensor2Triggered == true) {//sensor detects
      Serial.print("metal detected");
      Serial.print(Sensor2ProxiCount);
      Serial.println(" times.");
      //detected_proxi1 = false;
    }
    

    


    //preferences.begin("sensor", true);
    //String storedInput1 = preferences.getString("InputSensor_1", "");//	getChar(const char* key, const int8_t defaultValue)
    //String storedInput2 = preferences.getString("InputSensor_2", "");
    //String storedInput1Mode=preferences.getString("Sensor_1mode", "");
   // String storedInput2Mode=preferences.getString("Sensor_2mode", "");
    //uint8_t storedsensor1offon=preferences.getInt("Sensor1onoff",0);
    //uint8_t storedsensor2offon=preferences.getInt("Sensor2onoff", 0);
    //uint8_t storedSensor1nonc=preferences.getInt("Sensor1nonc", 0);
    //uint8_t storedSensor2nonc=preferences.getInt("Sensor2nonc", 0);
    //uint8_t storedSensorShiftchoise=preferences.getString("Sensorshift", "");
    //uint8_t storedSensorproxicounteronoff=preferences.getInt("proxionoff", 0);
    //int storedSensor_timedifference = preferences.getInt("proxi_time",0);//getInt(const char* key, const int32_t defaultValue)
    //String storedOutputonoff=preferences.getString("Outputonoff", "");
   // preferences.end();

   // DynamicJsonDocument doc(1024);or
    Serial.println(StoredSensor1Name);
    Serial.println(StoredSensor2Name);
   // Serial.println(storedInput1Mode);
    //Serial.println(storedInput2Mode);
    Serial.println(StoredSensor1Offon);
    Serial.println(StoredSensor2Offon);
    Serial.println(StoredSensorShiftChoise);
    Serial.println(*Jsoncounteron);
    Serial.println(StoredUserInputSensorsTimeDifferenceInSeconds);
    Serial.println(StoredOutputONSetupChoise);
    Serial.println(StoredSensorIndividualAcceptTimeSelectAndOutputOn);
    Serial.println(StoredSensorIndividualAcceptTimeUserInputInSeconds);
    Serial.println(StoredSensorDiffOutputAlerton);
    /*JsonDocument doc;
    doc[storedInput1] = 0;
    doc[storedInput2] = 0;
    serializeJson(doc,Serial);//first convert the JSON object into a string (serialization)
    char buffer[256];
    serializeJson(doc, buffer);
    mqtt_client.publish("kinjal/esp32/countertype", buffer);*/

    
    

    if (*Jsoncounteron == 1){

      if (StoredSensor1Offon == 1 && StoredSensor2Offon == 1){// if both sensors are on
      char msg2[50]; 
      snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", StoredSensor1Name ,Sensor1ProxiCount, StoredSensor2Name, Sensor2ProxiCount);
      MqttClient.publish("kinjal/esp32/counter1", msg2);
      Serial.println(msg2);
      }
      else if (StoredSensor1Offon== 2 && StoredSensor2Offon == 1){//if sensor1 is off and sensor2 is on
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", StoredSensor2Name, Sensor2ProxiCount);
        MqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      else if (StoredSensor1Offon == 1 && StoredSensor2Offon == 2){//if sensor1 is on and sensor2 off
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", StoredSensor1Name, Sensor1ProxiCount);
        MqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      else if (StoredSensor1Offon == 1 && StoredSensor2Offon == 3){//if sensor1 is on and sensor2 is reset switch for sensor1
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", StoredSensor1Name ,Sensor1ProxiCount, StoredSensor2Name, Sensor2ProxiCount);
        MqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }

      else{
        char msg2[50]; 
        snprintf ( msg2, 50, " {no sensor on %d}", CommomMqttDataCountValue );
        MqttClient.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);

       }


     // proxi_counteron_Config();

      /*char msg2[50]; 
      snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedInput1 ,proxi_counter1, storedInput2, proxi_counter2);
      mqtt_client.publish("kinjal/esp32/counter1", msg2);
      Serial.println(msg2);*/
    }



    if (StoredSensorShiftChoise == 1){
      RightShiftStatus = true;
      SensorShiftRight();
       
    }
    else{
      //leftshiftcount = true;
      SensorShiftLeft();
    }

    if (*shiftcount == 1){
       if(RightShiftStatus == true){
       char msg1[50];
       snprintf(msg1, 50, "Proxi Counter: %d", ProxiCounterAfterBothSensorDetects);
       MqttClient.publish("kinjal/esp32/counter", msg1);
       Serial.println(msg1);
       }
       else{
        char msg1[50];
        snprintf(msg1, 50, "Proxi Counter: %d", ProxiCounterAfterBothSensorDetects );  //for both proximity detects one object than counter increments
        volatile int32_t Sensor1ProxiCount = 0;
        volatile int32_t Sensor2ProxiCount = 0;
        volatile bool Senson1Triggered = false;
        volatile bool Sensor2Triggered = false;
        MqttClient.publish("kinjal/esp32/counter", msg1);
        Serial.println(msg1);
       }
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