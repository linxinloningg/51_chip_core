#include "reg51.h"
#include "character.h"
sbit DQ=P3^6;
unsigned char temperature;
unsigned char value;
sbit SCL=P1^1;
sbit SDA=P1^0;
sbit led=P2^7;
sbit led_time=P2^3;
/*****************oled*********************/
void I2cStart(void);
void I2cStop(void);
unsigned char I2cSendByte(unsigned char dat);
void oled_Write_cmd(unsigned char cmd);
void oled_Write_data(unsigned char dat);
void oled_clear(void);
void oled_initial(void);
void oled_put_char_16x16(unsigned char x,unsigned char y,unsigned char t);
void oled_display_init(void);
void oled_display_number(int horizontal,int vertical,int order);
void oled_display_char(int horizontal,int vertical,int order);
void delay(unsigned char i);
void display_temperature(void);
void display_time(void);
void dispaly_state(void);
/******************ds18b20*****************/
void Init_DS18B20(void);
unsigned char ReadOneChar(void);
void WriteOneChar(unsigned char dat);
void ReadTemperature(void);
/******************************************/
void time0_init(void);
void Delay10us(void);
int frequency=0;
unsigned long runtime=0;
int main()
{
	time0_init();
	oled_initial();
	oled_display_init();
	while(1)
	{
		display_temperature();
		display_time();
		dispaly_state();
		led=0;
	}
}
void time0_init(void)
{
TMOD=0x01; 
TH0=(65536-1000)/256; 
TL0=(65536-1000)%256; 
ET0=1; 
TR0=1; 
EA=1; 
}
void Time0(void) interrupt 1 
{
TH0=(65536-1000)/256; 
TL0=(65536-1000)%256;
frequency++;
while(frequency==1000)
{
	runtime++;
	led_time=~led_time;
	frequency=0;
}
}
void Delay10us(void)
{
	unsigned char a,b;
	for(b=1;b>0;b--)
	{
	for(a=2;a>0;a--);
	}
}

void delay(unsigned char i)
{
    while(i--);
}
/*start*/
void I2cStart(void)
{
	SDA=1;
	Delay10us();
	SCL=1;
	Delay10us();//建立时间是SDA保持时间>4.7us
	SDA=0;
	Delay10us();//保持时间是>4us
	SCL=0;			
	Delay10us();		
}
/*stop*/
void I2cStop(void)
{
	SDA=0;
	Delay10us();
	SCL=1;
	Delay10us();//建立时间大于4.7us
	SDA=1;
	Delay10us();		
}
unsigned char I2cSendByte(unsigned char dat)
{
    unsigned char a=0,b=0;//最大255，一个机器周期为1us，最大延时255us。		
	for(a=0;a<8;a++)//要发送8位，从最高位开始
	{
		SDA=dat>>7;	 //起始信号之后SCL=0，所以可以直接改变SDA信号
		dat=dat<<1;
		Delay10us();
		SCL=1;
		Delay10us();//建立时间>4.7us
		SCL=0;
		Delay10us();//时间大于4us		
	}
	SDA=1;
	Delay10us();
	SCL=1;
	while(SDA)//等待应答，也就是等待从设备把SDA拉低
	{
		b++;
		if(b>200)	 //如果超过2000us没有应答发送失败，或者为非应答，表示接收结束
		{
			SCL=0;
			Delay10us();
			return 0;
		}
	}
	SCL=0;
	Delay10us();
 	return 1;		
}

void oled_Write_cmd(unsigned char cmd)
{
	I2cStart();
	I2cSendByte(0x78);//发送写器件地址
	I2cSendByte(0x00);//发送要写入内存地址
	I2cSendByte(cmd);	//写入命令
	I2cStop();
}

void oled_Write_data(unsigned char dat)
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
void oled_display_char(int horizontal,int vertical,int order)
{
	int i;
	oled_put_char_16x16(horizontal,vertical,0);
	for(i=0;i<16;i++)
	{
	oled_Write_data(character[order][i]);
	}
}
void oled_display_number(int horizontal,int vertical,int order)
{
	int i;
	oled_put_char_16x16(horizontal,vertical,0);
	for(i=0;i<16;i++)
	{
	oled_Write_data(number[order][i]);
	}
}
void oled_display_init(void)
{
	/*cpu:*/
	oled_display_char(0,0,34);
	oled_display_char(2,0,47);
	oled_display_char(4,0,52);
	oled_display_char(6,0,25);
	/*time:*/
	oled_display_char(0,1,51);
	oled_display_char(1,1,40);
	oled_display_char(2,1,44);
	oled_display_char(3,1,36);
	oled_display_char(4,1,25);
	/*status:*/
	oled_display_char(0,2,50);
	oled_display_char(1,2,51);
	oled_display_char(2,2,32);
	oled_display_char(3,2,51);
	oled_display_char(4,2,36);
	oled_display_char(5,2,25);
}
void display_time(void)
{
	int hour,minute,second;
	hour=runtime/3600;
	minute=runtime%3600/60;
	second=runtime%60;
	oled_display_number(6,1,(hour/10));
	oled_display_number(7,1,(hour%10));
	oled_display_char(8,1,71);
	oled_display_number(9,1,(minute/10));
	oled_display_number(10,1,(minute%10));
	oled_display_char(11,1,76);
	oled_display_number(12,1,(second/10));
	oled_display_number(13,1,(second%10));
	oled_display_char(14,1,82);
}
void display_temperature(void)
{
	ReadTemperature();
	oled_display_number(7,0,(value/10));
	oled_display_number(9,0,(value%10));
}
/************************************************/
void Init_DS18B20(void) 
{
	DQ=1; 					
	delay(8); 				
	DQ=0; 					
	delay(80); 				
	DQ=1; 					
	delay(5);
	delay(20);  	
}
unsigned char ReadOneChar(void)  			//主机数据线先从高拉至低电平1us以上，再使数据线升为高电平，从而产生读信号
{
	unsigned char i=0; 		//每个读周期最短的持续时间为60us，各个读周期之间必须有1us以上的高电平恢复期
	unsigned char dat=0; 
	for (i=8;i>0;i--) 		//一个字节有8位 
	{
		DQ=1; 
		delay(1); 
		DQ=0;
		dat>>=1; 
		DQ=1; 
		if(DQ) 
		dat|=0x80; 
		delay(4);
	} 
	return(dat);
}
void WriteOneChar(unsigned char dat) 
{ 
	unsigned char i=0; 		//数据线从高电平拉至低电平，产生写起始信号。15us之内将所需写的位送到数据线上，
	for(i=8;i>0;i--) 		//在15~60us之间对数据线进行采样，如果是高电平就写1，低写0发生。 
	{
		DQ=0; 				//在开始另一个写周期前必须有1us以上的高电平恢复期。 
		DQ=dat&0x01; 
		delay(5); 
		DQ=1; 
		dat>>=1;
	} 
	delay(4);
}
void ReadTemperature(void) 
{
unsigned char a;
unsigned char b;
Init_DS18B20(); 
WriteOneChar(0xcc); 
WriteOneChar(0x44);
delay(125);
Init_DS18B20();
WriteOneChar(0xcc);
WriteOneChar(0xbe);
a=ReadOneChar();
b=ReadOneChar();
temperature=b;
temperature=a+(temperature<<8);
	if((temperature&0xf800)==0xf800)
	{
		temperature=(~temperature)+1;
		value=temperature*(-0.125);
	}
	else
	{
		value=temperature*0.125;	
	}	
}
void dispaly_state(void)
{
if(value<=60)
{
	oled_display_char(6,2,77);
	oled_display_char(7,2,78);
	oled_display_char(8,2,81);
	oled_display_char(9,2,76);
	oled_display_char(10,2,64);
	oled_display_char(11,2,75);
}
else
{
	oled_display_char(6,2,78);
	oled_display_char(7,2,85);
	oled_display_char(8,2,68);
	oled_display_char(9,2,81);
	oled_display_char(10,2,71);
	oled_display_char(11,2,68);
	oled_display_char(12,2,64);
	oled_display_char(13,2,83);
	oled_display_char(14,2,68);
	oled_display_char(15,2,67);
}
{
	
}
}