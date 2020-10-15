/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *
 *  .c file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define TIME0_INT_NUMBER       3                             // Time0�жϴ���
#define MS_2_TIME              (2000 / (RF_DECODE_TIME * TIME0_INT_NUMBER))
#define TIME0_INT_NUMBER       3                             // Time0�жϴ���
#define HALL1                  0
#define HALL2                  1
#define HALL_SR1               BIT3
#define HALL_SR2               BIT4
#define HALL1_IN()            (P2IN & HALL_SR1)              /* P2.3 ------------*/
#define HALL2_IN()            (P2IN & HALL_SR2)              /* P2.4 ------------*/
#define RF_SIGNAL_SAMPLE()    (P2IN & BIT5)                  /* P2.5 ------------*/
/* �ڲ����� ---------------------------------------------------------------------*/
u8 Time_200uS_Cnt = TIME_200US_VALUE;

u8 Time2_Int_Cnt = TIME0_INT_NUMBER - 2;
u8 Ms_2_Cnt = MS_2_TIME - 1;

u8 RF_Signal_Sample1;                                 /* ���ݵ�ƽ���� ----*/
u8 RF_Signal_Sample2;                                 /* ���ݵ�ƽ���� ----*/
u8 RF_High_Cnt_Buffer;                                /* ����ʱ�仺�� ----*/
u8 RF_Low_Cnt_Buffer;                                 /* ����ʱ�仺�� ----*/
/*********************************************************************************
  ������:    DetectHallSignal
  ��  ��:   �������ź�
*********************************************************************************/
void DetectHallSignal (void)
{
  u8 hall_value;
  
  hall_value = P2IN & HALL_SR1;
  
  if (Hall_Time_Cnt < 15000) 
  {
    Hall_Time_Cnt++;
  }
  else
  {
    Motor_Speed = 0;
  }
  
  // ����1Ϊ��׼���Ƿ��б��ش���
  if (Hall_Status_Buffer != hall_value)
  {
    // ����3�η�ֹ����
    if (++hall_change_cnt >= 3)
    {
      hall_change_cnt = 0;
      
      // ȷ�����з���
      Motor_Array[0].Motor_Flag.Fact_Direction = 0;
      if (hall_value != 0)   // 0 -> 1
      {
        if (!(P2IN & HALL_SR2))
        {
          Motor_Array[0].Motor_Flag.Fact_Direction = TRUE;
        }
      }
      else   // 1 -> 0
      {
        if (P2IN & HALL_SR2)
        {
          Motor_Array[0].Motor_Flag.Fact_Direction = TRUE;
        }
      }
      
      // ��ȡ������
      Hall_Pulse_Width = Hall_Time_Cnt;
      Hall_Time_Cnt = 0;
      Flag_Motor.Hall_Pulse_edge = TRUE;  
      
      Motor_Array[0].Motor_Flag.SaveEE = TRUE;
      if (Motor_Array[0].Motor_Flag.Fact_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
      {
        // �Ƿ������Χ��
        if (Hall_Pulse_Amount_Cnt < HALL_PULSE_MAX)
        {
          ++Hall_Pulse_Amount_Cnt;
        }
      }
      else
      {
        // �Ƿ������Χ��
        if (Hall_Pulse_Amount_Cnt > HALL_PULSE_MIN)
        {
          Hall_Pulse_Amount_Cnt--;
        }
      }
      Hall_Status_Buffer = hall_value;
    }
  }
}
/**********************************************************************************
  ������:  Timer0_A0
  ����:    Timer0_A0�ж�
**********************************************************************************/
// Timer_A3 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
  TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;    // 40 uS �����480us
  
  if (RF_SIGNAL_SAMPLE())                                    /* ��һ�β��� ------*/
  {
    RF_Signal_Sample1++;
  }
  DetectHallSignal();
  /*-----------------------------------------------------------------------------*/
  if (RF_SIGNAL_SAMPLE())                                    /* �ڶ��β��� ------*/
  {
    RF_Signal_Sample1++;
  }
  
  if (--Time_200uS_Cnt == 0)
  {
    Flag.Time_200us = TRUE;
    Time_200uS_Cnt = TIME_200US_VALUE;
  }
  
  if (!Flag.Low_VOL_Protect)
  {
      /*-----------------------------------------------------------------------------*/
      if (RF_SIGNAL_SAMPLE())                                    /* �����β��� ------*/
      {
        RF_Signal_Sample1++;
      }
      /*-----------------------------------------------------------------------------*/
      if (RF_Signal_Sample1 >> 1)                                /* �Ƿ�����Ϊ�� ----*/
      {
        RF_Signal_Sample2++;
      }
      RF_Signal_Sample1 = NULL;
      /*-----------------------------------------------------------------------------*/
      if (--Time2_Int_Cnt == NULL)
      {
        Time2_Int_Cnt = TIME0_INT_NUMBER;
    
        if (RF_Signal_Sample2 >> 1)
        {
          RF_High_Cnt_Buffer++;
      
          if (!Flag_RF.Signal_Status)
          {
            Flag_RF.Raise_Edge = TRUE;                           // ��RF�����ر�־  
            RF_High_Cnt = RF_High_Cnt_Buffer;
            RF_Low_Cnt = RF_Low_Cnt_Buffer;
            RF_High_Cnt_Buffer = NULL;
            RF_Low_Cnt_Buffer = NULL;
          }
          Flag_RF.Signal_Status = TRUE;
        }
        else
        {
          Flag_RF.Signal_Status = NULL;
          RF_Low_Cnt_Buffer++;
        }
        RF_Signal_Sample2 = NULL;
      }
  }
} //�жϽ���������
