#include "reg51.h"
#include "character.h"
sbit led=P2^0;
sbit led_usart=P2^7;
sbit SCL=P1^7;
sbit SDA=P1^6;
//unsigned char segment[][15];
/**********I2C init**********i2c???????*/
void I2cStart(void);
void I2cStop(void);
unsigned char I2cSendByte(unsigned char dat);
void oled_Write_cmd(unsigned char cmd);
void oled_Write_data(unsigned char dat);
void oled_clear(void);
/**********Oled Initialization configuration***********/
void oled_initial(void);
/**********Print characters and numbers************/
/**********delay***********/
void delay(unsigned char i);
void delay_10us(unsigned int i);
void delay_ms(unsigned int ms);
//void UART_init(unsigned int baud_rate);
void oled_setpos(unsigned char x,unsigned char y);
void oled_showpictues(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
int main()
{
	//UART_init(19200);
	oled_initial();
	//oled_showpictues(0,0,128,64,BMP_1);
	//oled_showpictues(0,0,128,64,BMP_2);
	//led_showpictues(0,0,128,64,BMP_3);
	//oled_showpictues(0,0,128,64,BMP_4);
	//oled_showpictues(0,0,128,64,BMP_5);
	oled_showpictues(0,0,64,64,pictures);
	while(1)
	{
	//oled_showpictues(0,0,64,64,pictures);
	oled_showpictues(64,0,128,64,pictures_1);
		delay_ms(500);
	oled_showpictues(64,0,128,64,pictures_2);
		delay_ms(500);
	led=0;
	}
}
/**********oled_init**********/
void I2cStart(void)                       //i2c????
{
	SDA=1;
	delay_10us(1);
	SCL=1;
	delay_10us(1);//?????SDA????>4.7us
	SDA=0;
	delay_10us(1);//?????>4us
	SCL=0;			
	delay_10us(1);		
}
/*stop*/
void I2cStop(void)						//i2c????
{
	SDA=0;
	delay_10us(1);
	SCL=1;
	delay_10us(1);//??????4.7us
	SDA=1;
	delay_10us(1);		
}
unsigned char I2cSendByte(unsigned char dat)            //??i2c?????????
{
    unsigned char a=0,b=0;//??255,???????1us,????255us?		
	for(a=0;a<8;a++)//???8?,??????
	{
		SDA=dat>>7;	 //??????SCL=0,????????SDA??
		dat=dat<<1;
		delay_10us(1);
		SCL=1;
		delay_10us(1);//????>4.7us
		SCL=0;
		delay_10us(1);//????4us		
	}
	SDA=1;
	delay_10us(1);
	SCL=1;
	while(SDA)//????,?????????SDA??
	{
		b++;
		if(b>200)	 //????2000us????????,??????,??????
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

void oled_Write_cmd(unsigned char cmd)			//????oled?????
{
	I2cStart();
	I2cSendByte(0x78);//???????
	I2cSendByte(0x00);//?????????
	I2cSendByte(cmd);	//????
	I2cStop();
}

void oled_Write_data(unsigned char dat)			//????oled?????
{
	I2cStart();
	I2cSendByte(0x78);//???????
	I2cSendByte(0x40);//?????????
	I2cSendByte(dat);	//????
	I2cStop();
}

void oled_clear(void)  //?????oled????		
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

void oled_initial(void)//oled?????
{
    delay(500);
			 
	oled_Write_cmd(0xae);//--turn off oled panel ????
    oled_Write_cmd(0x00);//---set low column address????????? 0x0x
    oled_Write_cmd(0x10);//---set high column address?????????0x1x
    oled_Write_cmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    oled_Write_cmd(0x81);//--set contrast control register?????
    oled_Write_cmd(0xff); // Set SEG Output Current Brightness????oxff
    oled_Write_cmd(0xa1);//--Set SEG/Column Mapping     0xa0???? 0xa1??
    oled_Write_cmd(0xc8);//Set COM/Row Scan Direction   0xc0???? 0xc8??
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
    oled_Write_cmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)??????
						//????,????,???
    oled_Write_cmd(0x02);//	????????
    oled_Write_cmd(0x8d);//--set Charge Pump enable/disable
    oled_Write_cmd(0x14);//--set(0x10) disable
    oled_Write_cmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    oled_Write_cmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
    oled_Write_cmd(0xaf);//--turn on oled panel????

	delay(100);
	oled_clear();//??
}
void oled_setpos(unsigned char x,unsigned char y)
{
	oled_Write_cmd(0xb0+y);
	oled_Write_cmd((((x+2)&0xf0)>>4)|0x10);
	oled_Write_cmd(((x+2)&0x0f));
}
void oled_showpictues(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int i=0;
	unsigned char x,y;
	//if(y1%8==0) y=y1/8;
	//else y=y1/8+1;
	y1/=8;
	for(y=y0;y<y1;y++)
	{
		oled_setpos(x0,y);
		for(x=x0;x<x1;x++)
	    {oled_Write_data(BMP[i++]);}
	}
}
/**********dealy*********/
void delay(unsigned char i)														//????
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
/*void UART_init(unsigned int baud_rate)
{
	EA=1;
	PCON=0x80;
	SCON=0x50;
	TMOD=0x20;
	switch(baud_rate)
	{
		case 1200:TH1=0xD0;TL1=0xD0;break;
		case 2400:TH1=0xE8;TL1=0xE8;break;
		case 4800:TH1=0xF4;TL1=0xF4;break;
		case 9600:TH1=0xFA;TL1=0xFA;break;
		case 19200:TH1=0xFD;TL1=0xFD;break;
	}
	TR1=1;//?????1
}*/
/*void UART_interrupt() interrupt 4
{
	unsigned char i;
	static unsigned char Row=0;
	static unsigned char Column=0;
	static unsigned char receive_data[16];
	if(RI==1)
	{
		receive_data[Column]=SBUF;
		++Column;
		if(Column==16)
		{
			Column=0;
			for(i=0;i<16;++i) segment[Row][i]=receive_data[i];
		}
		Row++;
		if(Row==64)
		RI=0;
	}
	led_usart=~led_usart;
}*/
void delay_ms(unsigned int ms)                
{
        unsigned int i,j;
        for(j=0;j<ms;j++)
                for(i=0;i<1000;i++);
}