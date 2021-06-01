#include <SPI.h>
#include "WiFiEsp.h"
#include <ArduinoJson.h>
// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(2, 3); // RX, TX
#endif

#define TEST_LED 12

char ssid[] = ""; //연결할 와이파이 이름//
char pass[] = ""; //와이파이의 비밀번호/
int status = WL_IDLE_STATUS;

//입력변수//
String rcvbuf;
boolean getIsConnected = false;
String jsondata = "";
StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
root["temp"] = temp;
root["hum"] = humi;
root.printTo(jsondata);
Serial.println(jsondata);

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in milliseconds

//서버의 정보//
IPAddress hostIp(192, 168, 0, 4);
// Initialize the Ethernet client object
WiFiEspClient client;// Initialize the Ethernet client object


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(9600);
  WiFi.init(&Serial1);
  
  // 현재 아두이노에 연결된 실드를 확인
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // 와이파이 실드가 아닐 경우
    while(true);
  } 
  
 // 와이파이에 연결 시도
  while ( status != WL_CONNECTED) { // 연결이 될 때 까지 반복
    Serial.print("Attempting to CONNECT to WPA SSID: ");
    Serial.println(ssid);
    // WPA/WAP2 네트워크에 연결 (시간이 필요)
    status = WiFi.begin(ssid, pass);

    delay(10000);
  }

//  pinMode(TEST_LED, OUTPUT);
//  motor.attach(9);
   
  // 연결에 성공했으면 연결확인 메시지와 연결된 네트워크 정보를 띄운다. 
  Serial.println("You're connected to the network");
  Serial.println("-------<network info>-------");
  
  printCurrentNet();
  printWifiData();

  Serial.println("-------<data trans>-------");
}

void loop() 
{
  //모터의 상황을 체크한다.//
  httpRequest_Boiler(jsondata);
  
  Serial.println("Boiler JSON data");
  
  while (client.connected()) {
    if (client.available() && status == WL_CONNECTED) {
      char c = client.read();

      //String에 담아서 원하는 부분만 파싱하도록 함//
      rcvbuf += c;
      
      if(c == '\r'){
        headcount ++; //해더 정보는 생략하기 위해서 설정//
    
        if(headcount != 13){
          rcvbuf = "";
        }
      }

      //데이터 영역/
      if(headcount == 13){
        //JSON파싱//
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(rcvbuf);
        String result = root["result"];
        
        Serial.println(result);
  
        client.stop(); //클라이언트 접속 해제//
        
        rcvbuf = "";
      }
    }
  }

  client.flush();
  client.stop();
}
//////////////////
void httpRequest_Boiler(String jsondata) {
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.flush();
  client.stop();

  delay(1000);
  
  // if there's a successful connection
  if (client.connect(hostIp, 3000)) {
    Serial.println("Connecting...");

    //post data set//
    String postdata = "";
    
    String key_1 = "motor_name=";

    //데이터 유무에 따라 값을 넣어주거나 넣지 않는다.//
    postdata.concat(key_1);
    postdata.concat(motor_name);

    Serial.print("post data [");
    Serial.print(postdata);
    Serial.println("]");

    // send the HTTP POST request
    client.print(F("POST /data"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 192.168.0.4\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/json;charset=UTF-8\r\n"));
    client.print(F("Content-Length: "));
    client.println(jsondata.length());
    client.println();
    client.println(jsondata);
    client.print(F("\r\n\r\n"));
    
    // note the time that the connection was made
    lastConnectionTime = millis();
    getIsConnected = true;

    digitalWrite(TEST_LED, HIGH);
    delay(1000);
    digitalWrite(TEST_LED, LOW);
    delay(1000);
    
  }
  
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    getIsConnected = false;
  }
}
//////////////////
void printWifiData() {
  // WI-FI 실드의 IP를 출력한다.
  IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);
  
  // MAC어드레스를 출력한다.
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void printCurrentNet() {
  // 접속하려는 네트워크의 SSID를 출력한다.
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // 접속하려는 router의 MAC 주소를 출력한다.
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);

  // 수신 신호 강도를 출력한다.
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // 암호화 타입을 출력한다.
//  byte encryption = WiFi.encryptionType();
//  Serial.print("Encryption Type:");
//  Serial.println(encryption,HEX);
//  Serial.println();
}

/*
 * 출처 
 * https://github.com/seochangwook/ArduinoProject/blob/master/wifi_shield_iot_light/wifi_shield_iot_light.ino
 */
