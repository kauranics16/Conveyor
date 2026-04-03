void ARDUINO_ISR_ATTR Proximity1_ISR() {
  proxi_counter1++;
  time1= micros();
  //Serial.print("timw1:");
  //Serial.println(time1);
  outputStartTime = micros(); 
  digitalWrite(10, HIGH);
  outputIsActive = true;
  detected_proxi1 = true;
  /*void ARDUINO_ISR_ATTR Proximity1_ISR() {
  if (!waitingForObject) {
    sensorTriggerTime = millis(); // Start the 2-second countdown
    waitingForObject = true;
  }
  detected_proxi1 = true;
}
*/

  //IfNeedAcceptableTime==true;
}

void ARDUINO_ISR_ATTR Proximity2_ISR() {
  proxi_counter2++;
  time2= micros();
  //Serial.print("timw2:");
  //Serial.println(time2);
  outputStartTime = micros(); 
  digitalWrite(10, HIGH);
  outputIsActive = true;
  detected_proxi2 = true;

  //IfNeedAcceptableTime==true;
}

void ARDUINO_ISR_ATTR Proximity3_ISR() {// for counter and reset mode
  proxi_counter1=0;
  proxi_counter2++;

  outputStartTimeNeg = micros(); 
  digitalWrite(10, HIGH);
  outputIsActiveNeg = true;
 
  detected_proxi2 = true;
  //IfNeedAcceptableTime== false;// not calculate the time difference as it is counter and Reset Mode
}




void ProximityConfig(){
  
  preferences.begin("sensor", true);
  String storedInput1Mode=preferences.getString("Sensor_1mode", "");
  String storedInput2Mode=preferences.getString("Sensor_2mode", "");
  storedsensor1offon=preferences.getInt("Sensor1onoff", 0);
  storedsensor2offon=preferences.getInt("Sensor2onoff", 0);
  storedSensor1nonc=preferences.getInt("Sensor1nonc", 0);
  storedSensor2nonc=preferences.getInt("Sensor2nonc", 0);
  storedSensorShiftchoise=preferences.getInt("Sensorshift", 0);

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
    //IfNeedAcceptableTime==true;
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
    //IfNeedAcceptableTime==true;
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
    //IfNeedAcceptableTime==false;
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
      proxi_counter++;
      Serial.print(proxi_counter);
      Serial.println(" times.");
      if (time1 > time2){// to avoid time overflow error
        TimeDifference= time1-time2;
        
      }
      else{
        TimeDifference= time2-time1;
      }
      //Serial.println("TimeDifference:");
     // Serial.println(TimeDifference);
    // Serial.println("IfNeedAcceptableTime");
     // Serial.println(IfNeedAcceptableTime);
      
     // if (IfNeedAcceptableTime==true){
      AcceptableTime();
     // }
      time1=0;
      time2=0;
      detected_proxi1 = false;
      detected_proxi2 = false;
    }

    char msg1[50];
    snprintf(msg1, 50, "Proxi Counter: %d", proxi_counter);
    mqtt_client.publish("kinjal/esp32/counter", msg1);
    Serial.println(msg1);


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

    char msg1[50];
    snprintf(msg1, 50, "Proxi Counter: %d", proxi_counter);
    mqtt_client.publish("kinjal/esp32/counter", msg1);
    Serial.println(msg1);


   return;

}

void AcceptableTime(){
  preferences.begin("sensor", true);
  int storedSensor_timedifference = preferences.getInt("proxi_time",0);//getInt(const char* key, const int32_t defaultValue)
  preferences.end();

  if (TimeDifference > storedSensor_timedifference*1000000UL){// IN MICROS
    char msg3[50];
    snprintf(msg3, 50, "Time difference not Acceptable: %d", proxi_counter);
    mqtt_client.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);
    outputStartTimeNeg = micros(); 
    digitalWrite(10, HIGH);
    outputIsActiveNeg = true;
    TimeDifference=0;  

    //IfNeedAcceptableTime==false;
  }
  else{

    char msg3[50];
    snprintf(msg3, 50, "Time difference Acceptable: %d", proxi_counter);
    mqtt_client.publish("kinjal/esp32/time", msg3);
    Serial.println(msg3);
    outputStartTime = micros(); 
    digitalWrite(10, HIGH);
    outputIsActive = true;
    TimeDifference=0;

    //IfNeedAcceptableTime==false;

  }
  //TimeDifference=0;
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

  preferences.begin("sensor", true);
  String storedOutputonoff=preferences.getString("Outputonoff", "");
  preferences.end();

  if (storedOutputonoff=="NO"){
    pinMode(10, OUTPUT);
    Serial.println("output ON mmmmmmmmm");
  }
  return;
}


void Sensor1AcceptableTime(){





}


