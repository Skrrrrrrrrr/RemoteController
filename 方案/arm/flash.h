#ifndef FLASH_H
#define FLASH_H
#include <stdint.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>

class Flash{
	public:
		static Flash &instance();
	
		/*
			//��ȡSPI FLASH  
			//��ָ����ַ��ʼ��ȡָ�����ȵ�����
			//pBuffer:���ݴ洢��
			//sectorOrder: [0,1023]
			//pageOrder: [0,15]
			//num:Ҫд����ֽ���(���256)
		*/
		void read(uint8_t* data, uint16_t sectorOrder, uint16_t pageOrder, uint16_t num);
		/*
			//SPI��һҳ(0~65535)��д������256���ֽڵ�����
			//��ָ����ַ��ʼд�����256�ֽڵ�����
			//pBuffer:���ݴ洢��
			//sectorOrder: [0,1023]
			//pageOrder: [0,15]
			//num:Ҫд����ֽ���(���256)
		*/
		void write(uint8_t* data, uint16_t sectorOrder, uint16_t pageOrder, uint16_t num);
		/*
			//����һ������
			//order:���� ����ʵ���������� 1024�� [0,1023]
			//����һ��ɽ��������ʱ��:150ms
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
			//��ȡW25QXX��״̬�Ĵ���
			//BIT7  6   5   4   3   2   1   0
			//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
			//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
			//TB,BP2,BP1,BP0:FLASH����д��������
			//WEL:дʹ������
			//BUSY:æ���λ(1,æ;0,����)
			//Ĭ��:0x00   
		*/
		uint8_t readSR();
		/*
			//дW25QXX״̬�Ĵ���
			//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!! 
		*/
		void writeSR(uint8_t ch);
		/*
			//W25QXXдʹ��	
			//��WEL��λ 
		*/
		void writeEnable();
		/*
			//W25QXXдʹ��	
			//��WEL���� 
		*/
		void writeDisable();
		void wait(){while((readSR()&0x01) == 0x01);}
		
	public:
		uint16_t id = 0;
	
	public:
		enum{//ָ���
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