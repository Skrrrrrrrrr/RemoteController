#include "flash.h"
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_utils.h>
/*
  SPI1接口初始化设置    
	PA7--SPI1_MOSI
  PA5--SPI1_SCK
  PA4--SPI1_NSS
	PB4--SPI1_MISO
*/

Flash::Flash(){
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
	
	LL_GPIO_InitTypeDef GPIO_InitTypeDef = {
		LL_GPIO_PIN_5 | LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE, 
		LL_GPIO_SPEED_FREQ_HIGH, LL_GPIO_OUTPUT_PUSHPULL, 
		LL_GPIO_PULL_UP, LL_GPIO_AF_5
	};
	LL_GPIO_Init(GPIOA, &GPIO_InitTypeDef);
	
	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_4;
	LL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);
	
	GPIO_InitTypeDef.Mode = LL_GPIO_MODE_OUTPUT;
	LL_GPIO_Init(GPIOA, &GPIO_InitTypeDef);
	
	LL_SPI_InitTypeDef SPI_InitTypeDef = {
		LL_SPI_FULL_DUPLEX, LL_SPI_MODE_MASTER, 
		LL_SPI_DATAWIDTH_8BIT, LL_SPI_POLARITY_HIGH, 
		LL_SPI_PHASE_2EDGE, LL_SPI_NSS_SOFT, 
		LL_SPI_BAUDRATEPRESCALER_DIV2, LL_SPI_MSB_FIRST, 
		LL_SPI_CRCCALCULATION_DISABLE, 7
	};
	LL_SPI_Init(SPI1, &SPI_InitTypeDef);
	LL_SPI_Enable(SPI1);

	cs_valid();
	send("\x90\x00\x00\x00", 4);
	receive((uint8_t*)&id, 2);
	cs_invalid();
}

uint8_t Flash::transmitData(uint8_t ch){
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));
	LL_SPI_TransmitData8(SPI1, ch);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1));
	return LL_SPI_ReceiveData8(SPI1);
}

void Flash::send(const char* data, uint32_t num){
	for(uint32_t i=0; i< num; i++){
		transmitData(data[i]);
	}
}
void Flash::receive(uint8_t* data, uint32_t num){
	for(uint32_t i=0; i< num; i++){
		data[i] = transmitData(0xff);
	}
}

Flash &Flash::instance(){
	static Flash singleton;
	return singleton;
}

uint8_t Flash::readSR(){
	cs_valid();
	transmitData(CODE_RSTATUS_REG);
	uint8_t rt = transmitData(0xFF);
	cs_invalid();
	return rt;
}

void Flash::writeSR(uint8_t ch){
	cs_valid();
	transmitData(CODE_WSTATUS_REG);
	transmitData(ch);
	cs_invalid();
}

void Flash::writeEnable(){
	cs_valid();
	transmitData(CODE_WRITE_ENABLE);
	cs_invalid();
}

void Flash::writeDisable(){
	cs_valid();
	transmitData(CODE_WRITE_DISABLE);
	cs_invalid();
}

void Flash::read(uint8_t* data, uint16_t sectorOrder, uint16_t pageOrder, uint16_t num){
	uint32_t addr = sectorOrder * 0x1000 + pageOrder * 0x100;
	cs_valid();
	transmitData(CODE_READ_DATA);
	transmitData(addr>>16);
	transmitData(addr>>8);
	transmitData(addr);
	for(uint16_t i=0; i<num; i++){
		data[i] = transmitData(0xFF);
	}
	cs_invalid();
}

void Flash::write(uint8_t* data, uint16_t sectorOrder, uint16_t pageOrder, uint16_t num){
	uint32_t addr = sectorOrder * 0x1000 + pageOrder * 0x100;
	writeEnable();
	cs_valid();
	transmitData(CODE_PAGE_PROGRAM);
	transmitData(addr>>16);
	transmitData(addr>>8);
	transmitData(addr);
	for(uint16_t i=0; i<num; i++){
		transmitData(data[i]);
	}
	cs_invalid();
	wait();
}

void Flash::eraseSector(uint32_t order){
	uint32_t addr = order * 0x1000;
	writeEnable();
	wait();
	cs_valid();
	transmitData(CODE_SECTOR_ERASE);
	transmitData(addr>>16);
	transmitData(addr>>8);
	transmitData(addr);
	cs_invalid();
	wait();
}

void Flash::powerDown(){
	uint16_t i = 0xFFFF;
	cs_valid();
	transmitData(CODE_POWERDOWN);
	cs_invalid();
	while(i--);
}

void Flash::wakeUp(){
	uint16_t i = 0xFFFF;
	cs_valid();
	transmitData(CODE_RELEASE_POWERDOWN);
	cs_invalid();
	while(i--);
}