#include "blink.h"
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_tim.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_utils.h>
#include <stm32f4xx_ll_system.h>
Blink::Blink():count(0){
	LL_RCC_HSE_EnableCSS();
	
	SystemCoreClockUpdate();
	
	LL_RCC_ClocksTypeDef RCC_ClocksTypeDef;
	LL_RCC_GetSystemClocksFreq(&RCC_ClocksTypeDef);
	
	LL_Init1msTick(RCC_ClocksTypeDef.HCLK_Frequency);
	LL_SetSystemCoreClock(RCC_ClocksTypeDef.HCLK_Frequency);
	
	LL_GPIO_InitTypeDef GPIO_InitTypeDef = {LL_GPIO_PIN_13,
		LL_GPIO_MODE_OUTPUT, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL,
		LL_GPIO_PULL_UP, LL_GPIO_AF_0
	};
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_GPIO_Init(GPIOC, &GPIO_InitTypeDef);
	
	LL_TIM_InitTypeDef TIM_InitTypeDef = {1599, LL_TIM_COUNTERMODE_DOWN, 499,
		LL_TIM_CLOCKDIVISION_DIV1, 0
	};//50ms
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	
	NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0));
	NVIC_EnableIRQ(TIM2_IRQn);
	
	LL_TIM_Init(TIM2,&TIM_InitTypeDef);
}
	
Blink &Blink::instance()
{
    static Blink singleton;
		if(LL_TIM_IsEnabledIT_UPDATE(TIM2) != 1){
			LL_TIM_EnableIT_UPDATE(TIM2);
		}
		if(LL_TIM_IsEnabledCounter(TIM2) != 1){
			LL_TIM_EnableCounter(TIM2);
		}
    return singleton;
}

void Blink::toggle(){
	LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
}

extern "C"{
	void TIM2_IRQHandler(){
		if(LL_TIM_IsActiveFlag_UPDATE(TIM2)){
			Blink::instance().count++;
			if((Blink::instance().count % 20) == 0){
				Blink::instance().toggle();
			}
		}
		LL_TIM_ClearFlag_UPDATE(TIM2);
	}
};