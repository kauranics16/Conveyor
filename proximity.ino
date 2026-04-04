void ARDUINO_ISR_ATTR Proximity1_ISR() {
  proxi_counter1++;
  time1= micros();
  //Serial.print("timw1:");
  //Serial.println(time1);
  WaitingForObjectDetectedSensor1 = true;
  //outputStartTime = micros(); 
  //digitalWrite(10, HIGH);
 // outputIsActive = true;
  detected_proxi1 = true;
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
  proxi_counter2++;
  time2= micros();
  //Serial.print("timw2:");
  //Serial.println(time2);
  WaitingForObjectDetectedSensor2 = true;
  //outputStartTime = micros(); 
 // digitalWrite(10, HIGH);
 // outputIsActive = true;
  detected_proxi2 = true;

  
}

void ARDUINO_ISR_ATTR Proximity3_ISR() {// for counter and reset mode
  proxi_counter1=0;
  proxi_counter2++;

  WaitingForReset = true;
  //outputStartTimeNeg = micros(); 
  //digitalWrite(10, HIGH);
  //outputIsActiveNeg = true;
 
  detected_proxi2 = true;
  //IfNeedAcceptableTime== false;// not calculate the time difference as it is counter and Reset Mode
}




void ProximityConfig(){
  
  preferences.begin("sensor", true);
  storedInput1 = preferences.getString("InputSensor_1", "");//	getChar(const char* key, const int8_t defaultValue)
  storedInput2 = preferences.getString("InputSensor_2", "");
  String storedInput1Mode=preferences.getString("Sensor_1mode", "");
  String storedInput2Mode=preferences.getString("Sensor_2mode", "");
  storedsensor1offon=preferences.getInt("Sensor1onoff", 0);
  storedsensor2offon=preferences.getInt("Sensor2onoff", 0);
  uint8_t storedSensor1nonc=preferences.getInt("Sensor1nonc", 0);
  uint8_t storedSensor2nonc=preferences.getInt("Sensor2nonc", 0);
  storedSensorShiftchoise=preferences.getInt("Sensorshift", 0);
  storedSensorIndividualAcceptTimeselect=preferences.getInt("individual", 0);
  storedSensorIndividualAcceptTimeinput=preferences.getInt("accepttime", 0);
  storedSensorIndividualTriggerOutputon=preferences.getInt("outputtrig", 0);
  storedSensorDiffOutputAlerton=preferences.getInt("outputalert", 0);
  storedSensor_timedifference = preferences.getInt("proxi_time",0);//getInt(const char* key, const int32_t defaultValue)
  storedOutputonoff=preferences.getString("Outputonoff", "");
        
  preferences.end();

  /*Serial.println(storedInput1Mode);
  Serial.println(storedInput2Mode);
  Serial.println(storedsensor1offon);
  Serial.println(storedsensor2offon);
  Serial.println(storedSensor1nonc);
  Serial.println(storedSensor2nonc);
  Serial.println(storedSensorShiftchoise);*/


  if (storedsensor1offon == 1){
    if(storedSensor1nonc == 1){//IF NO choosen for sensor
      if (storedInput1Mode == "FALLING"){
        //Serial.println("5555555555555555555555555555555555555555");
        attachInterrupt(6, Proximity1_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(6, Proximity1_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == "RISING"){
        attachInterrupt(6, Proximity1_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(6, Proximity1_ISR, FALLING );//for high to low detected
      }

    }
    
  }


  if (storedsensor2offon == 1){
    if(storedSensor2nonc == 1){//if NO
      if (storedInput2Mode == "FALLING"){
      
        attachInterrupt(7, Proximity2_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(7, Proximity2_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == "RISING"){
        attachInterrupt(7, Proximity2_ISR, RISING );//FOR METAL DETECTED //for low to high
      }
      else{
        attachInterrupt(7, Proximity2_ISR, FALLING );//for high to low detected
      }

    }
    
  }


  if (storedsensor2offon == 3){// if sensor2 is in reset mode using button
    if(storedSensor2nonc == 1){//if NO
      if (storedInput2Mode == "FALLING"){
      
        attachInterrupt(7, Proximity3_ISR, FALLING );//FOR METAL DETECTED low to high detected
      }
      else{
        attachInterrupt(7, Proximity3_ISR, RISING );//for high to low detected
      }
    }
    else{//IF NC choosen for sensor
      if (storedInput1Mode == "RISING"){
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
  
    
    if (detected_proxi1 == false && detected_proxi2 == true) {//if 
      Serial.print("metal detected");
     // proxi_counter++;
      //Serial.print(proxi_counter);
      //Serial.println(" times.");
      detected_proxi1 = false;
      detected_proxi2 = false;
    }
    
    
    if (detected_proxi1  && detected_proxi2 == true) {//if sensor 1 detxtes than and only than sensor2 count increment 
      Serial.print("metal detected");
      //lastdetectedtime = currenttime;
      proxi_counter++;
      Serial.print(proxi_counter);
      Serial.println(" times.");
      if (time1 > time2){// to avoid time overflow error
        TimeDifference= time1-time2;
        
      }
      else{
        TimeDifference= time2-time1;
      }
      
      
     // if (IfNeedAcceptableTime==true){
      AcceptableTime();
     // }
      time1=0;
      time2=0;
      detected_proxi1 = false;
      detected_proxi2 = false;
    }

    /*char msg1[50];
    snprintf(msg1, 50, "Proxi Counter: %d", proxi_counter);
    mqtt_client.publish("kinjal/esp32/counter", msg1);
    Serial.println(msg1);*/


   return;
}



void SensorShiftLeft(){
   if (detected_proxi1 == true && detected_proxi2 == false) {//if 
      Serial.print("metal detected");
     // proxi_counter++;
      //Serial.print(proxi_counter);
      //Serial.println(" times.");
       detected_proxi1 = false;
       detected_proxi2 = false;
    }
    

    if (detected_proxi1  && detected_proxi2 == true) {//if sensor 2 detxtes than and only than sensor1 count increment 
      Serial.print("metal detected");
      proxi_counter++;
      Serial.print(proxi_counter);
      Serial.println(" times.");
      if (time1 > time2){
        TimeDifference= time1-time2;
        
      }
      else{
        TimeDifference= time2-time1;
      }
      
     // Serial.println("TimeDifference:");
      //Serial.println(TimeDifference);
     AcceptableTime();
      time1=0;
      time2=0;
      detected_proxi1 = false;
      detected_proxi2 = false;
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


  if (storedOutputonoff=="ON"){
    pinMode(10, OUTPUT);
    Serial.println("output ON mmmmmmmmm");
  }
  return;
}




void SensorTriggerOutputon(){// output on for some time when sensor detects something  
  //outputStartTime = millis();

  if (WaitingForObjectDetectedSensor1 == true || WaitingForObjectDetectedSensor2 == true){
    Serial.println("objectdetected");
    outputStartTime = micros();
    WaitingForObjectDetectedSensor1 = false;
    WaitingForObjectDetectedSensor2 = false;
    digitalWrite(10, HIGH);
    outputIsActive = true;
    //outputstatesensor1=true;

  }
  if (outputIsActive && (micros() - outputStartTime >= 100000)) {//nbuzzer on for 100 mili seconds
    digitalWrite(10, LOW);
    outputIsActive = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;
} 

void AcceptableTime(){
  
  
  

  if (TimeDifference > storedSensor_timedifference*1000000UL){// IN MICROS
    char msg3[50];
    snprintf(msg3, 50, "Time difference not Acceptable: %d", proxi_counter);
    mqtt_client.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);

    TimeDifference=0;  
    if (storedSensorDiffOutputAlerton == 1){
       Serial.println("hhhhhhhhhhhhdddddddddd");
       TimeDiffAlertBetweenSensors();
    }
  

    
  }
  else{

    char msg3[50];
    snprintf(msg3, 50, "Time difference Acceptable: %d", proxi_counter);
    mqtt_client.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);
    TimeDifference=0;
    

  

  }
  //TimeDifference=0;
  return;
}


void TimeDiffAlertBetweenSensors(){
   outputStartTimeNeg = micros(); 
   digitalWrite(10, HIGH);
   outputIsActiveNeg = true;
   
   Serial.println("555555555555555555555f5554545sss5");
   if (outputIsActiveNeg && (micros() - outputStartTimeNeg >= 500000)) {//nbuzzer 500 mili seconds
      digitalWrite(10, LOW);
      outputIsActiveNeg = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
  }
  return;

}

void Sensor1AcceptableTime(){

  CurrentTimesensor1 = millis();

  if (WaitingForObjectDetectedSensor1 == true){
    Serial.println("objectdetected");
    lastdetectectedtimeSensor1 = CurrentTimesensor1;
    WaitingForObjectDetectedSensor1 = false;
    digitalWrite(10, LOW);
    outputstatesensor1=true;

  }

  long timedifferenceSensor1 = CurrentTimesensor1 -  lastdetectectedtimeSensor1;
  if (timedifferenceSensor1>storedSensorIndividualAcceptTimeinput*1000UL){
     if(outputstatesensor1==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(10, HIGH);
       outputstatesensor1=false;
     }

  }
  return;
}


void Sensor2AcceptableTime(){

  CurrentTimesensor2 = millis();

  if (WaitingForObjectDetectedSensor2 == true){
    Serial.println("objectdetected");
    lastdetectectedtimeSensor2 = CurrentTimesensor2;
    WaitingForObjectDetectedSensor2 = false;
    digitalWrite(10, LOW);
    outputstatesensor2=true;

  }

  long timedifferenceSensor2 = CurrentTimesensor2 -  lastdetectectedtimeSensor2;
  if (timedifferenceSensor2>storedSensorIndividualAcceptTimeinput*1000UL){
     if(outputstatesensor2==true){
       Serial.println("output onnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
       digitalWrite(10, HIGH);
       outputstatesensor2=false;
     }

  }
  return;
}

void OutputOnReset(){
  if (WaitingForReset == true ){
    //Serial.println("objectdetected");
    outputStartTime = micros();
    WaitingForReset = false;
    digitalWrite(10, HIGH);
    outputIsActive = true;
    //outputstatesensor1=true;

  }
  if (outputIsActive && (micros() - outputStartTime >= 100000)) {//nbuzzer on for 100 mili seconds
    digitalWrite(10, LOW);
    outputIsActive = false;
    Serial.println("Output pin 10 turned OFF after seconds.");
  }
  return;

}

