#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "image2lcd.h"
#include "mmc_sd.h"



/*  如果需要更换成别的CPU，只需要定义好IO口，其他程序可以照搬使用

*/

//以下为主函数入口
 int main(void)
 {	 
 	u8 x=0;
	//SCB->VTOR = FLASH_BASE | 0x1000; //如果使用BOOTLOADER 
	delay_init();	    	     //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	   //串口初始化为115200
	LCD_Init();
	POINT_COLOR=RED;	
#if 0 //不需要用到SD卡则屏蔽
  while(SD_Initialize())//检测不到SD卡
	{
		LCD_CS=1;
		LCD_ShowString(60,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(60,150,200,16,16,"Please Check! ");
		delay_ms(500);
	}
	 LCD_ShowString(60,150,200,16,16,"SD Card OK ");
	 delay_ms(500);
#endif
  while(1) 
	{	
#if 1		
		switch(x)
		{

			case 1:LCD_Clear(WHITE);break;
			case 2:LCD_Clear(BLACK);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(GREEN);break;
			case 5:LCD_Clear(BLUE);break;

		}
#endif
		POINT_COLOR=RED;	
		LCD_ShowString(30,70,200,16,16,"TFTLCD TEST");	    					 
	  x++;
		if(x==9)x=0;	 
		delay_ms(1000);	
	} 
}
