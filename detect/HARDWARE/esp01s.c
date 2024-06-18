/*************************************************************************************************************
 * 文件名:		esp01s.h
 * 功能:		驱动esp01s模块
 * 串口: 		usart2.h
使用方法:	
CUBEMX配置说明：配置I2C1，Standard Mode
在main.h中添加代码
#include "esp01s.h"

LED1,OLED_ShowString(0, 0, "MQTT Disconnect", 16);
NBiot_Init();      //NBIOT初始化
LED0,OLED_ShowString(0, 0, "MQTT Connect   ", 16);
Sub_MQTT("AT+MQTTSUB=0,\"10soft\",1");
delay_ms(100);

*************************************************************************************************************/

#include "esp01s.h"
//*****在mu_usart.c注释掉memset(rx2_buff,0,data_length)
//**********************数据包默认值定义（顺序）*******************//
extern uint8_t usart2_rx_flag;          //接收中断标志位

extern uint8_t rx1_buff[255];
extern uint8_t rx2_buff[255];
extern uint8_t tx2_buff[];
//*****************数据包默认值定义与函数初始化**********************//
char *DeviceID = "912551933";
char *ProduceID = "489708";
char *Password = "stm10";

char TEMP[100];

int error_flag = 0;                                                       //失败重连标志位
int error_time = 0;   

/***********************************************************
	入网指令
  函数功能：初始化NBIOT
  入口参数：无
  返回值：无
  备注：
   更新了硬件复位+AT指令复位，同时点亮LED灯
***********************************************************/

/***********************************************************
  函数名称：NBiot_Init(void)
  函数功能：初始化NBIOT
  入口参数：无
  返回值：无
  备注：
   更新了硬件复位+AT指令复位，同时点亮LED灯
***********************************************************/
void NBiot_Init(void)
{
	int StaFlag = 4;
	while (StaFlag != 0)
	{
		u2_printf("AT+RESTORE\r\n");
		delay_ms(2000);
		printf("开始配置WIFI\r\n");
		if(NBiot_SendCmd("AT+CWMODE=1","OK",200))StaFlag = 3;
		delay_ms(200);
		printf("请打开WIFI\r\n");
		if(NBiot_SendCmd("AT+CWJAP=\"mu\",\"12345678wd\"","OK",200))StaFlag = 2;
		delay_ms(200);
		printf("配置入网\r\n");
		if(NBiot_SendCmd("AT+MQTTUSERCFG=0,1,\"912551933\",\"489708\",\"stm10\",0,0,\"\"","OK",200))StaFlag = 1;
		delay_ms(200);
		printf("开始连接服务器\r\n");
		if(NBiot_SendCmd("AT+MQTTCONN=0,\"183.230.40.39\",6002,0","OK",200))StaFlag = 0;
	}printf("IOT Config OK\r\n");
}
/***********************************************************
  函数名称：Get_IP(void)
  函数功能：获取网络状态
  入口参数：无
  返回值：无
  备注：
   循环等待连接网络
   2019年4月23日发现有可能循环连接等待无效的情况，故添加200次计次，失败则复位NB
***********************************************************/
void Get_IP(int time) {
  int try_time = 0;
  while (!NBiot_SendCmd("AT+IOTSTATUS", "+IOT:status:2", 500)) //循环连接，直到连接到服务器
  {
    try_time += 1;
    printf("Try_Time:%d\r\n", try_time);
    if (try_time > time)
			{
				try_time = 0;
				NBiot_Init();
			}
	}
}
/***********************************************************
  函数名称：connected_IP
  函数功能：NB连接服务器   47.105.157.158  60.205.203.64
  入口参数：无
  出口参数：无
  备 注：分为第一次上电连接 与出错后的复位连接两种情况
       如果连续尝试200次尝试失败，则复位NB重连。
char* TOPIC = "/device/NB/zx11111111111111";                              //定义数据topic
char* SUB_TOPIC = "AT+MQTTSUB=\"/device/NB/zx11111111111111_back\",1";    //订阅回传主题
char* Connect = "AT+MQTTCFG=\"47.105.157.158\",1883,\"zx11111111111111\",60,\"root\",\"citc2018\",1"; //服务器配置指令
***********************************************************/
uint8_t connected_IP(char* id,char* user,char* pwd)
{ 
//	sprintf(TEMP, "AT+IOTCFG=%s,%s,%s",id,user,pwd); //拼接字符串
	if (NBiot_SendCmd("AT+IOTCFG=896574444,489708,stm2","+Event:Connect:0", 500)) {
		printf("%s", "connected_success！");
		return 1;
	}
	printf("connected_error！\r\n");
	return 0;
	
}

/***********************************************************
  函数名称：Sub_MQTT
  函数功能：NB订阅消息到MQTT服务器
  入口参数：
  char* Sub:   订阅消息的主题
  //AT+MQTTPUB="device/nb/citc",1,1,0,0,"##0112QN=20190306152401;ST=32;CN=51;PW=CITC2018+;MN=112019015222;Flag=5 ;CP=&&LA=30;TE=25;HU=75;&&1C80"
***********************************************************/
void Sub_MQTT(char* Sub)
{
	if (NBiot_SendCmd(Sub,"OK", 200)) {
		printf("%s", "Sub Success\r\n");
	}else printf("Sub Fail！\r\n");

}

/***********************************************************
  函数名称：Pub_MQTT
  函数功能：NB发送消息到MQTT服务器
  入口参数：
  char* topic:   发布消息的主题
  char* message: 发送的消息内容
  //AT+MQTTPUB="device/nb/citc",1,1,0,0,"##0112QN=20190306152401;ST=32;CN=51;PW=CITC2018+;MN=112019015222;Flag=5 ;CP=&&LA=30;TE=25;HU=75;&&1C80"
***********************************************************/
void Pub_MQTT(char* topic, char* message)
{
	uint8_t len = strlen(message);
  sprintf(TEMP,"AT+MQTTPUBRAW=0,\"%s\",%d,0,0",topic,len+1); //拼接字符串

	if (!NBiot_SendCmd(TEMP,"OK", 200))    //延时可以适当调整
  {
		printf("Pub init Error\r\n");
  }
  if(!NBiot_SendCmd(message,"+MQTTPUB:OK", 200))    //延时可以适当调整
  {
		printf("Pub Send Error\r\n");
  }
	memset(TEMP,0x00,sizeof(TEMP)); 
}



/*******************************************************************************
  发送字符串  并解析返回值是否正确
  cmd为传入值  reply 为校验返回值 wait 为延时
*******************************************************************************/
int NBiot_SendCmd(char* cmd, char* reply, int wait)
{
	memset(&rx2_buff, 0, sizeof(rx2_buff)); //先清空缓冲区
	u2_printf("%s\r\n",cmd);
  while(wait--)
	{
		if(usart2_rx_flag == 1)                       //收到中断
		{
			if (strstr((char*)rx2_buff, reply))   //是否包含数据
			{
				memset(rx2_buff,0,sizeof(rx2_buff)); 
				return 1;
			}usart2_rx_flag = 0;
		}delay_ms(20);
	}
	printf("error\r\n");
	return 0;
}

