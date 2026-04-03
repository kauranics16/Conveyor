void WifiConfig(){
  preferences.begin("mywifi", true);
   storedSSID = preferences.getString("ssid", "");
   storedPass = preferences.getString("password", "");
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
    WiFi.mode(WIFI_AP); // Switch to pure AP mode
    WiFi.softAP(ssid_ap, password_ap);
    Serial.print("Connect to: "); Serial.println(ssid_ap);
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
  } 
  else {
    Serial.println("\nWiFi Connected!");
    Serial.print("Local IP: "); Serial.println(WiFi.localIP());
    //Optional: keep AP running even when connected
   // WiFi.mode(WIFI_AP_STA); 
    //WiFi.softAP(ssid_ap, password_ap);
  }
  return;
}