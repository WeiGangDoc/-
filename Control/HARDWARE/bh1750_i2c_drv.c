#include "bh1750_i2c_drv.h"
#include "i2c.h"
#include "sys.h"

#define	BH1750_ADDR_WRITE	0x46
#define	BH1750_ADDR_READ	0x47
#define BH1750_IIC  hi2c2

/************************************************************************************************************
注意：IIC读取速度不可过快，100ms以上
使用方法:	
    PB6 ------> I2C1_SCL	PB7------> I2C1_SDA
    PB10------> I2C2_SCL 	PB3------> I2C2_SDA
		PA8 ------> I2C3_SCL 	PB4------> I2C3_SDA
在main.h中添加代码
#include "bh1750_i2c_drv.h"
uint16_t lux = 0;
uint8_t luxdata[2] = {0}; //dat[0]是高字节，dat[1]是低字节
if (HAL_OK == BH1750_Send_Cmd(ONCE_L_MODE)) {
	if (HAL_OK == BH1750_Read_Dat(luxdata))
		lux = BH1750_Dat_To_Lux(luxdata);
	else
		printf("error \r\n");
}printf("lux:%d\r\n",lux);



*************************************************************************************************************/

uint8_t	BH1750_Send_Cmd(BH1750_MODE cmd)
{
	return HAL_I2C_Master_Transmit(&hi2c2, BH1750_ADDR_WRITE, (uint8_t*)&cmd, 1, 0xFFFF);
}

uint8_t BH1750_Read_Dat(uint8_t* dat)
{
	return HAL_I2C_Master_Receive(&hi2c2, BH1750_ADDR_READ, dat, 2, 0xFFFF);
}

uint16_t BH1750_Dat_To_Lux(uint8_t* dat)
{
	uint16_t lux = 0;
	lux = dat[0];
	lux <<= 8;
	lux += dat[1];
	lux = (int)(lux / 1.2);

	return lux;
}
