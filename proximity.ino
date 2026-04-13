void ARDUINO_ISR_ATTR Proximity1_ISR() {//Intterupt Service Routine for Input1 (Sensor1)
  sensor1Count++;
  sensor1TimeMqttPub = millis();
  sensor1TimeTriggered = millis();
  objectDetectedSensor1 = true;
  sensor1MqttPub = true;
  sensor1TriggeredAlert = true;
  sensor1Shift = true;
}

void ARDUINO_ISR_ATTR Proximity2_ISR() {//Interrupt Service Routine of Input2 (Sensor2)
  sensor2Count++;
  sensor2TimeMqttPub= millis();
  sensor2TimeTriggered = millis();
  objectDetectedSensor2 = true;
  sensor2MqttPub = true;
  sensor2TriggeredAlert = true;
  sensor2Shift = true;
}

void ARDUINO_ISR_ATTR RESET_ISR() {// Interupt Routine for Input2 as Reset Button
  sensor1Count=0;
  sensor2Count++;
  waitingForReset = true;
  sensor2Shift = true;
}



//Configure and Load all data stored in Esp32 Memory
void configInputOutput(){
  preferences.begin("sensor", true);
  storedSensor1Name = preferences.getString("InputSensor_1", "");
  storedSensor2Name = preferences.getString("InputSensor_2", "");
  bool storedInput1Mode=preferences.getBool("Sensor_1mode", false);
  bool storedInput2Mode=preferences.getBool("Sensor_2mode", false);
  storedSensor1OnChoice=preferences.getBool("Sensor1onoff", false);
  storedSensor2OnChoice=preferences.getInt("Sensor2onoff", 0);
  bool storedSensor1NoNcChoice=preferences.getBool("Sensor1nonc", false);
  bool storedSensor2NoNcChoice=preferences.getBool("Sensor2nonc", false);
  storedSensorShiftChoise=preferences.getBool("Sensorshift", false);
  storedAcceptTimeSelect = preferences.getInt("individual", 0);
  storedAcceptTimeSeconds=preferences.getInt("accepttime", 0);
  storedSensorTriggerOut=preferences.getBool("outputtrig", false);
  storedMqttPubSensorTimeDiff=preferences.getBool("outputpublish", false);
  storedSensorDiffOut=preferences.getBool("outputalert", false);
  storedSensorTimeDiffSeconds = preferences.getInt("proxi_time",0);
  bool storedOutputChoice=preferences.getBool("Outputonoff",false);
  storedsensor1OnTime=preferences.getBool("sensor1OnTime",0);      
  preferences.end();


  //If In Sensor1 mode
  if (storedSensor1OnChoice == true){
    if(storedSensor1NoNcChoice == true){
      if (storedInput1Mode == true){
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING );
      }
      else{
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING );
      }
    }
    else{
      if (storedInput1Mode == false){
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING );
      }
      else{
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING );
      }

    }
    
  }

  //If Input2 Is In Sensor2 mode
  if (storedSensor2OnChoice == 1){
    if(storedSensor2NoNcChoice == true){
      if (storedInput2Mode == true){
      
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING );
      }
      else{
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING );
      }
    }
    else{//If NC Choosen 
      if (storedInput1Mode == false){
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING );
      }
      else{
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING );
      }

    }
    
  }

  // If Input2 Is In Reset Mode (Button)
  if (storedSensor2OnChoice == 3){
    if(storedSensor2NoNcChoice == true){
      if (storedInput2Mode == true){
      
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING );
      }
      else{
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING );
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == false){
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING );
      }
      else{
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING );
      }

    }
    
  }

  if (storedOutputChoice==true){
    pinMode(OUTPUT_PIN, OUTPUT);
    Serial.println("output ON mmmmmmmmm");
  }


  return;

}


//Sensor Shift Right is Choosen by User
void sensorShiftRight(){
  if (sensor1Shift == false && sensor2Shift == true) {
    Serial.print("metal detected");
    sensor1Shift = false;
    sensor2Shift = false;
  }
    
  //If Sensor 1 Detects Then And Only Than sensor2 Detects And Count Incremented For Shift Count
  if (sensor1Shift  && sensor2Shift == true) {
    Serial.print("metal detected");
    sensorCount++;
    Serial.print(sensorCount);
    Serial.println(" times.");
    /*if (Sensor1TimeWhenTriggered > Sensor2TimeWhenTriggered){
      timeDiffInSensors = Sensor1TimeWhenTriggered - Sensor2TimeWhenTriggered;
    }
    else{
      timeDiffInSensors = Sensor2TimeWhenTriggered - Sensor1TimeWhenTriggered;
    }

    if (storedMqttPubSensorTimeDiff==1){
      AcceptableTime();
    }
    Sensor1TimeWhenTriggered = 0;
    Sensor2TimeWhenTriggered = 0;*/
    sensor1Shift = false;
    sensor2Shift = false;
  }
  return;
}


//Sensor Shift Left is Choosen by User
void sensorShiftLeft(){
  if (sensor1Shift == true && sensor2Shift == false) {
    Serial.print("metal detected");
    sensor1Shift = false;
    sensor2Shift = false;
  }
    
  //If Sensor 2 Detects Then And Only Than sensor1 Detects And Count Incremented For Shift Count
  if (sensor1Shift  && sensor2Shift == true) { 
    Serial.print("metal detected");
    sensorCount++;
    Serial.print(sensorCount);
    Serial.println(" times.");
    /*if (Sensor1TimeWhenTriggered > Sensor2TimeWhenTriggered){
      timeDiffInSensors = Sensor1TimeWhenTriggered - Sensor2TimeWhenTriggered;
    }
    else{
      timeDiffInSensors= Sensor2TimeWhenTriggered- Sensor1TimeWhenTriggered;
    }
      
    if (storedMqttPubSensorTimeDiff==1){
      AcceptableTime();
    }
    Sensor1TimeWhenTriggered = 0;
    Sensor2TimeWhenTriggered=0;*/
    sensor1Shift = false;
    sensor2Shift = false;
  }
  return;
}

/*
/// Integrated with RightShift, leftShift Function for MqttPublish Alert on Time Difference Gap (Same Conveyor) (MqttPublish data)
void AcceptableTime(){

  if (timeDiffInSensors > storedSensorTimeDiffSeconds*1000UL){
    char msg3[50];
    snprintf(msg3, 50, "Time difference not Acceptable: %d", sensorCount);
    mqttClient.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);

    timeDiffInSensors = 0;  
  }
  else{

    char msg3[50];
    snprintf(msg3, 50, "Time difference Acceptable: %d", sensorCount);
    mqttClient.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);
    timeDiffInSensors = 0;
  }
  //TimeDifference=0;
  return;
}*/



//Time Difference Between Sensors MqttPublish Alert Message (Different Conveyor)
void acceptableTimeAlert(){
  static unsigned long timeDiffInSensors = 0;
  if (sensor1MqttPub==true && sensor2MqttPub == true){
    Serial.println("hhhhhhhhhhhhdddddddddd");
    sensor1MqttPub=false;
    sensor2MqttPub =false;
    if (sensor1TimeMqttPub >  sensor2TimeMqttPub){
      timeDiffInSensors =  sensor1TimeMqttPub-  sensor2TimeMqttPub; 
    }
    else{
      timeDiffInSensors =  sensor2TimeMqttPub - sensor1TimeMqttPub;
    }
  }
  if (timeDiffInSensors> storedSensorTimeDiffSeconds*1000UL){
    char msg3[50];
    snprintf(msg3, 50, "Time difference not Acceptable:");
    mqttClient.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);

    timeDiffInSensors=0;
    sensor1TimeMqttPub=0;
    sensor2TimeMqttPub=0;
  }
          
  return;
}


//Time Difference Between Sensors Output Alert Of Few Seconds
void timeDiffAlertBetweenSensors(){ 
  static bool outActiveOnSensorDiff = false;
  static unsigned long timeDifferenceAlert=0;
  static unsigned long outTimeOnSensorDiff = 0;
  if (sensor1TriggeredAlert==true && sensor2TriggeredAlert == true){
    Serial.println("hhhhhhhhhhhhdddddddddd");
    outTimeOnSensorDiff = millis();
    sensor1TriggeredAlert = false;
    sensor2TriggeredAlert = false;
      
    if (sensor1TimeTriggered > sensor2TimeTriggered){
      timeDifferenceAlert = sensor1TimeTriggered - sensor2TimeTriggered; 
    }
    else{
      timeDifferenceAlert = sensor2TimeTriggered - sensor1TimeTriggered;
    }
  }
  //Output On For Few Seconds Only When Time Difference Between 2 Sensors Is Beyond Time Gap Inputed by User
  if (timeDifferenceAlert > storedSensorTimeDiffSeconds*1000UL){
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnSensorDiff = true;
    Serial.println("555555555555555555555f5554545sss5");
    Serial.println(millis() - outTimeOnSensorDiff);
    if (outActiveOnSensorDiff && (millis() - outTimeOnSensorDiff >= 1000UL)) {//Buzzer on 1000 mili seconds
      Serial.println("lowwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
      digitalWrite(OUTPUT_PIN, LOW);
      outActiveOnSensorDiff = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
      timeDifferenceAlert=0;
      sensor1TimeTriggered=0;
      sensor2TimeTriggered=0;
    }      
  }
  return;
}


//Output On For Few seconds When Sensors detect Respectively
void sensorTriggerOutputOn(){
  static bool outActiveOnTrigger = false;
  static unsigned long outTimeOnSensorTrig = 0;
  if (objectDetectedSensor1 == true || objectDetectedSensor2 == true){
    Serial.println("objectdetected");
    outTimeOnSensorTrig = millis();
    objectDetectedSensor1 = false;
    objectDetectedSensor2 = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnTrigger = true;
  }
  //Output On For 100 mili seconds
  if (outActiveOnTrigger && (millis() - outTimeOnSensorTrig >= 100UL)) {
    digitalWrite(OUTPUT_PIN, LOW);
    outActiveOnTrigger = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;
} 





void sensor1AcceptableTime(){
  static unsigned long lastOutTimeSensor1 = 0; 
  static bool outStateSensor1 = false;
  static unsigned long timeDifferenceSensor1 = 0;
  outTimeSensor1 = millis();
  if (objectDetectedSensor1 == true){
    Serial.println("objectdetected");
    lastOutTimeSensor1 = outTimeSensor1;
    objectDetectedSensor1 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    outStateSensor1=true;
  }
  timeDifferenceSensor1 = outTimeSensor1 -  lastOutTimeSensor1;
  //Output On continuously when Time needed by sensor1 to detect something is beyond Time Gap Inputed by User
  if (timeDifferenceSensor1>storedAcceptTimeSeconds*1000UL){
     if(outStateSensor1==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(OUTPUT_PIN, HIGH);
       outStateSensor1=false;
     }
  }
  return;
}


void sensor2AcceptableTime(){
  static unsigned long lastOutTimeSensor2 = 0; 
  static bool outStateSensor2 = false;
  static unsigned long timeDifferenceSensor2 = 0;
  outTimeSensor2 = millis();
  if (objectDetectedSensor2 == true){
    Serial.println("objectdetected");
    lastOutTimeSensor2 = outTimeSensor2;
    objectDetectedSensor2 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    outStateSensor2=true;

  }
  timeDifferenceSensor2 = outTimeSensor2 -  lastOutTimeSensor2;
  //Output On continuously when Time needed by sensor2 to detect something is beyond Time Gap Inputed by User
  if (timeDifferenceSensor2 > storedAcceptTimeSeconds*1000UL){
     if(outStateSensor2==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(OUTPUT_PIN, HIGH);
       outStateSensor2=false;
     }

  }
  return;
}


//Output On For Seconds When Reset Button Pressed
void outputOnReset(){
  static unsigned long outTimeOnReset = 0;
  static bool outActiveOnReset = false; 
  if (waitingForReset == true ){
    Serial.println("Resetttttttttttttt");
    outTimeOnReset = millis();
    waitingForReset = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnReset = true;
  }
  //Output on for mili seconds
  if (outActiveOnReset && (millis() - outTimeOnReset >=100UL)) {
    digitalWrite(OUTPUT_PIN, LOW);
    outActiveOnReset = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;

}



/*void proxi_counteron_Config(){
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




 return;

}*/




