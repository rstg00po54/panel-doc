/********** 深圳市金逸晨电子有限公司 *************************************
********* ShenZhen GoldenMorning Electronics Co., Ltd. ****************
******************************************************************************
//  文 件 名   : main.c
//  版 本 号   : v1.0
//  作    者   :  zy
//  生成日期   : 2025-06-24
//  模组类型   : XL1.9 + ST7789
//  分 辨 率   : 170*320
//  功能描述   : 4线SPI 演示例程(STM32F103系列)
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC    3.3v电源
//              SCL     PB5
//              SDA     PB6
//              RESET   PB7
//              DC       PB8
//              CS        PB9
//              ----------------------------------------------------------------
// Copyright(C) 金逸晨电子
******************************************************************************/

#include "stm32f10x.h"
#include "led.h"
#include "key.h"  
#include "bmp.h"  

/**********SPI引脚分配，连接TFT屏，更具实际情况修改*********/

#define TFT_COLUMN_NUMBER 170
#define TFT_LINE_NUMBER 320
#define TFT_COLUMN_OFFSET 35//1
#define TFT_LINE_OFFSET 0//2
#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#define LCD_W 170
#define LCD_H 320

/**********SPI引脚分配，连接TFT屏，更具实际情况修改*********/

#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_4)//BLK
#define LCD_BLK_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_4)

#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_5)//SCL=SCLK
#define LCD_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_5)

#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_6)//SDA=MOSI
#define LCD_MOSI_Set() GPIO_SetBits(GPIOB,GPIO_Pin_6)

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_7)//RES
#define LCD_RES_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_7)

#define LCD_DC_Clr()   GPIO_ResetBits(GPIOB,GPIO_Pin_8)//DC
#define LCD_DC_Set()   GPIO_SetBits(GPIOB,GPIO_Pin_8)
 		     
#define LCD_CS_Clr()   GPIO_ResetBits(GPIOB,GPIO_Pin_9)//CS
#define LCD_CS_Set()   GPIO_SetBits(GPIOB,GPIO_Pin_9)


//#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_4)//BLK
//#define LCD_BLK_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_4)
// 		     
//#define LCD_CS_Clr()   GPIO_ResetBits(GPIOB,GPIO_Pin_5)//CS
//#define LCD_CS_Set()   GPIO_SetBits(GPIOB,GPIO_Pin_5)

//#define LCD_DC_Clr()   GPIO_ResetBits(GPIOB,GPIO_Pin_6)//DC
//#define LCD_DC_Set()   GPIO_SetBits(GPIOB,GPIO_Pin_6)

//#define LCD_RES_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_7)//RES
//#define LCD_RES_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_7)

//#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_8)//SDA=MOSI
//#define LCD_MOSI_Set() GPIO_SetBits(GPIOB,GPIO_Pin_8)

//#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_9)//SCL=SCLK
//#define LCD_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_9)




/*****************颜色数据**************************/
#define WHITE            0xFFFF
#define BLACK            0x0000   
#define BLUE             0x001F  
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define GREEN            0x07E0


unsigned int page = 0;
unsigned int autoplay = 0;



void delay_us(unsigned int _us_time)
{       
  unsigned char x=0;
  for(;_us_time>0;_us_time--)
  {
    x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;
	  x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;
	  
  }
}

void delay_ms2(unsigned int _ms_time)
  {
    unsigned int i,j;
    for(i=0;i<_ms_time;i++)
    {
    for(j=0;j<900;j++)
      {;}
    }
  }


void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOA
 	GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
}


/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	u8 i;
	LCD_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		LCD_SCLK_Clr();
		if(dat&0x80)
		{
		   LCD_MOSI_Set();
		}
		else
		{
		   LCD_MOSI_Clr();
		}
		LCD_SCLK_Set();
		dat<<=1;
	}	
  LCD_CS_Set();	
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据
}



 void LCD_Address_Set3(unsigned short int x_start,unsigned short int y_start,unsigned short int x_end,unsigned short int y_end)
{
	unsigned short int x = x_start + TFT_COLUMN_OFFSET,y=x_end+ TFT_COLUMN_OFFSET;
    LCD_WR_REG(0x2a);     //Column address set
    LCD_WR_DATA(x>>8);    //start column
    LCD_WR_DATA(x); 
    LCD_WR_DATA(y>>8);    //end column
    LCD_WR_DATA(y);
	x = y_start + TFT_LINE_OFFSET;
	y=y_end+ TFT_LINE_OFFSET;
    LCD_WR_REG(0x2b);     //Row address set
    LCD_WR_DATA(x>>8);    //start row
    LCD_WR_DATA(x); 
    LCD_WR_DATA(y>>8);    //end row
    LCD_WR_DATA(y);
    LCD_WR_DATA(0x2C);     //Memory write
    
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+TFT_COLUMN_OFFSET);
		LCD_WR_DATA(x2+TFT_COLUMN_OFFSET);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+TFT_LINE_OFFSET);
		LCD_WR_DATA(y2+TFT_LINE_OFFSET);
		LCD_WR_REG(0x2c);//储存器写
	return;
	
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//储存器写
	}
}

void LCD_Init(void)
{
	LCD_GPIO_Init();//初始化GPIO
	
	LCD_RES_Clr();//复位
	delay_ms2(100);
	LCD_RES_Set();
	delay_ms2(100);
	
	LCD_BLK_Set();//打开背光
  delay_ms2(100);
	
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); //Sleep out 
	delay_ms2(120);              //Delay 120ms 
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
                LCD_WR_REG(0x3A);
                LCD_WR_DATA8(0x05);
                LCD_WR_REG(0xC5);
                LCD_WR_DATA8(0x1A);
                LCD_WR_REG(0x36);
                LCD_WR_DATA8(0x00);
                //-------------ST7789V Frame rate setting-----------//
                LCD_WR_REG(0xb2);
                LCD_WR_DATA8(0x05);
                LCD_WR_DATA8(0x05);
                LCD_WR_DATA8(0x00);
                LCD_WR_DATA8(0x33);
                LCD_WR_DATA8(0x33);

                LCD_WR_REG(0xb7);
                LCD_WR_DATA8(0x05);
                //--------------ST7789V Power setting---------------//
                LCD_WR_REG(0xBB);
                LCD_WR_DATA8(0x3F);

                LCD_WR_REG(0xC0);
                LCD_WR_DATA8(0x2c);

                LCD_WR_REG(0xC2);
                LCD_WR_DATA8(0x01);

                LCD_WR_REG(0xC3);
                LCD_WR_DATA8(0x0F);

                LCD_WR_REG(0xC4);
                LCD_WR_DATA8(0x20);

                LCD_WR_REG(0xC6);
                LCD_WR_DATA8(0X01);

                LCD_WR_REG(0xd0);
                LCD_WR_DATA8(0xa4);
                LCD_WR_DATA8(0xa1);

                LCD_WR_REG(0xE8);
                LCD_WR_DATA8(0x03);

                LCD_WR_REG(0xE9);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x08);
                //---------------ST7789V gamma setting-------------//
                LCD_WR_REG(0xE0);
                LCD_WR_DATA8(0xD0);
                LCD_WR_DATA8(0x05);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x08);
                LCD_WR_DATA8(0x14);
                LCD_WR_DATA8(0x28);
                LCD_WR_DATA8(0x33);
                LCD_WR_DATA8(0x3F);
                LCD_WR_DATA8(0x07);
                LCD_WR_DATA8(0x13);
                LCD_WR_DATA8(0x14);
                LCD_WR_DATA8(0x28);
                LCD_WR_DATA8(0x30);
                 
                LCD_WR_REG(0XE1);
                LCD_WR_DATA8(0xD0);
                LCD_WR_DATA8(0x05);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x08);
                LCD_WR_DATA8(0x03);
                LCD_WR_DATA8(0x24);
                LCD_WR_DATA8(0x32);
                LCD_WR_DATA8(0x32);
                LCD_WR_DATA8(0x3B);
                LCD_WR_DATA8(0x14);
                LCD_WR_DATA8(0x13);
                LCD_WR_DATA8(0x28);
                LCD_WR_DATA8(0x2F);

                LCD_WR_REG(0x21);
               
                LCD_WR_REG(0x29);
} 



void LCD_Init2(void)
{
	LCD_BLK_Set();//打开背光
  delay_ms2(100);
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); //Sleep out 
	delay_ms2(120);              //Delay 120ms 
                LCD_WR_REG(0x3A);
                LCD_WR_DATA8(0x05);
                LCD_WR_REG(0xC5);
                LCD_WR_DATA8(0x1A);
                LCD_WR_REG(0x36);
                LCD_WR_DATA8(0x00);
                //-------------ST7789V Frame rate setting-----------//
                LCD_WR_REG(0xb2);
                LCD_WR_DATA8(0x05);
                LCD_WR_DATA8(0x05);
                LCD_WR_DATA8(0x00);
                LCD_WR_DATA8(0x33);
                LCD_WR_DATA8(0x33);

                LCD_WR_REG(0xb7);
                LCD_WR_DATA8(0x05);
                //--------------ST7789V Power setting---------------//
                LCD_WR_REG(0xBB);
                LCD_WR_DATA8(0x3F);

                LCD_WR_REG(0xC0);
                LCD_WR_DATA8(0x2c);

                LCD_WR_REG(0xC2);
                LCD_WR_DATA8(0x01);

                LCD_WR_REG(0xC3);
                LCD_WR_DATA8(0x0F);

                LCD_WR_REG(0xC4);
                LCD_WR_DATA8(0x20);

                LCD_WR_REG(0xC6);
                LCD_WR_DATA8(0X01);

                LCD_WR_REG(0xd0);
                LCD_WR_DATA8(0xa4);
                LCD_WR_DATA8(0xa1);

                LCD_WR_REG(0xE8);
                LCD_WR_DATA8(0x03);

                LCD_WR_REG(0xE9);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x08);
                //---------------ST7789V gamma setting-------------//
                LCD_WR_REG(0xE0);
                LCD_WR_DATA8(0xD0);
                LCD_WR_DATA8(0x05);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x08);
                LCD_WR_DATA8(0x14);
                LCD_WR_DATA8(0x28);
                LCD_WR_DATA8(0x33);
                LCD_WR_DATA8(0x3F);
                LCD_WR_DATA8(0x07);
                LCD_WR_DATA8(0x13);
                LCD_WR_DATA8(0x14);
                LCD_WR_DATA8(0x28);
                LCD_WR_DATA8(0x30);
                 
                LCD_WR_REG(0XE1);
                LCD_WR_DATA8(0xD0);
                LCD_WR_DATA8(0x05);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x09);
                LCD_WR_DATA8(0x08);
                LCD_WR_DATA8(0x03);
                LCD_WR_DATA8(0x24);
                LCD_WR_DATA8(0x32);
                LCD_WR_DATA8(0x32);
                LCD_WR_DATA8(0x3B);
                LCD_WR_DATA8(0x14);
                LCD_WR_DATA8(0x13);
                LCD_WR_DATA8(0x28);
                LCD_WR_DATA8(0x2F);

                LCD_WR_REG(0x21);
               
                LCD_WR_REG(0x29);
} 



/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
								color       要填充的颜色
      返回值：  无
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	LCD_Address_Set(xsta,ysta,xend-1,yend-1);//设置显示范围
	for(i=ysta;i<yend;i++)
	{													   	 	
		for(j=xsta;j<xend;j++)
		{
			LCD_WR_DATA(color);
		}
	} 					  	    
}




/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组    
      返回值：  无
******************************************************************************/
void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
{
	u16 i,j;
	u32 k=0;
	LCD_Address_Set(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			LCD_WR_DATA8(pic[k*2]);
			LCD_WR_DATA8(pic[k*2+1]);
			k++;
		}
	}			
}



void TFT_Page_1()
{
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);//设置显示范围
	
	int c = 0;
	int r = 0;

	int c2 = 0;
	int r2 = 0;

	int c3 = 0;
	int r3 = 0;

	for(r=0;r<LCD_H;r++)
	{
		for(c=0;c<LCD_W;c++)
		{
			c2 = (int)(c/32);
			r2 = (int)(r/32);
			c3 = c2 % 4;
			r3 = r2 % 5;

			if(r3 == 0){
				LCD_Writ_Bus(WHITE>>8);
				LCD_Writ_Bus(WHITE);
			}else if(r3 == 1){
				LCD_Writ_Bus(RED>>8);
				LCD_Writ_Bus(RED);
			}else if(r3 == 2){
				LCD_Writ_Bus(GREEN>>8);
				LCD_Writ_Bus(GREEN);
			}else if(r3 == 3){
				LCD_Writ_Bus(BLUE>>8);
				LCD_Writ_Bus(BLUE);
			}else if(r3 == 4){
				LCD_Writ_Bus(GRED>>8);
				LCD_Writ_Bus(GRED);
			}
			
		}
	}
}
	


void TFT_Page_2()
{
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);//设置显示范围
	
	int c = 0;
	int r = 0;

	int c2 = 0;
	int r2 = 0;

	int c3 = 0;
	int r3 = 0;

	for(r=0;r<LCD_H;r++)
	{
		for(c=0;c<LCD_W;c++)
		{
			c2 = (int)(c/10);
			r2 = (int)(r/10);
			c3 = c2 % 2;
			r3 = r2 % 2;

			if(c3 == r3){
				LCD_Writ_Bus(WHITE>>8);
				LCD_Writ_Bus(WHITE);
			}else{
				LCD_Writ_Bus(BLACK>>8);
				LCD_Writ_Bus(BLACK);
			}			
		}
	}
}



void Picture_display(u16 x,u16 y,u16 length,u16 width,const unsigned char *ptr_pic)
{
	u16 i,j;
	LCD_Address_Set(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{

      LCD_WR_DATA(*ptr_pic++);
			LCD_WR_DATA(*ptr_pic++);
		}
	}			
}


void Picture_display2()
{
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);

	u16 i,j, a = 0;
		
		a = 0;
		for(i=0;i<170;i++)
		{
			for(j=0;j<80;j++)
			{
				LCD_Writ_Bus(gImage_color[a+1]);
				LCD_Writ_Bus(gImage_color[a]);
				a=a+2;
			}
		}
		
		
		a = 0;
		for(i=0;i<170;i++)
		{
			for(j=0;j<80;j++)
			{
				LCD_Writ_Bus(gImage_color[a+1]);
				LCD_Writ_Bus(gImage_color[a]);
				a=a+2;
			}
		}
		

		
		a = 0;
		for(i=0;i<170;i++)
		{
			for(j=0;j<80;j++)
			{
				LCD_Writ_Bus(gImage_color[a+1]);
				LCD_Writ_Bus(gImage_color[a]);
				a=a+2;
			}
		}
		

		
		a = 0;
		for(i=0;i<170;i++)
		{
			for(j=0;j<80;j++)
			{
				LCD_Writ_Bus(gImage_color[a+1]);
				LCD_Writ_Bus(gImage_color[a]);
				a=a+2;
			}
		}
		
		
}

//静态页面测试
//int main(void)
//{
//	LCD_Init();
//	Picture_display2();
//}



//
int main(void)
{
	delay_ms2(100);
	
	LCD_Init();
	
	int speeds = 0;

	LED_GPIO_Config(); //LED 端口初始化   	
  Key_GPIO_Config();//按键端口初始化
	
	//LCD_ShowPicture(0,0,128,128,gImage_128);
//	TFT_Page_1();
//	return 0;
	
	page = 0;
	
  while(1)
  {
		if( Key_Scan(GPIOB,GPIO_Pin_12) == KEY_OFF  )	 //判断KEY1是否按下
		{
			LED1( OFF );
			page++;
			if(page > 8){ page = 1; }
			
			LCD_Init2();

			if(page == 1){
				LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
			}else if(page == 2){
				LCD_Fill(0,0,LCD_W,LCD_H,RED);
			}else if(page == 3){
				LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
			}else if(page == 4){
				LCD_Fill(0,0,LCD_W,LCD_H,GREEN);
			}else if(page == 5){
				LCD_Fill(0,0,LCD_W,LCD_H,BLUE);
			}else if(page == 6){
				TFT_Page_1();
			}else if(page == 7){
				TFT_Page_2();
			}else if(page == 8){
				Picture_display2();
			}else{
				page = 1;
			}
			delay_ms2(speeds);
		}else{
			LED1( ON );
		}
	}		

}



