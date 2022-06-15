/*
 * NTP
 * night timer
 * LINE notify resend timer
 */
#include "M5Atom.h"
#include <EEPROM.h>
#include <WiFi.h> 
#include "NTRIPClient.h"
#include "esp_wifi.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
// WiFiAP:"M5Atom" Password:"m5atompass"
#include <WiFiClientSecure.h>

#define water_sensor 32
#define water_threshold 2000

// LINE Notify
const char* host = "notify-api.line.me";
const char* message = "戸磯 雨センサーbot！";
const char* LINE_TOKEN = "vi0ImFzWKBTydImc92UsPHzGEnM52wyJionPjcYYs7S";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(water_sensor,INPUT);
  M5.begin();
  delay(10);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  bool res;
  res= wm.autoConnect("M5Atom","m5atompass");
  if (!res) {
    Serial.println("Failed to connect");
    
  } else{
    Serial.println("Connected...yey :)");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int water = analogRead(water_sensor);
  String str = "ame:"+String (water);
  Serial.println(str);
  if (water_threshold > water) {
    send_line();
  }
  delay(10000);    
}

void send_line() {

  // HTTPSへアクセス（SSL通信）するためのライブラリ
  WiFiClientSecure client;

  // サーバー証明書の検証を行わずに接続する場合に必要
  client.setInsecure();
  
  Serial.println("Try");
  
  //LineのAPIサーバにSSL接続（ポート443:https）
  if (!client.connect(host, 443)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Connected");

  // リクエスト送信
  String query = String("message=") + String(message);
  String request = String("") +
               "POST /api/notify HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Bearer " + LINE_TOKEN + "\r\n" +
               "Content-Length: " + String(query.length()) +  "\r\n" + 
               "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
                query + "\r\n";
  client.print(request);
 
  // 受信完了まで待機 
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  
  String line = client.readStringUntil('\n');
  Serial.println(line);
}
