#include <SoftwareSerial.h>
#include "Bluee.h"
        
SoftwareSerial serial2(2, 3);
Bluee bluee;
void setup() {

    Serial.begin(57600);
    while (!Serial);
    serial2.begin(9600);
    
    pinMode(11,OUTPUT); 
    pinMode(10,OUTPUT); 
    pinMode(9,OUTPUT); 

    analogWrite(11,0); 
    analogWrite(10,0); 
    analogWrite(9,0);
    
    bluee.init(&serial2);
    bluee.setDataEventCallback(eventDataBluee); 
	
}
void loop() {

 bluee.handle();
 
}


void eventDataBluee() {
   Serial.println("funcion");
   Serial.println(bluee.getFunction().getString()); 
   Serial.println("datos");
   Serial.println(bluee.getData().getString()); 

 if (bluee.getFunction().getString().equals("rgb"))
  {
    int r = bluee.getValueAsInt("r");
    Serial.println(r);
    analogWrite(11,r); 
    int g = bluee.getValueAsInt("g");
    Serial.println(g);
    analogWrite(10,g);
    int b = bluee.getValueAsInt("b");
    Serial.println(b);
    analogWrite(9,b);
    bluee.setFunction("ok"); 
    bluee.addParam("RGB","OK"); 
    bluee.send(0);
  }else if(bluee.getFunction().getString().equals("off"))
  {
    analogWrite(11,0);
    analogWrite(10,0);
    analogWrite(9,0);
    bluee.setFunction("ok"); 
    bluee.addParam("RGB","off"); 
    bluee.send(0);
  }
}
