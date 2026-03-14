#include <util/delay.h>

#include "mfrc522.h"
#include "spi-config.h"

uint8_t rfidBuffer[FIFO_BUFFER_SIZE];

// Communication MCU with MFRC522
void mfrc522RegWrite(uint8_t reg, uint8_t byteOfData) { // zapis 1-ego bajtu danych
	setSSPinLow();
	spiTransferByte((reg << 1) & 0x7E); // MSB 1-odczyt, 0-zapis; 6->1 adres; LSB - logic 0
	spiTransferByte(byteOfData);
	setSSPinHigh();
}

uint8_t mfrc522RegRead(uint8_t reg) { // odczyt 1-ego bajtu danych
	uint8_t value;
	setSSPinLow();
	spiTransferByte(((reg << 1) & 0x7E ) | 0x80);
	value = spiTransferByte(0x00); // bajt konczacy komunikacje
	setSSPinHigh();

	return value; // odczyt z SPDR
}

void mfrc522FifoWrite(uint8_t reg, uint8_t* bytesOfData, uint8_t dataToSendLen) {
	setSSPinLow();
	spiTransferByte((reg << 1) & 0x7E); // MSB 1-odczyt, 0-zapis; 6->1 adres; LSB - logic 0
	for(uint16_t i = 0; i < dataToSendLen; i++) {
		spiTransferByte(bytesOfData[i]);
	}
	setSSPinHigh();
}

void mfrc522FifoRead(uint8_t reg, uint8_t bytesToRead, uint8_t* dataReadDest) {
	if(bytesToRead > FIFO_BUFFER_SIZE)
		bytesToRead = FIFO_BUFFER_SIZE;
	setSSPinLow();
	spiTransferByte( ((reg << 1) & 0x7E) | 0x80 ); // first byte
	for(uint8_t i = 0; i < bytesToRead; i++)
		dataReadDest[i] = spiTransferByte(0x00);
		
	setSSPinHigh();
}

void flushFIFOBuffer() {
	uint8_t byte;
	// interrupts
	byte = mfrc522RegRead(ComIrqReg);
	mfrc522RegWrite(ComIrqReg, byte & (~0x80));
	byte = mfrc522RegRead(DivIrqReg);
	mfrc522RegWrite(DivIrqReg, byte & (~0x80));
	// FIFO Buffer
	byte = mfrc522RegRead(FIFOLevelReg);
	mfrc522RegWrite( FIFOLevelReg, byte | 0x80 );
}

void mfrc522Initialize() {
	uint8_t regContent;
	mfrc522SoftReset();
	_delay_ms(200);

	for (uint8_t i = 0; i < FIFO_BUFFER_SIZE; i++)
		rfidBuffer[i] = 0x00;

	// TIMER & PRESCALER
	//mfrc522RegWrite(TModeReg, 0x8D); // TAuto -> 1, timer starts automatically at the end of the transmission in all communication modes at all speeds / 4 high bity prescalera
  	//mfrc522RegWrite(TPrescalerReg, 0x3E); // ustawienie 8 low bitow prescalera [1101 00111110] / ustawienie f dla timera f = 13.56MHz / (2*TPreScaler+1) 3390 w dziesietnym = 2kHz
  	//mfrc522RegWrite(TReloadReg_Hi, 0x30); // high bits of 16-bit timer reload val -> 48
  	//mfrc522RegWrite(TReloadReg_Low, 0x00); // lower 8 bits of the 16-bit timer reload value
	regContent = mfrc522RegRead(TxASKReg);
	mfrc522RegWrite(TxASKReg, regContent | 0x40); // Force100ASK -> 1; forces a 100 % ASK modulation independent of the ModGsPReg register setting
	// ModeReg - Defines general mode settings for transmitting and receiving
	mfrc522RegWrite(ModeReg, 0x29); // TxWaitRF -> 1; CRCPreset 0x6363

	// INTERRUPTS
	mfrc522RegWrite(ComIEnReg, 0x80); // IRQ State is inverted to Status1Reg IRQ bit state
	regContent = mfrc522RegRead(DivIEnReg);
	mfrc522RegWrite(DivIEnReg, regContent|0x80); // ustawienie pinu IRQ do pracy jako wyjscie CMOS

	//ANTENNA CONFIG
	regContent = mfrc522RegRead(TxControlReg); // ustawienie aktywnego wzmacniacza dla pinow TX1 i TX2
	mfrc522RegWrite(TxControlReg, regContent | 0x03);

	// FIFO buffer
	//mfrc522RegWrite(WaterLevelReg, 0x01);
	flushFIFOBuffer();
}

void mfrc522SoftReset() {
	mfrc522RegWrite(CommandReg, SoftReset);
}

void mfrc522HardReset() {
	uint8_t resetContent[25];
	flushFIFOBuffer();
	for(uint8_t i = 0; i < 25; i++)
		resetContent[i] = 0x00;
	
	mfrc522FifoWrite(FIFODataReg, resetContent, 25);
	mfrc522RegWrite(CommandReg, Mem);
	SPI_PORT &= ~(1 << SPI_RST);
	_delay_ms(1000);
	SPI_PORT |= (1 << SPI_RST);
}

uint8_t prepareCRC() {
	uint8_t regContent;

	mfrc522RegWrite(CommandReg, CalcCRC);
	uint8_t i = 50;	// timeout max 50ms
	do {
		_delay_ms(1);
		regContent = mfrc522RegRead(Status1Reg);
		--i;
	} while ((i > 0) && !(regContent & 0x20));

	if (i != 0 && !(mfrc522RegRead(ErrorReg) & 0x04)) {
		regContent = mfrc522RegRead(CRCResultReg_Low);
		mfrc522RegWrite(FIFODataReg, regContent);
		regContent = mfrc522RegRead(CRCResultReg_Hi);
		mfrc522RegWrite(FIFODataReg, regContent);
		return 1;
	}
	else
		return 0;

	return 0;
}

void startTransceive() {
	uint8_t regContent;
	mfrc522RegWrite(CommandReg, Transceive); // Transceive command into CommandReg

	// Start transmiting by setting StartSend to 1
	regContent = mfrc522RegRead(BitFramingReg);
	mfrc522RegWrite(BitFramingReg, regContent | 0x80); // StartSend bit
}

void stopTransceive() {
	uint8_t regContent;
	// Stop transciving
	regContent = mfrc522RegRead(BitFramingReg);
	mfrc522RegWrite(BitFramingReg, regContent & (~0x80));
	mfrc522RegWrite(CommandReg, Idle);
}

enum TAG_STATUS mfrc522ConnectWithPICC(uint8_t* dataToSend, uint8_t dataToSendLen, uint8_t* dataReceived, uint8_t* dataReceivedLen) {
	uint8_t regContent = 0x00;
	uint8_t FIFOByteCount = 0x00;
	uint32_t i;

	if (mfrc522RegRead(FIFOLevelReg))
		flushFIFOBuffer();

	//Writing data to the FIFO
	mfrc522FifoWrite(FIFODataReg, dataToSend, dataToSendLen);
	startTransceive();

	// Waiting to receive all data
	i = 50;	// timeout 50ms
	do {
		_delay_ms(1);
		regContent = mfrc522RegRead(ComIrqReg);
		--i;
	} while ((i > 0) && !(regContent & 0x30));

  	regContent = 0x00;
	regContent = (mfrc522RegRead(Status2Reg) & 0x07);
	if(regContent != 1 && regContent != 5 ) {
		mfrc522RegWrite(CommandReg, Idle);
		flushFIFOBuffer();
		return ERROR;
	}

  	stopTransceive();
  	regContent = mfrc522RegRead(ComIrqReg);

	if (regContent & 0x02 || ((regContent & 0x0A) && i != 0)) { // some error occured
		flushFIFOBuffer();
		mfrc522RegWrite(CommandReg, Idle);
		return ERROR;
	}

	if (i != 0) {
		FIFOByteCount = mfrc522RegRead(FIFOLevelReg);

		//Reading the received data from FIFO buffer
		mfrc522FifoRead(FIFODataReg, FIFOByteCount, dataReceived);
		*dataReceivedLen = FIFOByteCount;

		flushFIFOBuffer();
		mfrc522RegWrite(CommandReg, Idle);
		return CONNECTED;
	} else {
		mfrc522RegWrite(CommandReg, Idle);
		return NOT_DETECTED;
	}
	flushFIFOBuffer();
	mfrc522RegWrite(CommandReg, Idle);
	return ERROR;
}

enum TAG_STATUS mfrc522CheckForPICC(void) {
	enum TAG_STATUS status;
	uint8_t atqa[2] = {0x00, 0x00};
	uint8_t transmitedData = WAKEUP; // or REQUEST
	uint8_t len;

	mfrc522RegWrite(BitFramingReg, 0x07); // TRANSMITS ONLY 7 BITS OF THE LAST BYTE
	//mfrc522RegWrite(BitFramingReg, 0x00);

	status = mfrc522ConnectWithPICC(&transmitedData, 1, atqa, &len);

	if(status == CONNECTED && (atqa[0] & 0x04)) // ONLY MIFARE CLASSIC ACCEPTED ATQA 0x0004
		return DETECTED;
	else if(status == NOT_DETECTED)
		return NOT_DETECTED;
	else
		return ERROR;
}

enum TAG_STATUS mfrc522AnticollLoop(uint8_t* receivedData, uint8_t* receivedDataLen) {
	enum TAG_STATUS status;
	uint8_t sendData[] = {ANTICOLL_10, ANTICOLL_11}; // ANTICOLL MF CMD
	mfrc522RegWrite(BitFramingReg, 0x00); // transmits entire last byte
	status = mfrc522ConnectWithPICC(sendData, 2, receivedData, receivedDataLen);

	return status;
}

uint8_t mfrc522GetTagUID(uint8_t* UID) {
	uint8_t status;
	uint8_t uid[4], bcc = 0x00;
	uint8_t receivedDataLength;

	status = mfrc522CheckForPICC();
	if (status == DETECTED) {
		status = mfrc522AnticollLoop(rfidBuffer, &receivedDataLength);
		if (status == CONNECTED && receivedDataLength == 5 && rfidBuffer[0] != 0x88) {
			for (uint8_t i = 0; i < 4; i++) {
				uid[i] = rfidBuffer[i];
				bcc ^= rfidBuffer[i];
			}
			if (bcc == rfidBuffer[4]) {
				for(uint8_t i = 0; i < 4; i++)
					UID[i] = uid[i];
				
				return CORRECT_UID;
			}
		} else
			return ERROR;
	} else if(status == NOT_DETECTED)
			return NOT_DETECTED;
	else
		return ERROR;
}
