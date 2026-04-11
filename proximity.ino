void ARDUINO_ISR_ATTR Proximity1_ISR() {//Intterupt Service Routine for Input1 (Sensor1)
  sensor1Count++;
  sensor1TimeMqttPub = millis();
  sensor1TimeTriggered = millis();
  //time1 = millis();
  //Serial.print("timw1:");
  //Serial.println(time1);
  objectDetectedSensor1 = true;
  //outputStartTime = micros(); 
  //digitalWrite(10, HIGH);
 // outputIsActive = true;
  sensor1MqttPub = true;
  sensor1TriggeredAlert = true;
  sensor1Shift = true;
  /*void ARDUINO_ISR_ATTR Proximity1_ISR() {
  if (!waitingForObject) {
    sensorTriggerTime = millis(); // Start the 2-second countdown
    waitingForObject = true;
  }
  detected_proxi1 = true;
}
*/

 
}

void ARDUINO_ISR_ATTR Proximity2_ISR() {//Interrupt Service Routine of Input2 sensor2
  sensor2Count++;
  sensor2TimeMqttPub= millis();
  sensor2TimeTriggered = millis();
  //time2=millis();
  //Serial.print("timw2:");
  //Serial.println(time2);
  objectDetectedSensor2 = true;
  //outputStartTime = micros(); 
 // digitalWrite(10, HIGH);
 // outputIsActive = true;
  sensor2MqttPub = true;
  sensor2TriggeredAlert = true;
  sensor2Shift = true;
  

  
}

void ARDUINO_ISR_ATTR RESET_ISR() {// Interupt Routine for Input2 as Reset Button
  sensor1Count=0;
  sensor2Count++;

  waitingForReset = true;
  //outputStartTimeNeg = micros(); 
  //digitalWrite(10, HIGH);
  //outputIsActiveNeg = true;
  
  sensor2Shift = true;
  //IfNeedAcceptableTime== false;// not calculate the time difference as it is counter and Reset Mode
}




void configInputOutput(){//Configure and Load all data stored in Esp32 Memory
  
  preferences.begin("sensor", true);
  storedSensor1Name = preferences.getString("InputSensor_1", "");//	getChar(const char* key, const int8_t defaultValue)
  storedSensor2Name = preferences.getString("InputSensor_2", "");
  bool storedInput1Mode=preferences.getBool("Sensor_1mode", false);
  bool storedInput2Mode=preferences.getBool("Sensor_2mode", false);
  storedSensor1OnChoice=preferences.getBool("Sensor1onoff", false);
  storedSensor2OnChoice=preferences.getInt("Sensor2onoff", 0);
  bool storedSensor1NoNcChoice=preferences.getBool("Sensor1nonc", false);
  bool storedSensor2NoNcChoice=preferences.getBool("Sensor2nonc", false);
  storedSensorShiftChoise=preferences.getBool("Sensorshift", false);
  storedIndividualAcceptTime = preferences.getInt("individual", 0);
  storedUserIndividualAcceptTime=preferences.getInt("accepttime", 0);
  storedSensorTriggerOut=preferences.getInt("outputtrig", 0);
  storedMqttPubSensorTimeDiff=preferences.getInt("outputpublish", 0);
  storedSensorDiffOut=preferences.getInt("outputalert", 0);
  storedInputSensorTimeDiff = preferences.getInt("proxi_time",0);//getInt(const char* key, const int32_t defaultValue)
  String storedOutputChoice=preferences.getString("Outputonoff", "");
  storedsensor1OnTime=preferences.getBool("sensor1OnTime",0);
        
  preferences.end();



  if (storedSensor1OnChoice == true){//if Sensor1 ON
    if(storedSensor1NoNcChoice == true){//IF NO choosen for sensor
      if (storedInput1Mode == true){
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == false){
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING );//for high to low detected
      }

    }
    
  }


  if (storedSensor2OnChoice == 1){//If Sensor2 ON
    if(storedSensor2NoNcChoice == true){//if NO
      if (storedInput2Mode == true){//true means Falling Mode
      
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == false){//Rising mode
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING );//for high to low detected
      }

    }
    
  }


  if (storedSensor2OnChoice == 3){// if sensor2 is in reset mode using button
    if(storedSensor2NoNcChoice == true){//if NO
      if (storedInput2Mode == true){
      
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == false){
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING );//for high to low detected
      }

    }
    
  }

  if (storedOutputChoice=="ON"){
    pinMode(OUTPUT_PIN, OUTPUT);
    Serial.println("output ON mmmmmmmmm");
  }


  return;

}

/*
void setupOutputOn(){//Output ON or OFF in Setup
  if (storedOutputChoice=="ON"){
    pinMode(OUTPUT_PIN, OUTPUT);
    Serial.println("output ON mmmmmmmmm");
  }
  return;
}*/


void sensorShiftRight(){//Function Runs when Sensor Shift Right is Choosen by User
  if (sensor1Shift == false && sensor2Shift == true) {//if 
    Serial.print("metal detected");
    sensor1Shift = false;
    sensor2Shift = false;
  }
    
    
  if (sensor1Shift  && sensor2Shift == true) {//if sensor 1 detects than and only than sensor2 count increment 
    Serial.print("metal detected");
    //lastdetectedtime = currenttime;
    sensorCount++;
    Serial.print(sensorCount);
    Serial.println(" times.");
    /*if (Sensor1TimeWhenTriggered > Sensor2TimeWhenTriggered){// to avoid time overflow error
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

void sensorShiftLeft(){//Function Runs when Sensor Shift Left is Choosen by User
  if (sensor1Shift == true && sensor2Shift == false) {//if 
    Serial.print("metal detected");
    sensor1Shift = false;
    sensor2Shift = false;
  }
    

  if (sensor1Shift  && sensor2Shift == true) {//if sensor 2 detects than and only than sensor1 count increment 
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
void AcceptableTime(){// Integrated with RightShift, leftShift Function for MqttPublish Alert on Time Difference Gap (Same Conveyor) (MqttPublish data)

  if (timeDiffInSensors > storedInputSensorTimeDiff*1000UL){
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


void acceptableTimeAlert(){//Time Difference between sensors Alert (difference conveyor)  (MqttPublish data)
  if (sensor1MqttPub==true && sensor2MqttPub == true){
    Serial.println("hhhhhhhhhhhhdddddddddd");
    sensor1MqttPub=false;
    sensor2MqttPub =false;
    if (sensor1TimeMqttPub >  sensor2TimeMqttPub){// to avoid time overflow error
      timeDiffInSensors =  sensor1TimeMqttPub-  sensor2TimeMqttPub; 
    }
    else{
      timeDiffInSensors =  sensor2TimeMqttPub - sensor1TimeMqttPub;
    }
  }
  if (timeDiffInSensors> storedInputSensorTimeDiff*1000UL){
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


/*void proxi_counteron_Config(){


  //preferences.begin("sensor", true);
  //String storedInput1 = preferences.getString("InputSensor_1", "");
  //String storedInput2 = preferences.getString("InputSensor_2", "");
  //uint8_t storedsensor1offon=preferences.getInt("Sensor1onoff", 0);
  //uint8_t storedsensor2offon=preferences.getInt("Sensor2onoff", 0);
  //preferences.end();

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





void timeDiffAlertBetweenSensors(){//Output On for Few seconds when Time Diffrence between 2 sensors is beyond Time Gap Inputed by User 
  
  if (sensor1TriggeredAlert==true && sensor2TriggeredAlert == true){
    Serial.println("hhhhhhhhhhhhdddddddddd");
    outTimeOnSensorDiff = millis();
    sensor1TriggeredAlert = false;
    sensor2TriggeredAlert = false;
      
    if (sensor1TimeTriggered > sensor2TimeTriggered){// to avoid time overflow error
      timeDifferenceAlert = sensor1TimeTriggered - sensor2TimeTriggered; 
    }
    else{
      timeDifferenceAlert = sensor2TimeTriggered - sensor1TimeTriggered;
    }
  }
  if (timeDifferenceAlert > storedInputSensorTimeDiff*1000UL){// IN MICROS
    //outTimeOnSensorDiff = millis(); 
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnSensorDiff = true;
    
    //snprintf(msg3, 50, "Time difference not Acceptable: %d", sensorCount);
    Serial.println("555555555555555555555f5554545sss5");
    Serial.println(millis() - outTimeOnSensorDiff);
    if (outActiveOnSensorDiff && (millis() - outTimeOnSensorDiff >= 1000UL)) {//nbuzzer 500 mili seconds
      Serial.println("lowwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
      digitalWrite(OUTPUT_PIN, LOW);
      outActiveOnSensorDiff = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
      timeDifferenceAlert=0;
      sensor1TimeTriggered=0;
      sensor2TimeTriggered=0;
    }
          
  }
  // sensor1TriggeredAlert = false;
  // sensor2TriggeredAlert = false;
  //timeDifferenceAlert = 0;
  return;
}


void sensorTriggerOutputOn(){//Output On for some seconds when Sensor detects something Respectively
  //outputStartTime = millis();
  //static unsigned long outTimeOnSensorTrig = 0;//static remembers the value in fub=nction call
  if (objectDetectedSensor1 == true || objectDetectedSensor2 == true){
    Serial.println("objectdetected");
    outTimeOnSensorTrig = micros();
    objectDetectedSensor1 = false;
    objectDetectedSensor2 = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnTrigger = true;
    //outputstatesensor1=true;

  }
  if (outActiveOnTrigger && (micros() - outTimeOnSensorTrig >= 100000)) {//buzzer on for 100 mili seconds
    digitalWrite(OUTPUT_PIN, LOW);
    outActiveOnTrigger = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;
} 





void sensor1AcceptableTime(){//Output On continuously when Time needed by sensor1 to detect something is beyond Time Gap Inputed by User

  outTimeSensor1 = millis();

  if (objectDetectedSensor1 == true){
    Serial.println("objectdetected");
    lastOutTimeSensor1 = outTimeSensor1;
    objectDetectedSensor1 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    outStateSensor1=true;

  }

  timeDifferenceSensor1 = outTimeSensor1 -  lastOutTimeSensor1;
  if (timeDifferenceSensor1>storedUserIndividualAcceptTime*1000UL){
     if(outStateSensor1==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(OUTPUT_PIN, HIGH);
       outStateSensor1=false;
     }

  }
  return;
}


void sensor2AcceptableTime(){//Output On continuously when Time needed by sensor2 to detect something is beyond Time Gap Inputed by User

  outTimeSensor2 = millis();

  if (objectDetectedSensor2 == true){
    Serial.println("objectdetected");
    lastOutTimeSensor2 = outTimeSensor2;
    objectDetectedSensor2 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    outStateSensor2=true;

  }

  long timeDifferenceSensor2 = outTimeSensor2 -  lastOutTimeSensor2;
  if (timeDifferenceSensor2 > storedUserIndividualAcceptTime*1000UL){
     if(outStateSensor2==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(OUTPUT_PIN, HIGH);
       outStateSensor2=false;
     }

  }
  return;
}

void outputOnReset(){//Output On for some seconds when Reset Button Pressed
  //static unsigned long outTimeOnReset = 0;//static ensures the value is remembered 
  
  if (waitingForReset == true ){
    Serial.println("Resetttttttttttttt");
    //Serial.println("objectdetected");
    outTimeOnReset = millis();
    waitingForReset = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnReset = true;
    //outputstatesensor1=true;

  }
  if (outActiveOnReset && (millis() - outTimeOnReset >=100UL)) {//Output on for 100 mili seconds
    digitalWrite(OUTPUT_PIN, LOW);
    outActiveOnReset = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;

}







