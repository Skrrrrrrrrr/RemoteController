#ifndef FLASH_H
#define FLASH_H
#include <stdint.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>

class Flash{
	public:
		static Flash &instance();
	
		/*
			//读取SPI FLASH  
			//在指定地址开始读取指定长度的数据
			//pBuffer:数据存储区
			//sectorOrder: [0,1023]
			//pageOrder: [0,15]
			//num:要写入的字节数(最大256)
		*/
		void read(uint8_t* data, uint16_t sectorOrder, uint16_t pageOrder, uint16_t num);
		/*
			//SPI在一页(0~65535)内写入少于256个字节的数据
			//在指定地址开始写入最大256字节的数据
			//pBuffer:数据存储区
			//sectorOrder: [0,1023]
			//pageOrder: [0,15]
			//num:要写入的字节数(最大256)
		*/
		void write(uint8_t* data, uint16_t sectorOrder, uint16_t pageOrder, uint16_t num);
		/*
			//擦除一个扇区
			//order:扇区 根据实际容量设置 1024个 [0,1023]
			//擦除一个山区的最少时间:150ms
		*/
		void eraseSector(uint32_t order);
		
		void powerDown();
		void wakeUp();
	private:
		explicit Flash();
		
		void cs_valid(){LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);}
		void cs_invalid(){LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);}
		
		uint8_t transmitData(uint8_t ch);
		void send(const char* data, uint32_t num);
		void receive(uint8_t* data, uint32_t num);
		
		/*
			//读取W25QXX的状态寄存器
			//BIT7  6   5   4   3   2   1   0
			//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
			//SPR:默认0,状态寄存器保护位,配合WP使用
			//TB,BP2,BP1,BP0:FLASH区域写保护设置
			//WEL:写使能锁定
			//BUSY:忙标记位(1,忙;0,空闲)
			//默认:0x00   
		*/
		uint8_t readSR();
		/*
			//写W25QXX状态寄存器
			//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!! 
		*/
		void writeSR(uint8_t ch);
		/*
			//W25QXX写使能	
			//将WEL置位 
		*/
		void writeEnable();
		/*
			//W25QXX写使能	
			//将WEL清零 
		*/
		void writeDisable();
		void wait(){while((readSR()&0x01) == 0x01);}
		
	public:
		uint16_t id = 0;
	
	public:
		enum{//指令表
			CODE_WSTATUS_REG = 0x01,
			CODE_PAGE_PROGRAM,
			CODE_READ_DATA,
			CODE_WRITE_DISABLE,
			CODE_RSTATUS_REG,
			CODE_WRITE_ENABLE,
			CODE_FAST_READDATA = 0x0B,
			CODE_SECTOR_ERASE = 0x20,
			CODE_FAST_READ_DUAL = 0x3B,
			CODE_MANUFACT_ID = 0x90,
			CODE_JEDEC_ID = 0x9F,
			CODE_RELEASE_POWERDOWN = 0xAB,
			CODE_POWERDOWN = 0xB9,
			CODE_CHIP_ERASE = 0xC7,
			CODE_BLOCK_ERASE = 0xD8,
		};
};
#endif