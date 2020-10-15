/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 
 *  Time.C file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
/* �ڲ����� ---------------------------------------------------------------------*/
void LedOFF             (void);
void LedSetSpeedStatus  (void);
void LedSpeedHint       (void);
void LedLearnStatus     (void);
void LedDeleteStatus    (void);
void LedOprationFinish  (void);
void LedDeleteFinish    (void);
void LedOverVoltage     (void);
void LedUnderVoltage    (void);
void LedOverTemperature (void);

void ClrNetStatus       (void);
void ClrNetFinish       (void);
void AddNetFinish       (void);
void BlueLedHint        (void);
/* �ڲ����� ---------------------------------------------------------------------*/
LedHint const Led_Hint_Array [LED_HINT_NUM] =
{
  LedOFF,
  LedSetSpeedStatus,
  LedSpeedHint,
  
  LedLearnStatus,
  LedDeleteStatus,
  LedOprationFinish,
  LedDeleteFinish,
 
  ClrNetStatus,
  ClrNetFinish,
  AddNetFinish,
  BlueLedHint,

  LedOverTemperature,
  LedOverVoltage,
  LedUnderVoltage,
};
/**********************************************************************************
  ������:  LedOFF
  ��  ��:   
 *********************************************************************************/
void LedOFF (void)
{
  RED_LED_OFF();
  BLUE_LED_OFF();
}
/**********************************************************************************
  ������:  LedLearnStatus
  ��  ��:   
 *********************************************************************************/
void LedLearnStatus (void)
{
  BLUE_LED_OFF();
  if (Flag.Time_500ms)
  {
    Flag.Time_500ms = NULL;
    RED_LED_BLINK();
  }
}
/**********************************************************************************
  ������:  LedDeleteStatus
  ��  ��:   
 *********************************************************************************/
void LedDeleteStatus (void)
{
  BLUE_LED_OFF();
  RED_LED_BLINK();
}
/**********************************************************************************
  ������:  LedOprationFinish
  ��  ��:   
 *********************************************************************************/
void LedOprationFinish (void)
{
  #define OPRETION_FINISH_HINT_TIME  6
  
  RED_LED_OFF();
  BLUE_LED_BLINK();
  if (++Opretion_Time_Cnt >= OPRETION_FINISH_HINT_TIME)
  {
    LedStatusUpdate(LED_OFF);
  }
}
/**********************************************************************************
  ������:  LedDeleteFinish
  ��  ��:   
 *********************************************************************************/
void LedDeleteFinish (void)
{
  #define DEL_FINISH_HINT_TIME  10
  
  RED_LED_OFF();
  BLUE_LED_ON();
  if (++Opretion_Time_Cnt >= DEL_FINISH_HINT_TIME)
  {
    LedStatusUpdate(LED_OFF);
  }
}

/**********************************************************************************
  ������:  ClrNetStatus
  ��  ��:  ����״̬��ʾ
 *********************************************************************************/
void ClrNetStatus (void)
{
  #define CLR_NET_STATUS_HINT_TIME  6
  
  RED_LED_BLINK();
  BLUE_LED_OFF();
  if (++Opretion_Time_Cnt > CLR_NET_STATUS_HINT_TIME)
  {
    LedStatusUpdate(LED_OFF);
  }
}
/**********************************************************************************
  ������:  ClrNetFinish
  ��  ��:  �����ɹ���ʾ
 *********************************************************************************/
void ClrNetFinish (void)
{
#if 0
  #define CLR_NET_FINISH_HINT_TIME  10
  
  if (Flag.Time_500ms)
  {
    Flag.Time_500ms = NULL;
    
    if (++Opretion_Time_Cnt > CLR_NET_FINISH_HINT_TIME)
    {
      LedStatusUpdate(LED_OFF);
    }
    if (Opretion_Time_Cnt & 0x01)
    {
      RED_LED_OFF();
      BLUE_LED_ON();
    }
    else
    {
      BLUE_LED_OFF();
      RED_LED_ON();
    }
  }
#else
   if (Flag.Time_500ms)
  {
    Flag.Time_500ms = NULL;
  ++Opretion_Time_Cnt;
  if (Opretion_Time_Cnt & 0x01)
  {
    RED_LED_OFF();
    BLUE_LED_ON();
  }
  else
  {
    BLUE_LED_OFF();
    RED_LED_ON();
  }
  }
#endif
}

/**********************************************************************************
  ������:  AddNetFinish
  ��  ��:  �����ɹ���ʾ
 *********************************************************************************/
void AddNetFinish (void)
{
  #define ADD_NET_FINISH_HINT_TIME  40
  
  RED_LED_OFF();
  if (++Opretion_Time_Cnt > ADD_NET_FINISH_HINT_TIME)
  {
    LedStatusUpdate(LED_OFF);
  }
  if (Opretion_Time_Cnt & 0x01)
  {
    BLUE_LED_BLINK();
  }
}
/**********************************************************************************
  ������:  BlueLedHint
  ��  ��:  LED��ʾ
 *********************************************************************************/
void BlueLedHint (void)
{
  RED_LED_OFF();
  BLUE_LED_ON();
  LedStatusUpdate(LED_OFF);
}

/**********************************************************************************
  ������:  LedOverVoltage
  ��  ��:  ��ѹ �����
 *********************************************************************************/
void LedOverVoltage (void)
{
  RED_LED_BLINK();
  BLUE_LED_OFF();
}
/**********************************************************************************
  ������:  LedUnderVoltage
  ��  ��:  Ƿѹ ������
 *********************************************************************************/
void LedUnderVoltage (void)
{
  BLUE_LED_BLINK();
  RED_LED_OFF();
}
/**********************************************************************************
  ������:  LedOverTemperature
  ��  ��:  ���� ����һ����
 *********************************************************************************/
void LedOverTemperature (void)
{
  if (Flag.Time_500ms)
  {
    Flag.Time_500ms = NULL;
    if (++Opretion_Time_Cnt & BIT0)
    {
      RED_LED_ON();
      BLUE_LED_OFF();
    }
    else
    {
      RED_LED_OFF();
      BLUE_LED_ON();
    }
  }
}
/**********************************************************************************
  ������:  SetSpeedStatus
  ��  ��:   
 *********************************************************************************/
void LedSetSpeedStatus (void)
{
  #define OPRETION_FINISH_HINT_TIME  6
  
  if (Opretion_Time_Cnt < OPRETION_FINISH_HINT_TIME)
  {
    Opretion_Time_Cnt++;
    RED_LED_OFF();
    BLUE_LED_BLINK();
  }
  else
  {
    LedSpeedHint();
  }
}
/**********************************************************************************
  ������:  LedSpeedHint
  ��  ��:   
 *********************************************************************************/
void LedSpeedHint (void)
{
  if (Motor_Max_Speed <= MAX_SPEED_80)
  {
    RED_LED_ON();
    BLUE_LED_OFF();
  }
  else if (Motor_Max_Speed <= MAX_SPEED_100)
  {
    BLUE_LED_ON();
    RED_LED_ON();
  }
  else if (Motor_Max_Speed <= MAX_SPEED_130)
  {
    BLUE_LED_ON();
    RED_LED_OFF();
  }
  Opretion_Time_Cnt++;
}

/**********************************************************************************
  ������:   LedDisplay
  ��  ��:   
 *********************************************************************************/
void LedDisplay (void)
{
  u8 i;
  
  for (i = 0; i < LED_HINT_NUM; i++)
  {
    if (i == LED_Status)
    {
      (*Led_Hint_Array[i])();
      break;
    }
  }
}
/**********************************************************************************
  ������:   LedStatusUpdate
  ��  ��:   
 *********************************************************************************/
void LedStatusUpdate (Led_Display_TypeDef status)
{
  if (LED_Status != status)
  {
    LED_Status = status;
    Opretion_Time_Cnt = NULL;
    Flag.Time_100ms = FALSE;
    Time_100ms_Cnt = TIME_100MS_VALUE;
    Time_500ms_Cnt = TIME_500MS_VALUE;
    Flag.Time_500ms = TRUE;                                  /* ��500MS��־      */
  }
}
/**********************************************************************************
  ������:   LearnDelCodeTime
  ��  ��:   
 *********************************************************************************/
void LearnDelCodeTime (void)
{
  #define LEARN_TIME                  10
  #define SET_SPEED_TIME              10
  #define SET_LIMIT_POINT_TIME        30
  
  // ѧ�롢ɾ��ʱ�����
  if ((LED_Status == LEARN_STATUS) || (LED_Status == DELETE_STATUS))
  {
    if (++Learn_Time_Cnt >= LEARN_TIME)
    {
      LedStatusUpdate(LED_OFF);
      Learn_Time_Cnt = NULL;
    }
  }
  else
  {
    Learn_Time_Cnt = NULL;
  }
  // ����ת��ʱ�����
  if (LED_Status == SET_SPEED_STATUS)
  {
    if (++Set_Speed_Time_Cnt >= SET_SPEED_TIME)
    {
      LedStatusUpdate(LED_OFF);
      Set_Speed_Time_Cnt = NULL;
    }
  }
  else
  {
    Set_Speed_Time_Cnt = NULL;
  }
  
  if(learn_key_cnt != 0)
  {
    if(++No_learn_key_cnt>=2)
    {
      learn_key_cnt = 0;
    }
  }
  if(Flag.Power_on)
  {
    if(++Time_Power_On >=15)//�ϵ�10s��
    {
        Time_Power_On = 0;
        Flag.Power_on = 0;
    }
  }
  if(Time_Fail_Test != 0)
  {
      if(--Time_Fail_Test == 0)
      {
        LedStatusUpdate(LED_OFF);//�˳�����
        Time_Power_On = 0;
        Flag.Power_on = 0;
      }
  }
  
  // ΢���г�ʱ�����
  if ((Flag_Motor.Adj_Limit_Point) && (Motor_Status == MOTOR_STOP))
  {
    if (++Set_Limit_Point_Cnt >= SET_LIMIT_POINT_TIME)
    {
      Set_Limit_Point_Cnt = NULL;
      Flag_Motor.Adj_Limit_Point = NULL;
      LedStatusUpdate(OPRATION_FINISH);
    }
  }
  else
  {
    Set_Limit_Point_Cnt = NULL;
  }
}
/**********************************************************************************
  ������:  Timer0_A1
  ����:    Timer0_A1�ж�
**********************************************************************************/
// Timer_A3 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1 (void)
{
  if (TA0IV == 2)
  {
   // if (!Flag.Low_VOL_Protect)
   // {
      TA0CCR1 += SYSTEM_TIME_VALUE * SYSTEM_CLOCK;     // 2mS
   // }
    //else
   // {
   //   TA0CCR1 += SYSTEM_TIME_VALUE;     // 2mS
   // }  
    Flag.Time_2ms = TRUE;
  }
}
/**********************************************************************************
  ������:   SystemTime
  ��  ��:   
 *********************************************************************************/
void SystemTime (void)
{
   if(Flag.Time_4ms == TRUE)
    {
      if (--Time_4ms_Cnt == NULL)                               /* �Ƿ�4MS       */
      {
        Time_4ms_Cnt = TIME_4MS_VALUE;
        Uart_Byte_Cnt = NULL;				        // ������ֽڼ�����
        Flag.Uart_Read_OK = TRUE;		                // �ý�����ɱ�־
        Flag.Time_4ms = NULL;  
      }
    }
    if (--Time_10ms_Cnt == NULL)                               /* �Ƿ�10MS       */
    {
      Time_10ms_Cnt = TIME_10MS_VALUE;
      Flag.Time_10ms = TRUE;                                   /* ��10MS��־       */
    }
    if (--Time_50ms_Cnt == NULL)                               /* �Ƿ�50MS       */
    {
      Time_50ms_Cnt = TIME_50MS_VALUE;
      Flag.Time_50ms = TRUE;                                   /* ��50MS��־       */
    }
    if (--Time_100ms_Cnt == NULL)                              /* �Ƿ�100MS      */
    {
      Time_100ms_Cnt = TIME_100MS_VALUE;
      Flag.Time_100ms = TRUE;                                  /* ��100MS��־      */
    }
    if (--Time_200ms_Cnt == NULL)                              /* �Ƿ�200MS      */
    {
      Time_200ms_Cnt = TIME_200MS_VALUE;
      Flag.Time_200ms = TRUE;                                  /* ��200MS��־      */
    }
    if (--Time_500ms_Cnt == NULL)                              /* �Ƿ�500MS      */
    {
      Time_500ms_Cnt = TIME_500MS_VALUE;
      Flag.Time_500ms = TRUE;                                  /* ��500MS��־      */
      if (--Time_1s_Cnt == NULL)                               /* �Ƿ�1S         */
      {
        Time_1s_Cnt = TIME_1S_VALUE;
        Flag.Time_1s = TRUE;                                   /* ��1S��־         */
      }
    }
}