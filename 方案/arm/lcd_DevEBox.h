#ifndef LCD_DEVEBOX_H
#define LCD_DEVEBOX_H
#include <stdint.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>
#include <stm32f4xx_ll_utils.h>

///////////////////////////  颜色值  ///////////////////////////////////////////////////////

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0xF81F
#define GRED 			       0xFFE0
#define GBLUE			       0x07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0xBC40 //棕色
#define BRRED 			     0xFC07 //棕红色
#define GRAY  			     0x8430 //灰色


//GUI颜色

#define DARKBLUE      	 0x01CF	//深蓝色
#define LIGHTBLUE      	 0x7D7C	//浅蓝色  
#define GRAYBLUE       	 0x5458 //灰蓝色
//以上三色为PANEL的颜色 
 
 
#define LIGHTGREEN     	 0x841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			     0xC618 //浅灰色(PANNEL),窗体背景色

#define GRAY0   0xEF7D   	    //灰色0 
#define GRAY1   0x8410      	//灰色1   
#define GRAY2   0x4208      	//灰色2  

#define LGRAYBLUE        0xA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0x2B12 //浅棕蓝色(选择条目的反色)

//扫描方向定义--扫描方式有不同规格，可能定义不左右和上下的参照方向不同，总结方式，只有一下八种

#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	


class LcdDevEBox{
	struct _LCD_DEV{
		uint16_t width;
		uint16_t height;
		uint16_t id;
		uint8_t dir;
		uint16_t wramcmd;
		uint16_t setxcmd;
		uint16_t setycmd;
	};
	enum{
		LCD_DIR_VERTICAL1,
		LCD_DIR_VERTICAL2,
		LCD_DIR_HORIZONTAL1,
		LCD_DIR_HORIZONTAL2,
	};
	public:
		static LcdDevEBox &instance();
		void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color);
		void drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color);
		void clear(uint16_t color);
		void fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
		void fast_drawPoint(uint16_t x,uint16_t y,uint16_t color);
	private:
		explicit LcdDevEBox();
		
		void data_mode(){LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);}
		void code_mode(){LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);}
		void cs_valid(){LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);}
		void cs_invalid(){LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);}
		void blk_open(){LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_10);}
		void blk_close(){LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_10);}
		uint8_t transmitData(uint8_t ch);
		
		void writeData(uint8_t ch);
		void writeData16(uint16_t ch);
		void writeData(const char* ch, uint16_t num);
		void writeCode(uint8_t ch);
		
		void softReset(){
			writeCode(0x01);
			LL_mDelay(50);
		}
		
		void setScanDir(uint8_t dir);
	
		void lcdInit();
		void lcdDisplayDir(uint8_t dir);
		void writeGRAM();
		void setCursor(uint16_t x, uint16_t y);
		void drawPoint(uint16_t x, uint16_t y);
		void setWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);

	public:
		_LCD_DEV lcdDev;
	private:
		
};
#endif