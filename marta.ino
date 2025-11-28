#include <WiFi.h>
#include <HTTPClient.h> 
#include <ArduinoJson.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// For direction circle
int x_pos = 40;
int y_pos = 80;
int x_center = x_pos - 8;
int y_center = y_pos - 10;

const char* ssid     = "Add Your Wifi Network Name Here"; 
const char* password = "Add Your Wifi Password Here"; 

const String serverPath = "Your personal server path"; 

String direction;

// I increased this slightly just in case, but keep an eye on memory usage!
const size_t CAPACITY = 1024; 


void setup()
{
    Serial.begin(9600);
    delay(10);

    // --- WiFi Connection Logic ---
    Serial.println("\nConnecting to " + String(ssid));
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

    // Set up the lcd
    tft.begin();
    tft.fillScreen(TFT_WHITE);
    tft.setRotation(1);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setTextFont(2);
    tft.setTextSize(2);
    
    // Initial display message
    tft.setCursor(220, 180);
    tft.setCursor(x_pos, y_pos + 60);
    tft.print("Fetching train times, please wait...");
    delay(5000);
}


void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverPath);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode);

      String payload = http.getString();
      Serial.println("Payload: " + payload);

      StaticJsonDocument<CAPACITY> doc;
      
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        tft.fillScreen(TFT_RED);
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.setTextSize(2);
        tft.setCursor(220, 180);
        tft.print("JSON Error!");
      } 
      
      else {        
        tft.fillScreen(TFT_WHITE); 
        tft.setTextSize(1);

        // Header
        tft.setCursor(120, 20);
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.setTextFont(2);
        tft.print("Inman Park/Reynoldstown Train Times");
        
        int y_change = 0;

        if (doc.size() > 0) {
    
          for (int i = 0; i < 4; i++) {
            JsonObject currentTrain = doc[i];
            printMartaLine(currentTrain, y_change);
            y_change += 60;
          } 
        }
        else {
          // Handle empty JSON array
          tft.fillScreen(TFT_BLACK);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.setTextSize(2);
          tft.setCursor(220, 180);
          tft.print("No Train Data Available.");
        }
      }
    }
    
    http.end(); 
    
    if (httpResponseCode <= 0) {
        Serial.print("HTTP Error: ");
        Serial.println(httpResponseCode);
        tft.fillScreen(TFT_RED);
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.setTextSize(2);
        tft.setCursor(220, 180);
        tft.print("HTTP FAILED: " + String(httpResponseCode));
    }
  }
  else {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(x_pos, y_pos);
    tft.print("WiFi Disconnected");
    Serial.println("WiFi Disconnected");
  }

  // Delay before refreshing
  delay(15000); 
}

void printMartaLine(JsonObject train, int y_down) {

  // Circle with direction in the middle, get color and direction
  if (train["LINE"] == "BLUE") {
    tft.fillCircle(x_pos, y_pos + y_down, 22, TFT_BLUE);
  }
  else if (train["LINE"] == "GREEN") {
    tft.fillCircle(x_pos, y_pos + y_down, 22, TFT_GREEN);
  }
  tft.setCursor(x_center, y_center + y_down);
  tft.setTextFont(4);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.print(String(train["DIRECTION"].as<const char*>()));

  // Desitnation train is headed
  tft.setCursor(x_pos + 30, y_center + y_down);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.print(String(train["DESTINATION"].as<const char*>()));

  // Wait time
  tft.setCursor(x_pos + 300, y_center + y_down);
  tft.print(String(train["WAITING_TIME"].as<const char*>()));
}