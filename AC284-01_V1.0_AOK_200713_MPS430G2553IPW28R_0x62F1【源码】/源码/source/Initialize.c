/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 
 *  Initialize.C file
 *  功能描述: 系统初始化
 *********************************************************************************/
#include "Define.h" /* 全局宏定义 ------*/
#include "Extern.h" /* 全局变量及函数 --*/
/**********************************************************************************
  函数名:   Delay
  功  能:    
*********************************************************************************/
void Delay(register unsigned int i)
{
    for (; i > 0; i--)
    {
        _NOP();
        WDTCTL = WDT_ARST_16;
    }
}
/*********************************************************************************
  函数名:   CLKSetting
  功  能:    
*********************************************************************************/
void CLKSetting(void)
{
    BCSCTL3 |= LFXT1S_2; // ACLK = VLO 12kHz

#if (SYSTEM_CLOCK == 1)
    BCSCTL1 = CALBC1_1MHZ;        // Set range
    DCOCTL = CALDCO_1MHZ;         // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN1; // MCLK/3 for Flash Timing Generator
#elif (SYSTEM_CLOCK == 8)
    BCSCTL1 = CALBC1_8MHZ;              // Set range
    DCOCTL = CALDCO_8MHZ;               // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN4 + FN1; // MCLK/3 for Flash Timing Generator
#elif (SYSTEM_CLOCK == 12)
    BCSCTL1 = CALBC1_12MHZ;       // Set range
    DCOCTL = CALDCO_12MHZ;        // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN5; // MCLK/3 for Flash Timing Generator
#elif (SYSTEM_CLOCK == 16)
    BCSCTL1 = CALBC1_16MHZ;             // Set range
    DCOCTL = CALDCO_16MHZ;              // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN5 + FN4; // MCLK/3 for Flash Timing Generator
#else
#error "Not define system clock!"
#endif
}
/*********************************************************************************
  函数名:   TIMXSetting
  功  能:    
*********************************************************************************/
void TIMXSetting(void)
{
    // Time0_A0 设置
    TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
    TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK; // 40 uS
    // Time0_A1 设置
    TA0CCTL1 = CCIE;                             // CCR1 interrupt enabled
    TA0CCR1 += SYSTEM_TIME_VALUE * SYSTEM_CLOCK; // 2mS

    TA0CTL = TASSEL_2 + MC_2; // SMCLK, Continous up

    // Time1 设置
    TA1CCTL0 = 0;           // CCR0 interrupt enabled
    TA1CCR0 = MAX_DUTY + 2; // CCR1 PWM duty cycle 2:100
    TA1CCTL1 = OUTMOD_6;    // CCR1 toggle/reset
    TA1CCTL2 = OUTMOD_6;    // CCR2 toggle/reset
    SetStopPWM();
    TA1CTL = TASSEL_2 + ID_0 + MC_3; // SMCLK, 不分频 up down mode
}
/*********************************************************************************
  函数名:   GPIOSetting
  功  能:    
*********************************************************************************/
void GPIOSetting(void)
{
    P1DIR = 0x45; // 0:in 1:out
    P2DIR = 0x07; // 0:in 1:out
    P3DIR = 0xFF; // 0:in 1:out

    P1SEL = 0x00; //  a3 a4
    P2SEL = 0x00; //  a3 a4
    P3SEL = 0x00; //  a3 a4

    P1IE = 0x00; //
    P2IE = 0x00; //

    P1IFG = 0;
    P2IFG = 0;
}
/*********************************************************************************
  函数名:   VariableInitialize
  功  能:    
*********************************************************************************/
void VariableInitialize(void)
{
    Hall_Pulse_Amount_Cnt = HALL_PULSE_MAX >> 1;
    Hall_Pulse_Width = 0;
    Hall_Pulse_Amount = 0;

    Hall_Status_Buffer = 0;
    Hall_Pulse_Width = 0;
    hall_change_cnt = 0;

    Motor_Array[0].Motor_Flag.Direction = FALSE;      /* 电机方向标志 ----*/
    Motor_Array[0].Motor_Flag.Run_Status = FALSE;     /* 电机运行状态 ----*/
    Motor_Array[0].Motor_Flag.Run_Direction = FALSE;  /* 电机运行方向标志 */
    Motor_Array[0].Motor_Flag.Fact_Direction = FALSE; /* 电机实际方向标志 */

    Motor_Array[0].Motor_Flag.Inching_Status = FALSE;     /* 点动状态标志 ----*/
    Motor_Array[0].Motor_Flag.StatusSwitch = FALSE;       /* 状态切换延迟标志 */
    Motor_Array[0].Motor_Flag.StatusSwitch_Delay = FALSE; /* 状态切换延迟标志 */

    Motor_Array[0].Motor_Flag.Single_Journey = FALSE;    /* 单个行程标志 ----*/
    Motor_Array[0].Motor_Flag.Journey_Set_OK = FALSE;    /* 行程设定OK标志 --*/
    Motor_Array[0].Motor_Flag.Journey_Direction = FALSE; /* 行程方向标志 ----*/

    Motor_Array[0].Motor_Flag.SaveEE = FALSE; /* 转速波动标志 ----*/

    Motor_Array[0].Motor_Flag.Hand_Drag_Status = FALSE;      /* 手拉状态标志 ----*/
    Motor_Array[0].Motor_Flag.No_Slow_Start = FALSE;         /* 无慢启动标志 ---*/
    Motor_Array[0].Motor_Flag.Serpentine_Curtain = FALSE;    /* 蛇形帘标志 -----*/
    Motor_Array[0].Motor_Flag.To_Journey_Meet_Plugb = FALSE; /* 到行程遇阻标志 -*/
#ifdef DEFAULT_UNREMEMBER
    Motor_Array[0].Motor_Flag.PowerDown_Remember = FALSE; /* 断电记忆标志 ---*/
#else
    Motor_Array[0].Motor_Flag.PowerDown_Remember = 1;
#endif
    Motor_Array[0].Motor_Flag.Manual_Control_Synchro = FALSE; /* 手控同步标志 ---*/

    Motor_Max_Speed = MAX_SPEED_100;
    Motor_Array[0].RunTime_Cnt = 0;

    Uart_ID = 0;
    Uart_CH_L = 0;
    Uart_CH_H = 0;
#ifdef GE_NING_SI_DING_ZHI // 是否格宁斯定制
    Dry_Contact_Control_Mode = REN_YI_JIAN_TING;
    P2DIR &= 0x7f;
#else
    Dry_Contact_Control_Mode = NORMAL;
#endif
    Uart_Baud_Rate = BR2400;
}
/*********************************************************************************
  函数名:   Initialize
  功  能:    
  输  入:   空
  输  出:   空    
  返  回:		空
  描  述：
*********************************************************************************/
void Initialize(void)
{
    _DINT(); // 关总中断

    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    IN1_H_0();
    IN1_L_0();
    IN2_H_0();
    IN2_L_0();

    RED_LED_ON();
    BLUE_LED_ON();
    RF_ENABLE();

    Delay(10000); // 上电延迟

    GPIOSetting(); // IO 口设置
    ADCSetting();
    CLKSetting(); // 时钟设置

    Time_Fail_Test = 0;
    Time_Power_On = 0; //上电时间计时
    Flag.Power_on = 1; //上电

#ifndef DEFAULT_UNREMEMBER
    Motor_Array[0].Motor_Flag.PowerDown_Remember = 1;
#endif

    FlashReadData();
    // 是否上电复位
    if ((IFG1 & PORIFG) || RAM_PSWD != 0xaa55)
    {
        Delay(60000); // 上电延迟
        if (Motor_Array[0].Motor_Flag.PowerDown_Remember &&
            (Motor_Array[0].Motor_Flag.Journey_Set_OK || Motor_Array[0].Motor_Flag.Single_Journey)) // 断电是否需要记忆
        {
            FlashReadPlace();
        }
        else
            Hall_Pulse_Amount_Cnt = HALL_PULSE_MAX >> 1;
        IFG1 &= ~PORIFG;
        RAM_PSWD = 0xaa55;
    }

#ifdef KEY_CARD_CONTROL
    Room_Card_Buf = (P1IN & BIT1);
#else
    LinUartInit();
#endif

    TIMXSetting(); // 定时器设置

    Motor_Status = MOTOR_STOP;
    Motor_Array[0].Motor_Status_Buffer = MOTOR_STOP;
    Motor_Array[0].Motor_Diver_Array[0] = MotorStop;
    Motor_Array[0].Motor_Diver_Array[1] = MotorUp;
    Motor_Array[0].Motor_Diver_Array[2] = MotorDown;

    Flag_Motor.Shut_OK = TRUE;
    hall_pulse_buf = Hall_Pulse_Amount_Cnt;
    JourneyCheck();
    WDTCTL = WDT_ARST_16;
    /*   //wifi 
  Flag.NetStatus = 0;
  Flag.NetEvent = TRUE;
  Flag.Uart_tx = 1;
  */
    IFG1 = 0;
    _EINT();             // 开总中断
    WiFi_Cnt = TIME_1S_WiFi;
    Flag.Wifi_cnt =TRUE;
}
