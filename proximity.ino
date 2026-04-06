void ARDUINO_ISR_ATTR Proximity1_ISR() {
  Sensor1ProxiCount++;
  Sensor1TimeWhenTriggered = micros();
  //Serial.print("timw1:");
  //Serial.println(time1);
  WaitingForObjectDetectedSensor1 = true;
  //outputStartTime = micros(); 
  //digitalWrite(10, HIGH);
 // outputIsActive = true;
  Senson1Triggered = true;
  /*void ARDUINO_ISR_ATTR Proximity1_ISR() {
  if (!waitingForObject) {
    sensorTriggerTime = millis(); // Start the 2-second countdown
    waitingForObject = true;
  }
  detected_proxi1 = true;
}
*/

 
}

void ARDUINO_ISR_ATTR Proximity2_ISR() {
  Sensor2ProxiCount++;
  Sensor2TimeWhenTriggered= micros();
  //Serial.print("timw2:");
  //Serial.println(time2);
  WaitingForObjectDetectedSensor2 = true;
  //outputStartTime = micros(); 
 // digitalWrite(10, HIGH);
 // outputIsActive = true;
  Sensor2Triggered = true;

  
}

void ARDUINO_ISR_ATTR Proximity3_ISR() {// for counter and reset mode
  Sensor1ProxiCount=0;
  Sensor2ProxiCount++;

  WaitingForReset = true;
  //outputStartTimeNeg = micros(); 
  //digitalWrite(10, HIGH);
  //outputIsActiveNeg = true;
 
  Sensor2Triggered = true;
  //IfNeedAcceptableTime== false;// not calculate the time difference as it is counter and Reset Mode
}




void ProximityConfig(){
  
  preferences.begin("sensor", true);
  StoredSensor1Name = preferences.getString("InputSensor_1", "");//	getChar(const char* key, const int8_t defaultValue)
  StoredInput2Name = preferences.getString("InputSensor_2", "");
  String StoredInput1Mode=preferences.getString("Sensor_1mode", "");
  String StoredInput2Mode=preferences.getString("Sensor_2mode", "");
  StoredSensor1Offon=preferences.getInt("Sensor1onoff", 0);
  StoredSensor2Offon=preferences.getInt("Sensor2onoff", 0);
  uint8_t storedSensor1nonc=preferences.getInt("Sensor1nonc", 0);
  uint8_t storedSensor2nonc=preferences.getInt("Sensor2nonc", 0);
  StoredSensorShiftChoise=preferences.getInt("Sensorshift", 0);
  StoredSensorIndividualAcceptTimeSelectAndOutputOn = preferences.getInt("individual", 0);
  StoredSensorIndividualAcceptTimeUserInputInSeconds=preferences.getInt("accepttime", 0);
  StoredSensorIndividualTriggerOutputon=preferences.getInt("outputtrig", 0);
  StoredSensorDiffOutputAlerton=preferences.getInt("outputalert", 0);
  StoredUserInputSensorsTimeDifferenceInSeconds = preferences.getInt("proxi_time",0);//getInt(const char* key, const int32_t defaultValue)
  StoredOutputONSetupChoise=preferences.getString("Outputonoff", "");
        
  preferences.end();

  /*Serial.println(storedInput1Mode);
  Serial.println(storedInput2Mode);
  Serial.println(storedsensor1offon);
  Serial.println(storedsensor2offon);
  Serial.println(storedSensor1nonc);
  Serial.println(storedSensor2nonc);
  Serial.println(storedSensorShiftchoise);*/


  if (StoredSensor1Offon == 1){
    if(storedSensor1nonc == 1){//IF NO choosen for sensor
      if (StoredInput1Mode == "FALLING"){
        //Serial.println("5555555555555555555555555555555555555555");
        attachInterrupt(6, Proximity1_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(6, Proximity1_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (StoredInput1Mode == "RISING"){
        attachInterrupt(6, Proximity1_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(6, Proximity1_ISR, FALLING );//for high to low detected
      }

    }
    
  }


  if (StoredSensor2Offon == 1){
    if(storedSensor2nonc == 1){//if NO
      if (StoredInput2Mode == "FALLING"){
      
        attachInterrupt(7, Proximity2_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(7, Proximity2_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (StoredInput1Mode == "RISING"){
        attachInterrupt(7, Proximity2_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(7, Proximity2_ISR, FALLING );//for high to low detected
      }

    }
    
  }


  if (StoredSensor2Offon == 3){// if sensor2 is in reset mode using button
    if(storedSensor2nonc == 1){//if NO
      if (StoredInput2Mode == "FALLING"){
      
        attachInterrupt(7, Proximity3_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(7, Proximity3_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (StoredInput1Mode == "RISING"){
        attachInterrupt(7, Proximity3_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(7, Proximity3_ISR, FALLING );//for high to low detected
      }

    }
    
  }

  return;

}




void SensorShiftRight(){
  
    
    if (Senson1Triggered == false && Sensor2Triggered == true) {//if 
      Serial.print("metal detected");
     // proxi_counter++;
      //Serial.print(proxi_counter);
      //Serial.println(" times.");
      Senson1Triggered = false;
      Sensor2Triggered = false;
    }
    
    
    if (Senson1Triggered  && Sensor2Triggered == true) {//if sensor 1 detxtes than and only than sensor2 count increment 
      Serial.print("metal detected");
      //lastdetectedtime = currenttime;
      ProxiCounterAfterBothSensorDetects++;
      Serial.print(ProxiCounterAfterBothSensorDetects);
      Serial.println(" times.");
      if (Sensor1TimeWhenTriggered > Sensor2TimeWhenTriggered){// to avoid time overflow error
        TimeDifferenceBetweenSensors = Sensor1TimeWhenTriggered - Sensor2TimeWhenTriggered;
        
      }
      else{
        TimeDifferenceBetweenSensors = Sensor2TimeWhenTriggered - Sensor1TimeWhenTriggered;
      }
      
      
     // if (IfNeedAcceptableTime==true){
      AcceptableTime();
     // }
      Sensor1TimeWhenTriggered = 0;
      Sensor2TimeWhenTriggered = 0;
      Senson1Triggered = false;
      Sensor2Triggered = false;
    }

    /*char msg1[50];
    snprintf(msg1, 50, "Proxi Counter: %d", proxi_counter);
    mqtt_client.publish("kinjal/esp32/counter", msg1);
    Serial.println(msg1);*/


   return;
}



void SensorShiftLeft(){
   if (Senson1Triggered == true && Sensor2Triggered == false) {//if 
      Serial.print("metal detected");
     // proxi_counter++;
      //Serial.print(proxi_counter);
      //Serial.println(" times.");
       Senson1Triggered = false;
       Sensor2Triggered = false;
    }
    

    if (Senson1Triggered  && Sensor2Triggered == true) {//if sensor 2 detxtes than and only than sensor1 count increment 
      Serial.print("metal detected");
      ProxiCounterAfterBothSensorDetects++;
      Serial.print(ProxiCounterAfterBothSensorDetects);
      Serial.println(" times.");
      if (Sensor1TimeWhenTriggered > Sensor2TimeWhenTriggered){
        TimeDifferenceBetweenSensors = Sensor1TimeWhenTriggered - Sensor2TimeWhenTriggered;
        
      }
      else{
        TimeDifferenceBetweenSensors= Sensor2TimeWhenTriggered- Sensor1TimeWhenTriggered;
      }
      
     // Serial.println("TimeDifference:");
      //Serial.println(TimeDifference);
     AcceptableTime();
      Sensor1TimeWhenTriggered = 0;
      Sensor2TimeWhenTriggered=0;
      Senson1Triggered = false;
      Sensor2Triggered = false;
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



void Outputonoff(){


  if (StoredOutputONSetupChoise=="ON"){
    pinMode(OUTPUT_PIN, OUTPUT);
    Serial.println("output ON mmmmmmmmm");
  }
  return;
}




void SensorTriggerOutputon(){// output on for some time when sensor detects something  
  //outputStartTime = millis();

  if (WaitingForObjectDetectedSensor1 == true || WaitingForObjectDetectedSensor2 == true){
    Serial.println("objectdetected");
    OutputStartTimeOnSensorTriggering = micros();
    WaitingForObjectDetectedSensor1 = false;
    WaitingForObjectDetectedSensor2 = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    OutputIsActiveForTriggering = true;
    //outputstatesensor1=true;

  }
  if (OutputIsActiveForTriggering && (micros() - OutputStartTimeOnSensorTriggering >= 100000)) {//nbuzzer on for 100 mili seconds
    digitalWrite(OUTPUT_PIN, LOW);
    OutputIsActiveForTriggering = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;
} 

void AcceptableTime(){
  
  
  

  if (TimeDifferenceBetweenSensors > StoredUserInputSensorsTimeDifferenceInSeconds*1000000UL){// IN MICROS
    char msg3[50];
    snprintf(msg3, 50, "Time difference not Acceptable: %d", ProxiCounterAfterBothSensorDetects);
    MqttClient.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);

    TimeDifferenceBetweenSensors = 0;  
    if (StoredSensorDiffOutputAlerton == 1){
       Serial.println("hhhhhhhhhhhhdddddddddd");
       TimeDiffAlertBetweenSensors();
    }
  

    
  }
  else{

    char msg3[50];
    snprintf(msg3, 50, "Time difference Acceptable: %d", ProxiCounterAfterBothSensorDetects);
    MqttClient.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);
    TimeDifferenceBetweenSensors = 0;
    

  

  }
  //TimeDifference=0;
  return;
}


void TimeDiffAlertBetweenSensors(){
   OutputStartTimeSensorDifferenceAlert = micros(); 
   digitalWrite(OUTPUT_PIN, HIGH);
   OutputIsActiveForSensorDifferenceAlert = true;
   
   Serial.println("555555555555555555555f5554545sss5");
   if (OutputIsActiveForSensorDifferenceAlert && (micros() - OutputStartTimeSensorDifferenceAlert >= 500000)) {//nbuzzer 500 mili seconds
      digitalWrite(OUTPUT_PIN, LOW);
      OutputIsActiveForSensorDifferenceAlert = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }
  return;

}

void Sensor1AcceptableTime(){

  CurrentTimeForOutputSensor1 = millis();

  if (WaitingForObjectDetectedSensor1 == true){
    Serial.println("objectdetected");
    LastDetectedOutoutTimeSensor1 = CurrentTimeForOutputSensor1;
    WaitingForObjectDetectedSensor1 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    OutputStateForSensor1=true;

  }

  TimeDifferenceSensor1 = CurrentTimeForOutputSensor1 -  LastDetectedOutoutTimeSensor1;
  if (TimeDifferenceSensor1>StoredSensorIndividualAcceptTimeUserInputInSeconds*1000UL){
     if(OutputStateForSensor1==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(OUTPUT_PIN, HIGH);
       OutputStateForSensor1=false;
     }

  }
  return;
}


void Sensor2AcceptableTime(){

  CurrentTimeForOutputSensor2 = millis();

  if (WaitingForObjectDetectedSensor2 == true){
    Serial.println("objectdetected");
    LastDetectedOutoutTimeSensor2 = CurrentTimeForOutputSensor2;
    WaitingForObjectDetectedSensor2 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    OutputStateForSensor2=true;

  }

  long TimeDifferenceSensor2 = CurrentTimeForOutputSensor2 -  LastDetectedOutoutTimeSensor2;
  if (TimeDifferenceSensor2 > StoredSensorIndividualAcceptTimeUserInputInSeconds*1000UL){
     if(OutputStateForSensor2==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(OUTPUT_PIN, HIGH);
       OutputStateForSensor2=false;
     }

  }
  return;
}

void OutputOnReset(){
  if (WaitingForReset == true ){
    //Serial.println("objectdetected");
    OutputStartTimeOnReset = micros();
    WaitingForReset = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    OutputIsActiveForReset = true;
    //outputstatesensor1=true;

  }
  if (OutputIsActiveForReset && (micros() - OutputStartTimeOnReset >= 100000)) {//nbuzzer on for 100 mili seconds
    digitalWrite(OUTPUT_PIN, LOW);
    OutputIsActiveForReset = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;

}

