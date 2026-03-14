#pragma once

// Macros for register values from docs
// 1 Command and Status
#define Page0_Reserved_0 	0x00
#define CommandReg			0x01
#define ComIEnReg			0x02
#define DivIEnReg			0x03
#define ComIrqReg			0x04
#define DivIrqReg			0x05
#define ErrorReg			0x06
#define Status1Reg			0x07
#define Status2Reg			0x08
#define FIFODataReg			0x09
#define FIFOLevelReg		0x0A
#define WaterLevelReg		0x0B
#define ControlReg			0x0C
#define BitFramingReg		0x0D
#define CollReg				0x0E
#define Page0_Reserved_1	0x0F

// 2 Command
#define Page1_Reserved_1	0x10
#define ModeReg				0x11
#define TxModeReg			0x12
#define RxModeReg			0x13
#define TxControlReg		0x14
#define TxASKReg			0x15
#define TxSelReg			0x16
#define RxSelReg			0x17
#define RxThresholdReg		0x18
#define	DemodReg			0x19
#define Page1_Reserved_2	0x1A
#define Page1_Reserved_3	0x1B
#define MfTxReg				0x1C
#define MfRxReg				0x1D
#define Page1_Reserved_4	0x1E
#define SerialSpeedReg		0x1F

// 3 Configuration
#define Page2_Reserved_1	0x20
#define CRCResultReg_Hi		0x21
#define CRCResultReg_Low	0x22
#define Page2_Reserved_2	0x23
#define ModWidthReg			0x24
#define Page2_Reserved_3	0x25
#define RFCfgReg			0x26
#define GsNReg				0x27
#define CWGsPReg			0x28
#define ModGsPReg			0x29
#define TModeReg			0x2A
#define TPrescalerReg		0x2B
#define TReloadReg_Hi		0x2C
#define TReloadReg_Low		0x2D
#define TCounterValReg_Hi	0x2E
#define TCounterValReg_Low 	0x2F

// 4 Test register
#define Page3_Reserved_1 	0x30
#define TestSel1Reg			0x31
#define TestSel2Reg			0x32
#define TestPinEnReg		0x33
#define TestPinValueReg		0x34
#define TestBusReg			0x35
#define AutoTestReg			0x36
#define VersionReg			0x37
#define AnalogTestReg		0x38
#define TestDAC1Reg			0x39
#define TestDAC2Reg			0x3A
#define TestADCReg			0x3B
#define Page3_Reserved_2 	0x3C
#define Page3_Reserved_3	0x3D
#define Page3_Reserved_4	0x3E
#define Page3_Reserved_5	0x3F
// ------------------------------

// MFRC Connection Management Commands
#define Idle  0x00 // No action; cancels current command execution.
#define Mem   0x01 // Stores 25 byte into the internal buffer
#define GenerateRandomId  0x02 // Generates a 10 byte random ID number
#define CalcCRC 0x03 // Activates the CRC co-processor or performs a selftest.
#define Transmit  0x04 // Transmits data from the FIFO buffer.
#define NoCmdChange 0x07 // No command change. This command can be used to modify different bits in the command register without touching th e command. E.g. Power-down.
#define Receive 0x08 // Activates the receiver circuitry
#define Transceive  0x0C // Transmits data from FIFO buffer to the antenna and activates automatically the receiver after transmission.
#define MFAuthent 0x0E // Performs the MIFARE standard authentication as a reader
#define SoftReset 0x0F // Resets the MFRC522
// ------------------------------

// MIFARE Classic Commands
#define AUTH_KEYA 0x60
#define AUTH_KEYB 0x61
#define REQUEST 0x26
#define WAKEUP  0x52
#define ANTICOLL_10 0x93
#define ANTICOLL_11 0x20
#define SELECT_10  0x93
#define SELECT_11 0x70
// #define ANTICOLL_2 0x95 0x20 -- FOR 7-UID PICC'S --
// #define SELECT_2  0x95 0x70
#define HALT_0  0x50
#define HALT_1  0x00
#define PERSONALIZE_UID_USAGE 0x40
#define SET_MOD_TYPE  0x43
#define MIFARE_READ 0x30
#define MIFARE_WRITE  0xA0
#define MIFARE_DECREMENT 0xC0
#define MIFARE_INCREMENT  0xC1
#define MIFARE_RESTORE  0xC2
#define MIFARE_TRANSFER 0xB0