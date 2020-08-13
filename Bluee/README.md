# Bluee
Librería para Arduino
===========================================
Descarga la librería y conecta tus proyectos a la nube con bluee. Visita http://wwww.bluee.com.mx para más información.

# Inicialización

Declara un objeto Bluee:
``` C++
    Bluee bluee;
````
Asigna el puerto de comunicación serial para comunicarse con el dispositivo blueeBridge dentro de la función **loop()**.
``` C++
    bluee.init(&serial2);
````
Adiciona los callbacks para leer datos y eventos provenientes del dispositvo blueeBridge.
``` C++
    bluee.setEventCallback(eventBluee);
    bluee.setDataEventCallback(eventDataBluee);
````
Debes declarar las funciones de eventos:

- Funcion que obtendrá datos de las peticiones realizadas al servidor web.
``` C++
    void eventDataBluee() {
    }
````
- Funcion que obtendrá los eventos que suceden en el dispositivo blueeBridge como eventos de conexión wifi, datos del servidor/cliente websocket,  pubnub, entre otros.
``` C++
    void eventBluee(int code) {
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
Añadir cuantos sea requeridos en la función:
``` C++
     bluee.addParam("nameOfParam1","value1");
     bluee.addParam("nameOfParam2","value2");
     bluee.addParam("nameOfParam3","value3");
     ...
````
Enviar la función a ejecutar:
``` C++
     bool response = bluee.send();
````
- Regresa **true** si la instrucción fue ejecutada correctamente o un **false** en caso contrario y se puede obtener el **código de error**.
``` C++
	if (bluee.send()) {
		Serial.println("OK");
	}   else {
		Serial.println(bluee.getErrorCode());
	}
````

# Leer parámetros de una instrucción

- Lectura de toda la trama de datos:
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

# Agregar un JSON como valor de un parámetro:

- Utilizando la librería de ArduinoJson:
``` C++
	StaticJsonDocument<200> doc;
	doc["nameOfValue"] = value
	String dataJson;  
	serializeJson(doc, dataJson);
	bluee.setFunction("nameOfFunction");
	bluee.addParam("nameOfParam", dataJson);
```

# Documentación

Revisa la [documentación] de tu dispositivo blueeBridge para más información.
 
  [documentación]: <https://www.bluee.com.mx/documentacion/>