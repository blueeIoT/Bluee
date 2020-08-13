#include <SoftwareSerial.h>
#include "Bluee.h"
#include <ArduinoJson.h>
        //             Rx Tx
SoftwareSerial serial2(2, 3);
Bluee bluee;
int steps=0;
int fuerza=0;

void setup() {
  
    Serial.begin(57600);
    while (!Serial);
    serial2.begin(9600);
    bluee.init(&serial2);

    pinMode(12,OUTPUT);
    pinMode(11,OUTPUT);
    pinMode(10,OUTPUT);
    digitalWrite(12,LOW);
    digitalWrite(11,LOW);
    digitalWrite(10,LOW);
 
}


void loop() {
 
bluee.handle();
  char carga[50];
  if (digitalRead(13)==0){
  
    switch(steps){
      case 0:
        //iniciar, mandamos carga
        bluee.setFunction("sendWsc");
        bluee.addParam("data","carga");
        bluee.send(0);
        Serial.println("carga");
        digitalWrite(10,HIGH);
   
        fuerza=0;
        steps++;
        delay(100);
        break;
		
      case 1:
          fuerza=fuerza+5;
          if (fuerza > 180){
            fuerza = 180;
            digitalWrite(12,HIGH);
          }
          StaticJsonDocument<200> doc;
          doc["funcion"] = "servo";
          doc["fuerza"] = fuerza;
          String carga;
          serializeJson(doc,carga);
          digitalWrite(11,HIGH);
          Serial.println(fuerza);
          bluee.setFunction("sendWsc");
          bluee.addParam("data",carga);
          bluee.send(0);
          break;
    }
}else{
    if (steps == 1){
        
        Serial.println("dispara");
        bluee.setFunction("sendWsc");
        bluee.addParam("data","dispara");
        bluee.send(0);
        steps=0;
        fuerza=0;

        delay(200); 
        digitalWrite(12,LOW);
        digitalWrite(11,LOW);
        digitalWrite(10,LOW);
        
    }
    delay(1000);
  }
  delay(100);
  
 }

