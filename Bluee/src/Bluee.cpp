/*
 Name:		Bluee.cpp
 Created:	11/02/2020 22:32:33
 Author:	bluee
*/

#include "Bluee.h"

void Bluee::init(Stream* COM) {
	comBluee = COM;
	comBluee->flush();
	clear();
}

bool Bluee::checkReceived(int timeOut) {
	char inData = '\0';
	stepsProtocol = STEP_WAKEUP;
	while (timeOut-- >= 0){
		if (checkDataIncoming()) {
			timeOut = 100;
			inData = comBluee->read();
			switch (stepsProtocol) {
				case STEP_WAKEUP:
					if (inData == DATA_START_PROTOCOL) {
						sizeRX = 0;
						countRx = 0;
						errorCode = OK;
						isWithSize = false;
						isEndReceiving = false;
						stepsProtocol++;
						clear();
					}
					break;

				case STEP_START_INSTRUCTION:
					if (inData == DATA_START_INSTRUCTION) {
						stepsProtocol++;
					}	else {
						stepsProtocol = STEP_WAKEUP;
					}
					break;

				case STEP_START_FUNCTION:
					if (inData == DATA_START_SIZE) {
						stepsProtocol = STEP_GET_SIZE;
						isWithSize = true;
					}	else if (inData == DATA_START_PARAMS) {
						stepsProtocol = STEP_RECEIVING;
					}	else if (inData == DATA_END_INSTRUCTION) {
						stepsProtocol = STEP_END_INSTRUCTION;
					}	else {
						pFunction.addData(inData);
						if (pFunction.getSize() >= CONSTANT_SIZE_PROTOCOL_BUFFER) {
							errorCode = ERROR_PROTOCOL;
							stepsProtocol = STEP_WAKEUP;
							return false;
						}
					}
					break;

				case STEP_GET_SIZE:
					if (inData == DATA_END_SIZE) {
						stepsProtocol = STEP_START_PARAMS;
					}	else {
						sizeRX = (sizeRX * 10) + (inData - '0');
						if (sizeRX >= CONSTANT_SIZE_PROTOCOL_BUFFER) {
							errorCode = ERROR_PROTOCOL;
							stepsProtocol = STEP_WAKEUP;
							return false;
						}
					}
					break;

				case STEP_START_PARAMS:
					if (inData == DATA_START_PARAMS) {
						if (isWithSize) {
							if (sizeRX == 0) {
								stepsProtocol = STEP_END_RECEIVING_WITH_SIZE;
							}	else {
								stepsProtocol = STEP_RECEIVING_WITH_SIZE;
							}
						}	else {
							stepsProtocol = STEP_RECEIVING;
						}
					}	else {
						errorCode = ERROR_PROTOCOL;
						stepsProtocol = STEP_WAKEUP;
						return false;
					}
					break;

				case STEP_RECEIVING_WITH_SIZE:
					pData.addData(inData);
					if (++countRx == sizeRX) {
						stepsProtocol = STEP_END_RECEIVING_WITH_SIZE;
						isEndReceiving = true;
					}
					break;

				case STEP_END_RECEIVING_WITH_SIZE:
					if (inData == DATA_END_PARAMS) {
						stepsProtocol = STEP_END_PARAMS;
					}	else {
						errorCode = ERROR_PROTOCOL;
						stepsProtocol = STEP_WAKEUP;
						return false;
					}
					break;

				case STEP_RECEIVING:
					if (inData == DATA_END_PARAMS) {
						auxData = inData;
						stepsProtocol = STEP_END_PARAMS;
						sizeRX = countRx;
					}	else {
						if (isEndReceiving) {
							errorCode = ERROR_PROTOCOL;
							stepsProtocol = STEP_WAKEUP;
							return false;
						}	else {
							pData.addData(inData);
							countRx++;
						}
					}
					break;

				case STEP_END_PARAMS:
					if (inData == DATA_END_INSTRUCTION) {
						stepsProtocol = STEP_END_INSTRUCTION;
					}	else {
						pData.addData(auxData);
						countRx++;
						if (inData == DATA_END_PARAMS) {
							auxData = inData;
						}	else {
							pData.addData(inData);
							countRx++;
							stepsProtocol = STEP_RECEIVING;
						}
					}
					break;

				case STEP_END_INSTRUCTION:
					if (inData == DATA_END_PROTOCOL) {
						if (pFunction.getSize() > 0) {
							stepsProtocol = STEP_WAKEUP;

	#ifdef DEBUG
							Serial.println();
							Serial.println("***function***");
							Serial.println(pFunction.getSize());
							Serial.println(pFunction.getData());
							Serial.println("*****data*****");
							Serial.println(pData.getSize());
							Serial.println(pData.getData());
							Serial.println("**************");
	#endif

							return true;
						}	else {
							errorCode = ERROR_PROTOCOL;
							stepsProtocol = STEP_WAKEUP;
							return false;
						}
					}	else {
						errorCode = ERROR_PROTOCOL;
						stepsProtocol = STEP_WAKEUP;
						return false;
					}
					break;

				default:
					errorCode = ERROR_PROTOCOL;
					return false;
			}

			if (countRx > CONSTANT_SIZE_PROTOCOL_BUFFER) {
				errorCode = ERROR_PROTOCOL;
				stepsProtocol = STEP_WAKEUP;
				return false;
			}
		}	else {
			delay(1);
		}
	}
	errorCode = ERROR_TIME_OUT;
	return false;
}

DataBuffer& Bluee::getFunction(){
	return pFunction;
}

DataBuffer& Bluee::getData(){
	return pData;
}

bool Bluee::checkDataIncoming() {
	if (comBluee->available() > 0) {
		return true;
	}
	return false;
}

void Bluee::setFunction(const char * _pFunction) {
	pFunction.clear();
	pFunction.addData(_pFunction);
	pData.clear();
}

void Bluee::setData(const char* _pData) {
	pData.clear();
	pData.addData(_pData);
}

void Bluee::sendBuffer() {
	pBuffer.clear();
	pBuffer.addData(pFunction);
	if (pData.getSize() > 0) {
		pBuffer.addData(DATA_START_SIZE);
		pBuffer.addData(pData.getSize());
		pBuffer.addData(DATA_END_SIZE);
		pBuffer.addData(DATA_START_PARAMS);
		pBuffer.addData(pData);
		pBuffer.addData(DATA_END_PARAMS);
	}

#ifdef DEBUG
	Serial.println();
	Serial.println("***********SENDING**************");
	Serial.write(DATA_START_PROTOCOL);
	Serial.write(DATA_START_INSTRUCTION);
	for (int i = 0; i < pBuffer.getSize(); i++) {
		Serial.write(pBuffer.get(i));
	}
	Serial.write(DATA_END_INSTRUCTION);
	Serial.write(DATA_END_PROTOCOL);
	Serial.println();
	Serial.println("********************************");
	Serial.flush();
#endif // DEBUG

	comBluee->write(DATA_START_PROTOCOL);
	comBluee->write(DATA_START_INSTRUCTION);
	for (int i = 0; i < pBuffer.getSize(); i++) {
		comBluee->write(pBuffer.get(i));
	}
	comBluee->write(DATA_END_INSTRUCTION);
	comBluee->write(DATA_END_PROTOCOL);
	comBluee->flush();
	clear();
}

void Bluee::clear() {
	pBuffer.clear();
	pFunction.clear();
	pData.clear();
}

String Bluee::getDataAsString()
{
	return (String )pData.getData();
}

bool Bluee::send(int timeout) {
	sendBuffer();
	if (timeout) {
		if (checkReceived(timeout)) {
			if (pFunction.getString().equals("OK")) {
				return true;
			}	else {
				errorCode = getValueAsInt("code");
			}
		}
		return false;
	}
	return true;
}

void Bluee::handle() {
	if (checkReceived(0)) {
		if (getFunction().getString().equals("EVENT")) {
			int code = getValueAsInt("code");
			if (code != NONE) {
				removeObject(0, pData);
				runEvent(code);
			}
		}	else {
			runDataEvent();
		}
	}
}

void Bluee::setEventCallback(BlueeEvent pEvent) {
	_pEvent = pEvent;
}

void Bluee::setDataEventCallback(BlueeDataEvent pEvent) {
	_pDataEvent = pEvent;
}

void Bluee::addParamSeparator() {
	addDataOnBuffer(",");
}

void Bluee::addObjectSeparator() {
	addDataOnBuffer(";");
}

void Bluee::addParam(const char* pParam, bool bValue) {
	if (bValue) {
		addParam(pParam, "true");
	}	else {
		addParam(pParam, "false");
	}
}

void Bluee::addParam(const char* pParam, int iValue) {
	char* pTemp = reserveMem(8);
	if (pTemp != NULL) {
		toCharArray(iValue, pTemp);
		addParam(pParam, pTemp, getSizeCharPointer(pTemp));
		freeMem(pTemp);
	}
}

void Bluee::addParam(const char* pParam, size_t sValue) {
	char* pTemp = reserveMem(32);
	if (pTemp != NULL) {
		sprintf(pTemp, "%d", sValue);
		addParam(pParam, pTemp, getSizeCharPointer(pTemp));
		freeMem(pTemp);
	}
}

void Bluee::addParam(const char* pParam, DataBuffer& pValue) {
	addParam(pParam, pValue.getData(), pValue.getSize());
}

void Bluee::addParam(const char* pParam, String& sValue) {
	addParam(pParam, &sValue[0], sValue.length());
}

void Bluee::addParam(const char* pParam, const char* pValue) {
	addParam(pParam, &pValue[0], (int) getSizeCharPointer(pValue));
}

void Bluee::addParam(const char* pParam, const char* pValue, int sizeValue) {
	if (pData.getSize() > 0) {
		addParamSeparator();
	}
	pData.addData(pParam);
	addSizeParam(&pValue[0], sizeValue);
}

void Bluee::addSizeParam(const char* pValue, int sizeValue) {
	char* pSize = reserveMem(8);
	if (pSize != NULL) {
		toCharArray(sizeValue, pSize);
		pData.addData("+");
		pData.addData(pSize);
		pData.addData("={");
		pData.addData(pValue, sizeValue);
		pData.addData("}");
		freeMem(pSize);
	}
}

void Bluee::addDataOnBuffer(DataBuffer& pData) {
	addDataOnBuffer(pData.getData(), pData.getSize());
}

void Bluee::addDataOnBuffer(const char* pData) {
	addDataOnBuffer(pData, getSizeCharPointer(pData));
}

void Bluee::addDataOnBuffer(String& pData) {
	addDataOnBuffer(&pData[0], pData.length());
}

void Bluee::addDataOnBuffer(int iValue) {
	pData.addData(iValue);
}

void Bluee::addDataOnBuffer(bool bValue) {
	if (bValue) {
		pData.addData("true");
	}	else {
		pData.addData("false");
	}
}
void Bluee::addDataOnBuffer(const char* cData, int sizeData) {
	pData.addData(cData, sizeData);
}

bool Bluee::compareCharBuffers(const char* buff1, char* buff2) {
	if (strcmp(buff1, buff2) == 0) {
		return true;
	}
	return false;
}

void Bluee::removeObject(int pos, DataBuffer& pData) {
	DataBuffer pTemp;
	DataBuffer pDataFinal;
	int index = 0;
	while (getObject(index, pTemp, pData) == OK) {
		if (index != pos) {
			if (pDataFinal.getSize() > 0) {
				pDataFinal.addData(";");
			}
			pDataFinal.addData(pTemp);
		}	
		index++;
	}
	pDataFinal.setCopyTo(pData);
}

bool Bluee::getValue(const char * pParam, DataBuffer & pValue, int pos) {
	if (pos == 0) {
		return getValueSplit(pParam, pValue, pData) == OK ? true : false;
	}	else {
		DataBuffer pTemp;
		if (getObject(pos, pTemp, pData) == OK) {
			return getValueSplit(pParam, pValue, pTemp) == OK ? true : false;
		}
	}
	return false;
}

String Bluee::getValueAsString(const char* pParam, int pos) {
	DataBuffer pTemp;
	if (getValue(pParam, pTemp, pos)) {
		return pTemp.getString();
	}
	return "";
}

bool Bluee::hasErrorMemory()
{
	return pData.hasErrorMemory();
}

int Bluee::getValueAsInt(const char* pParam, int pos) {
	DataBuffer pTemp;
	if (getValue(pParam, pTemp, pos)) {
		if (pTemp.getSize() > 0) {
			return toInt(pTemp.getData());
		}
	}
	return NONE;
}

long Bluee::getValueAsLong(const char* pParam, int pos) {
	DataBuffer pTemp;
	if (getValue(pParam, pTemp, pos)) {
		if (pTemp.getSize() > 0) {
			return toLong(pTemp.getData());
		}
	}
	return NONE;
}

double Bluee::getValueAsDouble(const char* pParam, int pos) {
	DataBuffer pTemp;
	if (getValue(pParam, pTemp, pos)) {
		if (pTemp.getSize() > 0) {
			return toDouble(pTemp.getData());
		}
	}
	return NONE;
}

bool Bluee::getValueAsBoolean(const char* pParam, int pos) {
	DataBuffer pTemp;
	if (getValue(pParam, pTemp, pos)) {
		if (pTemp.getSize() > 0) {
			return pTemp.getString().equals("true")? true: false;
		}
	}
	return false;
}

int Bluee::getErrorCode() {
	return errorCode;
}

int Bluee::getObject(int pos, DataBuffer& pResultData, DataBuffer& pSourceData) {
	return getElementSplit(pos, pResultData, pSourceData);
}

int Bluee::getValueSplit(const char * pParam, DataBuffer& pResultData, DataBuffer& pSourceData) {
	bool isFoundParam = false;
	bool isParam = false;
	bool isPlusData = false;
	bool isGettingData = false;
	bool isObject = false;
	bool isNoObject = true;
	bool isSimpleData = false;
	int falseObjects = 0;
	int indexParam = 0;
	int indexValue = 0;
	int indexNumber = 0;
	int numberData = 0;
	bool validatingParam = true;
	bool validatingPlusParam = false;
	int i = 0;
	char number[8];
	memset(number, 0, 8);
	pResultData.clear();
	for (i = 0; i < pSourceData.getSize(); i++) {
		if (isParam) {
			if (pSourceData.get(i) == '{' && isNoObject == true) {
				isObject = true;
				isNoObject = false;
				indexValue = i + 1;
			}	else {
				if (isObject) {
					if (isGettingData == false) {
						if (pSourceData.get(i) == '{') {
							falseObjects++;
						}
						if (pSourceData.get(i) == '}') {
							if (falseObjects == 0) {
								pResultData.addData(&pSourceData.getData()[indexValue], i - indexValue);
								if (pResultData.hasErrorMemory()) {
				
									return ERROR_RESERVING_RAM_MEMORY;
								}
								return OK;
							}	else {
								falseObjects--;
							}
						}
					}
					else {
						if (numberData == 0) {
							if (pSourceData.get(i) == '}') {
								pResultData.addData(&pSourceData.getData()[indexValue], i - indexValue);
								if (pResultData.hasErrorMemory()) {
									return ERROR_RESERVING_RAM_MEMORY;
								}
								return OK;
							}
							return ERROR_INVALID_PROTOCOL;
						}	else {
							numberData--;
						}
					}
				}
				else {
					if (pSourceData.get(i) == ',') {
						pResultData.addData(&pSourceData.getData()[indexValue], i - indexValue);
						if (pResultData.hasErrorMemory()) {
							return ERROR_RESERVING_RAM_MEMORY;
						}
						return OK;
					}	else if ((i + 1) == pSourceData.getSize()) {
						pResultData.addData(&pSourceData.getData()[indexValue], i - indexValue + 1);
						if (pResultData.hasErrorMemory()) {
							return ERROR_RESERVING_RAM_MEMORY;
						}
						return OK;
					}
				}
			}
		}
		else {
			if (indexParam >= ((String)pParam).length()) {
				indexParam = 0;
			}
			if (pSourceData.get(i) == pParam[indexParam] && isFoundParam == false && validatingParam) {
				indexParam++;
				if ((i + 1) < pSourceData.getSize() && indexParam == ((String)pParam).length() && (pSourceData.get(i + 1) == '=' || pSourceData.get(i + 1) == '+')) {
					isFoundParam = true;
				}
			}	else if (pSourceData.get(i) == '=' && isGettingData == false && isSimpleData == false) {
				indexParam = 0;
				validatingParam = false;
				if (isFoundParam) {
					isParam = true;
					indexValue = i + 1;
				}
				if (isPlusData) {
					numberData = toInt(number);
					if (numberData > CONSTANT_SIZE_PROTOCOL_DATA) {
						return ERROR_SIZE_EXCEED_ALLOWED;
					}
					isPlusData = false;
					isGettingData = true;
				}	else {
					isSimpleData = true;
				}
			}	else if (pSourceData.get(i) == '+' && validatingParam) {
				indexParam = 0;
				validatingParam = false;
				isPlusData = true;
				indexNumber = 0;
				memset(number, '\0', 8);
			}	else if (isPlusData) {
				number[indexNumber] = pSourceData.get(i);
				indexNumber++;
				if (indexNumber >= 8) {
					return ERROR_DATA_SIZE;
				}
			}	else if (isGettingData) {
				if (numberData == 0) {
					validatingParam = true;
					isGettingData = false;
					indexParam = 0;
					indexNumber = 0;
				}	else {
					numberData--;
				}
			}	else if (validatingParam == false && pSourceData.get(i) == ',') {
				validatingParam = true;
				isPlusData = false;
				isGettingData = false;
				isSimpleData = false;
				indexParam = 0;
				numberData = 0;
			}
		}
	}
	if (i == pSourceData.getSize() && isParam) {
		return OK;
	}
	return  ERROR_DATA_PARAM_NOT_FOUND;
}


int Bluee::getElementSplit(int numElement, DataBuffer& pResultData, DataBuffer& pSourceData) {
	bool isParam = false;
	bool isPlusData = false;
	bool isObject = false;
	bool isNoObject = true;
	int indexNumber = 0;
	int numberData = 0;
	int indexElement = 0;
	int i = 0;
	char number[8];
	pResultData.clear();
	for (i = 0; i < pSourceData.getSize(); i++) {
		pResultData.addData(pSourceData.get(i));
		if (isParam) {
			if (pSourceData.get(i) == '{' && isNoObject == true) {
				isObject = true;
				isNoObject = false;
			}	else {
				if (isObject) {
					if (numberData == 0) {
						if (pSourceData.get(i) == '}') {
							isObject = false;
							isNoObject = true; 
							if ((i + 1) == pSourceData.getSize()) {
								if (numElement == indexElement) {
									if (pResultData.hasErrorMemory()) {
										return ERROR_RESERVING_RAM_MEMORY;
									}
									return OK;
								}
								return ERROR_DATA_ELEMENT_NOT_EXIST;
							}
						}	else {
							return ERROR_INVALID_PROTOCOL;
						}
					}	else {
						numberData--;
					}
				}	else {
					if (pSourceData.get(i) == ',' || pSourceData.get(i) == ';')
					{
						isParam = false;
						indexNumber = 0;
					}
					if (pSourceData.get(i) == ';') {
						if (numElement == indexElement) {
							pResultData.removeLast();
							if (pResultData.hasErrorMemory()) {
								return ERROR_RESERVING_RAM_MEMORY;
							}
							return OK;
						}	else {
							pResultData.clear();
							indexElement++;
						}
					}	else if ((i + 1) == pSourceData.getSize()) {
						if (numElement == indexElement) {
							if (pResultData.hasErrorMemory()) {
								return ERROR_RESERVING_RAM_MEMORY;
							}
							return OK;
						}	else {
							return ERROR_DATA_PARAM_NOT_FOUND;
						}
					}
				}
			}
		}	else {
			if (pSourceData.get(i) == '=')
			{
				isParam = true;
				if (isPlusData)
				{
					numberData = toInt(number);
					if (numberData > CONSTANT_SIZE_PROTOCOL_DATA) {
						return ERROR_SIZE_EXCEED_ALLOWED;
					}
					isPlusData = false;
				}
			}	else if (pSourceData.get(i) == '+')	{
				memset(number, '\0', 8);
				isPlusData = true;
			}
			if (isPlusData) {
				number[indexNumber] = pSourceData.get(i);
				indexNumber++;
				if (indexNumber >= 8)
				{
					return ERROR_DATA_SIZE;
				}
			}
		}
	}
	if (i == pSourceData.getSize()) {
		if (numElement == indexElement) {
			if (pResultData.hasErrorMemory()) {
				return ERROR_RESERVING_RAM_MEMORY;
			}
			return OK;
		}
	}
	return ERROR_DATA_PARAM_NOT_FOUND;
}

int Bluee::toInt(char* pData) {
	return ((String)pData).toInt();
}

int Bluee::toIntFromHexValue(char* pData) {
	char* pEnd;
	long int value = strtol(pData, &pEnd, 16);
	return (int)value;
}

long Bluee::toLong(char* pData) {
	return atol(pData);
}

double Bluee::toDouble(char* pData) {
	return atof(pData);
}

unsigned long Bluee::toULong(char* pData) {
	return strtoul(pData, NULL, 0);
}

void Bluee::toCharArray(int value, char* pData) {
	itoa(value, pData, 10);
}

unsigned long Bluee::getSizeCharPointer(const char* data) {
	return ((String)data).length();
}

char* Bluee::reserveMem(int sizeBuffer) {
	return (char*)calloc(sizeBuffer + 1, 1);
}

void Bluee::freeMem(char* pBuffer) {
	free(pBuffer);
	pBuffer = NULL;
}

/***************************DATABUFFER***************************/
DataBuffer::DataBuffer()
{
	init();
}

DataBuffer::DataBuffer(const char* pSource)
{
	init();
	addData(pSource);
}


void DataBuffer::init()
{
	pData = NULL;
	size = 0;
	error = false;
}

void DataBuffer::clear()
{
	if (pData != NULL) {
		free(pData);
	}
	init();
}

bool DataBuffer::resize(int newSize)
{
	char * pTemp = pData;
	pData = (char*)realloc(pData, newSize + 1);
	if (pData != NULL) {
		size = newSize;
		pData[size] = '\0';
		return true;
	}

#ifdef  DEBUG
	Serial.println("ERROR EN MEMORIA");
#endif 

	free(pTemp);
	error = true;
	return false;
}

bool DataBuffer::increase(int addSize)
{
	return resize(size + addSize);
}

void DataBuffer::addData(char newData)
{
	if (increase(1)) {
		pData[size - 1] = newData;
	}
}

void DataBuffer::addData(const char* pNewData)
{
	addData(pNewData, ((String)pNewData).length());
}

void DataBuffer::addData(String& pNewData)
{
	addData(&pNewData[0]);
}

void DataBuffer::addData(int iValue)
{
	char* pNumber = (char*)calloc(9, 1);
	itoa(iValue, pNumber, 10);
	addData(pNumber, ((String)pNumber).length());
	free(pNumber);
}

void DataBuffer::addByte(byte bValue)
{
	int auxSize = size;
	if (increase(1)) {
		pData[auxSize] = bValue;
	}
}

void DataBuffer::addData(const char* pNewData, int length)
{
	int auxSize = size;
	if (increase(length)) {
		for (int i = 0; i < length; i++) {
			pData[auxSize + i] = pNewData[i];
		}
	}
}

void DataBuffer::addData(DataBuffer& pNewData)
{
	addData(pNewData.getData(), pNewData.getSize());
}

void DataBuffer::setData(int iValue)
{
	clear();
	addData(iValue);
}

void DataBuffer::setData(String& sValue)
{
	setData(&sValue[0]);
}

void DataBuffer::setData(const char* pSource)
{
	clear();
	addData(pSource, ((String)pSource).length());
}

void DataBuffer::setData(const char* pSource, int size)
{
	clear();
	addData(pSource, size);
}

void DataBuffer::setDataByOffset(const char* pSource, int offset, int size)
{
	clear();
	addData(&pSource[offset], size);
}

int DataBuffer::getSize()
{
	return size;
}

char* DataBuffer::getData()
{
	return pData;
}

String DataBuffer::getString()
{
	return (String) pData;
}

char DataBuffer::get(int position)
{
	return pData[position];
}

void DataBuffer::set(char data, int position)
{
	pData[position] = data;
}

bool DataBuffer::hasErrorMemory() {
	return error;
}

void DataBuffer::setCopyTo(DataBuffer& db) {
	db.clear();
	db.addData(pData, size);
}

void DataBuffer::removeLast() {
	increase(-1);
}

int DataBuffer::getLastCharacterIndex(char character)
{
	for (int i = size - 1; i >= 0; i--) {
		if (get(i) == character) {
			return i;
		}
	}
	return -1;
}

DataBuffer::~DataBuffer()
{
	clear();
}