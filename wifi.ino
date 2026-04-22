void wifiConfig(){
  preferences.begin("mywifi", true);
  String storedSSID = preferences.getString("ssid", "");
  String storedPass = preferences.getString("password", "");
  preferences.end();
  if (storedSSID != "") {
    Serial.println("Attempting to connect to: " + storedSSID);
    WiFi.begin(storedSSID.c_str(), storedPass.c_str());
    // Wait 10 seconds for connection
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 20) {
      delay(500);
      Serial.print(".");
      retry++;
    }
  }
 
  //If connection failed or no credentials, start Access Point
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi Failed. Starting AP Mode...");
    WiFi.mode(WIFI_AP); 
    WiFi.softAP(SSID_AP, PASSWORD_AP);
    Serial.print("Connect to: "); Serial.println(SSID_AP);
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
  } 
  else {
    Serial.println("\nWiFi Connected!");
    Serial.print("Local IP: "); Serial.println(WiFi.localIP());
  }
  return;
}


void wifiCheck(){
  static unsigned long lastWifiCheck = 0;//Time line for Wifi Data Publish
  if (millis() - lastWifiCheck > 5000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("wifi Diconneted");
      //Make Wifi of Esp Connect To Previously Connected Credencial OR If Not Connected Then Start AP Mode As Well for Credential From User
      //WiFi.reconnect();
      WiFi.mode(WIFI_AP_STA);
      //WiFi.softAP(SSID_AP, PASSWORD_AP);
    } 
    else {
      Serial.println("wifi connected");
    }
    lastWifiCheck = millis();
  }
  return;
}