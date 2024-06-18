
/*************************************************************************************************************
 * 文件名:		rc522.h
 * 功能:		驱动RC522模块
 * PA4-SDA	PA5-SCK	PA7-MOSI	PA6-MISO	IRQ-		GND-GND	RST-3V3	VCC-3V3
使用方法:	
CUBEMX配置说明：配置SPI1	16分频，速率6.25MBit/s
在main.h中添加代码
#include "rc522.h"
char		txBuffer[10];
uint8_t flag = 0;

MFRC522_Init();		//初始化RFID刷卡模块
printf("RC522 init OK\r\n");

flag = RC522_GET_Card(txBuffer);
if(flag == 1)printf("%s\r\n",txBuffer);
else printf("no Card\r\n");

*************************************************************************************************************/

// CS (SDA)		PA4					SPI1_NSS	Chip select for SPI
// SCK				PA5					SPI1_SCK	Serial Clock for SPI
// MOSI				PA7 				SPI1_MOSI	Master In Slave Out for SPI
// MISO				PA6					SPI1_MISO	Master Out Slave In for SPI
// IRQ				-						Irq
// GND				GND					Ground
// RST				3.3V				Reset pin (3.3V)
// VCC				3.3V				3.3V power


#include "rc522.h"
#include "mu_usart.h"
extern SPI_HandleTypeDef hspi1;

uint8_t 	retstr[10];

void char_to_hex(uint8_t data) {
	uint8_t digits[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	
	if (data < 16) {
		retstr[0] = '0';
		retstr[1] = digits[data];
	} else {
		retstr[0] = digits[(data & 0xF0)>>4];
		retstr[1] = digits[(data & 0x0F)];
	}
}

uint8_t RC522_GET_Card(char *buffer)
{
	uint8_t i = 0,b,state = 0;
	uint8_t		str[MFRC522_MAX_LEN];
	//此处if内为读取卡片数据
	//返回值为0，代表寻卡成功；并把卡类型存入str中
	if (!MFRC522_Request(PICC_REQIDL, str)) {
		state = 1;
		//防冲撞，完成这部就可以简单地 读取卡号，本次不涉及更高层次应用
		if (!MFRC522_Anticoll(str)) {
			b = 0;
				for (i=0; i<4; i++) {
					char_to_hex(str[i]);
					buffer[b] = retstr[0];
					b++;
					buffer[b] = retstr[1];
					b++;
				}
		}
	}else 
	{
		state = 0;
		buffer = "00000000\r";
	}
	return state;
}



uint8_t SPI1SendByte(uint8_t data) {
	unsigned char writeCommand[1];
	unsigned char readValue[1];
	
	writeCommand[0] = data;
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&writeCommand, (uint8_t*)&readValue, 1, 10);
	return readValue[0];
	
	//while (SPI1->SR & SPI_SR_BSY);								// STM32F030 - ждем конец передачи
	//while (SPI1->SR & SPI_I2S_FLAG_BSY);					// STM32F103 - ждем конец передачи

	//while (!(SPI1->SR & SPI_SR_TXE));     				// убедиться, что предыдущая передача завершена (STM32F103)
	//SPI1->DR = data;															// вывод в SPI1
	//while (!(SPI1->SR & SPI_SR_RXNE));     				// ждем окончания обмена (STM32F103)
	//for (uint8_t i=0; i<50; i++) {};
	//data = SPI1->DR;															// читаем принятые данные
	//return data;
}

void SPI1_WriteReg(uint8_t address, uint8_t value) {
	cs_reset();
	SPI1SendByte(address);
	SPI1SendByte(value);
	cs_set();
}

uint8_t SPI1_ReadReg(uint8_t address) {
	uint8_t	val;

	cs_reset();
	SPI1SendByte(address);
	val = SPI1SendByte(0x00);
	cs_set();
	return val;
}

void MFRC522_WriteRegister(uint8_t addr, uint8_t val) {
	addr = (addr << 1) & 0x7E;															// Address format: 0XXXXXX0
  SPI1_WriteReg(addr, val);
}

uint8_t MFRC522_ReadRegister(uint8_t addr) {
	uint8_t val;

	addr = ((addr << 1) & 0x7E) | 0x80;
	val = SPI1_ReadReg(addr);
	return val;	
}

uint8_t MFRC522_Check(uint8_t* id) {
	uint8_t status;
	status = MFRC522_Request(PICC_REQIDL, id);							// Find cards, return card type
	if (status == MI_OK) status = MFRC522_Anticoll(id);			// Card detected. Anti-collision, return card serial number 4 bytes
	MFRC522_Halt();																					// Command card into hibernation 
	return status;
}

uint8_t MFRC522_Compare(uint8_t* CardID, uint8_t* CompareID) {
	uint8_t i;
	for (i = 0; i < 5; i++) {
		if (CardID[i] != CompareID[i]) return MI_ERR;
	}
	return MI_OK;
}

void MFRC522_SetBitMask(uint8_t reg, uint8_t mask) {
	MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) | mask);
}

void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask){
	MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) & (~mask));
}
/***************************************************************************************
* 函数名称：MFRC522_Request
* 功能描述：寻卡
* 入口参数： -RequestMode：寻卡方式
*                 PICC_REQIDL：寻天线区内未进入休眠状态
*                 PICC_REQALL：寻天线区内全部卡
*             -pCardType：用于保存卡片类型
* 出口参数：-pCardType：卡片类型
*                       0x4400：Mifare_UltraLight
*                       0x0400：Mifare_One(S50)
*                       0x0200：Mifare_One(S70)
*                       0x0800：Mifare_Pro(X)
*                       0x4403：Mifare_DESFire
* 返 回 值：-status：错误代码(MI_OK、MI_NOTAGERR、MI_ERR)
* 说    明：无
***************************************************************************************/

uint8_t MFRC522_Request(uint8_t reqMode, uint8_t* TagType) {
	uint8_t status;  
	uint16_t backBits;																			// The received data bits

	MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07);		// TxLastBists = BitFramingReg[2..0]
	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
	if ((status != MI_OK) || (backBits != 0x10)) status = MI_ERR;
//	switch (status)
//	{
//		case MI_OK:
//			printf("Find OK\r\n");break;
//		case MI_ERR:
//			printf("Find ERROR\r\n");break;
//		case MI_NOTAGERR:	
//			printf("None\r\n");break;
//	}

	return status;
}

uint8_t MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen) {
	uint8_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint16_t i;

	switch (command) {
		case PCD_AUTHENT: {
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE: {
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
		break;
	}

	MFRC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
	MFRC522_ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
	MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);
	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);

	// Writing data to the FIFO
	for (i = 0; i < sendLen; i++) MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);

	// Execute the command
	MFRC522_WriteRegister(MFRC522_REG_COMMAND, command);
	if (command == PCD_TRANSCEIVE) MFRC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80);		// StartSend=1,transmission of data starts 

	// Waiting to receive data to complete
	i = 2000;	// i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms
	do {
		// CommIrqReg[7..0]
		// Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = MFRC522_ReadRegister(MFRC522_REG_COMM_IRQ);
		i--;
	} while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	MFRC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80);																// StartSend=0

	if (i != 0)  {
		if (!(MFRC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B)) {
			status = MI_OK;
			if (n & irqEn & 0x01) status = MI_NOTAGERR;
			if (command == PCD_TRANSCEIVE) {
				n = MFRC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);
				lastBits = MFRC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07;
				if (lastBits) *backLen = (n-1)*8+lastBits; else *backLen = n*8;
				if (n == 0) n = 1;
				if (n > MFRC522_MAX_LEN) n = MFRC522_MAX_LEN;
				for (i = 0; i < n; i++) backData[i] = MFRC522_ReadRegister(MFRC522_REG_FIFO_DATA);		// Reading the received data in FIFO
			}
		} else status = MI_ERR;
	}
	return status;
}
/***************************************************************************************
* 函数名称：MFRC522_Anticoll
* 功能描述：防冲突,获取卡号
* 入口参数：-pSnr：用于保存卡片序列号,4字节
* 出口参数：-pSnr：卡片序列号,4字节
* 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
* 说    明：无
***************************************************************************************/
uint8_t MFRC522_Anticoll(uint8_t* serNum) {
	uint8_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint16_t unLen;

	MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00);												// TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
	if (status == MI_OK) {
		// Check card serial number
		for (i = 0; i < 4; i++) serNumCheck ^= serNum[i];
		if (serNumCheck != serNum[i]) status = MI_ERR;
	}
//	switch (status)
//		{
//			case MI_OK:
//				printf("Find OK\r\n");break;
//			case MI_ERR:
//				printf("Find ERROR\r\n");break;
//			case MI_NOTAGERR:	
//				printf("None\r\n");break;
//		}
	return status;
} 

void MFRC522_CalculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData) {
	uint8_t i, n;

	MFRC522_ClearBitMask(MFRC522_REG_DIV_IRQ, 0x04);													// CRCIrq = 0
	MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);													// Clear the FIFO pointer
	// Write_MFRC522(CommandReg, PCD_IDLE);

	// Writing data to the FIFO	
	for (i = 0; i < len; i++) MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata+i));
	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);

	// Wait CRC calculation is complete
	i = 0xFF;
	do {
		n = MFRC522_ReadRegister(MFRC522_REG_DIV_IRQ);
		i--;
	} while ((i!=0) && !(n&0x04));																						// CRCIrq = 1

	// Read CRC calculation result
	pOutData[0] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
	pOutData[1] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
}

uint8_t MFRC522_SelectTag(uint8_t* serNum) {
	uint8_t i;
	uint8_t status;
	uint8_t size;
	uint16_t recvBits;
	uint8_t buffer[9]; 

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++) buffer[i+2] = *(serNum+i);
	MFRC522_CalculateCRC(buffer, 7, &buffer[7]);		//??
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
	if ((status == MI_OK) && (recvBits == 0x18)) size = buffer[0]; else size = 0;
	return size;
}

uint8_t MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum) {
	uint8_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[12]; 

	// Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++) buff[i+2] = *(Sectorkey+i);
	for (i=0; i<4; i++) buff[i+8] = *(serNum+i);
	status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
	if ((status != MI_OK) || (!(MFRC522_ReadRegister(MFRC522_REG_STATUS2) & 0x08))) status = MI_ERR;
	return status;
}

uint8_t MFRC522_Read(uint8_t blockAddr, uint8_t* recvData) {
	uint8_t status;
	uint16_t unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	MFRC522_CalculateCRC(recvData,2, &recvData[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
	if ((status != MI_OK) || (unLen != 0x90)) status = MI_ERR;
	return status;
}

uint8_t MFRC522_Write(uint8_t blockAddr, uint8_t* writeData) {
	uint8_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[18]; 

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	MFRC522_CalculateCRC(buff, 2, &buff[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
	if (status == MI_OK) {
		// Data to the FIFO write 16Byte
		for (i = 0; i < 16; i++) buff[i] = *(writeData+i);
		MFRC522_CalculateCRC(buff, 16, &buff[16]);
		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
	}
	return status;
}
/***************************************************************************************
* 函数名称：PCD_Init
* 功能描述：读写器初始化
* 入口参数：无
* 出口参数：无
* 返 回 值：无
* 说    明：无
***************************************************************************************/

void MFRC522_Init(void) {
	MFRC522_Reset();
	MFRC522_WriteRegister(MFRC522_REG_T_MODE, 0x8D);
	MFRC522_WriteRegister(MFRC522_REG_T_PRESCALER, 0x3E);
	MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_L, 30);           
	MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_H, 0);
	MFRC522_WriteRegister(MFRC522_REG_RF_CFG, 0x70);				// 48dB gain	
	MFRC522_WriteRegister(MFRC522_REG_TX_AUTO, 0x40);
	MFRC522_WriteRegister(MFRC522_REG_MODE, 0x3D);
	MFRC522_AntennaOn();																		// Open the antenna
}

void MFRC522_Reset(void) {
	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

void MFRC522_AntennaOn(void) {
	uint8_t temp;

	temp = MFRC522_ReadRegister(MFRC522_REG_TX_CONTROL);
	if (!(temp & 0x03)) MFRC522_SetBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}

void MFRC522_AntennaOff(void) {
	MFRC522_ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}

void MFRC522_Halt(void) {
	uint16_t unLen;
	uint8_t buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0;
	MFRC522_CalculateCRC(buff, 2, &buff[2]);
	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

