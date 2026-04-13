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
    //Optional: keep AP running even when connected
    //WiFi.mode(WIFI_AP_STA); 
    //WiFi.softAP(ssid_ap, password_ap);
  }
  return;
}