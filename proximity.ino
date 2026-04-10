void ARDUINO_ISR_ATTR Proximity1_ISR() {//Intterupt Service Routine for Input1 (Sensor1)
  sensor1ProxiCount++;
  sensor1TimeWhenTriggeredMqttPublish = millis();
  sensor1TimeWhenTriggeredAlert = millis();
  //time1 = millis();
  //Serial.print("timw1:");
  //Serial.println(time1);
  waitingForObjectDetectedSensor1 = true;
  //outputStartTime = micros(); 
  //digitalWrite(10, HIGH);
 // outputIsActive = true;
  sensor1TriggeredMqttPublishAlert = true;
  sensor1TriggeredAlert = true;
  sensor1Triggered = true;
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
  sensor2ProxiCount++;
  sensor2TimeWhenTriggeredMqttPublish= millis();
  sensor2TimeWhenTriggeredAlert = millis();
  //time2=millis();
  //Serial.print("timw2:");
  //Serial.println(time2);
  waitingForObjectDetectedSensor2 = true;
  //outputStartTime = micros(); 
 // digitalWrite(10, HIGH);
 // outputIsActive = true;
  sensor2TriggeredMqttPublishAlert = true;
  sensor2TriggeredAlert = true;
  sensor2Triggered = true;
  

  
}

void ARDUINO_ISR_ATTR RESET_ISR() {// Interupt Routine for Input2 as Reset Button
  sensor1ProxiCount=0;
  sensor2ProxiCount++;

  waitingForReset = true;
  //outputStartTimeNeg = micros(); 
  //digitalWrite(10, HIGH);
  //outputIsActiveNeg = true;
  
  sensor2Triggered = true;
  //IfNeedAcceptableTime== false;// not calculate the time difference as it is counter and Reset Mode
}




void proximityConfig(){//Configure and Load all data stored in Esp32 Memory
  
  preferences.begin("sensor", true);
  storedSensor1Name = preferences.getString("InputSensor_1", "");//	getChar(const char* key, const int8_t defaultValue)
  storedSensor2Name = preferences.getString("InputSensor_2", "");
  String storedInput1Mode=preferences.getString("Sensor_1mode", "");
  String storedInput2Mode=preferences.getString("Sensor_2mode", "");
  storedSensor1OnChoice=preferences.getInt("Sensor1onoff", 0);
  storedSensor2OnChoice=preferences.getInt("Sensor2onoff", 0);
  uint8_t storedSensor1NoNcChoice=preferences.getInt("Sensor1nonc", 0);
  uint8_t storedSensor2NoNcChoice=preferences.getInt("Sensor2nonc", 0);
  storedSensorShiftChoise=preferences.getInt("Sensorshift", 0);
  storedSensorIndividualAcceptTimeSelectAndOutputOn = preferences.getInt("individual", 0);
  storedSensorIndividualAcceptTimeUserInputInSeconds=preferences.getInt("accepttime", 0);
  storedSensorIndividualTriggerOutputon=preferences.getInt("outputtrig", 0);
  storedSensorDifferenceAlertMqttPublish=preferences.getInt("outputpublish", 0);
  storedSensorDifferenceOutputAlerton=preferences.getInt("outputalert", 0);
  storedUserInputSensorsTimeDifferenceInSeconds = preferences.getInt("proxi_time",0);//getInt(const char* key, const int32_t defaultValue)
  storedOutputONSetupChoise=preferences.getString("Outputonoff", "");
        
  preferences.end();



  if (storedSensor1OnChoice == 1){
    if(storedSensor1NoNcChoice == 1){//IF NO choosen for sensor
      if (storedInput1Mode == "FALLING"){
        //Serial.println("5555555555555555555555555555555555555555");
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == "RISING"){
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING );//for high to low detected
      }

    }
    
  }


  if (storedSensor2OnChoice == 1){
    if(storedSensor2NoNcChoice == 1){//if NO
      if (storedInput2Mode == "FALLING"){
      
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == "RISING"){
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING );//for high to low detected
      }

    }
    
  }


  if (storedSensor2OnChoice == 3){// if sensor2 is in reset mode using button
    if(storedSensor2NoNcChoice == 1){//if NO
      if (storedInput2Mode == "FALLING"){
      
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == "RISING"){
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING );//for high to low detected
      }

    }
    
  }

  return;

}




void sensorShiftRight(){//Function Runs when Sensor Shift Right is Choosen by User
  if (sensor1Triggered == false && sensor2Triggered == true) {//if 
    Serial.print("metal detected");
    sensor1Triggered = false;
    sensor2Triggered = false;
  }
    
    
  if (sensor1Triggered  && sensor2Triggered == true) {//if sensor 1 detects than and only than sensor2 count increment 
    Serial.print("metal detected");
    //lastdetectedtime = currenttime;
    proxiCounterAfterBothSensorDetects++;
    Serial.print(proxiCounterAfterBothSensorDetects);
    Serial.println(" times.");
    /*if (Sensor1TimeWhenTriggered > Sensor2TimeWhenTriggered){// to avoid time overflow error
      timeDifferenceBetweenSensors = Sensor1TimeWhenTriggered - Sensor2TimeWhenTriggered;
    }
    else{
      timeDifferenceBetweenSensors = Sensor2TimeWhenTriggered - Sensor1TimeWhenTriggered;
    }

    if (storedSensorDifferenceAlertMqttPublish==1){
      AcceptableTime();
    }
    Sensor1TimeWhenTriggered = 0;
    Sensor2TimeWhenTriggered = 0;*/
    sensor1Triggered = false;
    sensor2Triggered = false;
  }
  return;
}

void sensorShiftLeft(){//Function Runs when Sensor Shift Left is Choosen by User
  if (sensor1Triggered == true && sensor2Triggered == false) {//if 
    Serial.print("metal detected");
    sensor1Triggered = false;
    sensor2Triggered = false;
  }
    

  if (sensor1Triggered  && sensor2Triggered == true) {//if sensor 2 detects than and only than sensor1 count increment 
    Serial.print("metal detected");
    proxiCounterAfterBothSensorDetects++;
    Serial.print(proxiCounterAfterBothSensorDetects);
    Serial.println(" times.");
    /*if (Sensor1TimeWhenTriggered > Sensor2TimeWhenTriggered){
      timeDifferenceBetweenSensors = Sensor1TimeWhenTriggered - Sensor2TimeWhenTriggered;
    }
    else{
      timeDifferenceBetweenSensors= Sensor2TimeWhenTriggered- Sensor1TimeWhenTriggered;
    }
      
    if (storedSensorDifferenceAlertMqttPublish==1){
      AcceptableTime();
    }
    Sensor1TimeWhenTriggered = 0;
    Sensor2TimeWhenTriggered=0;*/
    sensor1Triggered = false;
    sensor2Triggered = false;
  }
  return;
}

/*
void AcceptableTime(){// Integrated with RightShift, leftShift Function for MqttPublish Alert on Time Difference Gap (Same Conveyor) (MqttPublish data)

  if (timeDifferenceBetweenSensors > storedUserInputSensorsTimeDifferenceInSeconds*1000UL){
    char msg3[50];
    snprintf(msg3, 50, "Time difference not Acceptable: %d", proxiCounterAfterBothSensorDetects);
    mqttClient.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);

    timeDifferenceBetweenSensors = 0;  
  }
  else{

    char msg3[50];
    snprintf(msg3, 50, "Time difference Acceptable: %d", proxiCounterAfterBothSensorDetects);
    mqttClient.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);
    timeDifferenceBetweenSensors = 0;
  }
  //TimeDifference=0;
  return;
}*/


void acceptableTimeAlert(){//Time Difference between sensors Alert (difference conveyor)  (MqttPublish data)
  if (sensor1TriggeredMqttPublishAlert==true && sensor2TriggeredMqttPublishAlert == true){
    Serial.println("hhhhhhhhhhhhdddddddddd");
    sensor1TriggeredMqttPublishAlert=false;
    sensor2TriggeredMqttPublishAlert =false;
    if (sensor1TimeWhenTriggeredMqttPublish >  sensor2TimeWhenTriggeredMqttPublish){// to avoid time overflow error
      timeDifferenceBetweenSensors =  sensor1TimeWhenTriggeredMqttPublish-  sensor2TimeWhenTriggeredMqttPublish; 
    }
    else{
      timeDifferenceBetweenSensors =  sensor2TimeWhenTriggeredMqttPublish - sensor1TimeWhenTriggeredMqttPublish;
    }
  }
  if (timeDifferenceBetweenSensors> storedUserInputSensorsTimeDifferenceInSeconds*1000UL){
    char msg3[50];
    snprintf(msg3, 50, "Time difference not Acceptable:");
    mqttClient.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);

      timeDifferenceBetweenSensors=0;
      sensor1TimeWhenTriggeredMqttPublish=0;
      sensor2TimeWhenTriggeredMqttPublish=0;
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



void setupOutputOn(){//Output ON or OFF in Setup
  if (storedOutputONSetupChoise=="ON"){
    pinMode(OUTPUT_PIN, OUTPUT);
    Serial.println("output ON mmmmmmmmm");
  }
  return;
}


void timeDiffAlertBetweenSensors(){//Output On for Few seconds when Time Diffrence between 2 sensors is beyond Time Gap Inputed by User 
  
  if (sensor1TriggeredAlert==true && sensor2TriggeredAlert == true){
    Serial.println("hhhhhhhhhhhhdddddddddd");
    outputStartTimeSensorDifferenceAlert = millis();
    sensor1TriggeredAlert = false;
    sensor2TriggeredAlert = false;
      
    if (sensor1TimeWhenTriggeredAlert > sensor2TimeWhenTriggeredAlert){// to avoid time overflow error
      timeDifferenceAlert = sensor1TimeWhenTriggeredAlert - sensor2TimeWhenTriggeredAlert; 
    }
    else{
      timeDifferenceAlert = sensor2TimeWhenTriggeredAlert - sensor1TimeWhenTriggeredAlert;
    }
  }
  if (timeDifferenceAlert > storedUserInputSensorsTimeDifferenceInSeconds*1000UL){// IN MICROS
    //outputStartTimeSensorDifferenceAlert = millis(); 
    digitalWrite(OUTPUT_PIN, HIGH);
    outputIsActiveForSensorDifferenceAlert = true;
    
    //snprintf(msg3, 50, "Time difference not Acceptable: %d", proxiCounterAfterBothSensorDetects);
    Serial.println("555555555555555555555f5554545sss5");
    Serial.println(millis() - outputStartTimeSensorDifferenceAlert);
    if (outputIsActiveForSensorDifferenceAlert && (millis() - outputStartTimeSensorDifferenceAlert >= 1000UL)) {//nbuzzer 500 mili seconds
      Serial.println("lowwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
      digitalWrite(OUTPUT_PIN, LOW);
      outputIsActiveForSensorDifferenceAlert = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
      timeDifferenceAlert=0;
      sensor1TimeWhenTriggeredAlert=0;
      sensor2TimeWhenTriggeredAlert=0;
    }
          
  }
  // sensor1TriggeredAlert = false;
  // sensor2TriggeredAlert = false;
  //timeDifferenceAlert = 0;
  return;
}


void sensorTriggerOutputOn(){//Output On for some seconds when Sensor detects something Respectively
  //outputStartTime = millis();
  //static unsigned long outputStartTimeOnSensorTriggering = 0;//static remembers the value in fub=nction call
  if (waitingForObjectDetectedSensor1 == true || waitingForObjectDetectedSensor2 == true){
    Serial.println("objectdetected");
    outputStartTimeOnSensorTriggering = micros();
    waitingForObjectDetectedSensor1 = false;
    waitingForObjectDetectedSensor2 = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outputIsActiveForTriggering = true;
    //outputstatesensor1=true;

  }
  if (outputIsActiveForTriggering && (micros() - outputStartTimeOnSensorTriggering >= 100000)) {//buzzer on for 100 mili seconds
    digitalWrite(OUTPUT_PIN, LOW);
    outputIsActiveForTriggering = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;
} 





void sensor1AcceptableTime(){//Output On continuously when Time needed by sensor1 to detect something is beyond Time Gap Inputed by User

  currentTimeForOutputSensor1 = millis();

  if (waitingForObjectDetectedSensor1 == true){
    Serial.println("objectdetected");
    lastDetectedOutoutTimeSensor1 = currentTimeForOutputSensor1;
    waitingForObjectDetectedSensor1 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    outputStateForSensor1=true;

  }

  timeDifferenceSensor1 = currentTimeForOutputSensor1 -  lastDetectedOutoutTimeSensor1;
  if (timeDifferenceSensor1>storedSensorIndividualAcceptTimeUserInputInSeconds*1000UL){
     if(outputStateForSensor1==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(OUTPUT_PIN, HIGH);
       outputStateForSensor1=false;
     }

  }
  return;
}


void sensor2AcceptableTime(){//Output On continuously when Time needed by sensor2 to detect something is beyond Time Gap Inputed by User

  currentTimeForOutputSensor2 = millis();

  if (waitingForObjectDetectedSensor2 == true){
    Serial.println("objectdetected");
    lastDetectedOutoutTimeSensor2 = currentTimeForOutputSensor2;
    waitingForObjectDetectedSensor2 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    outputStateForSensor2=true;

  }

  long timeDifferenceSensor2 = currentTimeForOutputSensor2 -  lastDetectedOutoutTimeSensor2;
  if (timeDifferenceSensor2 > storedSensorIndividualAcceptTimeUserInputInSeconds*1000UL){
     if(outputStateForSensor2==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(OUTPUT_PIN, HIGH);
       outputStateForSensor2=false;
     }

  }
  return;
}

void outputOnReset(){//Output On for some seconds when Reset Button Pressed
  //static unsigned long outputStartTimeOnReset = 0;//static ensures the value is remembered 
  
  if (waitingForReset == true ){
    Serial.println("Resetttttttttttttt");
    //Serial.println("objectdetected");
    outputStartTimeOnReset = millis();
    waitingForReset = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outputIsActiveForReset = true;
    //outputstatesensor1=true;

  }
  if (outputIsActiveForReset && (millis() - outputStartTimeOnReset >=100UL)) {//Output on for 100 mili seconds
    digitalWrite(OUTPUT_PIN, LOW);
    outputIsActiveForReset = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;

}







