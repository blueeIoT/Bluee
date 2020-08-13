#include <SoftwareSerial.h>
#include "Bluee.h"
#include <ArduinoJson.h>

int count=0;
SoftwareSerial serial2(2, 3);
Bluee bluee;

void setup() {

     Serial.begin(57600);
     while (!Serial);
     serial2.begin(9600);
     bluee.init(&serial2);

}

void loop() {
	bluee.handle();
    if (count-- == 0) {
        count = 2000;
		StaticJsonDocument<200> doc;
		doc["sensor"] = analogRead(A0);

		String valor;  
		serializeJson(doc, valor);
		//envio a pubnub
		bluee.setFunction("publish");
		bluee.addParam("data",valor);
		Serial.println(bluee.getValueAsString("data"));
		bluee.send(0);		
    }
	delay(1);
}
