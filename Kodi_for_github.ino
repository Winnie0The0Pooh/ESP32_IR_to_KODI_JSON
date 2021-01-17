/*
 * MIT License

Copyright (c) 2021 Winnie0The0Pooh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


/*
 * IRremote: IRreceiveDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Initially coded 2009 Ken Shirriff http://www.righto.com/
 */

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <IRremote.h>

#define USE_SERIAL Serial

static uint8_t LED = 2;

WiFiMulti wifiMulti;

#if defined(ESP32)
int IR_RECEIVE_PIN = 15;
#elif defined(ARDUINO_AVR_PROMICRO)
int IR_RECEIVE_PIN = 10;
#else
int IR_RECEIVE_PIN = 11;
#endif

IRrecv IrReceiver(IR_RECEIVE_PIN);

// On the Zero and others we switch explicitly to SerialUSB
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

void setup() {
    pinMode(LED, OUTPUT);
    digitalWrite(LED,HIGH);

    Serial.begin(115200);
    delay(1000);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)
    delay(2000); // To be able to connect Serial monitor after reset and before first printout
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__));

    
        wifiMulti.addAP("Your AP name", "Your WiFi pass");

    // In case the interrupt driver crashes on setup, give a clue
    // to the user what's going on.
    Serial.println("Enabling IRin");
    IrReceiver.enableIRIn();  // Start the receiver
    IrReceiver.blink13(true); // Enable feedback LED doesn't work on esp32

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);
    digitalWrite(LED,LOW);
}

unsigned long last = millis();

void loop() {
    if (IrReceiver.decode()) {
        
        IrReceiver.printResultShort(&Serial);
        Serial.println();
        Serial.println(IrReceiver.results.value, HEX);
        uint32_t mvalue;
        mvalue = IrReceiver.results.value;
        uint32_t mask=0xFFFF;
        uint32_t dvalue;
        dvalue = mvalue & mask;
        Serial.println(dvalue, HEX);

        if(dvalue == 0x273F){
          Serial.println("Stop");
          kodi(1);
        }
        if(dvalue == 0x273E){
          Serial.println("Play");
          kodi(2);
        }
        
        if(dvalue == 0x6DF2){
          Serial.println("Next track");
          kodi(3);
        }

        if(dvalue == 0xF10E){
          Serial.println("Previous track");
          kodi(4);
        }

        if(dvalue == 0x276F){
          Serial.println("Yellow");
          svet(1);
        }
        if(dvalue == 0x2770){
          Serial.println("Blue");
          svet(2);
        }
        if(dvalue == 0x276E){
          Serial.println("Green");
          svet(3);
        }
        if(dvalue == 0x276D){
          Serial.println("Red");
          svet(4);
        }

        IrReceiver.resume(); // Receive the next value
    }
    delay(100);
}

void kodi(uint8_t command){

digitalWrite(LED,HIGH);

        String msg = "";

//        msg = "{ 'jsonrpc': '2.0', 'method': 'JSONRPC.Introspect', 'params': { 'filter': { 'id': 'AudioLibrary.GetAlbums', 'type': 'method' } }, 'id': 1 }";

char c1;
char c2;

c1=0x22; //"
c2=0x27; //'

//  xbmc("Player.Seek","playerid\":1,\"value\":\"smallbackward\"");
//  xbmc("Player.Seek","playerid\":1,\"value\":\"smallforward\"");
//  xbmc("Player.Seek","playerid\":1,\"value\":\"bigbackward\"");
//  xbmc("Player.Seek","playerid\":1,\"value\":\"bigforward\"");
  
    switch (command) {
  case 1:
    msg = "{'jsonrpc': '2.0','method': 'Player.Stop', 'params': { 'playerid': 1 }, 'id': 1}";
    break;
  case 2:
    msg = "{'jsonrpc': '2.0','method': 'Player.PlayPause', 'params': { 'playerid': 1 }, 'id': 1}";
    break;
  case 3:
    msg = "{'jsonrpc': '2.0','method': 'Player.GoTo', 'params': { 'playerid': 1, 'to':'next'}, 'id': 1}";
//    {«jsonrpc»:«2.0»,«id»:«1»,«method»:«Player.GoTo»,«params»:{«playerid»:1,«to»:«next»}}
    break;
  case 4:
    msg = "{'jsonrpc': '2.0','method': 'Player.GoTo', 'params': { 'playerid': 1, 'to':'previous'}, 'id': 1}";
    break;

  default:
    msg = "{ 'jsonrpc': '2.0', 'method': 'JSONRPC.Introspect', 'params': { 'filter': { 'id': 'AudioLibrary.GetAlbums', 'type': 'method' } }, 'id': 1 }";
  break;
} 

        msg.replace(c2, c1);

      if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url


        http.begin("http://kodi:kodi@192.168.1.65:8080/jsonrpc");

        /*
          // or
          http.begin("http://192.168.1.12/test.html");
          http.setAuthorization("user", "password");

          // or
          http.begin("http://192.168.1.12/test.html");
          http.setAuthorization("dXNlcjpwYXN3b3Jk");
         */

        http.addHeader("Content-Type", "application/json"); //text/json

        USE_SERIAL.print("[HTTP] POST...\n");
        USE_SERIAL.println(msg);
        // start connection and send HTTP header
        int httpCode = http.POST(msg);

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

  String response = http.getString();                       //Get the response to the request
  
//    Serial.println(httpCode);   //Print return code
    Serial.print("response: ");
    Serial.println(response);           //Print request answer
  
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    digitalWrite(LED,LOW);
}

void svet(uint8_t command){

digitalWrite(LED,HIGH);

  String msg = "http://192.168.1.74/Light/Control/";

  switch (command) {
  case 1: //Yellow btn
    msg = msg + "bigroom/togglefirst";
    break;
  case 2: //Blue btn
    msg = msg + "bigroom/togglesecond";
    break;
  case 3: //Green btn
    msg = msg + "bigroom/bothon";
    break;
  case 4: //Red btn
    msg = msg + "bigroom/bothoff";
    break;

  default:
    msg = msg + "/bigroom/togglefirst";
  break;
} 

      if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url


        http.begin(msg);


        USE_SERIAL.print("[HTTP] POST...\n");
        USE_SERIAL.println(msg);
        // start connection and send HTTP header
        int httpCode = http.GET(); //http.POST(msg);

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

  String response = http.getString();                       //Get the response to the request
  
//    Serial.println(httpCode);   //Print return code
    Serial.print("response: ");
    Serial.println(response);           //Print request answer
  
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    digitalWrite(LED,LOW);
}
