#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H
#include "sys.h"
#include "includes.h"
 

 
#define TCP_SERVER_RX_BUFSIZE	1000	//定义tcp server最大接收数据长度
#define TCP_SERVER_PORT			8088	//定义tcp server的端口
#define LWIP_SEND_DATA			0X80	//定义有数据发送

extern u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
extern u8 tcp_server_flag;			//TCP服务器数据发送标志位


INT8U tcp_server_init(void);		//TCP服务器初始化(创建TCP服务器线程)





//用户需要根据设备信息完善以下宏定义中的四元组内容
#define PRODUCT_KEY    	"a1RhIkLZfBX"															//阿里云颁发的产品唯一标识，11位长度的英文数字随机组合
#define DEVICE_NAME    	"MY_MQTT"																//用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一
#define DEVICE_SECRET  	"PHX7MubtQtlovxy3OM19ScyDrAU5uBYd"				//设备密钥，与DeviceName成对出现，可用于一机一密的认证方案

														//阿里云颁发的产品加密密钥，通常与ProductKey成对出现，可用于一型一密的认证方案
//以下宏定义固定，不需要修改
#define HOST_NAME  			PRODUCT_KEY".iot-as-mqtt.cn-shanghai.aliyuncs.com"															//阿里云域名
#define HOST_PORT 			1883																																						//阿里云域名端口，固定1883
#define CONTENT				"clientId"DEVICE_NAME"deviceName"DEVICE_NAME"productKey"PRODUCT_KEY"timestamp789"	//计算登录密码用
#define CLIENT_ID			DEVICE_NAME"|securemode=3,signmethod=hmacsha1,timestamp=789|"						//客户端ID
#define USER_NAME			DEVICE_NAME"&"PRODUCT_KEY															//客户端用户名																		//客户端登录password通过hmac_sha1算法得到，大小写不敏感
#define DEVICE_PUBLISH		"/sys/"PRODUCT_KEY"/"DEVICE_NAME"/thing/event/property/post"									
#define DEVICE_SUBSCRIBE	"/sys/"PRODUCT_KEY"/"DEVICE_NAME"/thing/service/property/set"							//设置设备属性



void mqtt_thread(void);
void HexToStr(uint8_t *pbDest, uint8_t *pbSrc, int nLen);																		//数值转16进制字符串
void getPassword(const char *device_secret, const char *content, char *password);						//用户密码获取
//u32 PublishData(float temp, float humid, unsigned char *payloadbuf);


#endif

