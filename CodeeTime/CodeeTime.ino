#include "M5StickCPlus.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define WIFI_SSID ""//<- wifi ssid
#define WIFI_PASSWORD "" //<- wifi password


// スプレッドシートURL
const char *host = "";//<- sheet url

// Json設定
StaticJsonDocument<255> json_request;
char buffer[255];

int codetime = 0;

int flag=1;
int seecodetime=1;
int sumcodetime=0;

void setup() {
    M5.begin();
    M5.Lcd.setRotation(3);
    // Wi-Fi接続
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    M5.Lcd.print("connecting");
    while (WiFi.status() != WL_CONNECTED) {
        M5.Lcd.print(".");
        delay(500);
    }
    Serial.println();
    // WiFi Connected
    M5.Lcd.println("\nWiFi Connected.");
    M5.Lcd.println(WiFi.localIP());
    M5.Lcd.setRotation(3); // 画面を横向きにする
    M5.Lcd.setTextSize(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.println("Setup Done.");
    delay(500);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.printf("Codee Time\n Standby");  
}

void sendCount() {
  json_request["count"] = sumcodetime;
  serializeJson(json_request, buffer, sizeof(buffer));

  HTTPClient http;
  http.begin(host);
  http.addHeader("Content-Type", "application/json");
  int status_code = http.POST((uint8_t*)buffer, strlen(buffer));
  Serial.println(status_code);
  if (status_code > 0) {   
      if (status_code == HTTP_CODE_FOUND) {
        String payload = http.getString();
        Serial.println(payload);

        M5.Lcd.setCursor(10, 100);
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(3);
        M5.Lcd.println("Send Done!");
        sumcodetime=0;
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(status_code).c_str());
  }  
  http.end();
  delay(2000);
}


void loop() {
    M5.update();
    //display codetime
    if(flag==0){
        if(seecodetime%2){
            M5.Lcd.setCursor(10, 10);
            M5.Lcd.fillScreen(BLACK);
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setTextSize(3);
            int nowcodetime,h,m,s;
            nowcodetime=millis()-codetime;
            nowcodetime/=1000;
            h=nowcodetime%3600;
            h/=3600;
            m=nowcodetime/60;
            s=nowcodetime%60;
            M5.Lcd.printf("Codee now");
            M5.Lcd.setTextSize(4);
            M5.Lcd.setCursor(10, 40);
            M5.Lcd.printf("%dh %dm %ds",h,m,s);
        }else{
            M5.Lcd.setCursor(10, 10);
            M5.Lcd.fillScreen(BLACK);
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setTextSize(3);
            M5.Lcd.println("Codee now");
        }
    }
    //start and stop
    if (M5.BtnA.isPressed()) {
        if(flag){
            //start
            codetime=millis();
            M5.Lcd.setCursor(10, 10);
            M5.Lcd.fillScreen(BLACK);
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setTextSize(3);
            M5.Lcd.printf("Let's Codee!");
            flag=0;
            delay(2000);
        }else{
            //end
            codetime=millis()-codetime;
            codetime/=1000;
            M5.Lcd.setCursor(10, 10);
            M5.Lcd.fillScreen(GREEN);
            M5.Lcd.setTextColor(BLACK);
            M5.Lcd.setTextSize(3);
            M5.Lcd.printf("Nice Codee!");
            int h,m,s;
            h=codetime/3600;
            codetime=codetime-3600*h;
            m=codetime/60;
            s=codetime%60;
            M5.Lcd.setTextSize(4);
            M5.Lcd.setCursor(10, 40);
            M5.Lcd.printf("%dh %dm %ds",h,m,s);
            sumcodetime+=codetime;
            flag=1;
            delay(2000);
            //sendCount();
            M5.Lcd.setCursor(10, 10);
            M5.Lcd.fillScreen(BLACK);
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setTextSize(3);
            M5.Lcd.printf("Codee Time\n Standby"); 
            M5.Lcd.setTextSize(4);
            M5.Lcd.setCursor(10, 80);
            h=sumcodetime%3600;
            h/=3600;
            m=sumcodetime/60;
            s=sumcodetime%60;
            M5.Lcd.printf("%dh %dm %ds",h,m,s);
        }
    }

    if(M5.BtnB.isPressed()) {
        seecodetime++;
    }
    // send
    if(M5.BtnB.pressedFor(2000)) {
        int h,m,s;
        h=sumcodetime%3600;
        h/=3600;
        m=sumcodetime/60;
        s=sumcodetime%60;
        M5.Lcd.setCursor(10, 10);
        M5.Lcd.fillScreen(BLUE);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(3);
        M5.Lcd.printf("Time Send");
        M5.Lcd.setCursor(10, 40);
        M5.Lcd.setTextSize(4);
        M5.Lcd.printf("%dh %dm %ds",h,m,s);
        delay(1000);
        sendCount();
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(10, 10);
        M5.Lcd.printf("Codee Time\n Standby");
        seecodetime=1;
    }
    delay(250);
}