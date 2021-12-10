#include "reg52.h"
#include "character.h"
#include "stdio.h"
sbit led=P2^7;
sbit SCL=P1^7;
sbit SDA=P1^6;
sbit TRIG = P1^0; 
sbit ECHO = P3^2;
float S;
/**********I2C init**********i2c的相关配置函数*/
void I2cStart(void);
void I2cStop(void);
unsigned char I2cSendByte(unsigned char dat);
void oled_Write_cmd(unsigned char cmd);
void oled_Write_data(unsigned char dat);
void oled_clear(void);
void oled_put_char_16x16(unsigned char x,unsigned char y,unsigned char t);
/**********Oled Initialization configuration**********oled相关配置函数，分别是oled初始化配置和oled初始屏显示函数，不可调换调用顺序*/
void oled_initial(void);
void dispaly_init(void);
/**********Print characters and numbers***********在oled屏上打印字符或数字的函数（对应character内的数组）*/
void oled_display_char(int horizontal,int vertical,int order);
void oled_display_number(int horizontal,int vertical,int order);
void dispaly_distance(void);
/**********delay**********延时函数*/
void delay(unsigned char i);
void delay_10us(unsigned int i);
/************ultrasonic**********超声波相关配置函数*/
void Time0_Init(void);
void StartModule(void);
void Conut(void);
void ultrasonic_init(void);
void ultrasonic_distance(void);
void Time0(void) interrupt 1 
{
}
int main(void)
{
	oled_initial();
	dispaly_init();
	ultrasonic_init();
	while(1)
	{
	ultrasonic_distance();	
	dispaly_distance();
	led=0;
	}
}

void dispaly_distance(void)                 //在oled屏上显示距离的函数
{	
	oled_display_number(0,2,((int)S/100));
	oled_display_number(2,2,((int)S%100/10));
	oled_display_number(4,2,((int)S%10));
}


/**********oled_init**********/
void I2cStart(void)                       //i2c起始时序
{
	SDA=1;
	delay_10us(1);
	SCL=1;
	delay_10us(1);//建立时间是SDA保持时间>4.7us
	SDA=0;
	delay_10us(1);//保持时间是>4us
	SCL=0;			
	delay_10us(1);		
}
/*stop*/
void I2cStop(void)						//i2c停止时序
{
	SDA=0;
	delay_10us(1);
	SCL=1;
	delay_10us(1);//建立时间大于4.7us
	SDA=1;
	delay_10us(1);		
}
unsigned char I2cSendByte(unsigned char dat)            //利用i2c发送字节数据的函数
{
    unsigned char a=0,b=0;//最大255，一个机器周期为1us，最大延时255us。		
	for(a=0;a<8;a++)//要发送8位，从最高位开始
	{
		SDA=dat>>7;	 //起始信号之后SCL=0，所以可以直接改变SDA信号
		dat=dat<<1;
		delay_10us(1);
		SCL=1;
		delay_10us(1);//建立时间>4.7us
		SCL=0;
		delay_10us(1);//时间大于4us		
	}
	SDA=1;
	delay_10us(1);
	SCL=1;
	while(SDA)//等待应答，也就是等待从设备把SDA拉低
	{
		b++;
		if(b>200)	 //如果超过2000us没有应答发送失败，或者为非应答，表示接收结束
		{
			SCL=0;
			delay_10us(1);
			return 0;
		}
	}
	SCL=0;
	delay_10us(1);
 	return 1;		
}

void oled_Write_cmd(unsigned char cmd)			//封装好的oled写使能函数
{
	I2cStart();
	I2cSendByte(0x78);//发送写器件地址
	I2cSendByte(0x00);//发送要写入内存地址
	I2cSendByte(cmd);	//写入命令
	I2cStop();
}

void oled_Write_data(unsigned char dat)			//封装好的oled写数据函数
{
	I2cStart();
	I2cSendByte(0x78);//发送写器件地址
	I2cSendByte(0x40);//发送要写入内存地址
	I2cSendByte(dat);	//写入数据
	I2cStop();
}

void oled_clear(void)  //页寻址下的oled清屏函数		
{	unsigned char i,j;
     
	oled_Write_cmd(0x20);
	oled_Write_cmd(0x02);
    for(i=0;i<8;i++)
	{
	   	oled_Write_cmd(0xb0+i);
		oled_Write_cmd(0x00);
		oled_Write_cmd(0x10);
		for(j=0;j<128;j++)
		{
		   oled_Write_data(0x00); 
		}

	}	
}

void oled_initial(void)//oled初始化函数
{
    delay(500);
			 
	oled_Write_cmd(0xae);//--turn off oled panel 关闭显示
    oled_Write_cmd(0x00);//---set low column address设置起始列的低四位 0x0x
    oled_Write_cmd(0x10);//---set high column address设置起始列的高四位0x1x
    oled_Write_cmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    oled_Write_cmd(0x81);//--set contrast control register设置对比度
    oled_Write_cmd(0xff); // Set SEG Output Current Brightness对比度为oxff
    oled_Write_cmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    oled_Write_cmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    oled_Write_cmd(0xa6);//--set normal display
    oled_Write_cmd(0xa8);//--set multiplex ratio(1 to 64)
    oled_Write_cmd(0x3f);//--1/64 duty
    oled_Write_cmd(0xd3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    oled_Write_cmd(0x00);//-not offset
    oled_Write_cmd(0xd5);//--set display clock divide ratio/oscillator frequency
    oled_Write_cmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    oled_Write_cmd(0xd9);//--set pre-charge period
    oled_Write_cmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    oled_Write_cmd(0xda);//--set com pins hardware configuration
    oled_Write_cmd(0x12);
    oled_Write_cmd(0xdb);//--set vcomh
    oled_Write_cmd(0x40);//Set VCOM Deselect Level
    oled_Write_cmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)设置地址模式
						//水平寻址，垂直寻址，页寻址
    oled_Write_cmd(0x02);//	地址模式为页寻址
    oled_Write_cmd(0x8d);//--set Charge Pump enable/disable
    oled_Write_cmd(0x14);//--set(0x10) disable
    oled_Write_cmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    oled_Write_cmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
    oled_Write_cmd(0xaf);//--turn on oled panel开启显示

	delay(100);
	oled_clear();//清屏
}

void oled_put_char_16x16(unsigned char x,unsigned char y,unsigned char t)
	/*设置显示坐标函数,t为0时，字符为8x16
					t为1时，字符为16x16*/                 
{	 
     oled_Write_cmd(0x20);
	 oled_Write_cmd(0x00);//设置地址模式为水平选址
     //set page
     oled_Write_cmd(0x22);
	 oled_Write_cmd(y*2);
	 oled_Write_cmd(0x01+y*2);

	 //set colum
     oled_Write_cmd(0x21);
	 oled_Write_cmd((0x08+0x08*t)*x);
	 oled_Write_cmd((0x08+0x08*t)*x+(0x07+0x08*t));
}
void dispaly_init(void)							//这个可根据自行需要更改，这是oled屏初始显示的函数
{
	oled_display_char(0,1,35);
	oled_display_char(1,1,40);
	oled_display_char(2,1,50);
	oled_display_char(3,1,51);
	oled_display_char(4,1,32);
	oled_display_char(5,1,45);
	oled_display_char(6,1,34);
	oled_display_char(7,1,36);
	oled_display_char(8,1,25);
	oled_display_char(6,2,66);
	oled_display_char(7,2,76);
	
}
void oled_display_char(int horizontal,int vertical,int order)				//oled打印字符的函数
{
	int i;
	oled_put_char_16x16(horizontal,vertical,0);
	for(i=0;i<16;i++)
	{
	oled_Write_data(character[order][i]);
	}
}	
void oled_display_number(int horizontal,int vertical,int order)				//oled打印阿拉伯数字的函数
{
	int i;
	oled_put_char_16x16(horizontal,vertical,0);
	for(i=0;i<16;i++)
	{
	oled_Write_data(number[order][i]);
	}
}
/********************/
/**********dealy*********/
void delay(unsigned char i)														//延时函数
{
    while(i--);
}
void delay_10us(unsigned int i)
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
/********************/
/**********ultrasonic**********/
void Time0_Init(void)									//定时器0配置函数，利用定时0计算高电平时间			
{
TMOD=0x01; 
TH0=0;
TL0=0;
ET0=1; 
TR0=1; 
EA=1; 
}
void ultrasonic_init(void)								//超声波初始化函数
{
	Time0_Init();
	TRIG=0;
	ECHO=0;
}
void StartModule(void) 									//超声波开始信号
 {
 	TRIG = 1;
	delay_10us(2);
	TRIG = 0;
 }
void Conut(void) 										//计算距离的函数
{	
	unsigned int time;
	time=TH0*256+TL0;
	TH0=0;
	TL0=0;
	S=(time*1.7)/100;
}
void ultrasonic_distance(void)							//判断是有有信号产生，有则计算距离
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
