#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__

#include <rtdevice.h>
#include <board.h>

/*#define BSP_USING_PWM2 1
#define BSP_USING_PWM4 1
#define BSP_USING_PWM
#define BSP_USING_PWM2
#define BSP_USING_PWM2_CH1
#define BSP_USING_PWM2_CH2
#define BSP_USING_PWM2_CH3
#define BSP_USING_PWM2_CH4
#define BSP_USING_PWM4
#define BSP_USING_PWM4_CH1
#define BSP_USING_PWM4_CH2
#define BSP_USING_PWM4_CH3
#define BSP_USING_PWM4_CH4

*/
//void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim);
extern void PWM_Init(rt_uint16_t arr, rt_uint16_t psc);

#endif /* __DRV_PWM_H__ */
