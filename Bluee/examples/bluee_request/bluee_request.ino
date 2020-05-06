#include <SoftwareSerial.h>
#include "Bluee.h"

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
	  
		char url[100];
		char* host = "http://example.com";
		char* var1 = "temperatura";
		char* var2 = "voltaje";
		int val1 = 4;
		int val2 = 10;
		sprintf(url, "%s?%s=%d&%s=%d", host, var1, analogRead(A0), var2, analogRead(A0));
		
		bluee.setFunction("request");
		bluee.addParam("host", url);
		Serial.println(url);
		bluee.send();

	}
    delay(1);
	
}



