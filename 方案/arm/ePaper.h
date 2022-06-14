#ifndef EPAPER_H
#define EPAPER_H
#include <stdint.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>

class EPaper{
	public:
		static EPaper &instance();
		void init();
		void deepSleep(void);
		void screen_all(const unsigned char *datas);
		void screen_white(void);
	private:
		explicit EPaper();
		void buzy(){while(LL_GPIO_IsOutputPinSet(GPIOB, LL_GPIO_PIN_10));}
		void reset_valid(){LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);}
		void reset_invalid(){LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);}
		void data_mode(){LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_14);}
		void code_mode(){LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_14);}
		void cs_valid(){LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);}
		void cs_invalid(){LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);}
		void transmitData(uint8_t ch);
		
		void writeData(uint8_t ch);
		void writeData(const char* ch, uint16_t num);
		void writeCode(uint8_t ch);
		
		void update(void);
		void update_part(void);
		
		void setRAMValue_BaseMap( const unsigned char * datas);
		
	public:

};
#endif