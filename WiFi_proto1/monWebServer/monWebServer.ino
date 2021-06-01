#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "WiFiEsp.h"
#include <SPI.h>
#include <Adafruit_PN532.h>

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(2, 3); // RX, TX
#endif

#define PN532_IRQ   (4)
#define PN532_RESET (5)
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);


int i =0;
char ssid[] = "";            // your network SSID (name)
char pass[] = "";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
int reqCount = 0;                // number of requests received
boolean getIsConnected = false;
unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in milliseconds
//서버의 정보//
IPAddress hostIp(0, 0, 0, 0);
// Initialize the Ethernet client object
WiFiEspClient client;// Initialize the Ethernet client object


int RFID_Count = 0;
boolean success;
String ID = "";
String data = "";
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup()
{
  // initialize the LCD
  lcd.init();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Hello, world!");

  
  Serial.begin(115200);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero
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

  
  nfc.begin();
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
//  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
//  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
//  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
}

void loop()
{
  if(i == 0){
    data = RFID();
    i++;
    ID = "";
  }else{
    if (i < 2){
      httpRequest_RFID(data);
      i++;
    } else i = 0;
      
    
  }
  
  
//    
//    if(i == 1){
//      data = RFID(), i = 0, ID = "";
//    }
//    i++;
}
  
void httpRequest_RFID(String rfid) {
  Serial.println();
  
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.flush();
  client.stop();

  delay(1000);
  
  // if there's a successful connection
  if (client.connect(hostIp, 80)) {
    Serial.println("Connecting...");
    
    //post data set//
    String postdata = "";
    
    //데이터 유무에 따라 값을 넣어주거나 넣지 않는다.//
    postdata.concat(rfid);

    Serial.print("post data [");
    Serial.print(postdata);
    Serial.println("]");

    // send the HTTP POST request
    client.print(F("POST /data"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 0.0.0.0\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/x-www-form-urlencoded\r\n"));
    client.print(F("Content-Length: "));
    client.println(postdata.length());
    client.println();
    client.println(postdata);
    client.print(F("\r\n\r\n"));
    
    // note the time that the connection was made
    lastConnectionTime = millis();
    getIsConnected = true;
    delay(10000);
  }
  
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    getIsConnected = false;
  }
}


String RFID(){
  Serial.println("Waiting for an ISO14443A card");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      //Serial.print(" 0x"); Serial.print(uid[i], HEX);
      ID = ID + String(uid[i], HEX);
    }
    Serial.println("");
    Serial.println(ID);
    return ID;
  // Wait 1 second before continuing
  delay(1000);
  }
  else
  {
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out waiting for a card");
  }
}
/*
 * 아두이노 예제 소스
 */
