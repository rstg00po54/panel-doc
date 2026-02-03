#include "ssd2828.h"

//LCD 视频显示参数，根据LCD不同更改
//----  Bei li jia -----//
#define LCD_VBPD		 5
#define LCD_VFPD	 	 30
#define LCD_VSPW		 2

#define LCD_HBPD		 100
#define LCD_HFPD		 60
#define LCD_HSPW	   	 4

#define LCD_XSIZE_TFT    1080 
#define LCD_YSIZE_TFT    1920

//----  END  -------//

void SSD2828_GPIO_Initial(void)
{
//初始化GPIO,配置SSD2828 SPI 3线模式
            GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_OUT_PP_HIGH_FAST);//PC5，推挽输出  SSD2828 SPI_SCLK
            GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_IN_PU_NO_IT);     //PC7，输入引脚  SSD2828 SPI_SDI,对应SSD2828芯片的SDO
            
            GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_HIGH_FAST);//PC6，推挽输出  SSD2828 SPI_SDO,对应SSD2828芯片的SDI
            GPIO_Init(GPIOE,GPIO_PIN_5,GPIO_MODE_OUT_PP_HIGH_FAST);//PE5，推挽输出  SSD2828 SPI_CS, PE5
            GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_OUT_PP_HIGH_FAST);//PC4，推挽输出  SSD2828 SPI_SDC, PC4
            GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_OUT_PP_HIGH_FAST);//PC3，推挽输出  SSD2828 SSD_SHUT, PC3
            GPIO_Init(GPIOD,GPIO_PIN_0,GPIO_MODE_OUT_PP_HIGH_FAST);//PB7，推挽输出  SSD2828 SSD_RST, PD0
	    GPIO_Init(GPIOB,GPIO_PIN_0,GPIO_MODE_OUT_PP_HIGH_FAST);//PB0，推挽输出  LCD RST, PB0

}

void SSD2828_Init(void)
{
SSD_RST_L;
LCD_RST_L;
SSD_SHUT_L;
SSD_SPI_SDC_L;
SSD_SPI_CS_H;
SSD_SPI_SCLK_L;
SSD_SPI_SDO_L;

SSD_RST_H;
LCD_RST_L;
SSD_SHUT_L;
SSD_SPI_SDC_H;

}


void SPI_3W_SET_Cmd(u8 cmd)
{
	u8 kk;
	
	SSD_SPI_SDO_L;			//Set DC=0, for writing to Command register
	SSD_SPI_SCLK_L;
	SSD_SPI_SCLK_H;
	
	SSD_SPI_SCLK_L;
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) SSD_SPI_SDO_H;
		else         SSD_SPI_SDO_L;
		SSD_SPI_SCLK_H;
		SSD_SPI_SCLK_L;
		cmd = cmd<<1;	
	}
}


void SPI_3W_SET_PAs(u8 value)
{
	u8 kk;

	SSD_SPI_SDO_H;			//Set DC=1, for writing to Data register
	SSD_SPI_SCLK_L;
	SSD_SPI_SCLK_H;
//	delay_us(1);	
	SSD_SPI_SCLK_L;
	for(kk=0;kk<8;kk++)
	{
		if((value&0x80)==0x80) SSD_SPI_SDO_H;
		else         SSD_SPI_SDO_L;
		SSD_SPI_SCLK_H;
		SSD_SPI_SCLK_L;
		value = value<<1;	
	}	
}
//-----------------------------------------------------
void Write_com(u16 vv)
{
	SSD_SPI_CS_L;
	SPI_3W_SET_Cmd(vv&0xff);	
}

void Write_register(u16 vv)
{
	SPI_3W_SET_PAs(vv&0xff);
	SPI_3W_SET_PAs((vv>>8)&0xff);	
}
//-----------------------------------------------------

void writec(u8 cmd)
{
	SSD_SPI_CS_L;
	SPI_3W_SET_Cmd(cmd);	
	SSD_SPI_CS_H;

}

void writed(u8 value)
{
	SSD_SPI_CS_L;
  SPI_3W_SET_PAs(value);
	SSD_SPI_CS_H;
}

void SPI_2828_WrCmd(u8 cmd)
{
	u8 i;
	
	SSD_SPI_CS_L;
	
	SSD_SPI_SDO_L;			//Set DC=0, for writing to Command register
	SSD_SPI_SCLK_L;
	SSD_SPI_SCLK_H;
	

	SSD_SPI_SCLK_L;
	for(i=0;i<8;i++)
	{
		if((cmd&0x80)==0x80) 
			SSD_SPI_SDO_H;
		else       
			SSD_SPI_SDO_L;
		
		SSD_SPI_SCLK_H;
		SSD_SPI_SCLK_L;
		cmd = cmd<<1;	
	}
	
	SSD_SPI_CS_H;	
}

//void SPI_2828_WrCmd(u8)
void SPI_2828_WrReg(u8 c,u16 value)
{
	SSD_SPI_CS_L;
	SPI_3W_SET_Cmd(c);
	SPI_3W_SET_PAs(value&0xff);
	SPI_3W_SET_PAs((value>>8)&0xff);	
	SSD_SPI_CS_H;	
}

u8 Read_PAs(void)
{
	u8 i;
	u8  rdValue;

//	SSD_SPI_SCLK_L;
	rdValue = 0;
	for(i=0;i<8;i++)
	{
		rdValue = rdValue<<1;
		
		SSD_SPI_SCLK_H;
		if(SSD_SPI_SDI==1)  rdValue |= 0x01;
		SSD_SPI_SCLK_L;
	}
//	Set_CSX(0);

	return rdValue;	
}

u16 SPI_READ(void)//SSD2828 ID Read 
{
	u8  cmd,rdT;
	u16 reValue;
	u8 i;
	
	SSD_SPI_CS_L;

	SSD_SPI_SDO_L;			//Set DC=0, for writing to Command register
	SSD_SPI_SCLK_L;
	SSD_SPI_SCLK_H;
	

	cmd = 0xB0;
	SSD_SPI_SCLK_L;
	for(i=0;i<8;i++)
	{
		if((cmd&0x80)==0x80) SSD_SPI_SDO_H;
		else         SSD_SPI_SDO_L;
		SSD_SPI_SCLK_H;
		SSD_SPI_SCLK_L;
		cmd = cmd<<1;	
	}
	
	SSD_SPI_SDO_L;			//Set DC=0, for writing to Command register
	SSD_SPI_SCLK_L;
	SSD_SPI_SCLK_H;
	

	cmd = 0xFA;
	SSD_SPI_SCLK_L;
	for(i=0;i<8;i++)
	{
		if((cmd&0x80)==0x80) SSD_SPI_SDO_H;
		else         SSD_SPI_SDO_L;
		SSD_SPI_SCLK_H;
		SSD_SPI_SCLK_L;
		cmd = cmd<<1;	
	}	
	
	rdT=0;
	for(i=0;i<8;i++)
	{
		rdT = rdT<<1;
		SSD_SPI_SCLK_H;
		if(SSD_SPI_SDI==0) 
                  rdT |= 0x00;
                else
                  rdT |= 0x01;
                
		SSD_SPI_SCLK_L;				
	}
	
	reValue = rdT;
	//reValue = (reValue<<8)&0xFF00;
	
	rdT=0;
	for(i=0;i<8;i++)
	{
		rdT = rdT<<1;
		SSD_SPI_SCLK_H;
		if(SSD_SPI_SDI==0) 
                  rdT |= 0x00;
                else
                  rdT |= 0x01;
                
		SSD_SPI_SCLK_L;				
	}
	
	reValue += (rdT<<8);
	
	SSD_SPI_CS_H;
	
	return reValue;			
}

u8   SPI_READ_ID(void)
{
	//SPI_2828_WrReg(0xd4, 0x00FA);?
	u16 Read_ID;
	u8 Flag;
	//Read_ID=0x2825;
	Read_ID=SPI_READ();
	printf("The Chip ID is : 0x%x\n",Read_ID);
	if(Read_ID == 0x2828)
			 Flag=1;
		else
			 Flag=0;
	return Flag;
}

u16 SSD2828_READ(u8 reg)//SSD2828 Read Reg 
{
	u8  cmd,rdT;
	u16 reValue;
	u8 i;
	
	SSD_SPI_CS_L;

	SSD_SPI_SDO_L;			//Set DC=0, for writing to Command register
	SSD_SPI_SCLK_L;
	SSD_SPI_SCLK_H;
	

	cmd = reg;//cmd = 0xB0;//read address
	SSD_SPI_SCLK_L;
	for(i=0;i<8;i++)
	{
		if((cmd&0x80)==0x80) SSD_SPI_SDO_H;
		else         SSD_SPI_SDO_L;
		SSD_SPI_SCLK_H;
		SSD_SPI_SCLK_L;
		cmd = cmd<<1;	
	}
	
	SSD_SPI_SDO_L;			//Set DC=0, for writing to Command register
	SSD_SPI_SCLK_L;
	SSD_SPI_SCLK_H;
	

	cmd = 0xFA;
	SSD_SPI_SCLK_L;
	for(i=0;i<8;i++)
	{
		if((cmd&0x80)==0x80) SSD_SPI_SDO_H;
		else         SSD_SPI_SDO_L;
		SSD_SPI_SCLK_H;
		SSD_SPI_SCLK_L;
		cmd = cmd<<1;	
	}	
	
	rdT=0;
	for(i=0;i<8;i++)
	{
		rdT = rdT<<1;
		SSD_SPI_SCLK_H;
		
                if(SSD_SPI_SDI==0) 
                  rdT |= 0x00;
                else
                  rdT |= 0x01;
                
		SSD_SPI_SCLK_L;				
	}
	
	reValue = rdT;
	//reValue = (reValue<<8)&0xFF00;
	
	rdT=0;
	for(i=0;i<8;i++)
	{
		rdT = rdT<<1;
		SSD_SPI_SCLK_H;
		
                if(SSD_SPI_SDI==0) 
                  rdT |= 0x00;
                else
                  rdT |= 0x01;
                
		SSD_SPI_SCLK_L;				
	}
	
	reValue += (rdT<<8);
	
	SSD_SPI_CS_H;
	
	return reValue;			
}

u16   SSD2828_READ_Dat(u8 reg)// SSD2828 Read Reg 
{
	//SPI_2828_WrReg(0xd4, 0x00FA);?
	u16 Read_Dat;
	//Read_ID=0x2825;
	Read_Dat=SSD2828_READ(reg);
	printf("SSD2828  Read  Reg  Address is  : 0x%x \r\n",(u16)reg);
	printf("SSD2828  Read         Dat   is  : 0x%x \r\n",Read_Dat);
        return Read_Dat;
}

void SPI_WriteData(u8 value)
{
//	printf("-%2x",value);
	SSD_SPI_CS_L;
	SPI_3W_SET_PAs(value);
	SSD_SPI_CS_H;	
}

void GP_COMMAD_PA(u16 num)//New
{
      SPI_2828_WrReg(0xbc, num);//Oxbc-0xbd---num 
	  //  Write_com(0x00bd);
		//	SPI_2828_WrReg(0xbe, num);
     // Write_com(0x00bf);
	  writec(0xbf);
      SSD_SPI_CS_H;	
}

//////////////////////////////////////////////////////////////////////////
u8   SPI_READ_new(void)
{
	u8  rdT;
	u8 reValue;
	u8 kk;
	
	SSD_SPI_CS_L;

	rdT=0;
	for(kk=0;kk<8;kk++)
	{
		rdT = rdT<<1;
		SSD_SPI_SCLK_H;
		if(SSD_SPI_SDI&0x0800) rdT |= 0x01;
		SSD_SPI_SCLK_L;				
	}
	
	reValue = rdT;
	//reValue = (reValue<<8)&0xFF00;
	
	SSD_SPI_CS_H;
	
	return reValue;			
}



void BLJ_CODE(void)
{

//#NT35596 + AUO5.0" Panel Initial Code
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x05); 
GP_COMMAD_PA(2);SPI_WriteData(0xFB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xC5); SPI_WriteData(0x31); 
//DIS_Delayms(30);//more than 30ms
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0xEE); 
GP_COMMAD_PA(2);SPI_WriteData(0xFB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x24); SPI_WriteData(0x4F); 
GP_COMMAD_PA(2);SPI_WriteData(0x38); SPI_WriteData(0xC8); 
GP_COMMAD_PA(2);SPI_WriteData(0x39); SPI_WriteData(0x27); 
GP_COMMAD_PA(2);SPI_WriteData(0x1E); SPI_WriteData(0x77); 
GP_COMMAD_PA(2);SPI_WriteData(0x1D); SPI_WriteData(0x0F); 
GP_COMMAD_PA(2);SPI_WriteData(0x7E); SPI_WriteData(0x71); 
GP_COMMAD_PA(2);SPI_WriteData(0x7C); SPI_WriteData(0x31); 
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xFB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x00); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x01); SPI_WriteData(0x55); 
GP_COMMAD_PA(2);SPI_WriteData(0x02); SPI_WriteData(0x40); 
GP_COMMAD_PA(2);SPI_WriteData(0x05); SPI_WriteData(0x50); 
GP_COMMAD_PA(2);SPI_WriteData(0x06); SPI_WriteData(0x4A); 
GP_COMMAD_PA(2);SPI_WriteData(0x07); SPI_WriteData(0x29); 
GP_COMMAD_PA(2);SPI_WriteData(0x08); SPI_WriteData(0x0C); 
GP_COMMAD_PA(2);SPI_WriteData(0x0B); SPI_WriteData(0x87); 
GP_COMMAD_PA(2);SPI_WriteData(0x0C); SPI_WriteData(0x87); 
GP_COMMAD_PA(2);SPI_WriteData(0x0E); SPI_WriteData(0xB0); 
GP_COMMAD_PA(2);SPI_WriteData(0x0F); SPI_WriteData(0xB3); 
GP_COMMAD_PA(2);SPI_WriteData(0x11); SPI_WriteData(0x10); 
GP_COMMAD_PA(2);SPI_WriteData(0x12); SPI_WriteData(0x10); 
GP_COMMAD_PA(2);SPI_WriteData(0x13); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x14); SPI_WriteData(0x4A); 
GP_COMMAD_PA(2);SPI_WriteData(0x15); SPI_WriteData(0x12); 
GP_COMMAD_PA(2);SPI_WriteData(0x16); SPI_WriteData(0x12); 
GP_COMMAD_PA(2);SPI_WriteData(0x18); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x19); SPI_WriteData(0x77); 
GP_COMMAD_PA(2);SPI_WriteData(0x1A); SPI_WriteData(0x55); 
GP_COMMAD_PA(2);SPI_WriteData(0x1B); SPI_WriteData(0x13); 
GP_COMMAD_PA(2);SPI_WriteData(0x1C); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x1D); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x1E); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x1F); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x58); SPI_WriteData(0x82); 
GP_COMMAD_PA(2);SPI_WriteData(0x59); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x5A); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x5B); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x5C); SPI_WriteData(0x82); 
GP_COMMAD_PA(2);SPI_WriteData(0x5D); SPI_WriteData(0x82); 
GP_COMMAD_PA(2);SPI_WriteData(0x5E); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x5F); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x72); SPI_WriteData(0x31); 
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x05); 
GP_COMMAD_PA(2);SPI_WriteData(0xFB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x00); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x01); SPI_WriteData(0x0B); 
GP_COMMAD_PA(2);SPI_WriteData(0x02); SPI_WriteData(0x0C); 
GP_COMMAD_PA(2);SPI_WriteData(0x03); SPI_WriteData(0x09); 
GP_COMMAD_PA(2);SPI_WriteData(0x04); SPI_WriteData(0x0A); 
GP_COMMAD_PA(2);SPI_WriteData(0x05); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x06); SPI_WriteData(0x0F); 
GP_COMMAD_PA(2);SPI_WriteData(0x07); SPI_WriteData(0x10); 
GP_COMMAD_PA(2);SPI_WriteData(0x08); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x09); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x0A); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x0B); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x0C); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x0D); SPI_WriteData(0x13); 
GP_COMMAD_PA(2);SPI_WriteData(0x0E); SPI_WriteData(0x15); 
GP_COMMAD_PA(2);SPI_WriteData(0x0F); SPI_WriteData(0x17); 
GP_COMMAD_PA(2);SPI_WriteData(0x10); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x11); SPI_WriteData(0x0B); 
GP_COMMAD_PA(2);SPI_WriteData(0x12); SPI_WriteData(0x0C); 
GP_COMMAD_PA(2);SPI_WriteData(0x13); SPI_WriteData(0x09); 
GP_COMMAD_PA(2);SPI_WriteData(0x14); SPI_WriteData(0x0A); 
GP_COMMAD_PA(2);SPI_WriteData(0x15); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x16); SPI_WriteData(0x0F); 
GP_COMMAD_PA(2);SPI_WriteData(0x17); SPI_WriteData(0x10); 
GP_COMMAD_PA(2);SPI_WriteData(0x18); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x19); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x1A); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x1B); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x1C); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x1D); SPI_WriteData(0x13); 
GP_COMMAD_PA(2);SPI_WriteData(0x1E); SPI_WriteData(0x15); 
GP_COMMAD_PA(2);SPI_WriteData(0x1F); SPI_WriteData(0x17); 
GP_COMMAD_PA(2);SPI_WriteData(0x20); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x21); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x22); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x23); SPI_WriteData(0x40); 
GP_COMMAD_PA(2);SPI_WriteData(0x24); SPI_WriteData(0x40); 
GP_COMMAD_PA(2);SPI_WriteData(0x25); SPI_WriteData(0xED); 
GP_COMMAD_PA(2);SPI_WriteData(0x29); SPI_WriteData(0x58); 
GP_COMMAD_PA(2);SPI_WriteData(0x2A); SPI_WriteData(0x12); 
GP_COMMAD_PA(2);SPI_WriteData(0x2B); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x4B); SPI_WriteData(0x04); 
GP_COMMAD_PA(2);SPI_WriteData(0x4C); SPI_WriteData(0x12); 
GP_COMMAD_PA(2);SPI_WriteData(0x4D); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x4E); SPI_WriteData(0x20); 
GP_COMMAD_PA(2);SPI_WriteData(0x4F); SPI_WriteData(0x20); 
GP_COMMAD_PA(2);SPI_WriteData(0x50); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x51); SPI_WriteData(0x61); 
GP_COMMAD_PA(2);SPI_WriteData(0x52); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x53); SPI_WriteData(0x63); 
GP_COMMAD_PA(2);SPI_WriteData(0x54); SPI_WriteData(0x77); 
GP_COMMAD_PA(2);SPI_WriteData(0x55); SPI_WriteData(0xED); 
GP_COMMAD_PA(2);SPI_WriteData(0x5B); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x5C); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x5F); SPI_WriteData(0x15); 
GP_COMMAD_PA(2);SPI_WriteData(0x60); SPI_WriteData(0x75); 
GP_COMMAD_PA(2);SPI_WriteData(0x63); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x64); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x67); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x68); SPI_WriteData(0x04); 
GP_COMMAD_PA(2);SPI_WriteData(0x6C); SPI_WriteData(0x40); 
GP_COMMAD_PA(2);SPI_WriteData(0x7A); SPI_WriteData(0x80); 
GP_COMMAD_PA(2);SPI_WriteData(0x7B); SPI_WriteData(0xA3); 
GP_COMMAD_PA(2);SPI_WriteData(0x7C); SPI_WriteData(0xD8); 
GP_COMMAD_PA(2);SPI_WriteData(0x7D); SPI_WriteData(0x60); 
GP_COMMAD_PA(2);SPI_WriteData(0x7F); SPI_WriteData(0x15); 
GP_COMMAD_PA(2);SPI_WriteData(0x80); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x83); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x93); SPI_WriteData(0x08); 
GP_COMMAD_PA(2);SPI_WriteData(0x94); SPI_WriteData(0x08); 
GP_COMMAD_PA(2);SPI_WriteData(0x8A); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x9B); SPI_WriteData(0x0F); 
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xFB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x75); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x76); SPI_WriteData(0x18); 
GP_COMMAD_PA(2);SPI_WriteData(0x77); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x78); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0x79); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x7A); SPI_WriteData(0x65); 
GP_COMMAD_PA(2);SPI_WriteData(0x7B); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x7C); SPI_WriteData(0x84); 
GP_COMMAD_PA(2);SPI_WriteData(0x7D); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x7E); SPI_WriteData(0x9B); 
GP_COMMAD_PA(2);SPI_WriteData(0x7F); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x80); SPI_WriteData(0xAF); 
GP_COMMAD_PA(2);SPI_WriteData(0x81); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x82); SPI_WriteData(0xC1); 
GP_COMMAD_PA(2);SPI_WriteData(0x83); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x84); SPI_WriteData(0xD2); 
GP_COMMAD_PA(2);SPI_WriteData(0x85); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x86); SPI_WriteData(0xDF); 
GP_COMMAD_PA(2);SPI_WriteData(0x87); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x88); SPI_WriteData(0x11); 
GP_COMMAD_PA(2);SPI_WriteData(0x89); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x8A); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0x8B); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x8C); SPI_WriteData(0x76); 
GP_COMMAD_PA(2);SPI_WriteData(0x8D); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x8E); SPI_WriteData(0xA7); 
GP_COMMAD_PA(2);SPI_WriteData(0x8F); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x90); SPI_WriteData(0xF3); 
GP_COMMAD_PA(2);SPI_WriteData(0x91); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x92); SPI_WriteData(0x2F); 
GP_COMMAD_PA(2);SPI_WriteData(0x93); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x94); SPI_WriteData(0x30); 
GP_COMMAD_PA(2);SPI_WriteData(0x95); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x96); SPI_WriteData(0x66); 
GP_COMMAD_PA(2);SPI_WriteData(0x97); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x98); SPI_WriteData(0xA0); 
GP_COMMAD_PA(2);SPI_WriteData(0x99); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x9A); SPI_WriteData(0xC5); 
GP_COMMAD_PA(2);SPI_WriteData(0x9B); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x9C); SPI_WriteData(0xF8); 
GP_COMMAD_PA(2);SPI_WriteData(0x9D); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x9E); SPI_WriteData(0x1B); 
GP_COMMAD_PA(2);SPI_WriteData(0x9F); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xA0); SPI_WriteData(0x46); 
GP_COMMAD_PA(2);SPI_WriteData(0xA2); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xA3); SPI_WriteData(0x52); 
GP_COMMAD_PA(2);SPI_WriteData(0xA4); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xA5); SPI_WriteData(0x62); 
GP_COMMAD_PA(2);SPI_WriteData(0xA6); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xA7); SPI_WriteData(0x71); 
GP_COMMAD_PA(2);SPI_WriteData(0xA9); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xAA); SPI_WriteData(0x83); 
GP_COMMAD_PA(2);SPI_WriteData(0xAB); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xAC); SPI_WriteData(0x94); 
GP_COMMAD_PA(2);SPI_WriteData(0xAD); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xAE); SPI_WriteData(0xA3); 
GP_COMMAD_PA(2);SPI_WriteData(0xAF); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xB0); SPI_WriteData(0xAD); 
GP_COMMAD_PA(2);SPI_WriteData(0xB1); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xB2); SPI_WriteData(0xCC); 
GP_COMMAD_PA(2);SPI_WriteData(0xB3); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xB4); SPI_WriteData(0x18); 
GP_COMMAD_PA(2);SPI_WriteData(0xB5); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xB6); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0xB7); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xB8); SPI_WriteData(0x65); 
GP_COMMAD_PA(2);SPI_WriteData(0xB9); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xBA); SPI_WriteData(0x84); 
GP_COMMAD_PA(2);SPI_WriteData(0xBB); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xBC); SPI_WriteData(0x9B); 
GP_COMMAD_PA(2);SPI_WriteData(0xBD); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xBE); SPI_WriteData(0xAF); 
GP_COMMAD_PA(2);SPI_WriteData(0xBF); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xC0); SPI_WriteData(0xC1); 
GP_COMMAD_PA(2);SPI_WriteData(0xC1); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xC2); SPI_WriteData(0xD2); 
GP_COMMAD_PA(2);SPI_WriteData(0xC3); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xC4); SPI_WriteData(0xDF); 
GP_COMMAD_PA(2);SPI_WriteData(0xC5); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xC6); SPI_WriteData(0x11); 
GP_COMMAD_PA(2);SPI_WriteData(0xC7); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xC8); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0xC9); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xCA); SPI_WriteData(0x76); 
GP_COMMAD_PA(2);SPI_WriteData(0xCB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xCC); SPI_WriteData(0xA7); 
GP_COMMAD_PA(2);SPI_WriteData(0xCD); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xCE); SPI_WriteData(0xF3); 
GP_COMMAD_PA(2);SPI_WriteData(0xCF); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xD0); SPI_WriteData(0x2F); 
GP_COMMAD_PA(2);SPI_WriteData(0xD1); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xD2); SPI_WriteData(0x30); 
GP_COMMAD_PA(2);SPI_WriteData(0xD3); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xD4); SPI_WriteData(0x66); 
GP_COMMAD_PA(2);SPI_WriteData(0xD5); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xD6); SPI_WriteData(0xA0); 
GP_COMMAD_PA(2);SPI_WriteData(0xD7); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xD8); SPI_WriteData(0xC5); 
GP_COMMAD_PA(2);SPI_WriteData(0xD9); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xDA); SPI_WriteData(0xF8); 
GP_COMMAD_PA(2);SPI_WriteData(0xDB); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xDC); SPI_WriteData(0x1B); 
GP_COMMAD_PA(2);SPI_WriteData(0xDD); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xDE); SPI_WriteData(0x46); 
GP_COMMAD_PA(2);SPI_WriteData(0xDF); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE0); SPI_WriteData(0x52); 
GP_COMMAD_PA(2);SPI_WriteData(0xE1); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE2); SPI_WriteData(0x62); 
GP_COMMAD_PA(2);SPI_WriteData(0xE3); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE4); SPI_WriteData(0x71); 
GP_COMMAD_PA(2);SPI_WriteData(0xE5); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE6); SPI_WriteData(0x83); 
GP_COMMAD_PA(2);SPI_WriteData(0xE7); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE8); SPI_WriteData(0x94); 
GP_COMMAD_PA(2);SPI_WriteData(0xE9); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xEA); SPI_WriteData(0xA3); 
GP_COMMAD_PA(2);SPI_WriteData(0xEB); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xEC); SPI_WriteData(0xAD); 
GP_COMMAD_PA(2);SPI_WriteData(0xED); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xEE); SPI_WriteData(0xCC); 
GP_COMMAD_PA(2);SPI_WriteData(0xEF); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xF0); SPI_WriteData(0x18); 
GP_COMMAD_PA(2);SPI_WriteData(0xF1); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xF2); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0xF3); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xF4); SPI_WriteData(0x65); 
GP_COMMAD_PA(2);SPI_WriteData(0xF5); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xF6); SPI_WriteData(0x84); 
GP_COMMAD_PA(2);SPI_WriteData(0xF7); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xF8); SPI_WriteData(0x9B); 
GP_COMMAD_PA(2);SPI_WriteData(0xF9); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xFA); SPI_WriteData(0xAF); 
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xFB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x00); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x01); SPI_WriteData(0xC1); 
GP_COMMAD_PA(2);SPI_WriteData(0x02); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x03); SPI_WriteData(0xD2); 
GP_COMMAD_PA(2);SPI_WriteData(0x04); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x05); SPI_WriteData(0xDF); 
GP_COMMAD_PA(2);SPI_WriteData(0x06); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x07); SPI_WriteData(0x11); 
GP_COMMAD_PA(2);SPI_WriteData(0x08); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x09); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0x0A); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x0B); SPI_WriteData(0x76); 
GP_COMMAD_PA(2);SPI_WriteData(0x0C); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x0D); SPI_WriteData(0xA7); 
GP_COMMAD_PA(2);SPI_WriteData(0x0E); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x0F); SPI_WriteData(0xF3); 
GP_COMMAD_PA(2);SPI_WriteData(0x10); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x11); SPI_WriteData(0x2F); 
GP_COMMAD_PA(2);SPI_WriteData(0x12); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x13); SPI_WriteData(0x30); 
GP_COMMAD_PA(2);SPI_WriteData(0x14); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x15); SPI_WriteData(0x66); 
GP_COMMAD_PA(2);SPI_WriteData(0x16); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x17); SPI_WriteData(0xA0); 
GP_COMMAD_PA(2);SPI_WriteData(0x18); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x19); SPI_WriteData(0xC5); 
GP_COMMAD_PA(2);SPI_WriteData(0x1A); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x1B); SPI_WriteData(0xF8); 
GP_COMMAD_PA(2);SPI_WriteData(0x1C); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x1D); SPI_WriteData(0x1B); 
GP_COMMAD_PA(2);SPI_WriteData(0x1E); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x1F); SPI_WriteData(0x46); 
GP_COMMAD_PA(2);SPI_WriteData(0x20); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x21); SPI_WriteData(0x52); 
GP_COMMAD_PA(2);SPI_WriteData(0x22); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x23); SPI_WriteData(0x62); 
GP_COMMAD_PA(2);SPI_WriteData(0x24); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x25); SPI_WriteData(0x71); 
GP_COMMAD_PA(2);SPI_WriteData(0x26); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x27); SPI_WriteData(0x83); 
GP_COMMAD_PA(2);SPI_WriteData(0x28); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x29); SPI_WriteData(0x94); 
GP_COMMAD_PA(2);SPI_WriteData(0x2A); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x2B); SPI_WriteData(0xA3); 
GP_COMMAD_PA(2);SPI_WriteData(0x2D); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x2F); SPI_WriteData(0xAD); 
GP_COMMAD_PA(2);SPI_WriteData(0x30); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x31); SPI_WriteData(0xCC); 
GP_COMMAD_PA(2);SPI_WriteData(0x32); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x33); SPI_WriteData(0x18); 
GP_COMMAD_PA(2);SPI_WriteData(0x34); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x35); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0x36); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x37); SPI_WriteData(0x65); 
GP_COMMAD_PA(2);SPI_WriteData(0x38); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x39); SPI_WriteData(0x84); 
GP_COMMAD_PA(2);SPI_WriteData(0x3A); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x3B); SPI_WriteData(0x9B); 
GP_COMMAD_PA(2);SPI_WriteData(0x3D); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x3F); SPI_WriteData(0xAF); 
GP_COMMAD_PA(2);SPI_WriteData(0x40); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x41); SPI_WriteData(0xC1); 
GP_COMMAD_PA(2);SPI_WriteData(0x42); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x43); SPI_WriteData(0xD2); 
GP_COMMAD_PA(2);SPI_WriteData(0x44); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x45); SPI_WriteData(0xDF); 
GP_COMMAD_PA(2);SPI_WriteData(0x46); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x47); SPI_WriteData(0x11); 
GP_COMMAD_PA(2);SPI_WriteData(0x48); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x49); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0x4A); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x4B); SPI_WriteData(0x76); 
GP_COMMAD_PA(2);SPI_WriteData(0x4C); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x4D); SPI_WriteData(0xA7); 
GP_COMMAD_PA(2);SPI_WriteData(0x4E); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x4F); SPI_WriteData(0xF3); 
GP_COMMAD_PA(2);SPI_WriteData(0x50); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x51); SPI_WriteData(0x2F); 
GP_COMMAD_PA(2);SPI_WriteData(0x52); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x53); SPI_WriteData(0x30); 
GP_COMMAD_PA(2);SPI_WriteData(0x54); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x55); SPI_WriteData(0x66); 
GP_COMMAD_PA(2);SPI_WriteData(0x56); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x58); SPI_WriteData(0xA0); 
GP_COMMAD_PA(2);SPI_WriteData(0x59); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x5A); SPI_WriteData(0xC5); 
GP_COMMAD_PA(2);SPI_WriteData(0x5B); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x5C); SPI_WriteData(0xF8); 
GP_COMMAD_PA(2);SPI_WriteData(0x5D); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x5E); SPI_WriteData(0x1B); 
GP_COMMAD_PA(2);SPI_WriteData(0x5F); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x60); SPI_WriteData(0x46); 
GP_COMMAD_PA(2);SPI_WriteData(0x61); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x62); SPI_WriteData(0x52); 
GP_COMMAD_PA(2);SPI_WriteData(0x63); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x64); SPI_WriteData(0x62); 
GP_COMMAD_PA(2);SPI_WriteData(0x65); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x66); SPI_WriteData(0x71); 
GP_COMMAD_PA(2);SPI_WriteData(0x67); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x68); SPI_WriteData(0x83); 
GP_COMMAD_PA(2);SPI_WriteData(0x69); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x6A); SPI_WriteData(0x94); 
GP_COMMAD_PA(2);SPI_WriteData(0x6B); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x6C); SPI_WriteData(0xA3); 
GP_COMMAD_PA(2);SPI_WriteData(0x6D); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x6E); SPI_WriteData(0xAD); 
GP_COMMAD_PA(2);SPI_WriteData(0x6F); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x70); SPI_WriteData(0xCC); 
GP_COMMAD_PA(2);SPI_WriteData(0x71); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x72); SPI_WriteData(0x18); 
GP_COMMAD_PA(2);SPI_WriteData(0x73); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x74); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0x75); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x76); SPI_WriteData(0x65); 
GP_COMMAD_PA(2);SPI_WriteData(0x77); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x78); SPI_WriteData(0x84); 
GP_COMMAD_PA(2);SPI_WriteData(0x79); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x7A); SPI_WriteData(0x9B); 
GP_COMMAD_PA(2);SPI_WriteData(0x7B); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x7C); SPI_WriteData(0xAF); 
GP_COMMAD_PA(2);SPI_WriteData(0x7D); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x7E); SPI_WriteData(0xC1); 
GP_COMMAD_PA(2);SPI_WriteData(0x7F); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x80); SPI_WriteData(0xD2); 
GP_COMMAD_PA(2);SPI_WriteData(0x81); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x82); SPI_WriteData(0xDF); 
GP_COMMAD_PA(2);SPI_WriteData(0x83); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x84); SPI_WriteData(0x11); 
GP_COMMAD_PA(2);SPI_WriteData(0x85); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x86); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0x87); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x88); SPI_WriteData(0x76); 
GP_COMMAD_PA(2);SPI_WriteData(0x89); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x8A); SPI_WriteData(0xA7); 
GP_COMMAD_PA(2);SPI_WriteData(0x8B); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0x8C); SPI_WriteData(0xF3); 
GP_COMMAD_PA(2);SPI_WriteData(0x8D); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x8E); SPI_WriteData(0x2F); 
GP_COMMAD_PA(2);SPI_WriteData(0x8F); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x90); SPI_WriteData(0x30); 
GP_COMMAD_PA(2);SPI_WriteData(0x91); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x92); SPI_WriteData(0x66); 
GP_COMMAD_PA(2);SPI_WriteData(0x93); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x94); SPI_WriteData(0xA0); 
GP_COMMAD_PA(2);SPI_WriteData(0x95); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x96); SPI_WriteData(0xC5); 
GP_COMMAD_PA(2);SPI_WriteData(0x97); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0x98); SPI_WriteData(0xF8); 
GP_COMMAD_PA(2);SPI_WriteData(0x99); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x9A); SPI_WriteData(0x1B); 
GP_COMMAD_PA(2);SPI_WriteData(0x9B); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x9C); SPI_WriteData(0x46); 
GP_COMMAD_PA(2);SPI_WriteData(0x9D); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0x9E); SPI_WriteData(0x52); 
GP_COMMAD_PA(2);SPI_WriteData(0x9F); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xA0); SPI_WriteData(0x62); 
GP_COMMAD_PA(2);SPI_WriteData(0xA2); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xA3); SPI_WriteData(0x71); 
GP_COMMAD_PA(2);SPI_WriteData(0xA4); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xA5); SPI_WriteData(0x83); 
GP_COMMAD_PA(2);SPI_WriteData(0xA6); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xA7); SPI_WriteData(0x94); 
GP_COMMAD_PA(2);SPI_WriteData(0xA9); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xAA); SPI_WriteData(0xA3); 
GP_COMMAD_PA(2);SPI_WriteData(0xAB); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xAC); SPI_WriteData(0xAD); 
GP_COMMAD_PA(2);SPI_WriteData(0xAD); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xAE); SPI_WriteData(0xCC); 
GP_COMMAD_PA(2);SPI_WriteData(0xAF); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xB0); SPI_WriteData(0x18); 
GP_COMMAD_PA(2);SPI_WriteData(0xB1); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xB2); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0xB3); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xB4); SPI_WriteData(0x65); 
GP_COMMAD_PA(2);SPI_WriteData(0xB5); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xB6); SPI_WriteData(0x84); 
GP_COMMAD_PA(2);SPI_WriteData(0xB7); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xB8); SPI_WriteData(0x9B); 
GP_COMMAD_PA(2);SPI_WriteData(0xB9); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xBA); SPI_WriteData(0xAF); 
GP_COMMAD_PA(2);SPI_WriteData(0xBB); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xBC); SPI_WriteData(0xC1); 
GP_COMMAD_PA(2);SPI_WriteData(0xBD); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xBE); SPI_WriteData(0xD2); 
GP_COMMAD_PA(2);SPI_WriteData(0xBF); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xC0); SPI_WriteData(0xDF); 
GP_COMMAD_PA(2);SPI_WriteData(0xC1); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xC2); SPI_WriteData(0x11); 
GP_COMMAD_PA(2);SPI_WriteData(0xC3); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xC4); SPI_WriteData(0x38); 
GP_COMMAD_PA(2);SPI_WriteData(0xC5); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xC6); SPI_WriteData(0x76); 
GP_COMMAD_PA(2);SPI_WriteData(0xC7); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xC8); SPI_WriteData(0xA7); 
GP_COMMAD_PA(2);SPI_WriteData(0xC9); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xCA); SPI_WriteData(0xF3); 
GP_COMMAD_PA(2);SPI_WriteData(0xCB); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xCC); SPI_WriteData(0x2F); 
GP_COMMAD_PA(2);SPI_WriteData(0xCD); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xCE); SPI_WriteData(0x30); 
GP_COMMAD_PA(2);SPI_WriteData(0xCF); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xD0); SPI_WriteData(0x66); 
GP_COMMAD_PA(2);SPI_WriteData(0xD1); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xD2); SPI_WriteData(0xA0); 
GP_COMMAD_PA(2);SPI_WriteData(0xD3); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xD4); SPI_WriteData(0xC5); 
GP_COMMAD_PA(2);SPI_WriteData(0xD5); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xD6); SPI_WriteData(0xF8); 
GP_COMMAD_PA(2);SPI_WriteData(0xD7); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xD8); SPI_WriteData(0x1B); 
GP_COMMAD_PA(2);SPI_WriteData(0xD9); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xDA); SPI_WriteData(0x46); 
GP_COMMAD_PA(2);SPI_WriteData(0xDB); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xDC); SPI_WriteData(0x52); 
GP_COMMAD_PA(2);SPI_WriteData(0xDD); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xDE); SPI_WriteData(0x62); 
GP_COMMAD_PA(2);SPI_WriteData(0xDF); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE0); SPI_WriteData(0x71); 
GP_COMMAD_PA(2);SPI_WriteData(0xE1); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE2); SPI_WriteData(0x83); 
GP_COMMAD_PA(2);SPI_WriteData(0xE3); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE4); SPI_WriteData(0x94); 
GP_COMMAD_PA(2);SPI_WriteData(0xE5); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE6); SPI_WriteData(0xA3); 
GP_COMMAD_PA(2);SPI_WriteData(0xE7); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xE8); SPI_WriteData(0xAD); 
GP_COMMAD_PA(2);SPI_WriteData(0xE9); SPI_WriteData(0x03); 
GP_COMMAD_PA(2);SPI_WriteData(0xEA); SPI_WriteData(0xCC); 
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xFB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x02); 
GP_COMMAD_PA(2);SPI_WriteData(0xFB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x04); 
GP_COMMAD_PA(2);SPI_WriteData(0xFB); SPI_WriteData(0x01); 
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0xD3); SPI_WriteData(0x05); 
GP_COMMAD_PA(2);SPI_WriteData(0xD4); SPI_WriteData(0x04); 
GP_COMMAD_PA(2);SPI_WriteData(0x11); SPI_WriteData(0x00); 
DIS_Delayms(100);
GP_COMMAD_PA(2);SPI_WriteData(0xFF); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x34); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x35); SPI_WriteData(0x00); 
GP_COMMAD_PA(2);SPI_WriteData(0x29); SPI_WriteData(0x00); 
DIS_Delayms(100);
}


void LS055T1SX01_CODE(void)
{
	//LCD_RST_L;
	//DIS_Delayms(100);
      //  LCD_RST_H;
      //  DIS_Delayms(100);
//LCD_1V8_H;
//Delay_us(200);
//LCD_2V8_H;
//Delay_us(200);
//VSP_EN;

printf("LCD Initial Start !!!  \r\n");
GP_COMMAD_PA(3);SPI_WriteData(0x51);SPI_WriteData(0x0F);SPI_WriteData(0xFF);
//Delay_us(100);
GP_COMMAD_PA(2);SPI_WriteData(0x53);SPI_WriteData(0x04);
//Delay_us(100);
GP_COMMAD_PA(2);SPI_WriteData(0x55); SPI_WriteData(0x00); 
//Delay_us(100);
GP_COMMAD_PA(1);SPI_WriteData(0x29);
//Delay_ms(200);
GP_COMMAD_PA(1);SPI_WriteData(0x11);
//Delay_ms(200);
printf("LCD Initial Start Done !!!  \r\n");
}



void LS055T1SX01_CODE_v2(void)
{

////

GP_COMMAD_PA(2);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x05);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFB);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC5);
	SPI_WriteData(0x31);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFB);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x00);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x01);
	SPI_WriteData(0x55);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x02);
	SPI_WriteData(0x40);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x05);
	SPI_WriteData(0xA0);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x06);
	SPI_WriteData(0x0A);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x07);
	SPI_WriteData(0x14);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x08);
	SPI_WriteData(0x0C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0B);
	SPI_WriteData(0x91);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0C);
	SPI_WriteData(0x91);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0E);
	SPI_WriteData(0xAB);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0F);
	SPI_WriteData(0xA4);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x11);
	SPI_WriteData(0x10);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x13);
	SPI_WriteData(0x43);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x14);
	SPI_WriteData(0x14);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x15);
	SPI_WriteData(0x13);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x16);
	SPI_WriteData(0x13);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x18);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x19);
	SPI_WriteData(0x77);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1A);
	SPI_WriteData(0x55);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1B);
	SPI_WriteData(0x13);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1C);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1D);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1E);
	SPI_WriteData(0x13);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x23);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x24);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x25);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x26);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x27);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x28);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x35);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x66);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x58);
	SPI_WriteData(0x81);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x59);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5A);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5B);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5C);
	SPI_WriteData(0x81);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5D);
	SPI_WriteData(0x81);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5E);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5F);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x6D);
	SPI_WriteData(0x22);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x72);
	SPI_WriteData(0x31);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x05);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFB);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x01);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x02);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x03);
	SPI_WriteData(0x04);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x04);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x05);
	SPI_WriteData(0x11);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x06);
	SPI_WriteData(0x0C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x07);
	SPI_WriteData(0x0B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x08);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x09);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0A);
	SPI_WriteData(0x18);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0B);
	SPI_WriteData(0x16);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0C);
	SPI_WriteData(0x14);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0D);
	SPI_WriteData(0x17);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0E);
	SPI_WriteData(0x15);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0F);
	SPI_WriteData(0x13);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x10);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x11);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x12);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x13);
	SPI_WriteData(0x04);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x14);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x15);
	SPI_WriteData(0x11);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x16);
	SPI_WriteData(0x0C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x17);
	SPI_WriteData(0x0B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x18);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x19);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1A);
	SPI_WriteData(0x18);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1B);
	SPI_WriteData(0x16);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1C);
	SPI_WriteData(0x14);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1D);
	SPI_WriteData(0x17);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1E);
	SPI_WriteData(0x15);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x13);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x20);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x21);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x22);
	SPI_WriteData(0x09);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x23);
	SPI_WriteData(0x67);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x24);
	SPI_WriteData(0x06);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x25);
	SPI_WriteData(0x2D);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x29);
	SPI_WriteData(0x58);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x2A);
	SPI_WriteData(0x11);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x2B);
	SPI_WriteData(0x04);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x2F);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x30);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x31);
	SPI_WriteData(0x49);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x32);
	SPI_WriteData(0x23);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x33);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x34);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x35);
	SPI_WriteData(0x6B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x36);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x37);
	SPI_WriteData(0x1D);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x38);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5D);
	SPI_WriteData(0x23);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x61);
	SPI_WriteData(0x15);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x65);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x69);
	SPI_WriteData(0x04);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x6C);
	SPI_WriteData(0x51);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7A);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7B);
	SPI_WriteData(0x80);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7C);
	SPI_WriteData(0xD8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7D);
	SPI_WriteData(0x10);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7E);
	SPI_WriteData(0x06);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7F);
	SPI_WriteData(0x1B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x81);
	SPI_WriteData(0x06);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x82);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8A);
	SPI_WriteData(0x33);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x93);
	SPI_WriteData(0x06);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x94);
	SPI_WriteData(0x06);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9B);
	SPI_WriteData(0x0F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA4);
	SPI_WriteData(0x0F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC5);
	SPI_WriteData(0x31);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE7);
	SPI_WriteData(0x80);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFB);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x75);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x76);
	SPI_WriteData(0x0F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x77);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x78);
	SPI_WriteData(0x21);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x79);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7A);
	SPI_WriteData(0x44);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7B);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7C);
	SPI_WriteData(0x65);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7D);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7E);
	SPI_WriteData(0x7F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7F);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x80);
	SPI_WriteData(0x95);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x81);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x82);
	SPI_WriteData(0xA9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x83);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x84);
	SPI_WriteData(0xBB);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x85);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x86);
	SPI_WriteData(0xCA);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x87);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x88);
	SPI_WriteData(0xFF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x89);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8A);
	SPI_WriteData(0x2A);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8B);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8C);
	SPI_WriteData(0x6B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8D);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8E);
	SPI_WriteData(0x9E);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8F);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x90);
	SPI_WriteData(0xEF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x91);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x92);
	SPI_WriteData(0x2D);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x93);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x94);
	SPI_WriteData(0x2F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x95);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x96);
	SPI_WriteData(0x68);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x97);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x98);
	SPI_WriteData(0xA2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x99);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9A);
	SPI_WriteData(0xC8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9B);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9C);
	SPI_WriteData(0xF9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9D);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9E);
	SPI_WriteData(0x1C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9F);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x46);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA2);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA3);
	SPI_WriteData(0x53);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA4);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA5);
	SPI_WriteData(0x61);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA6);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA7);
	SPI_WriteData(0x71);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA9);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAA);
	SPI_WriteData(0x81);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAB);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAC);
	SPI_WriteData(0x94);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAD);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAE);
	SPI_WriteData(0xA8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAF);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB0);
	SPI_WriteData(0xB9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB1);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB2);
	SPI_WriteData(0xC2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB3);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB4);
	SPI_WriteData(0x0F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB5);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB6);
	SPI_WriteData(0x21);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB7);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB8);
	SPI_WriteData(0x44);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB9);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBA);
	SPI_WriteData(0x65);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBB);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBC);
	SPI_WriteData(0x7F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBD);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBE);
	SPI_WriteData(0x95);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBF);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC0);
	SPI_WriteData(0xA9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC1);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC2);
	SPI_WriteData(0xBB);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC3);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC4);
	SPI_WriteData(0xCA);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC5);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC6);
	SPI_WriteData(0xFF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC7);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC8);
	SPI_WriteData(0x2A);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC9);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCA);
	SPI_WriteData(0x6B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCB);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCC);
	SPI_WriteData(0x9E);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCD);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCE);
	SPI_WriteData(0xEF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCF);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD0);
	SPI_WriteData(0x2D);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD1);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD2);
	SPI_WriteData(0x2F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD3);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD4);
	SPI_WriteData(0x68);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD5);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD6);
	SPI_WriteData(0xA2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD7);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD8);
	SPI_WriteData(0xC8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD9);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDA);
	SPI_WriteData(0xF9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDB);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDC);
	SPI_WriteData(0x1C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDD);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDE);
	SPI_WriteData(0x46);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDF);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE0);
	SPI_WriteData(0x53);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE1);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE2);
	SPI_WriteData(0x61);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE3);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE4);
	SPI_WriteData(0x71);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE5);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE6);
	SPI_WriteData(0x81);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE7);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE8);
	SPI_WriteData(0x94);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE9);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xEA);
	SPI_WriteData(0xA8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xEB);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xEC);
	SPI_WriteData(0xB9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xED);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xEE);
	SPI_WriteData(0xC2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xEF);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF0);
	SPI_WriteData(0x0F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF1);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF2);
	SPI_WriteData(0x21);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF3);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF4);
	SPI_WriteData(0x44);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF5);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF6);
	SPI_WriteData(0x65);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF7);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF8);
	SPI_WriteData(0x7F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xF9);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFA);
	SPI_WriteData(0x95);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFB);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x01);
	SPI_WriteData(0xA9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x02);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x03);
	SPI_WriteData(0xBB);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x04);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x05);
	SPI_WriteData(0xCA);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x06);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x07);
	SPI_WriteData(0xFF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x08);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x09);
	SPI_WriteData(0x2A);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0A);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0B);
	SPI_WriteData(0x6B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0C);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0D);
	SPI_WriteData(0x9E);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0E);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x0F);
	SPI_WriteData(0xEF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x10);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x11);
	SPI_WriteData(0x2D);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x12);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x13);
	SPI_WriteData(0x2F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x14);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x15);
	SPI_WriteData(0x68);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x16);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x17);
	SPI_WriteData(0xA2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x18);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x19);
	SPI_WriteData(0xC8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1A);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1B);
	SPI_WriteData(0xF9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1C);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1D);
	SPI_WriteData(0x1C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1E);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x46);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x20);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x21);
	SPI_WriteData(0x53);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x22);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x23);
	SPI_WriteData(0x61);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x24);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x25);
	SPI_WriteData(0x71);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x26);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x27);
	SPI_WriteData(0x81);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x28);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x29);
	SPI_WriteData(0x94);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x2A);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x2B);
	SPI_WriteData(0xA8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x2D);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x2F);
	SPI_WriteData(0xB9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x30);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x31);
	SPI_WriteData(0xC2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x32);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x33);
	SPI_WriteData(0x0F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x34);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x35);
	SPI_WriteData(0x21);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x36);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x37);
	SPI_WriteData(0x44);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x38);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x39);
	SPI_WriteData(0x65);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x3A);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x3B);
	SPI_WriteData(0x7F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x3D);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x3F);
	SPI_WriteData(0x95);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x40);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x41);
	SPI_WriteData(0xA9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x42);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x43);
	SPI_WriteData(0xBB);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x44);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x45);
	SPI_WriteData(0xCA);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x46);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x47);
	SPI_WriteData(0xFF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x48);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x49);
	SPI_WriteData(0x2A);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x4A);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x4B);
	SPI_WriteData(0x6B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x4C);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x4D);
	SPI_WriteData(0x9E);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x4E);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x4F);
	SPI_WriteData(0xEF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x50);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x51);
	SPI_WriteData(0x2D);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x52);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x53);
	SPI_WriteData(0x2F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x54);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x55);
	SPI_WriteData(0x68);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x56);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x58);
	SPI_WriteData(0xA2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x59);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5A);
	SPI_WriteData(0xC8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5B);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5C);
	SPI_WriteData(0xF9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5D);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5E);
	SPI_WriteData(0x1C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x5F);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x60);
	SPI_WriteData(0x46);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x61);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x62);
	SPI_WriteData(0x53);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x63);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x64);
	SPI_WriteData(0x61);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x65);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x66);
	SPI_WriteData(0x71);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x67);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x68);
	SPI_WriteData(0x81);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x69);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x6A);
	SPI_WriteData(0x94);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x6B);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x6C);
	SPI_WriteData(0xA8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x6D);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x6E);
	SPI_WriteData(0xB9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x6F);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x70);
	SPI_WriteData(0xC2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x71);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x72);
	SPI_WriteData(0x0F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x73);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x74);
	SPI_WriteData(0x21);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x75);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x76);
	SPI_WriteData(0x44);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x77);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x78);
	SPI_WriteData(0x65);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x79);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7A);
	SPI_WriteData(0x7F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7B);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7C);
	SPI_WriteData(0x95);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7D);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7E);
	SPI_WriteData(0xA9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x7F);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x80);
	SPI_WriteData(0xBB);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x81);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x82);
	SPI_WriteData(0xCA);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x83);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x84);
	SPI_WriteData(0xFF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x85);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x86);
	SPI_WriteData(0x2A);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x87);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x88);
	SPI_WriteData(0x6B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x89);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8A);
	SPI_WriteData(0x9E);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8B);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8C);
	SPI_WriteData(0xEF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8D);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8E);
	SPI_WriteData(0x2D);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x8F);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x90);
	SPI_WriteData(0x2F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x91);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x92);
	SPI_WriteData(0x68);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x93);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x94);
	SPI_WriteData(0xA2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x95);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x96);
	SPI_WriteData(0xC8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x97);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x98);
	SPI_WriteData(0xF9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x99);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9A);
	SPI_WriteData(0x1C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9B);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9C);
	SPI_WriteData(0x46);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9D);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9E);
	SPI_WriteData(0x53);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x9F);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x61);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA2);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA3);
	SPI_WriteData(0x71);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA4);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA5);
	SPI_WriteData(0x81);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA6);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA7);
	SPI_WriteData(0x94);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xA9);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAA);
	SPI_WriteData(0xA8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAB);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAC);
	SPI_WriteData(0xB9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAD);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAE);
	SPI_WriteData(0xC2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xAF);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB0);
	SPI_WriteData(0x0F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB1);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB2);
	SPI_WriteData(0x21);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB3);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB4);
	SPI_WriteData(0x44);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB5);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB6);
	SPI_WriteData(0x65);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB7);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB8);
	SPI_WriteData(0x7F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xB9);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBA);
	SPI_WriteData(0x95);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBB);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBC);
	SPI_WriteData(0xA9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBD);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBE);
	SPI_WriteData(0xBB);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xBF);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC0);
	SPI_WriteData(0xCA);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC1);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC2);
	SPI_WriteData(0xFF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC3);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC4);
	SPI_WriteData(0x2A);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC5);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC6);
	SPI_WriteData(0x6B);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC7);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC8);
	SPI_WriteData(0x9E);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xC9);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCA);
	SPI_WriteData(0xEF);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCB);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCC);
	SPI_WriteData(0x2D);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCD);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCE);
	SPI_WriteData(0x2F);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xCF);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD0);
	SPI_WriteData(0x68);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD1);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD2);
	SPI_WriteData(0xA2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD3);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD4);
	SPI_WriteData(0xC8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD5);
	SPI_WriteData(0x02);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD6);
	SPI_WriteData(0xF9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD7);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD8);
	SPI_WriteData(0x1C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD9);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDA);
	SPI_WriteData(0x46);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDB);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDC);
	SPI_WriteData(0x53);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDD);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDE);
	SPI_WriteData(0x61);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xDF);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE0);
	SPI_WriteData(0x71);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE1);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE2);
	SPI_WriteData(0x81);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE3);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE4);
	SPI_WriteData(0x94);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE5);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE6);
	SPI_WriteData(0xA8);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE7);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE8);
	SPI_WriteData(0xB9);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xE9);
	SPI_WriteData(0x03);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xEA);
	SPI_WriteData(0xC2);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x04);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x08);
	SPI_WriteData(0x0C);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFB);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x00);
	GP_COMMAD_PA(2);
	SPI_WriteData(0x35);
	SPI_WriteData(0x01);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD3);
	SPI_WriteData(0x06);
	GP_COMMAD_PA(2);
	SPI_WriteData(0xD4);
	SPI_WriteData(0x04);
        
}
//BLJ FHD MIPI LCD Initial Code
void BLJ_LCD_Init(void)
{

	DIS_Delayms(200);
	SSD_RST_L;// ( rGPFDAT &= (~(1<<3))) ;
	LCD_RST_L;
	DIS_Delayms(50);
	SSD_RST_H;//  ( rGPFDAT |= (1<<3) ) ;
	LCD_RST_H;
	DIS_Delayms(150);

		SPI_2828_WrCmd(0xb7);
		SPI_WriteData(0x50);//50=TX_CLK 70=PCLK
		SPI_WriteData(0x00);   //Configuration Register

		SPI_2828_WrCmd(0xb8);
		SPI_WriteData(0x00);
		SPI_WriteData(0x00);   //VC(Virtual ChannelID) Control Register

		SPI_2828_WrCmd(0xb9);
		SPI_WriteData(0x00);//1=PLL disable
		SPI_WriteData(0x00);
		//TX_CLK/MS should be between 5Mhz to100Mhz
		SPI_2828_WrCmd(0xBA);//PLL=(TX_CLK/MS)*NS 8228=480M 4428=240M  061E=120M 4214=240M 821E=360M 8219=300M
		SPI_WriteData(0x14);//D7-0=NS(0x01 : NS=1)
		SPI_WriteData(0x42);//D15-14=PLL范围 00=62.5-125 01=126-250 10=251-500 11=501-1000  DB12-8=MS(01:MS=1)

		SPI_2828_WrCmd(0xBB);//LP Clock Divider LP clock = 400MHz / LPD / 8 = 240 / 8 / 4 = 7.5MHz
		SPI_WriteData(0x03);//D5-0=LPD=0x1 – Divide by 2
		SPI_WriteData(0x00);

		SPI_2828_WrCmd(0xb9);
		SPI_WriteData(0x01);//1=PLL disable
		SPI_WriteData(0x00);
		//MIPI lane configuration
		SPI_2828_WrCmd(0xDE);//通道数
		SPI_WriteData(0x00);//11=4LANE 10=3LANE 01=2LANE 00=1LANE
		SPI_WriteData(0x00);

		SPI_2828_WrCmd(0xc9);
		SPI_WriteData(0x02);
		SPI_WriteData(0x23);   //p1: HS-Data-zero  p2: HS-Data- prepare  --> 8031 issue
		DIS_Delayms(100);
 
///////////////////////////////////////////////////////////////

			SPI_2828_WrCmd(0xB7);
			SPI_WriteData(0x10);//10=TX_CLK 30=PCLK
			SPI_WriteData(0x02);


			SPI_2828_WrCmd(0xBD);
			SPI_WriteData(0x00);
			SPI_WriteData(0x00);
			
			SPI_2828_WrCmd(0xD9);// HS LP Tx Driver current 
		        SPI_WriteData(0x22);
		
///////////////////////////////////////////////////////////////

	              //   BLJ_CODE();   //BLJ LCD Code 
                       LS055T1SX01_CODE();
                    //  SPI_2828_WrReg(0xee,0x0600);
                       
                    //   LS055T1SX01_CODE_v2();
                        GP_COMMAD_PA(1);
			SPI_WriteData(0x29);
			DIS_Delayms(150);
			
			GP_COMMAD_PA(1);
			SPI_WriteData(0x11);
			DIS_Delayms(200);

///////////////////////////////////////////////////////////////	
	

		DIS_Delayms(20);

		//SSD2828_Initial
		SPI_2828_WrCmd(0xb7);
		SPI_WriteData(0x50);
		SPI_WriteData(0x00);   //Configuration Register

		SPI_2828_WrCmd(0xb8);
		SPI_WriteData(0x00);
		SPI_WriteData(0x00);   //VC(Virtual ChannelID) Control Register

		SPI_2828_WrCmd(0xb9);
		SPI_WriteData(0x00);//1=PLL disable
		SPI_WriteData(0x00);

		SPI_2828_WrCmd(0xBA);//PLL=(TX_CLK/MS)*NS 8228=480M 4428=240M  061E=120M 4214=240M 821E=360M 8219=300M 8225=444M 8224=432
//		SPI_WriteData(0x57);//D7-0=NS(0x01 : NS=1)   //0X28
//		SPI_WriteData(0xC2);//D15-14=PLL范围 00=62.5-125 01=126-250 10=251-500 11=501-1000  DB12-8=MS(01:MS=1) //0X82
                
		SPI_WriteData(0x50);//D7-0=NS(0x01 : NS=1)   //0X28
		SPI_WriteData(0xC2);//D15-14=PLL范围 00=62.5-125 01=126-250 10=251-500 11=501-1000  DB12-8=MS(01:MS=1) //0X82
                
		SPI_2828_WrCmd(0xBB);//LP Clock Divider LP clock = 400MHz / LPD / 8 = 480 / 8/ 8 = 7.5MHz
		SPI_WriteData(0x07);//D5-0=LPD=0x1 – Divide by 2
		SPI_WriteData(0x00);

		SPI_2828_WrCmd(0xb9);
		SPI_WriteData(0x01);//1=PLL disable
		SPI_WriteData(0x00);

		SPI_2828_WrCmd(0xc9);
		SPI_WriteData(0x02); //0x02
		SPI_WriteData(0x23);   //p1: HS-Data-zero  p2: HS-Data- prepare  --> 8031 issue  0x23
		DIS_Delayms(10);

		SPI_2828_WrCmd(0xCA);
		SPI_WriteData(0x01);//CLK Prepare //0x01
		SPI_WriteData(0x23);//Clk Zero   //0x23

		SPI_2828_WrCmd(0xCB); //local_write_reg(addr=0xCB,data=0x0510)
		SPI_WriteData(0x10); //Clk Post  //0x10
		SPI_WriteData(0x05); //Clk Per    /0x05

		SPI_2828_WrCmd(0xCC); //local_write_reg(addr=0xCC,data=0x100A)
		SPI_WriteData(0x05); //HS Trail
		SPI_WriteData(0x10); //Clk Trail
		DIS_Delayms(10);

		SPI_2828_WrCmd(0xD0); //local_write_reg(addr=0xCC,data=0x100A)
		SPI_WriteData(0x00); //HS Trail
		SPI_WriteData(0x00); //Clk Trail
		DIS_Delayms(10);


		//RGB interface configuration

		SPI_2828_WrReg(0xb1,(LCD_VSPW<<8)|LCD_HSPW);	//Vertical sync and horizontal sync active period 
		SPI_2828_WrReg(0xb2,(LCD_VBPD<<8)|LCD_HBPD);	//Vertical and horizontal back porch period  
		SPI_2828_WrReg(0xb3,(LCD_VFPD<<8)|LCD_HFPD);	//Vertical and horizontal front porch period 
		SPI_2828_WrReg(0xb4, LCD_XSIZE_TFT);		//Horizontal active period 
		SPI_2828_WrReg(0xb5, LCD_YSIZE_TFT);		//Vertical active period
	//	SPI_2828_WrReg(0xb6, 0x001B);				//Video mode and video pixel format //HS=L,VS=L,PCLK=L;
	//	SPI_2828_WrReg(0xb6, 0xC01B);				//Video mode and video pixel format //HS=H,VS=H,PCLK=L;
		SPI_2828_WrReg(0xb6, 0x001B);				//Video mode and video pixel format //HS=H,VS=H,PCLK=L;	blj is ok	
	//	SPI_2828_WrReg(0xb6, 0xe01B);				//Video mode and video pixel format //HS=H,VS=H,PCLK=L;				
		//MIPI lane configuration
		SPI_2828_WrCmd(0xDE);//通道数
		SPI_WriteData(0x03);//11=4LANE 10=3LANE 01=2LANE 00=1LANE
		SPI_WriteData(0x00);

		SPI_2828_WrCmd(0xD6);//  05=BGR  04=RGB
		SPI_WriteData(0x05);//D0=0=RGB 1:BGR D1=1=Most significant byte sent first
		SPI_WriteData(0x00);

		SPI_2828_WrCmd(0xB7);
		SPI_WriteData(0x4B); //0X4B
		SPI_WriteData(0x02);
		DIS_Delayms(100);

		SPI_2828_WrCmd(0x2C);

		DIS_Delayms(50);
			

}

//软延时函数
void DIS_Delayms(u16 Num)
{
		u16 Timer;
		while(Num--)
		{
		 	Timer = 1000;
			while(Timer--); 
		}
}		

