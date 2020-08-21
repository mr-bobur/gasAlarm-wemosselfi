#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
const uint16_t kRecvPin = D6;
IRrecv irrecv(kRecvPin);

decode_results results;

unsigned long time1, time2;
int        mode1         = 0;
int        k             = 0;
int        userNum       = 0;
int        closetime     = 10;
long       autocloseTime = 10001;
int        autoClose     = 0;
int        runCount      = 0;
String     password      = ""; //for wifi
String     clientid      = ""; //for identcation client
String     s             = ""; //for server
String     request       = ""; //for server
String     Carray[30];
bool       identic       = false;
bool       statusgate    = false;
IPAddress  accessPointIP = IPAddress(192, 168, 1, 1);
WiFiServer server(80);
WiFiClient client;

//   joset["CloseTime"] = closetime = joset["CloseTime"] | closetime;
//  joset["users"] = userNum = joset["users"] | userNum;
//  Serial.println( password);

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  EEPROM.begin(512);
  Serial.println( );
  Serial.println( "Assalomu Alekum Tizim Ishga tushdi!!!" );
  irrecv.enableIRIn();  // Start the receiver

   pinMode(D2,OUTPUT);
   pinMode(D3,OUTPUT);
   pinMode(D4,OUTPUT);
   digitalWrite(D2,HIGH); 
   digitalWrite(D3,HIGH);
   digitalWrite(D4  ,HIGH); 
      
   
  SPIFFS.exists("/conff.json");
  File file = SPIFFS.open("/conff.json", "r");
  size_t size = file.size();
  char filebuffer[size];
  file.readBytes(filebuffer, size);
  DynamicJsonDocument jsonDocument(1024);
  deserializeJson(jsonDocument, filebuffer);
  serializeJsonPretty(jsonDocument, Serial);
  file.close();

  // initialization congif
  //jsonDocument["wifiPass"]  = password  = jsonDocument["wifiPass"]  | password;
  password = "12312312";
    Carray[0] = "353757095823512";
    Carray[1] = "123456787654323";
    Carray[2] = "234567654323455";
  
  autoClose = 2;//jsonDocument["closeTime"];
  userNum = 3;
  
  ///jsonDocument["UserNum"];
//  for (int i = 0 ; i < userNum; i++) {
//    String temp = jsonDocument["users"][i];
//    Carray[i] = temp;
//    Serial.print("ID:");
//    Serial.println( Carray[i]);
//  }


  WiFi.mode(WIFI_STA);
  WiFi.softAPConfig(accessPointIP, accessPointIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("B_QUANTUM", password);
  server.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // chala xolat uchun

  int xolat  = EEPROM.read(220);
  if (xolat == 221) {
    autocloseTime  = EEPROM.read(210) * 100;
    int xolat12  = EEPROM.read(200);
    if (xolat12 == 111) {
      statusgate = true;
    }
    if (xolat12 == 222) {
      statusgate = false;
    }
  }
  //  data.add("353757095823510");
}
void loop() {
  if (time2 + 80 < millis()) {
    time2 = millis();
      if (irrecv.decode(&results)) {
    serialPrintUint64(results.value);
    Serial.println("");
    if(results.value == 16195807){
      statusgate = 1;
        autocloseTime = 0;
      }
    if(results.value == 16212127){
      statusgate = 0;
        autocloseTime = 0;
      }
    irrecv.resume(); 
  }



    client = server.available();
    if (!client) {
      return ;
    }
    request = client.readStringUntil('\n') + "\r\n";//IP ni oladi
    request = client.readStringUntil('\n') + "\r\n";//head json akanligi
    request = client.readStringUntil('\n') + "\r\n";//general body
    request = request.substring(7);
    DynamicJsonDocument currentjsonDocument(1024);
    DeserializationError jsonError = deserializeJson(currentjsonDocument, request);
    if (!jsonError) {
      serializeJson(currentjsonDocument, request);
      JsonObject joset = currentjsonDocument.as<JsonObject>();
      joset["mode"]     = mode1         = joset["mode"]     | mode1;
      String respBody;
      DynamicJsonDocument resp(1024);
      joset["myid"] = clientid = joset["myid"] | clientid;
      if (mode1 == 1) {
        for (int i = 0 ; i < userNum; i++) {
          if ( Carray[i] == clientid) {
            identic = true;
            Serial.print("Successed Id -> ");
            Serial.println(clientid);
          }
        }
        statusgate = joset["status"];
        autocloseTime = 0;
        if (identic) {
          resp["identication"] = "access";
        } else {
          resp["identication"] = "fail";
        }
        resp["time"] = autoClose;
        if (statusgate) {
          resp["status"] = "open";
        } else {
          resp["status"] = "close";
        }
        serializeJson(resp, respBody);
         resp.clear();
      }
      if (mode1 == 4) {
        // reInstall configurate;
        joset["wifiPass"]  = password  = joset["wifiPass"]  | password;
        autoClose = joset["closeTime"];
        userNum = currentjsonDocument["UserNum"];
        for (int i = 0 ; i < userNum; i++) {
          String temp = joset["users"][i];
          Carray[i] = temp;
          Serial.print("ID:");
          Serial.println( Carray[i]);
        }
        File file;
        String stringBuffer;
        DynamicJsonDocument jsonbek(1024);
        SPIFFS.exists("/conff.json");
        file = SPIFFS.open("/conff.json", "w");
        for (auto kvp : currentjsonDocument.as<JsonObject>()) {
          jsonbek[kvp.key()] = kvp.value();
        }
        serializeJson(jsonbek, file);
        file.close();
        //serializeJsonPretty(currentjsonDocument, Serial);
      }

      if (mode1 == 2) {
        String reader  = joset["reader"];
        for (int i = 0 ; i < userNum; i++) {
          if ( Carray[i] == clientid) {
            identic = true;
            Serial.print("Successed Id -> ");
            Serial.println(clientid);
          }
        }
        if (identic) {
          resp["identication"] = "access";
        } else {
          resp["identication"] = "fail";
        }
        resp["time"] = autoClose;
        if (statusgate) {
          resp["status"] = "open";
        } else {
          resp["status"] = "close";
        }
        if (reader != "reader") {
          joset["status"] = statusgate = joset["status"] | statusgate;
        }
        serializeJson(resp, respBody);
         resp.clear();

      }

      client.flush();
      s = "HTTP/1.1 200 OK\r\n";
      s += "Content-Type: application/json\r\n\r\n";
      s += respBody;
      s += "\r\n\n";
      client.print(s);
    }
  }
  if (time1 + 100 < millis() && autocloseTime <= autoClose * 1000) {
    time1 = millis();
    
    autocloseTime += 100;

    if (statusgate==1) {
      EEPROM.write(200, 111);
      EEPROM.write(210, autocloseTime / 100);
      digitalWrite(D2,LOW);  
      digitalWrite(D3,HIGH);
      digitalWrite(D4,HIGH);
      Serial.print("Time ------> : ");Serial.print(autocloseTime);Serial.println("  status ------> Closing");

    } else {
      EEPROM.write(200, 222);
      EEPROM.write(210, autocloseTime / 100);
      digitalWrite(D2,LOW);  
      digitalWrite(D3,LOW);
      digitalWrite(D4,LOW);
      
      Serial.print("Time ------> : ");Serial.print(autocloseTime);Serial.print("Job Status ------> : ");Serial.print(autocloseTime);Serial.println("  status ------> Opening");
    }
    if (autocloseTime < closetime*1000) {
      EEPROM.write(220, 220);
      Serial.print("unComplead");
    } else {
      EEPROM.write(220, 221);
      digitalWrite(D2,HIGH); 
      Serial.print("Complead");
      
    }
//  
//    if (digitalRead(LED_BUILTIN)) {
//      digitalWrite(LED_BUILTIN, LOW);
//    } else {
//      digitalWrite(LED_BUILTIN, HIGH);
//    }
    EEPROM.commit();
  }
}
