/******************************************************************
 * 2017 (C)   IST Copyright. All Rights Reserved.
 * File name    : di.c
 * Author       : yxy
 * Version      : V1.0
 * Date         : 2017
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

/**
  * @brief  Di pin GPIO init
  * @param  None
  * @retval None
  */
void Di0Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // Configure GPIO pins
  GPIO_InitStruct.Pin = IN_S0_Pin|IN_S1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = IN_S2_Pin|IN_S3_Pin|IN_S4_Pin|IN_S5_Pin|IN_S6_Pin|IN_S7_Pin|IN_S8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = IN_S9_Pin|IN_S10_Pin|IN_S11_Pin|IN_S12_Pin|IN_S13_Pin|IN_S14_Pin|IN_S15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = IN_BOARD_TYPE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief  Read di pin state
  * @param  None
  * @retval None
  */
uint16_t ReadPinDiState(void)
{
    uint16_t di_port_state = 0;

    if(DI0_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_0;}  //取位值
    else { di_port_state |= DI_0; }
    if(DI1_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_1; }
    else { di_port_state |= DI_1; }
    if(DI2_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_2;}
    else { di_port_state |= DI_2; }
    if(DI3_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_3;}
    else { di_port_state |= DI_3; }
    if(DI4_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_4;}
    else { di_port_state |= DI_4; }
    if(DI5_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_5;}
    else { di_port_state |= DI_5; }
    if(DI6_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_6;}
    else { di_port_state |= DI_6; }
    if(DI7_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_7;}
    else { di_port_state |= DI_7; }
    if(DI8_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_8;}
    else { di_port_state |= DI_8; }

    if(DI9_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_9;}
    else { di_port_state |= DI_9; }
    if(DI10_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_10;}
    else { di_port_state |= DI_10; }
    if(DI11_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_11;}
    else { di_port_state |= DI_11; }
    if(DI12_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_12;}
    else { di_port_state |= DI_12; }
    if(DI13_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_13;}
    else { di_port_state |= DI_13; }
    if(DI14_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_14;}
    else { di_port_state |= DI_14; }
    if(DI15_PIN_STATE == GPIO_PIN_RESET) { di_port_state &=~DI_15;}
    else { di_port_state |= DI_15; }

    return di_port_state;
}


/**
  * @brief  Find if there's any input port state changes. If changed, DI_ChangeState <>0;
 *          1) DI_ChangeState ---- define which input state has been changed and need transfer to IPC.
            2) global: DI_NowState ---- all port input state now (after changed)
            It will be called by timer.
  * @param  None
  * @retval None
  */
void DI_GetDiState(void){

    DI_port_state = ReadPinDiState();               // Read input state获取输入端口状态

    if (DI_port_state != DI_BufState)   // Is a new port state input  端口状态有变化
    {
        SameInputTimes = STATE_CHANGE_TIME;     // yes. re-count to filter this state  过滤时间  10ms
        DI_BufState = DI_port_state;
    }
    else
    {                                              // If filter time reachs, the input is availability.
        if (DI_BufState != DI_NewState)
        {                               // if state is not change, there's not any filter need.  无变化，无需过滤
            if(SameInputTimes)                                  // filter time  过滤时间
                -- SameInputTimes;
            else
            {                                                   // is a new input after filter time  过滤时间后是否有新的输入
                DI_NewState = DI_BufState;
                temp_DI_ChangeState = (DI_NewState ^ DI_NowState);      //Define which state is changing. 定义哪个端口状态正在改变
                if (temp_DI_ChangeState)                        // There's some change for input state.输入状态有一些变化
                    DI_NowState = DI_NewState;                  //Judge what changes, and prepare send this message to IPC.判断发生了什么变化，并准备将此信息发送给IPC。
                DI_ChangeState |= temp_DI_ChangeState;              //Output DI changed state.输出DI改变状态
//#ifdef LOG
    DiTest();  //测试输入端口变化
//#endif
           }
       }
    }
}


/**
  * @brief  Get the first input state.
  * @param  None
  * @retval None
  */
void DI_GetFirstInputState(void)
{
    uint16_t temp;

    temp = STATE_CHANGE_TIME + 1;

    while(--temp)     //10次
        DI_GetDiState();
}

/**
  * @brief  Get one bit of the Di state.
            One byte of Di change state for RS232 Send buf.
  * @param  None
  * @retval None
  */
uint8_t GetOneDiState(uint8_t Di_Channel) //获取某位DI状态
{
    uint16_t di_channel_bit = 0;

    //di_channel_bit |= (0x01<<(Di_Channel-1));
    di_channel_bit |= (0x01<<(Di_Channel));
    #ifdef LOG
        printf("\r\n GetOneDiState di_channel: %3x",Di_Channel);
        printf("\r\n GetOneDiState di_channel_bit:  %3x",di_channel_bit);
    #endif
    switch (di_channel_bit)
    {
        case DI_0:
            if(DI_NowState & DI_0)
                return 1;
            else
                return 0;
        case DI_1:
            if(DI_NowState & DI_1)
                return 1;
            else
                return 0;
        case DI_2:
            if(DI_NowState & DI_2)
                return 1;
            else
                return 0;
        case DI_3:
            if(DI_NowState & DI_3)
                return 1;
            else
                return 0;
        case DI_4:
            if(DI_NowState & DI_4)
                return 1;
            else
                return 0;
        case DI_5:
            if(DI_NowState & DI_5)
                return 1;
            else
                return 0;
        case DI_6:
            if(DI_NowState & DI_6)
                return 1;
            else
                return 0;
        case DI_7:
            if(DI_NowState & DI_7)
                return 1;
            else
                return 0;
        case DI_8:
            if(DI_NowState & DI_8)
                return 1;
            else
                return 0;
        case DI_9:
            if(DI_NowState & DI_9)
                return 1;
            else
                return 0;
        case DI_10:
            if(DI_NowState & DI_10)
                return 1;
            else
                return 0;
        case DI_11:
            if(DI_NowState & DI_11)
                return 1;
            else
                return 0;
        case DI_12:
            if(DI_NowState & DI_12)
                return 1;
            else
                return 0;
        case DI_13:
            if(DI_NowState & DI_13)
                return 1;
            else
                return 0;
        case DI_14:
            if(DI_NowState & DI_14)
                return 1;
            else
                return 0;
        case DI_15:
            if(DI_NowState & DI_15)
                return 1;
            else
                return 0;
        default:
            break;
    }
    return 0;
}


/**
  * @brief  Get DI_NowState
  * @param  None
  * @retval None
  */
uint16_t GetDiState(void){
    return DI_NowState;
}

/**
  * @brief  通信查询状态
  * @param  None
  * @retval None
  */
uint16_t DiStatus(void)
{
    return SensorDiStatus;
}
/**
  * @brief  更新di数据
  * @param  None
  * @retval None
  */
uint16_t SensorStatusCheck(void)
{
    uint16_t di_channel_status = 0;
    uint8_t di_status_L = 0;

    di_channel_status = GetDiState();//获取当前输入端口值

    if(di_channel_status != SensorDiStatus)
    {
        SensorDiStatus = di_channel_status;
        di_status_L = (uint8_t)di_channel_status;//取低8位

        if(((di_channel_status>>0)&0x01) == 1)
            SenorStatusT.Bit.bCardBottom = 1;
        else if(((di_channel_status>>0)&0x01) == 0)
            SenorStatusT.Bit.bCardBottom = 0;

        if(((di_channel_status>>1)&0x01) == 1)
            SenorStatusT.Bit.bCardTop = 1;
        else if(((di_channel_status>>1)&0x01) == 0)
            SenorStatusT.Bit.bCardTop = 0;

        if(((di_channel_status>>2)&0x01) == 1)
            SenorStatusT.Bit.bCardIsEmpty= 1;
        else if(((di_channel_status>>2)&0x01) == 0)
            SenorStatusT.Bit.bCardIsEmpty = 0;

        if(((di_channel_status>>3)&0x01) == 1)
            SenorStatusT.Bit.bPushCardLeft = 1;
        else if(((di_channel_status>>3)&0x01) == 0)
            SenorStatusT.Bit.bPushCardLeft = 0;

        if(((di_channel_status>>4)&0x01) == 1)
            SenorStatusT.Bit.bPushCardRight = 1;
        else if(((di_channel_status>>4)&0x01) == 0)
            SenorStatusT.Bit.bPushCardRight = 0;

        if(((di_channel_status>>5)&0x01) == 1)
            SenorStatusT.Bit.bOutCardLeft = 1;
        else if(((di_channel_status>>5)&0x01) == 0)
            SenorStatusT.Bit.bOutCardLeft = 0;

        if(((di_channel_status>>6)&0x01) == 1)
            SenorStatusT.Bit.bOutCardRight = 1;
        else if(((di_channel_status>>6)&0x01) == 0)
            SenorStatusT.Bit.bOutCardRight = 0;

        if(((di_channel_status>>7)&0x01) == 1)
            SenorStatusT.Bit.bCardDoor= 1;
        else if(((di_channel_status>>7)&0x01) == 0)
            SenorStatusT.Bit.bCardDoor= 0;

        if(((di_channel_status>>8)&0x01) == 1)
            SenorStatusT.Bit.bBookInPlace= 1;
        else if(((di_channel_status>>8)&0x01) == 0)
            SenorStatusT.Bit.bBookInPlace= 0;

          if(((di_channel_status>>9)&0x01) == 1)
            SenorStatusT.Bit.bBookTop = 1;
        else if(((di_channel_status>>9)&0x01) == 0)
            SenorStatusT.Bit.bBookTop= 0;

          if(((di_channel_status>>10)&0x01) == 1)
            SenorStatusT.Bit.bBookIsEmpty = 1;
        else if(((di_channel_status>>10)&0x01) == 0)
            SenorStatusT.Bit.bBookIsEmpty = 0;

        if(((di_channel_status>>11)&0x01) == 1)
            SenorStatusT.Bit.bPushBookLeft = 1;
        else if(((di_channel_status>>11)&0x01) == 0)
            SenorStatusT.Bit.bPushBookLeft= 0;

        if(((di_channel_status>>12)&0x01) == 1)
            SenorStatusT.Bit.bPushBookRight= 1;
        else if(((di_channel_status>>12)&0x01) == 0)
            SenorStatusT.Bit.bPushBookRight= 0;

        if(((di_channel_status>>13)&0x01) == 1)
            SenorStatusT.Bit.bOutBookLeft= 1;
        else if(((di_channel_status>>13)&0x01) == 0)
            SenorStatusT.Bit.bOutBookLeft= 0;

        if(((di_channel_status>>14)&0x01) == 1)
            SenorStatusT.Bit.bOutBookRight = 1;
        else if(((di_channel_status>>14)&0x01) == 0)
            SenorStatusT.Bit.bOutBookRight= 0;

        if(((di_channel_status>>15)&0x01) == 1)
            SenorStatusT.Bit.bBookDoor = 1;
        else if(((di_channel_status>>15)&0x01) == 0)
            SenorStatusT.Bit.bBookDoor = 0;


#ifdef LOG
        printf("\r\n DI :");
        printf("%x\r\n", SenorStatusT.ShortByte);
/*      printf("\r\n SendCardSenorStatus.Bit.CardReset :");
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

/**
  * @brief  DI 测试函数
  * @param  None
  * @retval None
  */
void DiTest(void)
{
    #ifdef LOG
	printf("\r\n DI_BIT_INPUT_STATE = %x !!!\n\r",GetDiState());

//    if(GetDiState() & DI_IN1_TEST)
//    {
//        printf("\r\n DI_BIT_INPUT_STATE = %x !!!\n\r",GetDiState());
//        //HalDoSet(HAL_DO_15,HAL_DO_MODE_ON);
//    }
//    else
//    {
//        printf("\n\r DDI_BIT_INPUT_STATE = %x !!!\n\r",GetDiState());
//        //HalDoSet(HAL_DO_15,HAL_DO_MODE_OFF);
//    }
    #endif
}
