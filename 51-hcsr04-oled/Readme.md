#### 用oled将HC-SR04所测数据进行打印

>#### 硬件电路分析：
>
>1.0.96英寸oled共VCC，GND，SCL，SDL四个引脚，分解接入单片机VCC，GND，P1^6和P1^7
>
>2.超声波测距模块相关接法请参考上一篇文章
>
>3.led点亮提示系统正常运行
>
>#### 显示距离
>
>唯一稍有不同的可能是显示距离的oled配置函数
>
>在之前的博文中显示的是温度，在这里我们显示的是所测的距离
>
>查阅上一篇博文我们可以得知，所测的距离都存放在全局变量S中
>
>因此：
>
>void dispaly_distance(void)                 //在oled屏上显示距离的函数
>{	
>	oled_display_number(0,2,((int)S/100));
>	oled_display_number(2,2,((int)S%100/10));
>	oled_display_number(4,2,((int)S%10));
>}
>
>只要将S进行显示出来即可，（int）是强制类型转换，如果你的S是浮点类型的则需此操作，若是整型，则不影响
>
>#### 主函数的相关配置
>
>int main(void)
>{
>	oled_initial();
>	dispaly_init();
>	ultrasonic_init();
>	while(1)
>	{
>	ultrasonic_distance();	
>	dispaly_distance();
>	led=0;
>	}
>}
>