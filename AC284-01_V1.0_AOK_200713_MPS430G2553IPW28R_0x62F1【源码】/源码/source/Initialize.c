/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 
 *  Initialize.C file
 *  ��������: ϵͳ��ʼ��
 *********************************************************************************/
#include "Define.h" /* ȫ�ֺ궨�� ------*/
#include "Extern.h" /* ȫ�ֱ��������� --*/
/**********************************************************************************
  ������:   Delay
  ��  ��:    
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
  ������:   CLKSetting
  ��  ��:    
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
  ������:   TIMXSetting
  ��  ��:    
*********************************************************************************/
void TIMXSetting(void)
{
    // Time0_A0 ����
    TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
    TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK; // 40 uS
    // Time0_A1 ����
    TA0CCTL1 = CCIE;                             // CCR1 interrupt enabled
    TA0CCR1 += SYSTEM_TIME_VALUE * SYSTEM_CLOCK; // 2mS

    TA0CTL = TASSEL_2 + MC_2; // SMCLK, Continous up

    // Time1 ����
    TA1CCTL0 = 0;           // CCR0 interrupt enabled
    TA1CCR0 = MAX_DUTY + 2; // CCR1 PWM duty cycle 2:100
    TA1CCTL1 = OUTMOD_6;    // CCR1 toggle/reset
    TA1CCTL2 = OUTMOD_6;    // CCR2 toggle/reset
    SetStopPWM();
    TA1CTL = TASSEL_2 + ID_0 + MC_3; // SMCLK, ����Ƶ up down mode
}
/*********************************************************************************
  ������:   GPIOSetting
  ��  ��:    
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
  ������:   VariableInitialize
  ��  ��:    
*********************************************************************************/
void VariableInitialize(void)
{
    Hall_Pulse_Amount_Cnt = HALL_PULSE_MAX >> 1;
    Hall_Pulse_Width = 0;
    Hall_Pulse_Amount = 0;

    Hall_Status_Buffer = 0;
    Hall_Pulse_Width = 0;
    hall_change_cnt = 0;

    Motor_Array[0].Motor_Flag.Direction = FALSE;      /* ��������־ ----*/
    Motor_Array[0].Motor_Flag.Run_Status = FALSE;     /* �������״̬ ----*/
    Motor_Array[0].Motor_Flag.Run_Direction = FALSE;  /* ������з����־ */
    Motor_Array[0].Motor_Flag.Fact_Direction = FALSE; /* ���ʵ�ʷ����־ */

    Motor_Array[0].Motor_Flag.Inching_Status = FALSE;     /* �㶯״̬��־ ----*/
    Motor_Array[0].Motor_Flag.StatusSwitch = FALSE;       /* ״̬�л��ӳٱ�־ */
    Motor_Array[0].Motor_Flag.StatusSwitch_Delay = FALSE; /* ״̬�л��ӳٱ�־ */

    Motor_Array[0].Motor_Flag.Single_Journey = FALSE;    /* �����г̱�־ ----*/
    Motor_Array[0].Motor_Flag.Journey_Set_OK = FALSE;    /* �г��趨OK��־ --*/
    Motor_Array[0].Motor_Flag.Journey_Direction = FALSE; /* �г̷����־ ----*/

    Motor_Array[0].Motor_Flag.SaveEE = FALSE; /* ת�ٲ�����־ ----*/

    Motor_Array[0].Motor_Flag.Hand_Drag_Status = FALSE;      /* ����״̬��־ ----*/
    Motor_Array[0].Motor_Flag.No_Slow_Start = FALSE;         /* ����������־ ---*/
    Motor_Array[0].Motor_Flag.Serpentine_Curtain = FALSE;    /* ��������־ -----*/
    Motor_Array[0].Motor_Flag.To_Journey_Meet_Plugb = FALSE; /* ���г������־ -*/
#ifdef DEFAULT_UNREMEMBER
    Motor_Array[0].Motor_Flag.PowerDown_Remember = FALSE; /* �ϵ�����־ ---*/
#else
    Motor_Array[0].Motor_Flag.PowerDown_Remember = 1;
#endif
    Motor_Array[0].Motor_Flag.Manual_Control_Synchro = FALSE; /* �ֿ�ͬ����־ ---*/

    Motor_Max_Speed = MAX_SPEED_100;
    Motor_Array[0].RunTime_Cnt = 0;

    Uart_ID = 0;
    Uart_CH_L = 0;
    Uart_CH_H = 0;
#ifdef GE_NING_SI_DING_ZHI // �Ƿ����˹����
    Dry_Contact_Control_Mode = REN_YI_JIAN_TING;
    P2DIR &= 0x7f;
#else
    Dry_Contact_Control_Mode = NORMAL;
#endif
    Uart_Baud_Rate = BR2400;
}
/*********************************************************************************
  ������:   Initialize
  ��  ��:    
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:		��
  ��  ����
*********************************************************************************/
void Initialize(void)
{
    _DINT(); // �����ж�

    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    IN1_H_0();
    IN1_L_0();
    IN2_H_0();
    IN2_L_0();

    RED_LED_ON();
    BLUE_LED_ON();
    RF_ENABLE();

    Delay(10000); // �ϵ��ӳ�

    GPIOSetting(); // IO ������
    ADCSetting();
    CLKSetting(); // ʱ������

    Time_Fail_Test = 0;
    Time_Power_On = 0; //�ϵ�ʱ���ʱ
    Flag.Power_on = 1; //�ϵ�

#ifndef DEFAULT_UNREMEMBER
    Motor_Array[0].Motor_Flag.PowerDown_Remember = 1;
#endif

    FlashReadData();
    // �Ƿ��ϵ縴λ
    if ((IFG1 & PORIFG) || RAM_PSWD != 0xaa55)
    {
        Delay(60000); // �ϵ��ӳ�
        if (Motor_Array[0].Motor_Flag.PowerDown_Remember &&
            (Motor_Array[0].Motor_Flag.Journey_Set_OK || Motor_Array[0].Motor_Flag.Single_Journey)) // �ϵ��Ƿ���Ҫ����
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

    TIMXSetting(); // ��ʱ������

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
    _EINT();             // �����ж�
    WiFi_Cnt = TIME_1S_WiFi;
    Flag.Wifi_cnt =TRUE;
}
