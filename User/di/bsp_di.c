/******************************************************************
 * 2017 (C)   IST Copyright. All Rights Reserved.
 * File name    : di.c
 * Author       : yxy
 * Version      : V1.0
 * Date         : 2020
 * Description  :
 * Function List:
 * History      :
*******************************************************************/

#include "bsp.h"

static uint16_t DI_NowState = 0x0000;                       // The input port state now.
static uint16_t DI_ChangeState = 0x00;                  // Which port state is changed.
static uint16_t SameInputTimes = STATE_CHANGE_TIME; // The same input times. if the state is changing, the times++ every call the getDI_State.
static uint16_t DI_port_state = 0x0000;             // the port state input
static uint16_t DI_BufState = 0x0000;                   // If the state is changing, Buf State is the changing state.
static uint16_t DI_NewState = 0x0000;                   // If state is changed, here's the new state
static uint16_t temp_DI_ChangeState = 0x00;         // Which port state is change. DI_ChangeState = DI_NowState ^ DI_Newstate
// 应用定义
static uint16_t SensorDiStatus = 0;
SENSOR_STATUS_T SenorStatusT;

/******************************************************************************
 * @fn      uint16_t ReadPinDiState(void)
 *
 * @brief   Read di pin state
 *
 * @param   None
 *              
 * @return  None
*******************************************************************************/
uint16_t ReadPinDiState(void)
{
    uint16_t di_port_state = 0;
        
    if(DI1_PIN_STATE == GPIO_PIN_SET) { di_port_state |= DI_1;}
    else { di_port_state &=~DI_1; }
    if(DI2_PIN_STATE == GPIO_PIN_SET) { di_port_state |= DI_2;}
    else { di_port_state &=~DI_2; }
    if(DI3_PIN_STATE == GPIO_PIN_SET) { di_port_state |= DI_3;}
    else { di_port_state &=~DI_3; }
    if(DI4_PIN_STATE == GPIO_PIN_SET) { di_port_state |= DI_4;}
    else { di_port_state &=~DI_4; }
    
    return di_port_state;
}

/******************************************************************************
 * @fn      void DI_GetDiState(void)
 *
 * @brief   Find if there's any input port state changes. If changed, DI_ChangeState <>0;
 *              1) DI_ChangeState ---- define which input state has been changed and need transfer to IPC.
                    2) global: DI_NowState ---- all port input state now (after changed)
                    It will be called by timer.
 * @param   None
 *              
 * @return  None
*******************************************************************************/
void DI_GetDiState(void){

    DI_port_state = ReadPinDiState();               // Read input state 
    
    if (DI_port_state != DI_BufState)   // Is a new port state input
    {         
        SameInputTimes = STATE_CHANGE_TIME;     // yes. re-count to filter this state
        DI_BufState = DI_port_state;
    }
    else
    {                                                                       // If filter time reachs, the input is availability.
        if (DI_BufState != DI_NewState)
        {                               // if state is not change, there's not any filter need.
            if(SameInputTimes)                                  // filter time
                -- SameInputTimes;  
            else
            {                                                                   // is a new input after filter time
                DI_NewState = DI_BufState;
                temp_DI_ChangeState = (DI_NewState ^ DI_NowState);      //Define which state is changing.
                if (temp_DI_ChangeState)                        // There's some change for input state.
                    DI_NowState = DI_NewState;                  //Judge what changes, and prepare send this message to IPC.
                DI_ChangeState |= temp_DI_ChangeState;              //Output DI changed state.
#ifdef LOG
    DiTest();
#endif
                
           if(((DI_NowState>>0)&0x01) == 1)
               SenorStatusT.Bit.bLmtPos = 1;
           else if(((DI_NowState>>0)&0x01) == 0)
               SenorStatusT.Bit.bLmtPos = 0;

           if(((DI_NowState>>1)&0x01) == 1)
               SenorStatusT.Bit.bLmtNeg = 1;
           else if(((DI_NowState>>1)&0x01) == 0)
               SenorStatusT.Bit.bLmtNeg = 0;
           
           if(((DI_NowState>>2)&0x01) == 1)
               SenorStatusT.Bit.bCheckStart = 1;
           else if(((DI_NowState>>2)&0x01) == 0)
               SenorStatusT.Bit.bCheckStart = 0;
           
           if(((DI_NowState>>3)&0x01) == 1)
               SenorStatusT.Bit.Bit3 = 1;
           else if(((DI_NowState>>3)&0x01) == 0)
               SenorStatusT.Bit.Bit3 = 0;
           }
       }
    }
}

/******************************************************************************
 * @fn      void DI_GetFirstInputState(void)
 *
 * @brief   Get the first input state.
 *
 * @param   None
 *              
 * @return  None
*******************************************************************************/
void DI_GetFirstInputState(void)
{
    uint16_t temp;

    temp = STATE_CHANGE_TIME + 1;

    while(--temp)
        DI_GetDiState();
}

/******************************************************************************
 * @fn      uint16_t GetOneDiState(uint16_t Di_Channel)
 *
 * @brief   Get one bit of the Di state.
                    One byte of Di change state for RS232 Send buf.
 *
 * @param   None
 *              
 * @return  None
*******************************************************************************/
uint8_t GetOneDiState(uint8_t Di_Channel)
{
    uint16_t di_channel_bit = 0;
    
    di_channel_bit |= (0x01<<(Di_Channel-1)); 
#ifdef LOG
    printf("\r\n GetOneDiState di_channel: %3x",Di_Channel);
    printf("\r\n GetOneDiState di_channel_bit:  %3x",di_channel_bit);
#endif
/*    
    switch (di_channel_bit)
    {
        case DI_IN1_TEST:
            if(DI_NowState & DI_IN1_TEST)
                return 1;
            else
                return 0;
        default:
            break;
    }
 */
    return 0;
}

/******************************************************************************
 * @fn      uint16_t GetDiState(void)
 *
 * @brief Get DI_NowState
 *
 * @param   None
 *              
 * @return  None
*******************************************************************************/
uint16_t GetDiState(void){
    return DI_NowState;
}

//通信查询用
uint16_t DiStatus(void)
{   
    return SenorStatusT.ShortByte;
}

uint16_t SensorStatusCheck(void)
{
    uint16_t di_channel_status = 0;
    uint8_t di_status_L = 0;

    di_channel_status = GetDiState();    
    
    if(di_channel_status != SensorDiStatus)
    {
        SensorDiStatus = di_channel_status;
        di_status_L = (uint8_t)di_channel_status;
        if(((di_channel_status>>0)&0x01) == 1)
            SenorStatusT.Bit.bLmtPos = 1;
        else if(((di_channel_status>>0)&0x01) == 0)
            SenorStatusT.Bit.bLmtPos = 0;
        
        if(((di_channel_status>>1)&0x01) == 1)
            SenorStatusT.Bit.bLmtNeg = 1;
        else if(((di_channel_status>>1)&0x01) == 0)
            SenorStatusT.Bit.bLmtNeg = 0;

        if(((di_channel_status>>2)&0x01) == 1)
            SenorStatusT.Bit.bCheckStart = 1;
        else if(((di_channel_status>>2)&0x01) == 0)
            SenorStatusT.Bit.bCheckStart = 0;

        if(((di_channel_status>>3)&0x01) == 1)
            SenorStatusT.Bit.Bit3 = 1;
        else if(((di_channel_status>>3)&0x01) == 0)
            SenorStatusT.Bit.Bit3 = 0;

#ifdef LOG  
        printf("\r\n DI :");
        printf("%x\r\n", SenorStatusT.ShortByte);
/*        printf("\r\n SendCardSenorStatus.Bit.CardReset :");
        printf("%x\r\n", SendCardSenorStatus.Bit.CardReset);
        printf("\r\n SendCardSenorStatus.Bit.CardRead :");
        printf("%x\r\n", SendCardSenorStatus.Bit.CardRead);
        printf("\r\n SendCardSenorStatus.Bit.CardOut :");
        printf("%x\r\n", SendCardSenorStatus.Bit.CardOut);
        printf("\r\n SendCardSenorStatus.Bit.CardStoreEmpty :");
        printf("%x\r\n", SendCardSenorStatus.Bit.CardStoreEmpty);
        printf("\r\n SendCardSenorStatus.Bit.NoteReset :");
        printf("%x\r\n", SendCardSenorStatus.Bit.NoteReset);
        printf("\r\n SendCardSenorStatus.Bit.NoteRead :");
        printf("%x\r\n", SendCardSenorStatus.Bit.NoteRead);
        printf("\r\n SendCardSenorStatus.Bit.NoteOut :");
        printf("%x\r\n", SendCardSenorStatus.Bit.NoteOut);
        printf("\r\n SendCardSenorStatus.Bit.NoteStoreEmpty :");
        printf("%x\r\n", SendCardSenorStatus.Bit.NoteStoreEmpty);
  */  
#endif
    }
    
    return di_channel_status;
}


void DiInit(void)
{
    
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* GPIO Ports Clock Enable */
    DI1_RCC_CLK_ENABLE();
    DI2_RCC_CLK_ENABLE();
    DI3_RCC_CLK_ENABLE();
    DI4_RCC_CLK_ENABLE();
        
    /*Configure GPIO pins : PCPin PCPin PCPin PCPin 
                             PCPin PCPin PCPin PCPin 
                             PCPin PCPin */
    GPIO_InitStruct.Pin = DI1_GPIO_PIN|DI2_GPIO_PIN|DI3_GPIO_PIN|DI4_GPIO_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

//DI 测试函数
void DiTest(void)
{

#ifdef LOG
    //if(GetDiState() & DI_IN1_TEST)
    //{
        printf("\r\n DI_BIT_INPUT_STATE = %x !!!\n\r",GetDiState());
        //HalDoSet(HAL_DO_15,HAL_DO_MODE_ON);
   // }
    //else
   // {
        //printf("\n\r DDI_BIT_INPUT_STATE = %x !!!\n\r",GetDiState());
        //HalDoSet(HAL_DO_15,HAL_DO_MODE_OFF);
   // }
#endif
}


/****************************************************
查询DI : 55 05 05 00 40
****************************************************/

