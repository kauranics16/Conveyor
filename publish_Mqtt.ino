void MqttReconnectCheck(){
  // Loop until we're reconnected
  
  if (!mqtt_client.connected() && millis()- lastmqttcheck > 2000) {
    lastmqttcheck = millis();
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // test.mosquitto.org does not require username/password
    const char* mqtt_client_id = "ESP32_Kinjal_99214po";
    if (mqtt_client.connect(mqtt_client_id)) { 
      Serial.println("connected");
      // Subscribe to a topic
      //client.subscribe("esp32/test/kinjal_mqtt_topic");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  return;
}



void MqttPublish_data(){
   //long now = millis();
     // Serial.println("----------------------------------------------------------------+");
  while (millis() - lastMsg > 2000) {
    lastMsg = millis();
    value++;
    Serial.println("----------------------------------------------------------------+");
    char msg[50];
    snprintf (msg, 50, "hello world #%d", value);
     //Serial.println("----------------------------------------------------------------+");
    // Publish a message to a topic
    mqtt_client.publish("esp32/test/kinjal_mqtt_topic", msg);
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

    if (detected_proxi1 == true ) {//sensor detects input
      Serial.print("metal detected");
      Serial.print(proxi_counter1);
      Serial.println(" times.");
      //detected_proxi1 = false;
    }

    if (detected_proxi2 == true) {//sensor detects
      Serial.print("metal detected");
      Serial.print(proxi_counter2);
      Serial.println(" times.");
      //detected_proxi1 = false;
    }
    

    


    preferences.begin("sensor", true);
    String storedInput1 = preferences.getString("InputSensor_1", "");//	getChar(const char* key, const int8_t defaultValue)
    String storedInput2 = preferences.getString("InputSensor_2", "");
    String storedInput1Mode=preferences.getString("Sensor_1mode", "");
    String storedInput2Mode=preferences.getString("Sensor_2mode", "");
    //uint8_t storedsensor1offon=preferences.getInt("Sensor1onoff",0);
    //uint8_t storedsensor2offon=preferences.getInt("Sensor2onoff", 0);
    //uint8_t storedSensor1nonc=preferences.getInt("Sensor1nonc", 0);
    //uint8_t storedSensor2nonc=preferences.getInt("Sensor2nonc", 0);
    //uint8_t storedSensorShiftchoise=preferences.getString("Sensorshift", "");
    uint8_t storedSensorproxicounteronoff=preferences.getInt("proxionoff", 0);
    int storedSensor_timedifference = preferences.getInt("proxi_time",0);//getInt(const char* key, const int32_t defaultValue)
    String storedOutputonoff=preferences.getString("Outputonoff", "");
    preferences.end();
   // DynamicJsonDocument doc(1024);or
    Serial.println(storedInput1);
    Serial.println(storedInput2);
    Serial.println(storedInput1Mode);
    Serial.println(storedInput2Mode);
    Serial.println(storedsensor1offon);
    Serial.println(storedsensor2offon);
    Serial.println(storedSensorShiftchoise);
    Serial.println(storedSensorproxicounteronoff);
    Serial.println(storedSensor_timedifference);
    Serial.println(storedOutputonoff);
    

    /*JsonDocument doc;
    doc[storedInput1] = 0;
    doc[storedInput2] = 0;
    serializeJson(doc,Serial);//first convert the JSON object into a string (serialization)
    char buffer[256];
    serializeJson(doc, buffer);
    mqtt_client.publish("kinjal/esp32/countertype", buffer);*/

    
    

    if (storedSensorproxicounteronoff == 1){

      if (storedsensor1offon == 1 && storedsensor2offon == 1){// if both sensors are on
      char msg2[50]; 
      snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedInput1 ,proxi_counter1, storedInput2, proxi_counter2);
      mqtt_client.publish("kinjal/esp32/counter1", msg2);
      Serial.println(msg2);
      }
      else if (storedsensor1offon == 2 && storedsensor2offon == 1){//if sensor1 is off and sensor2 is on
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", storedInput2, proxi_counter2);
        mqtt_client.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      else if (storedsensor1offon == 1 && storedsensor2offon == 2){//if sensor1 is on and sensor2 off
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \" : %d }", storedInput1, proxi_counter1);
        mqtt_client.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }
      else if (storedsensor1offon == 1 && storedsensor2offon == 3){//if sensor1 is on and sensor2 is reset switch for sensor1
        char msg2[50]; 
        snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedInput1 ,proxi_counter1, storedInput2, proxi_counter2);
        mqtt_client.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);
      }

      else{
        char msg2[50]; 
        snprintf ( msg2, 50, " {no sensor on %d}", value );
        mqtt_client.publish("kinjal/esp32/counter1", msg2);
        Serial.println(msg2);

       }


     // proxi_counteron_Config();

      /*char msg2[50]; 
      snprintf ( msg2, 50, " {\" %s \": %d ,\" %s \" : %d }", storedInput1 ,proxi_counter1, storedInput2, proxi_counter2);
      mqtt_client.publish("kinjal/esp32/counter1", msg2);
      Serial.println(msg2);*/
    }

    if (storedSensorShiftchoise == 1){
      SensorShiftRight();
    }
    else{
      SensorShiftLeft();
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