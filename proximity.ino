void ARDUINO_ISR_ATTR Proximity1_ISR() {  //Intterupt Service Routine for Input1 (Sensor1) and time difference alert
  sensor1Count++;
  sensor1Isr = true;
}

void ARDUINO_ISR_ATTR Proximity2_ISR() {  //Interrupt Service Routine of Input2 (Sensor2) and time difference alert
  sensor2Count++;
  sensor2Isr = true;
}

void ARDUINO_ISR_ATTR RESET_ISR() {  // Interupt Routine for Input2 as Reset Button
  sensor1Count = 0;
  sensor2Count++;
  waitingForResetIsr = true;
}

void ARDUINO_ISR_ATTR TIMEDIFF1_ISR() {  // Interupt Routine for Mqtt Publish Time Difference between sensor
  timeDiffPubIsr1 = true;
  timeDiffPubIsrTime1 = millis();
}

void ARDUINO_ISR_ATTR TIMEDIFF2_ISR() {  // Interupt Routine for Mqtt Publish Time Difference between sensor
  timeDiffPubIsr2 = true;
  timeDiffPubIsrTime2 = millis();
}

void ARDUINO_ISR_ATTR TIMEDIFFOUT1_ISR() {  // Interupt Routine for Output On Time Difference Between Sensor
  timeDiffIsrOutTime1 = millis();
  timeDiffIsrOut1 = true;
}

void ARDUINO_ISR_ATTR TIMEDIFFOUT2_ISR() {  // Interupt Routine for Output On Time Difference Between Sensor
  timeDiffIsrOutTime2 = millis();
  timeDiffIsrOut2 = true;
}

//To Handle Extra variables of ISR
void sensor1IsrFlags() {
  sensor1Shift = true;
  acceptTimeIsr1 = true;
}

void sensor2IsrFlags() {
  sensor2Shift = true;
  acceptTimeIsr2 = true;
}

// Interrupt Service Routine (ISR)
void ARDUINO_ISR_ATTR Sensor1Time_ISR() {
  isStateChanged1 = true;
  currentStateSensor1 = digitalRead(6);
  sensor1Count++;
  sensor1Shift = true;
}

void ARDUINO_ISR_ATTR Sensor2Time_ISR() {
  isStateChanged2 = true;
  currentStateSensor2 = digitalRead(7);
  sensor2Count++;
  sensor2Shift = true;
}

//Configure and Load all data stored in Esp32 Memory
void configInputOutput() {
  preferences.begin("sensor", true);
  storedSensor1Name = preferences.getString("sensor1Name", "");
  storedSensor2Name = preferences.getString("sensor2Name", "");
  bool storedInput1Mode = preferences.getBool("sensor1Mode", false);
  bool storedInput2Mode = preferences.getBool("sensor2Mode", false);
  bool storedSensor1On = preferences.getBool("sensor1On", false);
  uint8_t storedSensor2On = preferences.getInt("sensor2On", 0);
  bool storedSensor1TypeChoice = preferences.getBool("sensor1Type", false);
  bool storedSensor2TypeChoice = preferences.getBool("sensor2Type", false);
  bool storedSensorShiftChoise = preferences.getBool("shiftChoice", false);
  storedShiftCounterOn = preferences.getBool("shiftCount", false);
  storedJsonCounterOn = preferences.getBool("jsonCountOn", false);
  storedTimeDiffSeconds = preferences.getInt("inputTimeDiff", 0);
  storedPubTimeDiffAlert = preferences.getBool("timeDiffPub", false);
  bool storedOutputChoice = preferences.getBool("outputOn", false);
  bool storedSensorTriggerOut = preferences.getBool("outputOnTrig", false);
  bool storedSensorDiffOut = preferences.getBool("timeDiffOut", false);
  uint8_t storedAcceptTimeSelect = preferences.getInt("acceptSelect", 0);  
  storedAcceptTimeSeconds = preferences.getInt("acceptTime", 0);
  uint8_t storedTimeBwObject = preferences.getInt("timeBwObject", false);
  uint8_t storedOnTimeChoice = preferences.getInt("sensorOnOffTime", 0);
  preferences.end();

  //---Enum Local Varaibles and Assigning State (Decision)---
  sensorType sensor1Type;
  sensorType sensor2Type;
  sensorMode sensor1Mode;
  sensorMode sensor2Mode;



  if (storedSensor1On == true) {
    sensor1State = STATE_ON;
  }

  if (storedSensor2On == 1) {
    sensor2State = STATE_ON;
  } else if (storedSensor2On == 2) {
    sensor2State = STATE_OFF;
  } else {
    sensor2State = STATE_RESET;
  }


  if (storedSensor1TypeChoice == true) {
    sensor1Type = TYPE_NO;
  } else {
    sensor1Type = TYPE_NC;
  }


  if (storedSensor2TypeChoice == true) {
    sensor2Type = TYPE_NO;
  } else {
    sensor2Type = TYPE_NC;
  }


  if (storedInput1Mode == true) {
    sensor1Mode = MODE_FALLING;
  } else {
    sensor1Mode = MODE_RISING;
  }
  if (storedInput1Mode == true) {
    sensor2Mode = MODE_FALLING;
  } else {
    sensor2Mode = MODE_RISING;
  }

  if (storedSensorShiftChoise == true) {
    shiftMode = RIGHT_SHIFT;
  } else {
    shiftMode = LEFT_SHIFT;
  }



  if (storedOnTimeChoice == 1) {
    pubOnOffCycle = SENSOR1_ONLY;
  } else if (storedOnTimeChoice == 2) {
    pubOnOffCycle = SENSOR2_ONLY;
  } else if (storedOnTimeChoice == 3) {
    pubOnOffCycle = BOTH_SENSORS;
  }


  if (storedTimeBwObject == 1) {
    pubTimeBwObject = SENSOR1_ONLY;
  } else if (storedTimeBwObject == 2) {
    pubTimeBwObject = SENSOR2_ONLY;
  } else if (storedTimeBwObject == 3) {
    pubTimeBwObject = BOTH_SENSORS;
  }


  if (storedSensorTriggerOut == true) {
    onSensorTrigger = OUTPUT_ON;
  }

  if (storedSensorDiffOut == true) {
    onSensorsTimeDiff = OUTPUT_ON;
  }

  //   User Selection of Acceptable Time Sensor1, Acceptable Time sensor2, Reset Mode
  if (storedAcceptTimeSelect == 1) {
    outputOnAcceptTime = SENSOR1;
  } else if (storedAcceptTimeSelect == 2) {
    outputOnAcceptTime = SENSOR2;
  } else if (storedAcceptTimeSelect == 3) {
    outputOnAcceptTime = RESET;
  }

  if (pubOnOffCycle == SENSOR1_ONLY) {
    attachInterrupt(PROX_SENSOR1, Sensor1Time_ISR, CHANGE);
  } else if (pubOnOffCycle == SENSOR2_ONLY) {
    attachInterrupt(PROX_SENSOR2, Sensor2Time_ISR, CHANGE);
  } else if (pubOnOffCycle == BOTH_SENSORS) {
    attachInterrupt(PROX_SENSOR1, Sensor1Time_ISR, CHANGE);
    attachInterrupt(PROX_SENSOR2, Sensor2Time_ISR, CHANGE);
  }


  //Sensor Configuration as per User Need
  if (sensor1State == STATE_ON) {
    if (sensor1Type == TYPE_NO) {
      if (sensor1Mode == MODE_FALLING) {
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING);
        if (storedPubTimeDiffAlert == true) {
          attachInterrupt(PROX_SENSOR1, TIMEDIFF1_ISR, FALLING);
        }
        if (onSensorsTimeDiff == OUTPUT_ON) {
          attachInterrupt(PROX_SENSOR1, TIMEDIFFOUT1_ISR, FALLING);
        }

      } else {
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING);
        if (storedPubTimeDiffAlert == true) {
          attachInterrupt(PROX_SENSOR1, TIMEDIFF1_ISR, RISING);
        }
        if (onSensorsTimeDiff == OUTPUT_ON) {
          attachInterrupt(PROX_SENSOR1, TIMEDIFFOUT1_ISR, RISING);
        }
      }
    } else {
      if (sensor1Mode == MODE_RISING) {
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, RISING);
        if (storedPubTimeDiffAlert == true) {
          attachInterrupt(PROX_SENSOR1, TIMEDIFF1_ISR, RISING);
        }
        if (onSensorsTimeDiff == OUTPUT_ON) {
          attachInterrupt(PROX_SENSOR1, TIMEDIFFOUT1_ISR, RISING);
        }
      } else {
        attachInterrupt(PROX_SENSOR1, Proximity1_ISR, FALLING);
        if (storedPubTimeDiffAlert == true) {
          attachInterrupt(PROX_SENSOR1, TIMEDIFF1_ISR, FALLING);
        }
        if (onSensorsTimeDiff == OUTPUT_ON) {
          attachInterrupt(PROX_SENSOR1, TIMEDIFFOUT1_ISR, FALLING);
        }
      }
    }
  }

  //If Input2 Is In Sensor2 mode
  if (sensor2State == STATE_ON) {
    if (sensor2Type == TYPE_NO) {
      if (sensor2Mode == MODE_FALLING) {
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING);
        if (storedPubTimeDiffAlert == true) {
          attachInterrupt(PROX_SENSOR2, TIMEDIFF2_ISR, FALLING);
        }
        if (onSensorsTimeDiff == OUTPUT_ON) {
          attachInterrupt(PROX_SENSOR2, TIMEDIFFOUT2_ISR, FALLING);
        }
      } else {
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING);
        if (storedPubTimeDiffAlert == true) {
          attachInterrupt(PROX_SENSOR2, TIMEDIFF2_ISR, RISING);
        }
        if (onSensorsTimeDiff == OUTPUT_ON) {
          attachInterrupt(PROX_SENSOR2, TIMEDIFFOUT2_ISR, RISING);
        }
      }
    } else {  //If NC Choosen
      if (sensor2Mode == MODE_RISING) {
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, RISING);
        if (storedPubTimeDiffAlert == true) {
          attachInterrupt(PROX_SENSOR2, TIMEDIFF2_ISR, RISING);
        }
        if (onSensorsTimeDiff == OUTPUT_ON) {
          attachInterrupt(PROX_SENSOR2, TIMEDIFFOUT2_ISR, RISING);
        }
      } else {
        attachInterrupt(PROX_SENSOR2, Proximity2_ISR, FALLING);
        if (storedPubTimeDiffAlert == true) {
          attachInterrupt(PROX_SENSOR2, TIMEDIFF2_ISR, FALLING);
        }
        if (onSensorsTimeDiff == OUTPUT_ON) {
          attachInterrupt(PROX_SENSOR2, TIMEDIFFOUT2_ISR, FALLING);
        }
      }
    }
  }

  // If Input2 Is In Reset Mode (Button)
  if (sensor2State == STATE_RESET) {
    if (sensor2Type == TYPE_NO) {
      if (sensor2Mode == MODE_FALLING) {
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING);
      } else if (storedInput2Mode == 2) {
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING);
      }
    } else {  //IF NC choosen for sensor
      if (sensor2Mode == MODE_RISING) {
        attachInterrupt(PROX_SENSOR2, RESET_ISR, RISING);
      } else {
        attachInterrupt(PROX_SENSOR2, RESET_ISR, FALLING);
      }
    }
  }


  if (storedOnTimeChoice == 1) {
    attachInterrupt(PROX_SENSOR1, Sensor1Time_ISR, CHANGE);
  } else if (storedOnTimeChoice == 2) {
    attachInterrupt(PROX_SENSOR2, Sensor2Time_ISR, CHANGE);
  } else if (storedOnTimeChoice == 3) {
    attachInterrupt(PROX_SENSOR1, Sensor1Time_ISR, CHANGE);
    attachInterrupt(PROX_SENSOR2, Sensor2Time_ISR, CHANGE);
  }


  //Output On or OFF
  if (storedOutputChoice == true) {
    pinMode(OUTPUT_PIN, OUTPUT);
  }
  return;
}


//Sensor Shift Right is Choosen by User
void sensorShiftRight() {
  if (sensor1Shift == false && sensor2Shift == true) {
    sensor1Shift = false;
    sensor2Shift = false;
  }

  //If Sensor 1 Detects Then And Only Than sensor2 Detects And Count Incremented For Shift Count
  if (sensor1Shift && sensor2Shift == true) {
    sensorCount++;
    Serial.print(sensorCount);
    Serial.println(" times.");
    sensor1Shift = false;
    sensor2Shift = false;
  }
  return;
}


//Sensor Shift Left is Choosen by User
void sensorShiftLeft() {
  if (sensor1Shift == true && sensor2Shift == false) {
    sensor1Shift = false;
    sensor2Shift = false;
  }

  //If Sensor 2 Detects Then And Only Than sensor1 Detects And Count Incremented For Shift Count
  if (sensor1Shift && sensor2Shift == true) {
    sensorCount++;
    Serial.print(sensorCount);
    Serial.println(" times.");
    sensor1Shift = false;
    sensor2Shift = false;
  }
  return;
}


//Time Difference Between Sensors MqttPublish Alert Message (Different Conveyor)
void acceptableTimeAlert() {
  static unsigned long timeDiffInSensors = 0;
  if (timeDiffPubIsr1 == true && timeDiffPubIsr2 == true) {
    timeDiffPubIsr1 = false;
    timeDiffPubIsr2 = false;
    if (timeDiffPubIsrTime1 > timeDiffPubIsrTime2) {
      timeDiffInSensors = timeDiffPubIsrTime1 - timeDiffPubIsrTime2;
    } else {
      timeDiffInSensors = timeDiffPubIsrTime2 - timeDiffPubIsrTime1;
    }
  }
  if (timeDiffInSensors > storedTimeDiffSeconds * 1000UL) {
    String alertMessage = "Time difference not Acceptable";
    mqttClient.publish("kinjal/esp32/time", alertMessage.c_str());
    Serial.println(alertMessage);

    timeDiffInSensors = 0;
    timeDiffPubIsrTime1 = 0;
    timeDiffPubIsrTime2 = 0;
  }
  return;
}


//Time Difference Between Sensors Output Alert Of Few Seconds
void timeDiffAlertBetweenSensors() {
  static bool outActiveOnSensorDiff = false;
  static unsigned long timeDifferenceAlert = 0;
  static unsigned long outTimeOn1SensorDiff = 0;
  if (timeDiffIsrOut1 == true && timeDiffIsrOut2 == true) {
    outTimeOn1SensorDiff = millis();
    timeDiffIsrOut1 = false;
    timeDiffIsrOut2 = false;

    if (timeDiffIsrOutTime1 > timeDiffIsrOutTime2) {
      timeDifferenceAlert = timeDiffIsrOutTime1 - timeDiffIsrOutTime2;
    } else {
      timeDifferenceAlert = timeDiffIsrOutTime2 - timeDiffIsrOutTime1;
    }
  }
  //Output On For Few Seconds Only When Time Difference Between 2 Sensors Is Beyond Time Gap Inputed by User
  if (timeDifferenceAlert > storedTimeDiffSeconds * 1000UL) {
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnSensorDiff = true;
    if (outActiveOnSensorDiff && (millis() - outTimeOn1SensorDiff >= 1000UL)) {  //Buzzer on 1000 mili seconds
      digitalWrite(OUTPUT_PIN, LOW);
      outActiveOnSensorDiff = false;
      Serial.println("Output pin 10 turned OFF after 2 seconds.");
      timeDifferenceAlert = 0;
      timeDiffIsrOutTime1 = 0;
      timeDiffIsrOutTime2 = 0;
    }
  }
  return;
}


//Output On For Few seconds When Sensors detect Respectively
void sensorTriggerOutputOn() {
  static bool outActiveOnTrigger = false;
  static unsigned long outTimeOn1SensorTrig = 0;

  if (acceptTimeIsr1 == true || acceptTimeIsr2 == true) {
    outTimeOn1SensorTrig = millis();
    acceptTimeIsr1 = false;
    acceptTimeIsr2 = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnTrigger = true;
  }
  //Output On For 100 mili seconds
  if (outActiveOnTrigger && (millis() - outTimeOn1SensorTrig >= 100UL)) {
    digitalWrite(OUTPUT_PIN, LOW);
    outActiveOnTrigger = false;
  }
  return;
}




//If Acceptable time to Detect Object is Greater Than User Input Acceptable time than Output On Continuously Until it Detects the Object (for Sensor1 or Input1 only)
void sensor1AcceptableTime() {
  static unsigned long lastOutTimeSensor1 = 0;
  static bool outStateSensor1 = false;
  static unsigned long timeDifferenceSensor1 = 0;
  outTimeSensor1 = millis();
  if (acceptTimeIsr1 == true) {
    lastOutTimeSensor1 = outTimeSensor1;
    acceptTimeIsr1 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    outStateSensor1 = true;
  }
  timeDifferenceSensor1 = outTimeSensor1 - lastOutTimeSensor1;
  //Output On continuously when Time needed by sensor1 to detect something is beyond Time Gap Inputed by User
  if (timeDifferenceSensor1 > storedAcceptTimeSeconds * 1000UL) {
    if (outStateSensor1 == true) {
      digitalWrite(OUTPUT_PIN, HIGH);
      outStateSensor1 = false;
    }
  }
  return;
}

//If Acceptable time to Detect Object is Greater Than User Input Acceptable time than Output On Continuously Until it Detects the Object (for Sensor2 or Input2 only)
void sensor2AcceptableTime() {
  static unsigned long lastOutTimeSensor2 = 0;
  static bool outStateSensor2 = false;
  static unsigned long timeDifferenceSensor2 = 0;
  outTimeSensor2 = millis();
  if (acceptTimeIsr2 == true) {
    lastOutTimeSensor2 = outTimeSensor2;
    acceptTimeIsr2 = false;
    digitalWrite(OUTPUT_PIN, LOW);
    outStateSensor2 = true;
  }
  timeDifferenceSensor2 = outTimeSensor2 - lastOutTimeSensor2;
  //Output On continuously when Time needed by sensor2 to detect something is beyond Time Gap Inputed by User
  if (timeDifferenceSensor2 > storedAcceptTimeSeconds * 1000UL) {
    if (outStateSensor2 == true) {
      digitalWrite(OUTPUT_PIN, HIGH);
      outStateSensor2 = false;
    }
  }
  return;
}


//Output On For Few Seconds When Reset Button Pressed
void outputOnReset() {
  static unsigned long outTimeOn1Reset = 0;
  static bool outActiveOnReset = false;
  if (waitingForResetIsr == true) {
    outTimeOn1Reset = millis();
    waitingForResetIsr = false;
    digitalWrite(OUTPUT_PIN, HIGH);
    outActiveOnReset = true;
  }
  //Output on for mili seconds
  if (outActiveOnReset && (millis() - outTimeOn1Reset >= 100UL)) {
    digitalWrite(OUTPUT_PIN, LOW);
    outActiveOnReset = false;
  }
  return;
}

//Publish Mqtt Message of Time Taken by Sensor1 to Detect 1 Object After Another Continuously
void TimeBetweenObject1() {
  if (sensor1Isr == true) {
    static unsigned long lastOutTime1 = 0;
    static unsigned long timeDifference1 = 0;
    timeDifference1 = millis() - lastOutTime1;
    lastOutTime1 = millis();
    String timeDiff1;
    timeDiff1 += "TimeDifference1:" + String(timeDifference1);
    mqttClient.publish("kinjal/esp32/timediff1", timeDiff1.c_str());
    Serial.println(timeDiff1);
    timeDifference1 = 0;
  }
  return;
}

//Publish Mqtt Message of Time Taken by Sensor2 to Detect 1 Object After Another Continuously
void TimeBetweenObject2() {
  if (sensor2Isr == true) {
    static unsigned long lastOutTime2 = 0;
    static unsigned long timeDifference2 = 0;
    timeDifference2 = millis() - lastOutTime2;
    lastOutTime2 = millis();
    String timeDiff2;
    timeDiff2 += "TimeDifference2:" + String(timeDifference2);
    mqttClient.publish("kinjal/esp32/timediff2", timeDiff2.c_str());
    Serial.println(timeDiff2);
    timeDifference2 = 0;
  }
  return;
}

//Publish Mqtt Data OFF and ON Cycle Time of Sensor1
void onOffTimeSensor1() {
  static bool lastState1 = true;
  static unsigned long timeLow1 = 0;
  static unsigned long timeHigh1 = 0;
  static unsigned long timeOn1 = 0;
  static unsigned long timeOff = 0;
  if (isStateChanged1 == true) {
    isStateChanged1 = false;
    if (currentStateSensor1 == false && lastState1 == true) {  //"low to high"
      timeLow1 = millis();
      if (timeHigh1 != 0) {
        timeOff = timeLow1 - timeHigh1;
        Serial.println(timeOff);
        String timeOfff;
        timeOfff += "OffTime1 in mili seconds:" + String(timeOff);
        mqttClient.publish("kinjal/esp32/offtime1", timeOfff.c_str());
        Serial.println(timeOfff);
        //Reset
        timeHigh1 = 0;
        timeOff = 0;
      }
      lastState1 = false;
    } else if (currentStateSensor1 == true && lastState1 == false) {  //ON state
      timeHigh1 = millis();
      timeOn1 = timeHigh1 - timeLow1;
      String timeOnn1;
      timeOnn1 += "OnTime1 in mili seconds:" + String(timeOn1);
      mqttClient.publish("kinjal/esp32/ontime1", timeOnn1.c_str());
      Serial.println(timeOnn1);
      //Reset the time
      timeLow1 = 0;
      timeOn1 = 0;
      lastState1 = currentStateSensor1;
    }
  }
}

//Publish Mqtt Data Of OFF and ON Time of Sensor2
void onOffTimeSensor2() {
  static bool lastState2 = true;
  static unsigned long timeLow2 = 0;
  static unsigned long timeHigh2 = 0;
  static unsigned long timeOn2 = 0;
  static unsigned long timeOff2 = 0;
  if (isStateChanged2 == true) {
    isStateChanged2 = false;
    if (currentStateSensor2 == false && lastState2 == true) {
      timeLow2 = millis();
      if (timeHigh2 != 0) {
        timeOff2 = timeLow2 - timeHigh2;
        Serial.println(timeOff2);
        String timeOfff2;
        timeOfff2 += "OffTime2 in mili seconds:" + String(timeOff2);
        mqttClient.publish("kinjal/esp32/offtime2", timeOfff2.c_str());
        Serial.println(timeOfff2);
        //Reset
        timeHigh2 = 0;
        timeOff2 = 0;
      }
      lastState2 = false;
    } else if (currentStateSensor2 == true && lastState2 == false) {  //ON state (high)
      timeHigh2 = millis();
      timeOn2 = timeHigh2 - timeLow2;
      String timeOnn2;
      timeOnn2 += "OnTime2 in mili seconds:" + String(timeOn2);
      mqttClient.publish("kinjal/esp32/ontime2", timeOnn2.c_str());
      Serial.println(timeOnn2);
      //Reset the time
      timeLow2 = 0;
      timeOn2 = 0;
      lastState2 = currentStateSensor2;
    }
  }
}
