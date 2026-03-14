#pragma once

#include <stdint.h>

#include "mfrc522-config.h"

enum TAG_STATUS{NOT_DETECTED = 0, DETECTED, CONNECTED, ERROR};

#define FIFO_BUFFER_SIZE 64
#define CORRECT_UID 0xFF

extern uint8_t rfidBuffer[FIFO_BUFFER_SIZE];

void mfrc522Initialize(void);
void mfrc522SoftReset(void);
void mfrc522HardReset(void);
uint8_t prepareCRC(void);
void flushFIFOBuffer(void);
void mfrc522RegWrite(uint8_t reg, uint8_t byteOfData);
void mfrc522FifoWrite(uint8_t reg, uint8_t* bytesOfData, uint8_t dataToSendLen);
uint8_t mfrc522RegRead(uint8_t reg);
void mfrc522FifoRead(uint8_t reg, uint8_t bytesToRead, uint8_t* dataReadDest);

void startTransceive(void);
void stopTransceive(void);

enum TAG_STATUS mfrc522CheckForPICC(void);
enum TAG_STATUS mfrc522ConnectWithPICC(uint8_t* dataToSend, uint8_t dataToSendLen, uint8_t* dataReceived, uint8_t* dataReceivedLen);
enum TAG_STATUS mfrc522AnticollLoop(uint8_t* receivedData, uint8_t* receivedDataLen);

uint8_t mfrc522GetTagUID(uint8_t* UID);
