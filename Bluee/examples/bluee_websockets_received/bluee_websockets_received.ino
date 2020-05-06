#include "PWMServo.h"
#include <SoftwareSerial.h>
#include "Bluee.h"
#include <ArduinoJson.h>
        //             Rx Tx
SoftwareSerial serial2(2, 3);
Bluee bluee;

// Declaramos la variable para controlar el servo
PWMServo  servoMotorb;
PWMServo  servoMotorp; 

void setup() {
    Serial.begin(57600);
    while (!Serial);
    serial2.begin(9600);
 
    // Iniciamos el servo para que empiece a trabajar con el pin 9
    servoMotorb.attach(9);
    servoMotorp.attach(10);
    delay(500);
    servoMotorb.write(180);
    delay(500);
    servoMotorp.write(0);

    bluee.init(&serial2);
    bluee.setEventCallback(eventBluee);  
  
}
 
void loop() {
   bluee.handle(); 
}

void eventBluee(int code) {
  if(code == 95){
    if (bluee.getValueAsString("data").equals("carga")){
      servoMotorb.write(80);
    }else if (bluee.getValueAsString("data").equals("dispara")){
      servoMotorb.write(180); 
      delay(500);
      servoMotorp.write(0);
    }else{ 
      StaticJsonDocument<200> doc;
      String valor=bluee.getValueAsString("data");
      DeserializationError error = deserializeJson(doc, valor);
      if (error) {
		  Serial.print(F("deserializeJson() failed: "));
		  Serial.println(error.c_str());
		  return;
      }
      const char* funcion = doc["servo"];
      int fuerza= doc["fuerza"]; 
      servoMotorp.write(fuerza);
      servoMotorb.write(80);
      }
    }

}
