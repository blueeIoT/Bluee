/*
 Name:		Bluee.h
 Created:	11/02/2020 22:32:33
 Author:	bluee
*/

#ifndef _Bluee_h
#define _Bluee_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//#define DEBUG

//TAMA�O DE DATOS
/*********************************************/
#define CONSTANT_SIZE_PROTOCOL_BUFFER	        3072
#define CONSTANT_SIZE_PROTOCOL_DATA				CONSTANT_SIZE_PROTOCOL_BUFFER - 128
/*********************************************/

//ERRORES GENERALES
/*********************************************/
#define NONE							-1
#define OK								0
#define ERROR_PROTOCOL                  1
#define ERROR_TIME_OUT           		2
#define ERROR_SIZE_EXCEED_ALLOWED		10
#define ERROR_RESERVING_RAM_MEMORY		21
#define ERROR_INVALID_PROTOCOL			22
#define ERROR_DATA_PARAM_NOT_FOUND		31
#define ERROR_DATA_ELEMENT_NOT_EXIST	33
#define ERROR_DATA_SIZE					34
/*********************************************/

//MAQUINA DE ESTADOS DEL PROTOCOLO
/*********************************************/
#define STEP_WAKEUP                     0
#define STEP_START_INSTRUCTION			1
#define STEP_START_FUNCTION				2
#define STEP_GET_SIZE					3
#define STEP_END_SIZE					4
#define STEP_START_PARAMS				5
#define STEP_RECEIVING					6
#define STEP_RECEIVING_WITH_SIZE		7
#define STEP_END_RECEIVING_WITH_SIZE	8
#define STEP_END_PARAMS					9
#define STEP_END_INSTRUCTION			10
/*********************************************/

//DEFINICIONES DEL PROTOCOLO
/*********************************************/
#define DATA_START_PROTOCOL				'*'
#define DATA_START_INSTRUCTION			'<'
#define DATA_START_SIZE					'+'
#define DATA_END_SIZE					'='
#define DATA_START_PARAMS				'{'
#define DATA_END_PARAMS					'}'
#define DATA_END_INSTRUCTION			'>'
#define DATA_END_PROTOCOL				'*' 
/*********************************************/

//MAPEO DE PUERTOS - BLUEE32S3 DEV IOT - ARDUINO ZERO
/*********************************************/
//PINES IO
#define P1				                19  //I/O, ADC
#define P2				                25  //I/O, ADC
#define P3				                14  //I/O, ADC, PWM, DAC
#define P4				                15  //I/O, ADC, PWM
#define P5				                16  //I/O, ADC
#define P6				                17  //I/O, ADC
#define P7				                34  //I/O, PWM
#define P8				                36  //I/O, PWM
#define P9				                37  //I/O, PWM
#define P10				                11  //I/O, PWM
//SPI
#define SS				                4
#define MI			                    22
#define MO			                    23
#define CK			                    24
//I2C
#define SDA                             20
#define SCL                             21

//INTERFAZ BLUEE32s3
#define RST                             3
#define BSY                             9
/*********************************************/

/*****************DATABUFFER******************/
class DataBuffer {

public:
    DataBuffer();
    ~DataBuffer();
    DataBuffer(const char* pSource);
    void init();
    void clear();
    void addData(char newData);
    void addData(const char* pNewData);
    void addData(String& pNewData);
    void addData(int iValue);
    void addByte(byte bValue);
    void addData(const char* pNewData, int length);
    void addData(DataBuffer& pNewData);
    void setData(int iValue);
    void setData(String& sValue);
    void setData(const char* pSource);
    void setData(const char* pSource, int size);
    void setDataByOffset(const char* pSource, int offset, int size);
    void set(char data, int position);
    void setCopyTo(DataBuffer& db);
    void removeLast();
    bool resize(int newSize);
    bool increase(int addSize);
    bool hasErrorMemory();
    int getSize();
    int getLastCharacterIndex(char character); 
    char get(int position);
    char* getData();
    String getString();

private:
    char* pData;
    int size;
    bool error;
};
/*********************************************/

/*****************BLUEE CLASS*****************/
class Bluee {

public:

    /*********************************************/
    typedef void (*BlueeEvent) (int code);
    typedef void (*BlueeDataEvent)(void);
    void setEventCallback(BlueeEvent pEvent);
    void setDataEventCallback(BlueeDataEvent pEvent);
    void setApplicationCallback(BlueeEvent pEvent);
    void setRequestCallback(BlueeDataEvent pEvent);
    /*********************************************/

    void init(Stream * COM);
    void setFunction(const char * _pFunction);
    void setResponse(const char* _pResponse);
    void setData(const char* _pData);

    void handle();
    void addParam(const char* pParam, bool bValue);
    void addParam(const char* pParam, int iValue);
    void addParam(const char* pParam, size_t sValue);
    void addParam(const char* pParam, DataBuffer& pValue);
    void addParam(const char* pParam, String& sValue);
    void addParam(const char* pParam, const char* pValue);
    void addParam(const char* pParam, double value);
    void addParam(const char* pParam, const char* pValue, int sizeValue);
    void addDataOnBuffer(DataBuffer& pData);
    void addDataOnBuffer(const char* pData);
    void addDataOnBuffer(String& pData);
    void addDataOnBuffer(int iValue);
    void addDataOnBuffer(bool bValue);
    void addDataOnBuffer(const char* cData, int sizeData);
    bool getValue(const char* pParam, DataBuffer& pValue, int pos = 0);
    bool getValueAsBoolean(const char* pParam, int pos);
    bool send(int timeout = 5000);
    int getErrorCode();
    int toInt(char* pData);
    int toIntFromHexValue(char* pData);
    int getValueAsInt(const char* pParam, int pos = 0);
    unsigned long toULong(char* pData);
    long toLong(char* pData);
    long getValueAsLong(const char* pParam, int pos = 0);
    double toDouble(char* pData);
    double getValueAsDouble(const char* pParam, int pos);
    String getValueAsString(const char* pParam, int pos = 0);
    void emptyBufferRx();
    bool hasErrorMemory();
    DataBuffer & getFunction();
	DataBuffer & getData();
    String getDataAsString();
    String getFunctionAsString();
    bool isFunction(String function);

private:

    void clear();
    void sendBuffer();
   
    void addParamSeparator();
    void addObjectSeparator();
    void freeMem(char* pBuffer);
    void toCharArray(int value, char* pData);
    void removeObject(int pos, DataBuffer& pData);
    void addSizeParam(const char* pValue, int sizeValue);
    char* reserveMem(int sizeBuffer);    
    bool checkReceived(int timeOut = 5000);
	bool checkDataIncoming(); 
    bool compareCharBuffers(const char* buff1, char* buff2);    
    int getObject(int pos, DataBuffer& pResultData, DataBuffer& pSourceData);
    int getValueSplit(const char* pParam, DataBuffer& pResultData, DataBuffer& pSourceData);
    int getElementSplit(int numElement, DataBuffer& pResultData, DataBuffer& pSourceData);   
    unsigned long getSizeCharPointer(const char* data);
       
    byte stepsProtocol;
    char auxData;
    int errorCode;
    int countRx;
    int sizeRX;
    int pinBusy;
    bool isWithSize;
	bool isEndReceiving;
    int timeOutRx;
	
    Stream * comBluee; 
    DataBuffer pBuffer;
	DataBuffer pFunction;
	DataBuffer pData;
	BlueeEvent _pEvent;
    BlueeDataEvent _pDataEvent;

protected:
    virtual void runEvent(int code) {
        if (_pEvent) {
            _pEvent(code);
        }
    }
    virtual void runDataEvent() {
        if (_pDataEvent) {
            _pDataEvent();
        }
    }
};
/*********************************************/

#endif

