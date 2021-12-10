#include "reg52.h"
#include "stdio.h"
sbit led=P2^0;
sbit TRIG = P1^0; 
sbit ECHO = P3^2;
char buf[32];
int S;
/************ultrasonic**********/
void Time0_Init(void);          
void StartModule(void);
void Conut(void);
void ultrasonic_init(void);
void ultrasonic_distance(void);
/********************/
/***********usart**********/
void senddelay(unsigned long time);
void sendMes(char *p);
void initUart(void);
void sendsingle(char single);
/*********************/
/**********delay**********/
void delay(unsigned char i);
void delay_10us(unsigned int i);
void Time0(void) interrupt 1   //空的定时器0服务函数，没有可能会发生不知名的错误
{
}
void usart(void) interrupt 4  //与上同理
{
}
int main()                    //主函数
{
	initUart();	              //初始化串口
	ultrasonic_init();			//初始化超声波模块
	while(1)					//不断打印出距离，led提示系统正常运行
	{
	ultrasonic_distance();
	sprintf(buf,"S=%d\r\n",S);
	sendMes(buf);		
	led=0;
	}
}
/**********ultrasonic**********/
void Time0_Init(void)			//定时器0配置函数
{
TMOD=0x01; 
TH0=0;
TL0=0;
ET0=1; 
TR0=1; 
EA=1; 
}
void ultrasonic_init(void)		//超声波初始化函数
{
	Time0_Init();
	TRIG=0;
	ECHO=0;
}
void StartModule(void) 			//开始模块，根据手册得知将TRIG拉高20us左右模块内部即可发送8段40khz的8个40khz的方波，自动检测是否有信号返回
 {
 	TRIG = 1;
	delay_10us(2);
	TRIG = 0;
 }
void Conut(void)                //计算距离的函数
{	
	unsigned int time;
	time=TH0*256+TL0;
	TH0=0;
	TL0=0;
	S=(time*1.7)/100;
}
void ultrasonic_distance(void)		
/* 检测是否有信号回来，有则进行计算，有信号返回，通过IO口ECHO输出一个高电平，
   高电平持续的时间就是超声波从发射到返回的时间。通过公式计算测试距离=(高电平时间*声速(340M/S))/2  */
{
	StartModule();
	while(!ECHO);
	TR0=1;
	while(ECHO);
	TR0=0;
	Conut();
	TH0=0;
	TL0=0;
	delay(80);
}
/******************/
/**********usart**********/
void senddelay(unsigned long time)		//串口发送消息时的延迟函数
{                 
	int i = 0;
	int j = 0;
	for(i = 0;i<100;i++)
	{
		for(j = 0;j<time;j++);
	}
}	
void initUart(void)						//串口初始化函数
{
	EA = 1;
	ES = 1;
	SCON = 0x40;
	TH2 = 0xFF;
	TL2 = 0xFD;
	RCAP2H = 0xFF;
	RCAP2L = 0xFD;
	T2CON = 0x34;
}
void sendMes(char *p)					//发送消息函数
{
	while(*p!='\0'){
	sendsingle(*p);
	p++;
	} 
	 senddelay(100);
}
void sendsingle(char single)			//传递数值函数
{ 		
	SBUF=single;
	while(TI!=1);
	TI=0;
}
/********************/
/**********delay**********/
void delay(unsigned char i)				//粗略延时函数
{
    while(i--);
}
 void delay_10us(unsigned int i)		//i=1时定时大约10us函数，以此类推
{ 
	unsigned char a,b,c;
	for(c=0;c<i;c++)
	{
		for(b=1;b>0;b--)
		{
			for(a=2;a>0;a--);
		}
	}
}