#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "sdram.h"
#include "malloc.h"
#include "w25qxx.h"
#include "ff.h"
#include "exfuns.h"
#include "string.h"
#include "sdio_sdcard.h"
#include "fontupd.h"
#include "text.h"
#include "ds18b20.h"
#include "pcf8574.h"
#include "timer.h"
#include "key.h"
void TIM_SetTIM3Compare4(u32 compare);
void TIM3_Init(u16 arr,u16 psc);

int main(void)
{	
	
	  u8 dir=1;
    u16 led0pwmval=0;  
		u8 m=0;			    
	short temperature; 
		u8 key,t;
    
    HAL_Init();                    
    Stm32_Clock_Init(360,25,2,8);   
    delay_init(180);                
    uart_init(115200);              
    LED_Init();                     
	  KEY_Init();  
	  TIM3_PWM_Init(500-1,90-1);     
    SDRAM_Init();                  
    LCD_Init();                     
	  PCF8574_Init();                 
	W25QXX_Init();				   
    my_mem_init(SRAMIN);           
    my_mem_init(SRAMEX);         
    my_mem_init(SRAMCCM);         
    exfuns_init();		            
    f_mount(fs[0],"0:",1);          
  	f_mount(fs[1],"1:",1);          
	
	    PCF8574_ReadBit(BEEP_IO);       
	while(font_init()) 		        
	{
  
		key=update_font(20,110,16,"0:");
		while(key)
		{		
			 LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
			 delay_ms(200);
			 LCD_Fill(20,110,200+20,110+16,WHITE);
			 delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);	       
	} 

  POINT_COLOR=GRAYBLUE;  
	Show_Str(30,150,200,32,"DS18B20 ����",24,0);

	POINT_COLOR=RED;       
	Show_Str(170,30,400,32,"о����;�����AI��",32,0);	
  POINT_COLOR=BROWN;  	
	Show_Str(230,70,200,32,"2025Ƕ��ʽ����",24,0);				    	 
	POINT_COLOR=BLACK; 
	Show_Str(400,500,200,32,"2025��6��25��",24,0);
	POINT_COLOR=BLUE; 
	Show_Str(250,100,400,32,"���BLUE",32,0);
	POINT_COLOR=DARKBLUE;  
	LCD_ShowString(30,300,200,32,32,"Temp:   . C");	
	while(1)
	{
		
    if(m%10==0)//ÿ100ms��ȡһ��
		{		
			PCF8574_ReadBit(BEEP_IO);   
		    temperature=DS18B20_Get_Temp();	
			if(temperature<0)
			{
				LCD_ShowChar(30+40,190,'-',16,0);			//��ʾ����
				temperature=-temperature;					//תΪ����
			}else LCD_ShowChar(30+40,190,' ',16,0);			//ȥ������
			LCD_ShowNum(120,300,temperature/10,2,32);	//��ʾ��������	    
			LCD_ShowNum(170,300,temperature%10,1,32);	//��ʾС������ 		   
		}				   
		delay_ms(10);
		t++;
		if(m==20)
		{
			m=0;
			LED0=!LED0;
		}
		
		
		if(temperature>330)
     {	
			 POINT_COLOR=RED;
			Show_Str(180,450,400,32,"����Ԥ������ֹͣ���ȣ�",24,0);
			 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);
		 }
		 else
		 {
			 POINT_COLOR=DARKBLUE;
			 Show_Str(180,450,400,32,"����Ԥ�������������ȣ�",24,0);
			 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);
		 }

			key=KEY_Scan(0);            //����ɨ��
				switch(key)
				{				 
					
					case  KEY2_PRES:	
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
					      Show_Str(180,400,400,32,"ˮ��״̬��������",24,0);
								break;
					case  KEY1_PRES: 	
								while(dir==1)
								{
									led0pwmval++;
									if(led0pwmval>100)dir=0;
									TIM_SetTIM3Compare4(led0pwmval);
								}
								break;
					case  KEY0_PRES:	
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
					      Show_Str(180,400,400,32,"ˮ��״̬���رգ�",24,0);
								break;
			}
        delay_ms(10);		 
		if(dir==0)led0pwmval--;				
		if(led0pwmval==0)dir=1;			
		TIM_SetTIM3Compare4(led0pwmval);	
		 
		}
	}
