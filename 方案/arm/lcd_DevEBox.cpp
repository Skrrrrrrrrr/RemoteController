#include "lcd_DevEBox.h"
#include <stm32f4xx_ll_bus.h>

LcdDevEBox::LcdDevEBox(){
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
	
	LL_GPIO_InitTypeDef GPIO_InitTypeDef = {
		LL_GPIO_PIN_12 | LL_GPIO_PIN_10,
		LL_GPIO_MODE_OUTPUT, LL_GPIO_SPEED_FREQ_HIGH,
		LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_NO, LL_GPIO_AF_0
	};
	LL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);
	
	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_2;
	GPIO_InitTypeDef.Pull = LL_GPIO_PULL_DOWN;
	LL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);
	
	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
	GPIO_InitTypeDef.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitTypeDef.Pull = LL_GPIO_PULL_UP;
	GPIO_InitTypeDef.Alternate = LL_GPIO_AF_5;
	LL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);
	
	LL_SPI_InitTypeDef SPI_InitTypeDef = {
		LL_SPI_FULL_DUPLEX, LL_SPI_MODE_MASTER, 
		LL_SPI_DATAWIDTH_8BIT, LL_SPI_POLARITY_HIGH, 
		LL_SPI_PHASE_2EDGE, LL_SPI_NSS_SOFT, 
		LL_SPI_BAUDRATEPRESCALER_DIV2, LL_SPI_MSB_FIRST, 
		LL_SPI_CRCCALCULATION_DISABLE, 7
	};
	LL_SPI_Init(SPI2, &SPI_InitTypeDef);
	LL_SPI_Enable(SPI2);
	
	lcdInit();
}

LcdDevEBox &LcdDevEBox::instance(){
	static LcdDevEBox singleton;
	return singleton;
}

void LcdDevEBox::fast_drawPoint(uint16_t x,uint16_t y,uint16_t color){
	writeCode(lcdDev.setxcmd); 
	writeData16(x);

	writeCode(lcdDev.setycmd); 
	writeData16(y);
		 
	writeCode(lcdDev.wramcmd); 
	writeData16(color); //д��16λ��ɫ
}

void LcdDevEBox::lcdInit(){
	softReset();
	
	writeCode(0xCF);
	writeData("\x00\xD9\x30", 3);
	
	writeCode(0xED);
	writeData("\x64\x03\x12\x81", 4);
	
	writeCode(0xE8);
	writeData("\x85\x10\x78", 3);
	
	writeCode(0xCB);
	writeData("\x39\x2c\x00\x34\x02", 5);
	
	writeCode(0xF7);
	writeData(0x20);
	
	writeCode(0xEA);
	writeData("\x00\x00", 2);
	
	writeCode(0xC0);
	writeData(0x21);
	
	writeCode(0xC1);
	writeData(0x12);
	
	writeCode(0xC5);
	writeData("\x32\x3c", 2);
	
	writeCode(0xC7);
	writeData(0xC1);
	
	writeCode(0x36);
	writeData(0x08);
	
	writeCode(0x3A);
	writeData(0x55);
	
	writeCode(0xB1);
	writeData("\x00\x18", 2);
	
	writeCode(0xB6);
	writeData("\x0a\xa2", 2);
	
	writeCode(0xF2);
	writeData(0x00);
	
	writeCode(0x26);
	writeData(0x01);
	
	writeCode(0xE0);
	writeData("\x0f\x20\x1e\x09\x12\x0b\x50\xba\x44\x09\x14\x05\x23\x21\x00", 15);
	
	writeCode(0xE1);
	writeData("\x00\x19\x19\x00\x12\x07\x2d\x28\x3f\x02\x0a\x08\x25\x2d\x0f", 15);
	
	writeCode(0x11);
	
	LL_mDelay(120);
	
	writeCode(0x29);

	
	lcdDisplayDir(LCD_DIR_HORIZONTAL1);
	
	blk_open();
	
	clear(WHITE);
}

void LcdDevEBox::setScanDir(uint8_t dir){
	uint16_t regval=0;

	//ɨ�跽����--ɨ�跽ʽ�в�ͬ��񣬿��ܶ��岻���Һ����µĲ��շ���ͬ���᷽ܽʽ��ֻ��һ�°���
	
	switch(dir)	{
		case L2R_U2D://������,���ϵ���
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://���µ���,���ҵ���
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
		
	//�ı�ɨ�跽������  
	writeCode(0x36);
	writeData(regval|0x08);
}

void LcdDevEBox::lcdDisplayDir(uint8_t dir){
	
	uint8_t SCAN_DIR;
	switch(dir){
		case LCD_DIR_VERTICAL1:
			lcdDev.dir=0;	     //����
			lcdDev.width=240;
			lcdDev.height=320;
			
			lcdDev.wramcmd=0X2C;
			lcdDev.setxcmd=0X2A;
			lcdDev.setycmd=0X2B;
		
			SCAN_DIR=L2R_U2D; //ѡ��ɨ�跽��
			break;
		case LCD_DIR_VERTICAL2:
			lcdDev.dir=0;	     //����
			lcdDev.width=240;
			lcdDev.height=320;
			
			lcdDev.wramcmd=0X2C;
			lcdDev.setxcmd=0X2A;
			lcdDev.setycmd=0X2B;
		
			SCAN_DIR=R2L_D2U; //ѡ��ɨ�跽��
			break;
		case LCD_DIR_HORIZONTAL1:
			lcdDev.dir=1;	     //����
			lcdDev.width=320;
			lcdDev.height=240;

			lcdDev.wramcmd=0X2C;
			lcdDev.setxcmd=0X2A;
			lcdDev.setycmd=0X2B;
		
			SCAN_DIR=U2D_R2L; //ѡ��ɨ�跽��
			break;
		case LCD_DIR_HORIZONTAL2:
			lcdDev.dir=1;	        //����
			lcdDev.width=320;
			lcdDev.height=240;

			lcdDev.wramcmd=0X2C;
			lcdDev.setxcmd=0X2A;
			lcdDev.setycmd=0X2B; 
			
			SCAN_DIR=D2U_L2R; //ѡ��ɨ�跽��
			break;
		default:
			lcdDev.dir=0;	     //����
			lcdDev.width=240;
			lcdDev.height=320;
			
			lcdDev.wramcmd=0X2C;
			lcdDev.setxcmd=0X2A;
			lcdDev.setycmd=0X2B;
		
			SCAN_DIR=L2R_U2D; //ѡ��ɨ�跽��
			break;
	}
	writeCode(lcdDev.setxcmd);
	writeData16(0);
	writeData16(lcdDev.width-1);
	
	writeCode(lcdDev.setycmd);
	writeData16(0);
	writeData16(lcdDev.height-1);
	
	setScanDir(SCAN_DIR);
}

void LcdDevEBox::writeGRAM(){
	writeCode(lcdDev.wramcmd);
}

void LcdDevEBox::setCursor(uint16_t x, uint16_t y){
	writeCode(lcdDev.setxcmd); 
	writeData16(x);

	writeCode(lcdDev.setycmd);
	writeData16(y);	
}

void LcdDevEBox::drawPoint(uint16_t x, uint16_t y){
	setCursor(x,y);		//���ù��λ�� 
	writeGRAM();	//��ʼд��GRAM
	writeData16(RED); 
}

void LcdDevEBox::setWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height){   
	width=sx+width-1;
	height=sy+height-1;

	writeCode(lcdDev.setxcmd); 
	writeData16(sx);      //���� X�������
	writeData16(width);   //���� X�����յ�	

	writeCode(lcdDev.setycmd);
	writeData16(sy);      //���� Y�������
	writeData16(height);  //���� Y�����յ�
} 

void LcdDevEBox::clear(uint16_t color){
	uint32_t index=0;
	uint32_t totalpoint;
	
	setWindow(0,0,lcdDev.width,lcdDev.height);//����ȫ������
	
	totalpoint=lcdDev.width * lcdDev.height; 			//�õ��ܵ���
 
	setCursor(0x00,0x00);	//���ù��λ�� 
	
	writeGRAM();     		//��ʼд��GRAM	
	
	for(index=0;index<totalpoint;index++)
	{
		writeData16(color);	
	}
} 

void LcdDevEBox::fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color){          
	uint32_t i;
	uint32_t xlen=0;

//���ô���
	writeCode(lcdDev.setxcmd); 
	writeData16(sx);//���� X�������
	writeData16(ex-1);//���� X�����յ�


	writeCode(lcdDev.setycmd);
	writeData16(sy);//���� Y�������
	writeData16(ey-1);//���� Y�����յ�

			
	writeCode(lcdDev.wramcmd);	  

	xlen=(ex-sx)*(ey-sy);//������ܹ���Ҫд��ĵ���
	
	writeGRAM();     			//��ʼд��GRAM
	
	for(i=0;i<=xlen;i++)
	{
		writeData16(color);	//��ʾ��ɫ 	    
	}


//�ָ�ȫ������
		
//�������ã�Ϊ���ڲ������ã�������ȫ������ʾ��Χ			
			
	writeCode(lcdDev.setxcmd); 
	writeData16(0);             //���� X�������
	writeData16(lcdDev.width-1);//���� X�����յ�

	writeCode(lcdDev.setycmd);
	writeData16(0);              //���� Y�������
	writeData16(lcdDev.height-1);//���� Y�����յ�		 
}

void LcdDevEBox::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color){
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	
	
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else
     {
		    incx=-1;
		    delta_x=-delta_x;
		 }

	
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else
     { 
		   incy=-1;
		   delta_y=-delta_y;
		 }

	
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		fast_drawPoint(uRow,uCol,Color);//���� --ʹ��������ɫ���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

void LcdDevEBox::drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color){
	drawLine(x1,y1,x2,y1,Color);
	drawLine(x1,y1,x1,y2,Color);
	drawLine(x1,y2,x2,y2,Color);
	drawLine(x2,y1,x2,y2,Color);
}

void LcdDevEBox::writeData(uint8_t ch){
	cs_valid();
	data_mode();
	transmitData(ch);
	cs_invalid();
}
void LcdDevEBox::writeData16(uint16_t ch){
	cs_valid();
	data_mode();
	transmitData(ch>>8);
	transmitData(ch);
	cs_invalid();
}
void LcdDevEBox::writeData(const char* ch, uint16_t num){
	cs_valid();
	data_mode();
	for(uint16_t i=0; i<num; i++){
		transmitData(ch[i]);
	}
	cs_invalid();
}

void LcdDevEBox::writeCode(uint8_t ch){
	cs_valid();
	code_mode();
	transmitData(ch);
	cs_invalid();
}

uint8_t LcdDevEBox::transmitData(uint8_t ch){
	while(!LL_SPI_IsActiveFlag_TXE(SPI2));
	LL_SPI_TransmitData8(SPI2, ch);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI2));
	return LL_SPI_ReceiveData8(SPI2);
}