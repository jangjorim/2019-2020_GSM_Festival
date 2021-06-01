#include "WiFiEsp.h"

#include <SoftwareSerial.h> 
#define rxPin 2 
#define txPin 3 
SoftwareSerial esp01(txPin, rxPin); // SoftwareSerial NAME(TX, RX);

const char ssid[] = "";    // your network SSID (name)
const char pass[] = "";         // your network password
int status = WL_IDLE_STATUS;        // the Wifi radio's status

#define ledPin 13
#define digiPin 7

WiFiEspServer server(80);

void setup() {
//  pinMode(ledPin, OUTPUT);
//  pinMode(digiPin, OUTPUT);
//  digitalWrite(ledPin, LOW);
//  digitalWrite(digiPin, LOW);
  Serial.begin(9600);   // initialize serial for debugging
  esp01.begin(9600);   //와이파이 시리얼
  WiFi.init(&esp01);   // initialize ESP module
  while ( status != WL_CONNECTED) {   // 약 10초동안 wifi 연결 시도
    Serial.print(F("Attempting to connect to WPA SSID: "));
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);    // Connect to WPA/WPA2 network
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
  }
  server.begin();
}

const char HTTP_HEAD[] PROGMEM     = "<!DOCTYPE html><html lang=\"en\"><head>"
                                     "<meta name=\"viewport\"content=\"width=device-width,initial-scale=1,user-scalable=no\"/>"
                                     "<link rel=\"icon\" href=\"data:,\">";
const char HTTP_STYLE[] PROGMEM    = "<style>"
                                     "body{text-align:center;font-family:verdana;}"  
                                     "button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%} "
                                     "</style>";
const char HTTP_HEAD_END[] PROGMEM = "</head><body><div style=\"text-align:center;display:inline-block;min-width:260px;\">"
                                     "<h3>ESP01 Digital Pin Control</h3>";
const char BUTTON_TYPE[] PROGMEM   = "<p><button style=\"width:40%;background-color:#12cb3d;\">ON</button>"
                                     "<button style=\"margin-left:10%;width:40%;background-color:#1fa3ec;\">OFF</button></a></p>";
const char BUTTON_A_ON[] PROGMEM   = "<p>Button A</p><a href=\"/A/0\"><button style=\"background-color:#12cb3d;\">ON</button></a></p>";
const char BUTTON_A_OFF[] PROGMEM  = "<p>Button A</p><a href=\"/A/1\"><button style=\"background-color:#1fa3ec;\">OFF</button></a></p>";
const char BUTTON_B_ON[] PROGMEM   = "<p>Button B</p><a href=\"/B/0\"><button style=\"background-color:#12cb3d;\">ON</button></a></p>";
const char BUTTON_B_OFF[] PROGMEM  = "<p>Button B</p><a href=\"/B/1\"><button style=\"background-color:#1fa3ec;\">OFF</button></a></p>";
const char HTTP_END[] PROGMEM      = "</div></body></html>";

bool button_a = LOW; // off
bool button_b = LOW; // off

void loop() {
  WiFiEspClient client = server.available();  // listen for incoming clients
  if (client) {                               // if you get a client,
    while (client.connected()) {              // loop while the client's connected
      if (client.available()) {               // if there's bytes to read from the client,
        String income_AP = client.readStringUntil('\n');
        if (income_AP.indexOf(F("A/1")) != -1) {
          Serial.println(F("button_A on"));
          button_a = HIGH;
          digitalWrite(ledPin, button_a);
        } else if (income_AP.indexOf(F("A/0")) != -1) {
          Serial.println(F("button_A off"));
          button_a = LOW;
          digitalWrite(ledPin, button_a);
        } else if (income_AP.indexOf(F("B/1")) != -1) {
          Serial.println(F("button_B on"));
          button_b = HIGH;
          digitalWrite(digiPin, button_b);
        } else if (income_AP.indexOf(F("B/0")) != -1) {
          Serial.println(F("button_B off"));
          button_b = LOW;
          digitalWrite(digiPin, button_b);
        }
        client.flush();
        client.println(F("HTTP/1.1 200 OK"));  // HTTP 프로토콜 헤더
        client.println(F("Content-type:text/html"));
        client.println(F("Connection: close"));
        client.println();
        String page;
        page  = (const __FlashStringHelper *)HTTP_HEAD;
        page += (const __FlashStringHelper *)HTTP_STYLE;
        page += (const __FlashStringHelper *)HTTP_HEAD_END;
        page += (const __FlashStringHelper *)BUTTON_TYPE;
        if (button_a == HIGH) {
          page += (const __FlashStringHelper *)BUTTON_A_ON;
        } else {
          page += (const __FlashStringHelper *)BUTTON_A_OFF;
        }
        if (button_b == HIGH) {
          page += (const __FlashStringHelper *)BUTTON_B_ON;
        } else {
          page += (const __FlashStringHelper *)BUTTON_B_OFF;
        }
        page += (const __FlashStringHelper *)HTTP_END;
        client.print(page);
        client.println();
        delay(1);
        break;
      }
    }
    client.stop();
    Serial.println(F("Client disconnected"));
  }
}

/* 
 *  출처
 *  https://postpop.tistory.com/93
 *  
 */
 
 
