#include <SoftwareSerial.h>
#include "Bluee.h"
#include <ArduinoJson.h>
        //             Rx Tx
SoftwareSerial serial2(2, 3);
Bluee bluee;
void setup() {
  
     Serial.begin(57600);
     while (!Serial);
     serial2.begin(9600);
     bluee.init(&serial2);
     bluee.setEventCallback(eventBluee);
     pinMode(11,OUTPUT); 
     digitalWrite(11,HIGH); 
     
}

void loop() {

	bluee.handle();
	 
}

void eventBluee(int code) {

  if(code == 116){
    
    StaticJsonDocument<500> doc;
    String datos= bluee.getValueAsString("data");
    Serial.println(datos);
    DeserializationError error = deserializeJson(doc,datos);   
    if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		return;
    }
    String comando= doc["comando"]; 
    Serial.println(comando);
    if(comando.equals("on")){
		digitalWrite(11,LOW); 
    } else if(comando.equals("off")){
		digitalWrite(11,HIGH); 
    }
  }
  
}

