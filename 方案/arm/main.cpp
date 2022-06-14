#include <stm32f4xx_hal.h>
#include <stm32f4xx_ll_utils.h>
#include "flash.h"
#include "blink.h"
#include "lcd_DevEBox.h"

//Transfer GuiLite 32 bits color to your LCD color
#define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))
//Encapsulate your LCD driver:
void gfx_draw_pixel(int x, int y, unsigned int rgb)
{
	LcdDevEBox::instance().fast_drawPoint(x, y, GL_RGB_32_to_16(rgb));
}

void gfx_fill_rect(int x1, int y1, int x2, int y2, uint32_t Color)
{
	LcdDevEBox::instance().drawRectangle(x1, y1, x2, y2, GL_RGB_32_to_16(Color));
}

//UI entry
struct EXTERNAL_GFX_OP
{
	void (*draw_pixel)(int x, int y, unsigned int rgb);
	void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} my_gfx_op;
extern void startHelloWave(void* phy_fb, int width, int height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op);

int main(){
	HAL_Init();
	Blink::instance();
	Flash::instance();
	LcdDevEBox::instance();
	
	//Link your LCD driver & start UI:
	my_gfx_op.draw_pixel = gfx_draw_pixel;
	my_gfx_op.fill_rect = gfx_fill_rect;//gfx_fill_rect;
	startHelloWave(NULL, 240, 320, 2, &my_gfx_op);
	
	while(1){
		
		
	}
}