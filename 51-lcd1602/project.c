#include "reg51.h"
#include "string.h"
sbit SCL=P2^0;
sbit SDA=P2^1;
void delay_ms(unsigned int ms);													
void delay_10us(unsigned int i);
/**********I2C init***********/
void I2cStart(void);
void I2cStop(void);
unsigned char I2cSendByte(unsigned char dat);
/********************/
void lcd_init(void);
void write_cmd(unsigned char cmd);
void write_data(unsigned char DData);
void dispaly(unsigned char X, unsigned char Y, unsigned char DData);
void dispaly_character(unsigned char X, unsigned char Y, unsigned char* Arry);
unsigned char code arry[]="successfully!";
int main()
{
	lcd_init();
	while(1)
	{
	dispaly_character(3,0,arry);		
	}
}
void I2cStart(void)                       
{
	SDA=1;
	delay_10us(1);
	SCL=1;
	delay_10us(1);
	SDA=0;
	delay_10us(1);
	SCL=0;			
	delay_10us(1);		
}
/*stop*/
void I2cStop(void)						
{
	SDA=0;
	delay_10us(1);
	SCL=1;
	delay_10us(1);
	SDA=1;
	delay_10us(1);		
}
unsigned char I2cSendByte(unsigned char dat)            
{
    unsigned char a=0,b=0;
	for(a=0;a<8;a++)
	{
		SDA=dat>>7;	 
		dat=dat<<1;
		delay_10us(1);
		SCL=1;
		delay_10us(1);
		SCL=0;
		delay_10us(1);
	}
	SDA=1;
	delay_10us(1);
	SCL=1;
	while(SDA)
	{
		b++;
		if(b>200)	 
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
void delay_ms(unsigned int ms)                
{
        unsigned int i,j;
        for(j=0;j<ms;j++)
                for(i=0;i<1000;i++);
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
void write_cmd(unsigned char cmd)                
{       
        I2cStart();                                                        
        I2cSendByte(0x7c);                                        
        I2cSendByte(0x80);                                        
        I2cSendByte(cmd);
        I2cStop();                                                               
}
void lcd_init(void)                                        
{
        write_cmd(0X38);                                      
        write_cmd(0x0c);                                        
        write_cmd(0x01);                                        
        delay_ms(2);                                                      
}
void dispaly(unsigned char X, unsigned char Y, unsigned char DData)
{
	Y&=0x1;
	X&=0xF; 
	if(Y) 
	X|=0x40; 
	X|=0x80;
	write_cmd(X);
	write_data(DData);
}
void write_data(unsigned char DData)
{
	I2cStart(); 
	I2cSendByte(0x7c);
	I2cSendByte(DData);
	I2cSendByte(0x40);
	I2cSendByte(DData);
	I2cStop();
}	
void dispaly_character(unsigned char X, unsigned char Y, unsigned char* Arry)
{
	unsigned int i;
	unsigned int length=strlen(Arry);
	for(i=0;i<length;i++)
	{
	dispaly(X,Y,Arry[i]);
	X++;
	if(X>16)
	{Y+=1;X=0;}	
	}
}	