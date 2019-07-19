#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	
#include "timer.h"
#include "sram.h"
#include "tsensor.h"
#include "malloc.h"
#include "string.h"
#include "usmart.h"	
#include "dm9000.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "includes.h"
#include "tcp_server_demo.h"


//技术支持：www.openedv.com


extern double my_temp ;
extern int Led_Value;

//KEY任务
#define KEY_TASK_PRIO 		9
//任务堆栈大小
#define KEY_STK_SIZE		128	
//任务堆栈
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//任务函数
void key_task(void *pdata);   

//LED任务
//任务优先级
#define LED_TASK_PRIO		10
//任务堆栈大小
#define LED_STK_SIZE		64
//任务堆栈
OS_STK	LED_TASK_STK[LED_STK_SIZE];
//任务函数
void led_task(void *pdata);  

//在LCD上显示地址信息任务
//任务优先级
#define DISPLAY_TASK_PRIO	11
//任务堆栈大小
#define DISPLAY_STK_SIZE	128
//任务堆栈
OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//任务函数
void display_task(void *pdata);

//START任务
//任务优先级
#define START_TASK_PRIO		12
//任务堆栈大小
#define START_STK_SIZE		128
//任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata); 
//在LCD上显示地址信息
//mode:1 显示DHCP获取到的地址
//	  其他 显示静态地址
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==2)
	{
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,190,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,210,210,16,16,buf); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,190,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,210,210,16,16,buf); 
	}	
}


 int main(void)
 {	 
	delay_init();	    	//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			    //LED端口初始化
	LCD_Init();				//初始化LCD
	KEY_Init();	 			//初始化按键
	T_Adc_Init();		  	//ADC初始化	
 	usmart_dev.init(72);	//初始化USMART		 
 	FSMC_SRAM_Init();		//初始化外部SRAM
	my_mem_init(SRAMIN);	//初始化内部内存池
	my_mem_init(SRAMEX);	//初始化外部内存池
	POINT_COLOR = RED; 		

	POINT_COLOR = BLUE; 		//蓝色字体
	 
	OSInit();					//UCOS初始化
	while(lwip_comm_init()) 	//lwip初始化
	{
		LCD_ShowString(30,10,200,20,16,"Lwip Init failed!"); 	//lwip初始化失败
		delay_ms(500);
		LCD_Fill(30,10,230,150,WHITE);
		delay_ms(500);
	}
	LCD_ShowString(30,10,200,20,16,"Lwip Init Success!"); 		//lwip初始化成功
	while(tcp_server_init()) 									//初始化tcp_client(创建tcp_client线程)
	{
		LCD_ShowString(30,30,200,20,16,"TCP Server failed!!"); //tcp客户端创建失败
		delay_ms(500);
		LCD_Fill(30,30,230,170,WHITE);
		delay_ms(500);
	}
	LCD_ShowString(30,30,200,20,16,"TCP Server Success!"); 			//TCP创建成功
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //开启UCOS
}
 
//start任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//初始化统计任务
	OS_ENTER_CRITICAL();  	//关中断
	OSTaskCreate(led_task,(void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO); 	//创建LED任务
	OSTaskCreate(key_task,(void*)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO); 	//创建KEY任务
	OSTaskCreate(display_task,(void*)0,(OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],DISPLAY_TASK_PRIO); //显示任务
	OSTaskSuspend(OS_PRIO_SELF); //挂起start_task任务
	OS_EXIT_CRITICAL();  //开中断
}

//显示地址等信息
void display_task(void *pdata)
{
	while(1)
	{ 
#if LWIP_DHCP									//当开启DHCP的时候
		if(lwipdev.dhcpstatus != 0) 			//开启DHCP
		{
			show_address(lwipdev.dhcpstatus );	//显示地址信息
			OSTaskSuspend(OS_PRIO_SELF); 		//显示完地址信息后挂起自身任务
		}
#else
		show_address(0); 						//显示静态地址
		OSTaskSuspend(OS_PRIO_SELF); 			//显示完地址信息后挂起自身任务
#endif //LWIP_DHCP
		OSTimeDlyHMSM(0,0,0,100);
	} 
}

//key任务
void key_task(void *pdata)
{
	u8 key; 
	LCD_ShowString(30,120,200,16,16,"LED: ");
	while(1)
	{
		key = KEY_Scan(0);
		if(key==KEY0_PRES) //模仿LED灯开关
		{
			LED0 = !LED0; 
			if(LED0 == 0)
			{
				Led_Value = 1;
				LCD_ShowString(30+4*8,120,200,16,16,"OPEN");
			}
			else
			{
				Led_Value = 0;
				LCD_ShowString(30+4*8,120,200,16,16,"CLOSE");
			}
			
		}
		OSTimeDlyHMSM(0,0,0,10);  //延时10ms
	}
}

//内部温度采集任务
void led_task(void *pdata)
{
	LCD_ShowString(30,100,200,16,16,"TEMPERATE: 00.00C");
	while(1)
	{
		my_temp = Get_Temprate();
		
		LCD_ShowxNum(30+11*8,100,my_temp/100,2,16,0);		//显示整数部分
		LCD_ShowxNum(30+14*8,100,(short)my_temp%100,2,16, 0X80);	//显示小数部分
		
		OSTimeDlyHMSM(0,0,0,500);  //延时500ms
 	}
}
