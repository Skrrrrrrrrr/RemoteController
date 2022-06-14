#ifndef LCD_DEVEBOX_H
#define LCD_DEVEBOX_H
#include <stdint.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>
#include <stm32f4xx_ll_utils.h>

///////////////////////////  ��ɫֵ  ///////////////////////////////////////////////////////

//������ɫ
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
#define BROWN 			     0xBC40 //��ɫ
#define BRRED 			     0xFC07 //�غ�ɫ
#define GRAY  			     0x8430 //��ɫ


//GUI��ɫ

#define DARKBLUE      	 0x01CF	//����ɫ
#define LIGHTBLUE      	 0x7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0x5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
 
#define LIGHTGREEN     	 0x841F //ǳ��ɫ
//#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			     0xC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define GRAY0   0xEF7D   	    //��ɫ0 
#define GRAY1   0x8410      	//��ɫ1   
#define GRAY2   0x4208      	//��ɫ2  

#define LGRAYBLUE        0xA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0x2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

//ɨ�跽����--ɨ�跽ʽ�в�ͬ��񣬿��ܶ��岻���Һ����µĲ��շ���ͬ���᷽ܽʽ��ֻ��һ�°���

#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���	


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