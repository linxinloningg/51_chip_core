#include "reg52.h"
#include "character.h"
#include "stdio.h"
sbit led=P2^7;
sbit SCL=P1^7;
sbit SDA=P1^6;
sbit TRIG = P1^0; 
sbit ECHO = P3^2;
float S;
/**********I2C init**********i2c��������ú���*/
void I2cStart(void);
void I2cStop(void);
unsigned char I2cSendByte(unsigned char dat);
void oled_Write_cmd(unsigned char cmd);
void oled_Write_data(unsigned char dat);
void oled_clear(void);
void oled_put_char_16x16(unsigned char x,unsigned char y,unsigned char t);
/**********Oled Initialization configuration**********oled������ú������ֱ���oled��ʼ�����ú�oled��ʼ����ʾ���������ɵ�������˳��*/
void oled_initial(void);
void dispaly_init(void);
/**********Print characters and numbers***********��oled���ϴ�ӡ�ַ������ֵĺ�������Ӧcharacter�ڵ����飩*/
void oled_display_char(int horizontal,int vertical,int order);
void oled_display_number(int horizontal,int vertical,int order);
void dispaly_distance(void);
/**********delay**********��ʱ����*/
void delay(unsigned char i);
void delay_10us(unsigned int i);
/************ultrasonic**********������������ú���*/
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

void dispaly_distance(void)                 //��oled������ʾ����ĺ���
{	
	oled_display_number(0,2,((int)S/100));
	oled_display_number(2,2,((int)S%100/10));
	oled_display_number(4,2,((int)S%10));
}


/**********oled_init**********/
void I2cStart(void)                       //i2c��ʼʱ��
{
	SDA=1;
	delay_10us(1);
	SCL=1;
	delay_10us(1);//����ʱ����SDA����ʱ��>4.7us
	SDA=0;
	delay_10us(1);//����ʱ����>4us
	SCL=0;			
	delay_10us(1);		
}
/*stop*/
void I2cStop(void)						//i2cֹͣʱ��
{
	SDA=0;
	delay_10us(1);
	SCL=1;
	delay_10us(1);//����ʱ�����4.7us
	SDA=1;
	delay_10us(1);		
}
unsigned char I2cSendByte(unsigned char dat)            //����i2c�����ֽ����ݵĺ���
{
    unsigned char a=0,b=0;//���255��һ����������Ϊ1us�������ʱ255us��		
	for(a=0;a<8;a++)//Ҫ����8λ�������λ��ʼ
	{
		SDA=dat>>7;	 //��ʼ�ź�֮��SCL=0�����Կ���ֱ�Ӹı�SDA�ź�
		dat=dat<<1;
		delay_10us(1);
		SCL=1;
		delay_10us(1);//����ʱ��>4.7us
		SCL=0;
		delay_10us(1);//ʱ�����4us		
	}
	SDA=1;
	delay_10us(1);
	SCL=1;
	while(SDA)//�ȴ�Ӧ��Ҳ���ǵȴ����豸��SDA����
	{
		b++;
		if(b>200)	 //�������2000usû��Ӧ����ʧ�ܣ�����Ϊ��Ӧ�𣬱�ʾ���ս���
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

void oled_Write_cmd(unsigned char cmd)			//��װ�õ�oledдʹ�ܺ���
{
	I2cStart();
	I2cSendByte(0x78);//����д������ַ
	I2cSendByte(0x00);//����Ҫд���ڴ��ַ
	I2cSendByte(cmd);	//д������
	I2cStop();
}

void oled_Write_data(unsigned char dat)			//��װ�õ�oledд���ݺ���
{
	I2cStart();
	I2cSendByte(0x78);//����д������ַ
	I2cSendByte(0x40);//����Ҫд���ڴ��ַ
	I2cSendByte(dat);	//д������
	I2cStop();
}

void oled_clear(void)  //ҳѰַ�µ�oled��������		
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

void oled_initial(void)//oled��ʼ������
{
    delay(500);
			 
	oled_Write_cmd(0xae);//--turn off oled panel �ر���ʾ
    oled_Write_cmd(0x00);//---set low column address������ʼ�еĵ���λ 0x0x
    oled_Write_cmd(0x10);//---set high column address������ʼ�еĸ���λ0x1x
    oled_Write_cmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    oled_Write_cmd(0x81);//--set contrast control register���öԱȶ�
    oled_Write_cmd(0xff); // Set SEG Output Current Brightness�Աȶ�Ϊoxff
    oled_Write_cmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    oled_Write_cmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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
    oled_Write_cmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)���õ�ַģʽ
						//ˮƽѰַ����ֱѰַ��ҳѰַ
    oled_Write_cmd(0x02);//	��ַģʽΪҳѰַ
    oled_Write_cmd(0x8d);//--set Charge Pump enable/disable
    oled_Write_cmd(0x14);//--set(0x10) disable
    oled_Write_cmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    oled_Write_cmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
    oled_Write_cmd(0xaf);//--turn on oled panel������ʾ

	delay(100);
	oled_clear();//����
}

void oled_put_char_16x16(unsigned char x,unsigned char y,unsigned char t)
	/*������ʾ���꺯��,tΪ0ʱ���ַ�Ϊ8x16
					tΪ1ʱ���ַ�Ϊ16x16*/                 
{	 
     oled_Write_cmd(0x20);
	 oled_Write_cmd(0x00);//���õ�ַģʽΪˮƽѡַ
     //set page
     oled_Write_cmd(0x22);
	 oled_Write_cmd(y*2);
	 oled_Write_cmd(0x01+y*2);

	 //set colum
     oled_Write_cmd(0x21);
	 oled_Write_cmd((0x08+0x08*t)*x);
	 oled_Write_cmd((0x08+0x08*t)*x+(0x07+0x08*t));
}
void dispaly_init(void)							//����ɸ���������Ҫ���ģ�����oled����ʼ��ʾ�ĺ���
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
void oled_display_char(int horizontal,int vertical,int order)				//oled��ӡ�ַ��ĺ���
{
	int i;
	oled_put_char_16x16(horizontal,vertical,0);
	for(i=0;i<16;i++)
	{
	oled_Write_data(character[order][i]);
	}
}	
void oled_display_number(int horizontal,int vertical,int order)				//oled��ӡ���������ֵĺ���
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
void delay(unsigned char i)														//��ʱ����
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
void Time0_Init(void)									//��ʱ��0���ú��������ö�ʱ0����ߵ�ƽʱ��			
{
TMOD=0x01; 
TH0=0;
TL0=0;
ET0=1; 
TR0=1; 
EA=1; 
}
void ultrasonic_init(void)								//��������ʼ������
{
	Time0_Init();
	TRIG=0;
	ECHO=0;
}
void StartModule(void) 									//��������ʼ�ź�
 {
 	TRIG = 1;
	delay_10us(2);
	TRIG = 0;
 }
void Conut(void) 										//�������ĺ���
{	
	unsigned int time;
	time=TH0*256+TL0;
	TH0=0;
	TL0=0;
	S=(time*1.7)/100;
}
void ultrasonic_distance(void)							//�ж��������źŲ���������������
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
