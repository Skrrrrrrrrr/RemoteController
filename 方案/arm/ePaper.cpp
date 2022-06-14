#include "ePaper.h"
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_utils.h>
/*
Pin		|	valid	|	stm32	|	Mode
1)BUSY|1 			| PB10	|in
2)RES	|0 			| PB2		|out
3)DC	|1-D 0-C| PB14	|out
4)CS	|0 			| PB12	|out
5)SCK	|x 			| PB13	|out
6)SDI	|x 			| PB15	|out
7)GND	|0 			| GND		|out
8)3.3	|3.3		| 3V3		|out
*/

EPaper::EPaper(){
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
	
	LL_GPIO_InitTypeDef GPIO_InitTypeDef = {
		LL_GPIO_PIN_12 | LL_GPIO_PIN_14,
		LL_GPIO_MODE_OUTPUT, LL_GPIO_SPEED_FREQ_HIGH,
		LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_NO, LL_GPIO_AF_0
	};
	LL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);
	
	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_2;
	GPIO_InitTypeDef.Pull = LL_GPIO_PULL_DOWN;
	LL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);
	
	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_10;
	GPIO_InitTypeDef.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitTypeDef.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);
	
	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_15;
	GPIO_InitTypeDef.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitTypeDef.Pull = LL_GPIO_PULL_UP;
	GPIO_InitTypeDef.Alternate = LL_GPIO_AF_5;
	LL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);
	
	LL_SPI_InitTypeDef SPI_InitTypeDef = {
		LL_SPI_HALF_DUPLEX_TX, LL_SPI_MODE_MASTER, 
		LL_SPI_DATAWIDTH_8BIT, LL_SPI_POLARITY_HIGH, 
		LL_SPI_PHASE_2EDGE, LL_SPI_NSS_SOFT, 
		LL_SPI_BAUDRATEPRESCALER_DIV2, LL_SPI_MSB_FIRST, 
		LL_SPI_CRCCALCULATION_DISABLE, 7
	};
	LL_SPI_Init(SPI2, &SPI_InitTypeDef);
	LL_SPI_Enable(SPI2);
}

void EPaper::init(){
	reset_valid();
	LL_mDelay(10);
	reset_invalid();
	LL_mDelay(10);
	
	buzy();
	writeCode(0x12);//SWRESET
	buzy();
	
	writeCode(0x01);//Driver output control
	writeData("\x27\x01\x00", 3);
	
	writeCode(0x11); //data entry mode       
	writeData(0x01);

	writeCode(0x44); //set Ram-X address start/end position   
	writeData("\x00\x0f", 2);    //0x0F-->(15+1)*8=128

	writeCode(0x45); //set Ram-Y address start/end position          
	writeData("\x27\x01\x00\x00", 4);   //0x0127-->(295+1)=296 

	writeCode(0x3C); //BorderWavefrom
	writeData(0x05);	
	  	
	writeCode(0x21); //  Display update control
	writeData("\x00\x80", 2);			
			
  writeCode(0x18); //Read built-in temperature sensor
	writeData(0x80);	

	writeCode(0x4E);   // set RAM x address count to 0;
	writeData(0x00);
	writeCode(0x4F);   // set RAM y address count to 0X199;    
	writeData("\x27\x01", 2);
  buzy();
}

/*When the electronic paper screen is updated, do not unplug the electronic paper to avoid damage to the screen*/

void EPaper::update(void){   
  writeCode(0x22); //Display Update Control
  writeData(0xF7);   
  writeCode(0x20); //Activate Display Update Sequence
  buzy();
}

/*When the electronic paper screen is updated, do not unplug the electronic paper to avoid damage to the screen*/
void EPaper::update_part(void){
	writeCode(0x22); //Display Update Control
	writeData(0xFF);   
	writeCode(0x20); //Activate Display Update Sequence
	buzy(); 			
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPaper::screen_all(const unsigned char *datas){
  unsigned int i;	
	writeCode(0x24);   //write RAM for black(0)/white (1)
	writeData((char*)datas, 4736);
//  for(i=0;i<4736;i++){               
//     Epaper_Write_Data(*datas);
//			datas++;
//   }
   update();	 
}
///////////////////////////Part update//////////////////////////////////////////////
//The x axis is reduced by one byte, and the y axis is reduced by one pixel.
void EPaper::setRAMValue_BaseMap( const unsigned char * datas){
	unsigned int i;   
	const unsigned char  *datas_flag;   
	datas_flag=datas;

  writeCode(0x24);   //Write Black and White image to RAM
	writeData((char*)datas, 4736);
//   for(i=0;i<4736;i++)
//   {               
//     Epaper_Write_Data(*datas);
//			datas++;
//   }
	datas=datas_flag;
  writeCode(0x26);   //Write Black and White image to RAM
	writeData((char*)datas, 4736);
//   for(i=0;i<4736;i++)
//   {               
//     Epaper_Write_Data(*datas);
//			datas++;
//   }
   update();		 
}

/////////////////////////////////Single display////////////////////////////////////////////////

void EPaper::deepSleep(void){  	
  writeCode(0x10); //enter deep sleep
  writeData(0x01); 
  LL_mDelay(100);
}

void EPaper::screen_white(void){
  unsigned int i,k;
	writeCode(0x24);   //write RAM for black(0)/white (1)
	for(k=0;k<250;k++){
		for(i=0;i<25;i++){
			writeData(0xff);
		}
  }
	update();
}

EPaper &EPaper::instance(){
	static EPaper singleton;
	return singleton;
}

void EPaper::writeData(uint8_t ch){
	cs_valid();
	data_mode();
	transmitData(ch);
	cs_invalid();
}

void EPaper::writeData(const char* ch, uint16_t num){
	cs_valid();
	data_mode();
	for(uint16_t i=0; i<num; i++){
		transmitData(ch[i]);
	}
	cs_invalid();
}

void EPaper::writeCode(uint8_t ch){
	cs_valid();
	code_mode();
	transmitData(ch);
	cs_invalid();
}

void EPaper::transmitData(uint8_t ch){
	while(!LL_SPI_IsActiveFlag_TXE(SPI2));
	LL_SPI_TransmitData8(SPI2, ch);
}