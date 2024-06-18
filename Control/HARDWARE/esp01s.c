#include "esp01s.h"
//*****��mu_usart.cע�͵�memset(rx2_buff,0,data_length)
//**********************���ݰ�Ĭ��ֵ���壨˳��*******************//
extern uint8_t usart2_rx_flag;          //�����жϱ�־λ

extern uint8_t rx1_buff[255];
extern uint8_t rx2_buff[255];
extern uint8_t tx2_buff[];
//*****************���ݰ�Ĭ��ֵ�����뺯����ʼ��**********************//
char *DeviceID = "912551939";
char *ProduceID = "489708";
char *Password = "stm16";

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
		if(NBiot_SendCmd("AT+CWMODE=1","OK",500))StaFlag = 3;
		delay_ms(200);
		printf("���WIFI\r\n");
//		if(NBiot_SendCmd("AT+CWJAP=\"mu\",\"12345678wd\"","WIFI CONNECTED",1000))StaFlag = 2;
		if(NBiot_SendCmd("AT+CWJAP=\"mu\",\"12345678wd\"","OK",500))StaFlag = 2;
		delay_ms(200);
		printf("��������\r\n");
		if(NBiot_SendCmd("AT+MQTTUSERCFG=0,1,\"912551939\",\"489708\",\"stm16\",0,0,\"\"","OK",500))StaFlag = 1;
		delay_ms(200);
		printf("��ʼ���ӷ�����\r\n");
//		if(NBiot_SendCmd("AT+MQTTCONN=0,\"183.230.40.39\",6002,0","MQTTCONNECTED",1000))StaFlag = 0;
		if(NBiot_SendCmd("AT+MQTTCONN=0,\"183.230.40.39\",6002,0","OK",500))StaFlag = 0;
	}printf("IOT Config OK\r\n");
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
	sprintf(TEMP,"AT+MQTTSUB=0,\"%s\",1",Sub); //ƴ���ַ���
	if (NBiot_SendCmd(TEMP,"OK", 200)) {
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
		}delay_ms(10);
	}
	printf("error\r\n");
	return 0;
}

