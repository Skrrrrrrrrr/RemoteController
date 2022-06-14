#ifndef __Text_H__
#define __Text_H__	 
#include <stm32f4xx.h>

//#include "font.h"

//////////////////////////////////////////////////////////////////////////////////	 
 
//汉字显示 驱动代码	

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
/******************************************************************************/		 
 					     
void Get_HzMat(const char *code,uint8_t *mat,uint8_t size,uint8_t Font);			//得到汉字的点阵码

void Show_Font(uint16_t x,uint16_t y,const char *font,uint8_t size,uint16_t color,uint8_t mode);					//在指定位置显示一个汉字

void Show_Str(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const char*str,uint8_t size, uint16_t color, uint8_t mode);	//在指定位置显示一个字符串 

void Draw_Font16B(uint16_t x,uint16_t y, uint16_t color, const char*str);//在指定位置--显示 16x16 大小的点阵字符串

void Draw_Font24B(uint16_t x,uint16_t y, uint16_t color, const char*str);//在指定位置--显示 24x24 大小的点阵字符串


#endif
