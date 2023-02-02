/*
*********************************************************************************************************
*
*   模块名称 : 开关量输入处理模块
*   文件名称 : di.h
*   版    本 : V1.0
*   说    明 : 头文件
*   修改记录 :
*
*********************************************************************************************************
*/

#ifndef _BSP_DI_H
#define _BSP_DI_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"

/* USER CODE BEGIN Includes */
//di GPIO引脚定义
#define DI1_RCC_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define DI1_GPIO_PIN                 GPIO_PIN_0
#define DI1_GPIO_PORT                GPIOC

#define DI2_RCC_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define DI2_GPIO_PIN                 GPIO_PIN_1
#define DI2_GPIO_PORT                GPIOC

#define DI3_RCC_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define DI3_GPIO_PIN                 GPIO_PIN_2
#define DI3_GPIO_PORT                GPIOC

#define DI4_RCC_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define DI4_GPIO_PIN                 GPIO_PIN_3
#define DI4_GPIO_PORT                GPIOC


/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define             DI_1                    0x0001  
#define             DI_2                    0x0002            
#define             DI_3                    0x0004  
#define             DI_4                    0x0008  
#define             DI_5                    0x0010       
#define             DI_6                    0x0020  
#define             DI_7                    0x0040  
#define             DI_8                    0x0080  
#define             DI_9                    0x0100  
#define             DI_10                   0x0200 
#define             DI_11                   0x0400  
#define             DI_12                   0x0800  
#define             DI_13                   0x1000  
#define             DI_14                   0x2000  
#define             DI_15                   0x4000 
#define             DI_16                   0x8000 

#define             DI1_PIN_STATE            HAL_GPIO_ReadPin(DI1_GPIO_PORT,DI1_GPIO_PIN)
#define             DI2_PIN_STATE            HAL_GPIO_ReadPin(DI2_GPIO_PORT,DI2_GPIO_PIN)
#define             DI3_PIN_STATE            HAL_GPIO_ReadPin(DI3_GPIO_PORT,DI3_GPIO_PIN)
#define             DI4_PIN_STATE            HAL_GPIO_ReadPin(DI4_GPIO_PORT,DI4_GPIO_PIN)


#define             STATE_CHANGE_TIME       10 //if the same input time reachs to STATE_CHANGE_TIME, we define the input state changed.

//设备的DI
typedef union
{
    uint16_t  ShortByte;
    struct
    {
        uint8_t bLmtPos                   :1;
        uint8_t bLmtNeg                   :1;
        uint8_t bCheckStart               :1;
        uint8_t Bit3                      :1;
        uint8_t Bit4                      :1;
        uint8_t Bit5                      :1;
        uint8_t Bit6                      :1;
        uint8_t Bit7                      :1;
        uint8_t Bit8                      :1;
        uint8_t Bit9                      :1;
        uint8_t Bit10                     :1;
        uint8_t Bit11                     :1;
        uint8_t Bit12                     :1;
        uint8_t Bit13                     :1;
        uint8_t Bit14                     :1;
        uint8_t Bit15                     :1;
    }Bit;
}SENSOR_STATUS_T; 



extern SENSOR_STATUS_T SenorStatusT;

/* USER CODE END Private defines */

/* USER CODE BEGIN */

//typedef void (*halDiCBack_t) (uint16_t di_data);

//extern void HalDiInit(void);

//extern void HalDiPoll (void);
extern void DI_GetDiState(void);
extern void DI_GetFirstInputState(void);
extern uint8_t GetOneDiState(uint8_t Di_Channel);
extern void DiTest(void);
extern uint16_t GetDiState(void);
extern uint16_t SensorStatusCheck(void);
extern uint16_t DiStatus(void);
extern void DiInit(void);
/* USER CODE END  */

#ifdef __cplusplus
}
#endif
#endif /*_BSP_DI_H */

/**
  * @}
  */
/***************************** (END OF FILE) *********************************/

