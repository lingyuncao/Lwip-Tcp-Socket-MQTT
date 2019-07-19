#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H
#include "sys.h"
#include "includes.h"
 

 
#define TCP_SERVER_RX_BUFSIZE	1000	//����tcp server���������ݳ���
#define TCP_SERVER_PORT			8088	//����tcp server�Ķ˿�
#define LWIP_SEND_DATA			0X80	//���������ݷ���

extern u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
extern u8 tcp_server_flag;			//TCP���������ݷ��ͱ�־λ


INT8U tcp_server_init(void);		//TCP��������ʼ��(����TCP�������߳�)





//�û���Ҫ�����豸��Ϣ�������º궨���е���Ԫ������
#define PRODUCT_KEY    	"a1RhIkLZfBX"															//�����ư䷢�Ĳ�ƷΨһ��ʶ��11λ���ȵ�Ӣ������������
#define DEVICE_NAME    	"MY_MQTT"																//�û�ע���豸ʱ���ɵ��豸Ψһ��ţ�֧��ϵͳ�Զ����ɣ�Ҳ��֧���û�����Զ����ţ���Ʒά����Ψһ
#define DEVICE_SECRET  	"PHX7MubtQtlovxy3OM19ScyDrAU5uBYd"				//�豸��Կ����DeviceName�ɶԳ��֣�������һ��һ�ܵ���֤����

														//�����ư䷢�Ĳ�Ʒ������Կ��ͨ����ProductKey�ɶԳ��֣�������һ��һ�ܵ���֤����
//���º궨��̶�������Ҫ�޸�
#define HOST_NAME  			PRODUCT_KEY".iot-as-mqtt.cn-shanghai.aliyuncs.com"															//����������
#define HOST_PORT 			1883																																						//�����������˿ڣ��̶�1883
#define CONTENT				"clientId"DEVICE_NAME"deviceName"DEVICE_NAME"productKey"PRODUCT_KEY"timestamp789"	//�����¼������
#define CLIENT_ID			DEVICE_NAME"|securemode=3,signmethod=hmacsha1,timestamp=789|"						//�ͻ���ID
#define USER_NAME			DEVICE_NAME"&"PRODUCT_KEY															//�ͻ����û���																		//�ͻ��˵�¼passwordͨ��hmac_sha1�㷨�õ�����Сд������
#define DEVICE_PUBLISH		"/sys/"PRODUCT_KEY"/"DEVICE_NAME"/thing/event/property/post"									
#define DEVICE_SUBSCRIBE	"/sys/"PRODUCT_KEY"/"DEVICE_NAME"/thing/service/property/set"							//�����豸����



void mqtt_thread(void);
void HexToStr(uint8_t *pbDest, uint8_t *pbSrc, int nLen);																		//��ֵת16�����ַ���
void getPassword(const char *device_secret, const char *content, char *password);						//�û������ȡ
//u32 PublishData(float temp, float humid, unsigned char *payloadbuf);


#endif

