/*************************************************************************************************************
 * �ļ���:		esp01s.h
 * ����:		����esp01sģ��
 * ����: 		usart2.h
ʹ�÷���:	
CUBEMX����˵��������I2C1��Standard Mode
��main.h����Ӵ���
#include "esp01s.h"

LED1,OLED_ShowString(0, 0, "MQTT Disconnect", 16);
NBiot_Init();      //NBIOT��ʼ��
LED0,OLED_ShowString(0, 0, "MQTT Connect   ", 16);
Sub_MQTT("AT+MQTTSUB=0,\"10soft\",1");
delay_ms(100);

*************************************************************************************************************/

#include "esp01s.h"
//*****��mu_usart.cע�͵�memset(rx2_buff,0,data_length)
//**********************���ݰ�Ĭ��ֵ���壨˳��*******************//
extern uint8_t usart2_rx_flag;          //�����жϱ�־λ

extern uint8_t rx1_buff[255];
extern uint8_t rx2_buff[255];
extern uint8_t tx2_buff[];
//*****************���ݰ�Ĭ��ֵ�����뺯����ʼ��**********************//
char *DeviceID = "912551933";
char *ProduceID = "489708";
char *Password = "stm10";

char TEMP[100];

int error_flag = 0;                                                       //ʧ��������־λ
int error_time = 0;   

/***********************************************************
	����ָ��
  �������ܣ���ʼ��NBIOT
  ��ڲ�������
  ����ֵ����
  ��ע��
   ������Ӳ����λ+ATָ�λ��ͬʱ����LED��
***********************************************************/

/***********************************************************
  �������ƣ�NBiot_Init(void)
  �������ܣ���ʼ��NBIOT
  ��ڲ�������
  ����ֵ����
  ��ע��
   ������Ӳ����λ+ATָ�λ��ͬʱ����LED��
***********************************************************/
void NBiot_Init(void)
{
	int StaFlag = 4;
	while (StaFlag != 0)
	{
		u2_printf("AT+RESTORE\r\n");
		delay_ms(2000);
		printf("��ʼ����WIFI\r\n");
		if(NBiot_SendCmd("AT+CWMODE=1","OK",200))StaFlag = 3;
		delay_ms(200);
		printf("���WIFI\r\n");
		if(NBiot_SendCmd("AT+CWJAP=\"mu\",\"12345678wd\"","OK",200))StaFlag = 2;
		delay_ms(200);
		printf("��������\r\n");
		if(NBiot_SendCmd("AT+MQTTUSERCFG=0,1,\"912551933\",\"489708\",\"stm10\",0,0,\"\"","OK",200))StaFlag = 1;
		delay_ms(200);
		printf("��ʼ���ӷ�����\r\n");
		if(NBiot_SendCmd("AT+MQTTCONN=0,\"183.230.40.39\",6002,0","OK",200))StaFlag = 0;
	}printf("IOT Config OK\r\n");
}
/***********************************************************
  �������ƣ�Get_IP(void)
  �������ܣ���ȡ����״̬
  ��ڲ�������
  ����ֵ����
  ��ע��
   ѭ���ȴ���������
   2019��4��23�շ����п���ѭ�����ӵȴ���Ч������������200�μƴΣ�ʧ����λNB
***********************************************************/
void Get_IP(int time) {
  int try_time = 0;
  while (!NBiot_SendCmd("AT+IOTSTATUS", "+IOT:status:2", 500)) //ѭ�����ӣ�ֱ�����ӵ�������
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
  �������ƣ�connected_IP
  �������ܣ�NB���ӷ�����   47.105.157.158  60.205.203.64
  ��ڲ�������
  ���ڲ�������
  �� ע����Ϊ��һ���ϵ����� ������ĸ�λ�����������
       �����������200�γ���ʧ�ܣ���λNB������
char* TOPIC = "/device/NB/zx11111111111111";                              //��������topic
char* SUB_TOPIC = "AT+MQTTSUB=\"/device/NB/zx11111111111111_back\",1";    //���Ļش�����
char* Connect = "AT+MQTTCFG=\"47.105.157.158\",1883,\"zx11111111111111\",60,\"root\",\"citc2018\",1"; //����������ָ��
***********************************************************/
uint8_t connected_IP(char* id,char* user,char* pwd)
{ 
//	sprintf(TEMP, "AT+IOTCFG=%s,%s,%s",id,user,pwd); //ƴ���ַ���
	if (NBiot_SendCmd("AT+IOTCFG=896574444,489708,stm2","+Event:Connect:0", 500)) {
		printf("%s", "connected_success��");
		return 1;
	}
	printf("connected_error��\r\n");
	return 0;
	
}

/***********************************************************
  �������ƣ�Sub_MQTT
  �������ܣ�NB������Ϣ��MQTT������
  ��ڲ�����
  char* Sub:   ������Ϣ������
  //AT+MQTTPUB="device/nb/citc",1,1,0,0,"##0112QN=20190306152401;ST=32;CN=51;PW=CITC2018+;MN=112019015222;Flag=5 ;CP=&&LA=30;TE=25;HU=75;&&1C80"
***********************************************************/
void Sub_MQTT(char* Sub)
{
	if (NBiot_SendCmd(Sub,"OK", 200)) {
		printf("%s", "Sub Success\r\n");
	}else printf("Sub Fail��\r\n");

}

/***********************************************************
  �������ƣ�Pub_MQTT
  �������ܣ�NB������Ϣ��MQTT������
  ��ڲ�����
  char* topic:   ������Ϣ������
  char* message: ���͵���Ϣ����
  //AT+MQTTPUB="device/nb/citc",1,1,0,0,"##0112QN=20190306152401;ST=32;CN=51;PW=CITC2018+;MN=112019015222;Flag=5 ;CP=&&LA=30;TE=25;HU=75;&&1C80"
***********************************************************/
void Pub_MQTT(char* topic, char* message)
{
	uint8_t len = strlen(message);
  sprintf(TEMP,"AT+MQTTPUBRAW=0,\"%s\",%d,0,0",topic,len+1); //ƴ���ַ���

	if (!NBiot_SendCmd(TEMP,"OK", 200))    //��ʱ�����ʵ�����
  {
		printf("Pub init Error\r\n");
  }
  if(!NBiot_SendCmd(message,"+MQTTPUB:OK", 200))    //��ʱ�����ʵ�����
  {
		printf("Pub Send Error\r\n");
  }
	memset(TEMP,0x00,sizeof(TEMP)); 
}



/*******************************************************************************
  �����ַ���  ����������ֵ�Ƿ���ȷ
  cmdΪ����ֵ  reply ΪУ�鷵��ֵ wait Ϊ��ʱ
*******************************************************************************/
int NBiot_SendCmd(char* cmd, char* reply, int wait)
{
	memset(&rx2_buff, 0, sizeof(rx2_buff)); //����ջ�����
	u2_printf("%s\r\n",cmd);
  while(wait--)
	{
		if(usart2_rx_flag == 1)                       //�յ��ж�
		{
			if (strstr((char*)rx2_buff, reply))   //�Ƿ��������
			{
				memset(rx2_buff,0,sizeof(rx2_buff)); 
				return 1;
			}usart2_rx_flag = 0;
		}delay_ms(20);
	}
	printf("error\r\n");
	return 0;
}

