# Bluee

Librería para Arduino
===========================================
Descarga la librería y conecta tus proyectos a diversas plataformas en la nube con **bluee** .

Visita http://wwww.bluee.com.mx para más información.

- Librería para los *blueeBridge*: **Bluee IoT, Bluee32 IoT y Bluee32 IoT Dev**.

# Inicialización

Antes de iniciar, declara un objeto de tipo **Bluee** como variable global:
``` C++
    Bluee bluee;
````
Asigna el puerto de comunicación serial para comunicarse con el dispositivo blueeBridge y la velocidad de comunicación dentro de la función **setup()**.
``` C++
    bluee.init(&Serial); 
    bluee.init(&Serial,115200);
````
Si no se define la velocidad de comunicación, se inicializa a *9600 baudios* por default.

- Puedes utilizar los hardware Serial1, Serial2.. (Arduino Mega o Zero) o SoftwareSerial (serial por software en Arduino Uno o Nano).

Adiciona los callbacks para leer datos y eventos provenientes del dispositivo blueeBridge.
``` C++
    bluee.setApplicationCallback(eventCodeBluee);
    bluee.setRequestCallback(eventRequestDataBluee);
````

- *Las funciones setEventCallback y setDataEventCallback han sido renombradas por setApplicationCallback y setRequestCallback*.

Debes declarar las funciones de los eventos para recibir los datos:

- Función que se dispara al obtener datos de las peticiones realizadas al servidor web.
``` C++
    void eventRequestDataBluee() {
    bluee.getFunction();
    bluee.getData();
    }
````
- Función que se dispara al recibir datos de las diversas aplicaciones que se estén ejecutando en el dispositivo blueeBridge como eventos de conexión wifi, datos del servidor/cliente websocket,  pubnub, mqtt, entre otros. Indica el código de evento de la aplicación que se ha disparado.
``` C++
    void eventCodeBluee(int code) {
	    switch (code){
		    case 100: //do something 1
			    break;
		    case 101: //do something 2
			    break;
		    ...
    }
````

# Manejador de Bluee

Debes agregar la función **handle()** dentro del **loop()** para monitorear los datos provenientes del blueeBridge. *Es requerido para poder utilizar los callbacks.*
``` C++
	void loop() {
	    bluee.handle();
	    ...
	}
````

# Ejecutar una instrucción

Definir una función a ejecutar:
``` C++
     bluee.setFunction("nameOfInstrucction");
````
Definir los parámetros de la función a ejecutar:
``` C++
     bluee.addParam("nameOfParam","value");
````
Adicional la cantidad de parámetros que sean requeridos en la función:
``` C++
     bluee.addParam("nameOfParam1","value1");
     bluee.addParam("nameOfParam2","value2");
     bluee.addParam("nameOfParam3","value3");
     ...
````
Envía al dispositivo blueeBridge, la función a ejecutar. Se puede indicar un *timeout* en milisegundos para esperar una respuesta o *0* para continuar sin importar la respuesta obtenida:
``` C++
     bool response = bluee.send();
     bool response = bluee.send(1000);
     bluee.send(0);
````
- Regresa **true** si la instrucción fue ejecutada correctamente o un **false** en caso contrario y se puede obtener el **código de error**.
``` C++
	if (bluee.send()) {
		Serial.println("OK");
	}   else {
		Serial.println(bluee.getErrorCode());
	}
````

# Respuesta de datos

Los datos recibidos por medio de los *callbacks* o directamente en la respuesta de *bluee.send()*, pueden ser procesados por medio de los siguientes métodos.

## Leer la instrucción recibida

- Lectura del nombre de la instrucción recibida:
``` C++
    String function = bluee.getFunctionAsString();
```
- Comparación de la instrucción recibida, con una específica:
``` C++
    if (bluee.isFunction("nameOfFunctionExpected")){
	    //do something
    }else{
	    //do something else
    }
```
- Permite comparar con funciones específicas para realizar tareas predeterminadas en Arduino.

## Leer parámetros de una instrucción

- Lectura de toda la trama de datos recibida:
``` C++
    String data = bluee.getDataAsString();
```
- Lectura de un parámetro específico como **String**:
``` C++
    String data = bluee.getValueAsString("nameOfParam", positionOfObject);
```
- Lectura de un parámetro específico como **Int**:
``` C++
    int data = bluee.getValueAsInt("nameOfParam", positionOfObject);
```
- Lectura de un parámetro específico como **Boolean**:
``` C++
    bool data = bluee.getValueAsBoolean("nameOfParam", positionOfObject);
```
- Lectura de un parámetro específico como **Long**:
``` C++
    long data = bluee.getValueAsLong("nameOfParam", positionOfObject);
```
- Lectura de un parámetro específico como **Double**:
``` C++
    double data = bluee.getValueAsDouble("nameOfParam", positionOfObject);
```
Nota: **positionOfObject**: posición del objeto para leer sus parámetros.

# Formato JSON

El traspaso de datos con las plataformas en la nube requieren sea en formato **JSON**. Bluee soporta enviar y recibir datos en este formado. Puedes utilizar la librería [ArduinoJson] para crearlos y leer valores provenientes de este formato.

- Puedes leer acerca de este formato en [JSON].

## Agregar un JSON como valor de un parámetro:

Utilizando la librería de *ArduinoJson*, crear un JSON:
``` C++
	String dataJson;  
	StaticJsonDocument<200> doc;
	doc["nameOfValue"] = value
	serializeJson(doc, dataJson);
```
JSON output:
``` C++
    { "nameOfValue" : value }
```
Agregar JSON como parámetro:
``` C++
	bluee.setFunction("nameOfFunction");
	bluee.addParam("nameOfParam", dataJson);
```

# Bluee32 IoT Dev 

- Utiliza el **Serial1** para comunicarte con Bluee32.

``` C++
    bluee.init(&Serial1,115200);
```

- Utiliza el mapeo de pines: **P1, P2, P3, P4, P5, P6, P7, P8, P9 y P10** para controlar los pines de entrada y salida de la tarjeta *Bluee32 IoT Dev*.

``` C++
    pinMode(P1,OUTPUT);
    pinMode(P2,INPUT);
    ...
    digitalWrite(P1,HIGH);
```
# Documentación

Revisa la [documentación] de tu dispositivo blueeBridge para más información.
 
  [documentación]: <https://bluee.com.mx/pages/ecosistema/documentacion>
  
[ArduinoJson]: <https://arduinojson.org/>

[JSON]: <https://www.json.org/json-es.html>