void ARDUINO_ISR_ATTR Proximity1_ISR() {//Intterupt Service Routine for Input1 (Sensor1)
  sensor1Count++;
  sensor1TimeMqttPub = millis();
  sensor1TimeTriggered = millis();
  objectDetectedSensor1 = true;
  objectDetected=true;
  sensor1MqttPub = true;
  sensor1TriggeredAlert = true;
  sensor1Shift = true;
  arrivalTime = millis();
  objectPresent = true;
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

// Interrupt Service Routine (ISR)
void ARDUINO_ISR_ATTR Sensor1Time_ISR() {
  isStateChanged1 = true;
  currentStateSensor1=digitalRead(6);
  sensor1Count++;
  sensor1Shift = true;
  //Other variable as per Requirement
}

void ARDUINO_ISR_ATTR Sensor2Time_ISR() {
  isStateChanged2 = true;
  currentStateSensor2=digitalRead(7);
  sensor2Count++;
  sensor2Shift = true;
  //Other variable as per Requirement
}

//Configure and Load all data stored in Esp32 Memory
void configInputOutput(){
  preferences.begin("sensor", true);
  storedSensor1Name = preferences.getString("InputSensor_1", "");
  storedSensor2Name = preferences.getString("InputSensor_2", "");
  uint8_t storedInput1Mode=preferences.getInt("Sensor_1mode", 0);
  uint8_t storedInput2Mode=preferences.getInt("Sensor_2mode", 0);
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
  storedOnTimeChoice=preferences.getInt("sensorOnOffTime",0);      
  preferences.end();


  //If In Sensor1 mode
  if (storedSensor1OnChoice == true){
    if(storedSensor1NoNcChoice == true){
      if (storedInput1Mode == 1){
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING );
      }
      else if (storedInput1Mode == 2){
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING );
      }
      else{
        attachInterrupt(PROX_SENSOR1, Sensor1Time_ISR, CHANGE );
      }
    }
    else{
      if (storedInput1Mode == 2){
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING );
      }
      else if (storedInput1Mode == 1){
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING );
      }
      else{
        attachInterrupt(PROX_SENSOR1, Sensor1Time_ISR, CHANGE );
      }

    }
    
  }

  //If Input2 Is In Sensor2 mode
  if (storedSensor2OnChoice == 1){
    if(storedSensor2NoNcChoice == true){
      if (storedInput2Mode == 1){
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING );
      }
      else if (storedInput2Mode == 2){
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING );
      }
      else{
        attachInterrupt(PROX_SENSOR2, Sensor2Time_ISR, CHANGE );
      }
    }
    else{//If NC Choosen 
      if (storedInput2Mode == 2){
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING );
      }
      else if (storedInput2Mode == 1){
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING );
      }
      else{
        attachInterrupt(PROX_SENSOR2, Sensor2Time_ISR, CHANGE );
      }

    }
    
  }

  // If Input2 Is In Reset Mode (Button)
  if (storedSensor2OnChoice == 3){
    if(storedSensor2NoNcChoice == true){
      if (storedInput2Mode == 1){
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING );
      }
      else if (storedInput2Mode == 2){
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING );
      }
      else{
        attachInterrupt(PROX_SENSOR2, Sensor2Time_ISR, CHANGE );
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == 2){
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING );
      }
      else if (storedInput2Mode == 1){
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING );
      }
      else{
        attachInterrupt(PROX_SENSOR2, Sensor2Time_ISR, CHANGE );
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
    String alertMessage="Time difference not Acceptable";
    mqttClient.publish("kinjal/esp32/time", alertMessage.c_str());
    Serial.println(alertMessage);

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
  static unsigned long outTimeOn1SensorDiff = 0;
  if (sensor1TriggeredAlert==true && sensor2TriggeredAlert == true){
    Serial.println("hhhhhhhhhhhhdddddddddd");
    outTimeOn1SensorDiff = millis();
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
    Serial.println(millis() - outTimeOn1SensorDiff);
    if (outActiveOnSensorDiff && (millis() - outTimeOn1SensorDiff >= 1000UL)) {//Buzzer on 1000 mili seconds
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
  static unsigned long outTimeOn1SensorTrig = 0;
  if (objectDetectedSensor1 == true || objectDetectedSensor2 == true){
    Serial.println("objectdetected");
    outTimeOn1SensorTrig = millis();
    objectDetectedSensor1 = false;
    objectDetectedSensor2 = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnTrigger = true;
  }
  //Output On For 100 mili seconds
  if (outActiveOnTrigger && (millis() - outTimeOn1SensorTrig >= 100UL)) {
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
  static unsigned long outTimeOn1Reset = 0;
  static bool outActiveOnReset = false; 
  if (waitingForReset == true ){
    Serial.println("Resetttttttttttttt");
    outTimeOn1Reset = millis();
    waitingForReset = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnReset = true;
  }
  //Output on for mili seconds
  if (outActiveOnReset && (millis() - outTimeOn1Reset >=100UL)) {
    digitalWrite(OUTPUT_PIN, LOW);
    outActiveOnReset = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;

}

void Time(){
  
  
  //timeSensor1 = millis();
  if (objectDetected == true){
    static unsigned long timeSensor1=0;
    static unsigned long lastOutTime = 0; 
    static unsigned long timeDifference = 0; 
    objectDetected = false;
    timeDifference = millis() -  lastOutTime;
    lastOutTime = millis();
    Serial.println("timeDifference");
    Serial.println(timeDifference);
    timeDifference =0;
    Serial.println("timeDifference after reset");
    Serial.println(timeDifference);
  }
 
 
  return;
}

//Publish Mqtt Data OFF and ON Cycle Time of Sensor1
void onOffTimeSensor1(){
  static bool lastState1= true; 
  static unsigned long timeLow1=0; 
  static unsigned long timeHigh1=0; 
  static unsigned long timeOn1=0; 
  static unsigned long timeOff=0; 
  if(isStateChanged1==true){
    isStateChanged1=false;
    if (currentStateSensor1==false && lastState1==true){
      Serial.println("low to high");
      timeLow1=millis();
      //Serial.println(timeLow1);

      if (timeHigh1!=0){
        timeOff=timeLow1-timeHigh1;
        Serial.println(timeOff);
        String timeOfff;
        timeOfff += "OffTime1:" + String(timeOff);
        mqttClient.publish("kinjal/esp32/offtime1", timeOfff.c_str());
        Serial.println(timeOfff); 
        //Reset
        timeHigh1=0;
        timeOff=0;
        
      }
      lastState1=false;
    }
    else if (currentStateSensor1==true && lastState1==false ){//ON state
      Serial.println("highhhhhhh");
      timeHigh1= millis();
      timeOn1=timeHigh1-timeLow1;
      //Serial.println(timeOn1); 
      String timeOnn1;
      timeOnn1 += "OnTimeLow1:" + String(timeOn1);
      mqttClient.publish("kinjal/esp32/ontime1", timeOnn1.c_str());
      Serial.println(timeOnn1); 
      //Reset the time 
      timeLow1=0;
      timeOn1=0;
      lastState1=currentStateSensor1;
    }
  
  }
}

//Publish Mqtt Data Of OFF and ON Time of Sensor1
void onOffTimeSensor2(){
  static bool lastState2= true; 
  static unsigned long timeLow2=0; 
  static unsigned long timeHigh2=0; 
  static unsigned long timeOn2=0; 
  static unsigned long timeOff2=0; 
  if(isStateChanged2==true){
    isStateChanged2=false;
    if (currentStateSensor2==false && lastState2==true){
      Serial.println("low to high");
      timeLow2=millis();
      //Serial.println(timeLow2);

      if (timeHigh2!=0){
        timeOff2=timeLow2-timeHigh2;
        Serial.println(timeOff2);
        String timeOfff2;
        timeOfff2 += "OffTime:" + String(timeOff2);
        mqttClient.publish("kinjal/esp32/offtime2", timeOfff2.c_str());
        Serial.println(timeOfff2); 
        //Reset
        timeHigh2=0;
        timeOff2=0;
        
      }
      lastState2=false;
    }
    else if (currentStateSensor2==true && lastState2==false ){//ON state
      Serial.println("highhhhhhh");
      timeHigh2= millis();
      timeOn2=timeHigh2-timeLow2;
      //Serial.println(timeOn2); 
      String timeOnn2;
      timeOnn2 += "OnTime:" + String(timeOn2);
      mqttClient.publish("kinjal/esp32/ontime2", timeOnn2.c_str());
      Serial.println(timeOnn2); 
      //Reset the time 
      timeLow2=0;
      timeOn2=0;
      lastState2=currentStateSensor2;
    }
  }
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




