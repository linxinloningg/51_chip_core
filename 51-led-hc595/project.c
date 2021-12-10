#include "reg51.h"
#include "intrins.h"
#include "stdlib.h"
sbit led=P1^7;
sbit led_g=P1^4;
sbit SER= P3^4;    
sbit SRCLK=P3^6;    
sbit RCLK=P3^5;
void appear(void);
void delay(unsigned long t);
void HC_595(unsigned char date);
void snake_init(void);
void auto_move(void);
void random_berry(void);
void Eating_berry(void);
void judgment(void);
unsigned char length=3;
unsigned char berry_x;			
unsigned char berry_y;
unsigned char x[32]={0},y[32]={0};
unsigned char flag_repaeat=0;
unsigned char code X_[8]={0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE};	 
unsigned char code Y_[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};	
unsigned char flag_eating=0;	  
char flag_x=1;		   
char flag_y=0;
unsigned char flag_death=0;
void EX_INIT()
{
	IT0=1;	
	EX0=1;	
													  
	IT1=1; 
	EX1=1;	
 
	EA =1;
}
void Timer0Init()				  
{
	TMOD|=0X01;
	TH0=(65536-1000)/256;	
	TL0=(65536-1000)/256;	
	ET0=1;
	EA=1;
	TR0=1;	
}
void Timer1Init()				  
{
	TMOD|=0X01;
	TH1=(65536-1000)/256;	
	TL1=(65536-1000)/256;	
	ET1=1;
	EA=1;
	TR1=1;	
}
void Timer1(void) interrupt 3
{
}
void Timer0(void) interrupt 1
{
	P0 = X_[berry_x-1];
	HC_595(Y_[berry_y-1]);
}
void EX1_SER(void) interrupt 0
{
	if(flag_y==1)      //up
	{flag_x=-1;     //left
	 flag_y=0;}
	else if(flag_y==-1) //down
	{flag_x=1;     //right
	 flag_y=0;}	
	else if(flag_x==-1) //left
	{flag_x=0;		//down
	flag_y=-1;}
	else if(flag_x==1)   //right
	{flag_x=0;			//up
	flag_y=1;}
}
void EX2_SER(void) interrupt 2
{
	if(flag_y==1)	//up
	{flag_x=1;     //right
	 flag_y=0;}
	
	else if(flag_y==-1) //down
	{flag_x=-1;     //left
	 flag_y=0;}
	
	else if(flag_x==-1) //left
	{flag_x=0;		//up
	flag_y=1;}
	
	else if(flag_x==1)   //right
	{flag_x=0;			//down			
	flag_y=-1;}	
}	
int main()
{
	EX_INIT();
	snake_init();
	Timer0Init();
	Timer1Init();
	random_berry();
	while(1)
	{
				  if(!flag_death)	  
				    {
						appear();
						auto_move();		 	
						Eating_berry();
						judgment();
					}
		led=~led;
	}
}
void appear(void)
{
	unsigned char i;
	P0=0xFF;		 
	HC_595(0x00);
	for(i=0;i<length;i++)					 
	{
		if((x[i]!=0)&&(y[i]!=0))
		{
			HC_595(Y_[y[i]-1]);
			P0=X_[x[i]-1];
			delay(5);		        
		}
	}
}
void delay(unsigned long t)
{
 	unsigned int x,y;
	for(x=t;x>0;x--)
	for(y=110;y>0;y--);
}
void auto_move(void)		
{	
	unsigned char i;					
	for(i=0;i<(length-1);i++)
	{
		x[i]=x[i+1];
		y[i]=y[i+1];
	}
		x[length-1]+=flag_x;
		y[length-1]+=flag_y;
			if(x[length-1]>8)			
				x[length-1] = 1;
				else if(x[length-1]<1)
					x[length-1] = 8;				
			if(y[length-1]>8)
				y[length-1] = 1;
				else if(y[length-1]<1)
					y[length-1] = 8;
	delay(500);
}
void HC_595(unsigned char date)				 
{	
	unsigned char i;
	SRCLK = 0;
	RCLK = 0;

	for(i=0;i<8;i++)
	{
		SER = date>>7;
		date<<=1;
		SRCLK = 1;
		_nop_();
		_nop_();
		SRCLK = 0;
	}
	RCLK = 1;
		_nop_();
		_nop_();
	RCLK = 0;
}
void snake_init(void)
{
	x[0] = 2;			   
	y[0] = 3;
	x[1] = 3;
	y[1] = 3;
	x[2] = 4;
	y[2] = 3;
}
void random_berry(void)						
{
	unsigned char i,random_x,random_y;		
	do{
		flag_repaeat=0;
		random_x=(rand()%8)+1;	   				
		random_y=(rand()%8)+1;
		for(i=0;i<length;i++)
		  {	
			if((random_x==x[i])&&(random_y==y[i]))	                     
					flag_repaeat = 1;		
		  }
		}while(flag_repaeat);		
	berry_x=random_x;
	berry_y=random_y;
}
void Eating_berry(void)			
{
	if(((x[length-1]+flag_x)==berry_x)&&((y[length-1]+flag_y)==berry_y)  )
					{flag_eating=1;}
	else if((x[length-1]==berry_x)&&(y[length-1]==berry_y))
					{flag_eating=1;}
	if(flag_eating)
	{
		flag_eating=0;
		TR0 = 0;
		length++;
		x[length-1]=berry_x+flag_x;
		y[length-1]=berry_y+flag_y;
        random_berry();  
		TR0=1;
	}
}
void judgment(void)
{
	unsigned char i;
	if(length>4)	
	{
	for(i=0;i<length-4;i++)				   
	  {	
		if(((x[length-1])==x[i])&&((y[length-1])==y[i])  )  
				 {
					flag_death=1; 
				 }
	  }
	if(flag_death==1)
		{
		HC_595(0x00);
		P0=0xff;
		led_g=~led_g;
		}
	}
}