
/*
 Name:		BlueeArduino.ino
 Created:	11/02/2020 22:31:55
 Author:	bluee
*/


/**********************************************
ENCIENDE Y APAGA UN LED DESDE UN NAVEGADOR WEB
http://192.168.1.10/setLed?value=0 -> APAGAR
http://192.168.1.10/setLed?value=1 -> PRENDER
**********************************************/

//AGREGAMOS LAS LIBRERIAS
#include <SoftwareSerial.h>
#include <Bluee.h>

SoftwareSerial serial2(2, 3);
Bluee bluee;

void setup() {
    //LED A ENCENDER
    pinMode(13, OUTPUT);
    
    //CONEXION SERIAL CON EL MONITOR SERIAL DE ARDUINO
    Serial.begin(9600);
    while (!Serial);
    
    //CONEXIÓN SERIAL CON BLUEEBRIDGE
    serial2.begin(9600);
    
    //INICIACION DE BLUEE
    bluee.init(&serial2);

    //CALLBACKS
    bluee.setEventCallback(eventBluee);
    bluee.setDataEventCallback(eventDataBluee);
}

void loop() {
    //MANEJADOR DE BLUEE
    bluee.handle();
    delay(1);
}

void eventDataBluee() {
    
    //LEEMOS LA INSTRUCCION DE LA URL -> setLed
    String function = bluee.getFunction().getString();
    if (function.equals("setLed")) {
        //OBTENEMOS EL VALOR DE VALUE EN FORMATO INT
        int value = bluee.getData.getValueAsInt("value");
        //ENCENDEMOS O APAGAMOS LED
        digitalWrite(13, value);
    
        //RESPONDEMOS LA PETICION DEL NAVEGADOR CON UN OK Y EL VALOR DEL LED ASIGNADO
        bluee.setFunction("OK");
        bluee.addParam("value", value);
    }else{
        //NO ES LA FUNCION ESPERADA, RESPONDEMOS CON UN WR (WRONG)
         bluee.setFunction("WR");
         bluee.addParam("resultado", "funcion no reconocida");
    }
    bluee.send(0);
}

void eventBluee(int code) {
   
   
}