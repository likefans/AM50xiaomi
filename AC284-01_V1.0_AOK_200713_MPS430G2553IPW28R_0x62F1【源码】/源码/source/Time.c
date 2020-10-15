/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 
 *  Time.C file
 *  功能描述: 
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
/* 内部函数 ---------------------------------------------------------------------*/
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
/* 内部变量 ---------------------------------------------------------------------*/
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
  函数名:  LedOFF
  功  能:   
 *********************************************************************************/
void LedOFF (void)
{
  RED_LED_OFF();
  BLUE_LED_OFF();
}
/**********************************************************************************
  函数名:  LedLearnStatus
  功  能:   
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
  函数名:  LedDeleteStatus
  功  能:   
 *********************************************************************************/
void LedDeleteStatus (void)
{
  BLUE_LED_OFF();
  RED_LED_BLINK();
}
/**********************************************************************************
  函数名:  LedOprationFinish
  功  能:   
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
  函数名:  LedDeleteFinish
  功  能:   
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
  函数名:  ClrNetStatus
  功  能:  清网状态提示
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
  函数名:  ClrNetFinish
  功  能:  清网成功提示
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
  函数名:  AddNetFinish
  功  能:  加网成功提示
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
  函数名:  BlueLedHint
  功  能:  LED提示
 *********************************************************************************/
void BlueLedHint (void)
{
  RED_LED_OFF();
  BLUE_LED_ON();
  LedStatusUpdate(LED_OFF);
}

/**********************************************************************************
  函数名:  LedOverVoltage
  功  能:  过压 红灯闪
 *********************************************************************************/
void LedOverVoltage (void)
{
  RED_LED_BLINK();
  BLUE_LED_OFF();
}
/**********************************************************************************
  函数名:  LedUnderVoltage
  功  能:  欠压 蓝灯闪
 *********************************************************************************/
void LedUnderVoltage (void)
{
  BLUE_LED_BLINK();
  RED_LED_OFF();
}
/**********************************************************************************
  函数名:  LedOverTemperature
  功  能:  过温 红蓝一起闪
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
  函数名:  SetSpeedStatus
  功  能:   
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
  函数名:  LedSpeedHint
  功  能:   
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
  函数名:   LedDisplay
  功  能:   
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
  函数名:   LedStatusUpdate
  功  能:   
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
    Flag.Time_500ms = TRUE;                                  /* 置500MS标志      */
  }
}
/**********************************************************************************
  函数名:   LearnDelCodeTime
  功  能:   
 *********************************************************************************/
void LearnDelCodeTime (void)
{
  #define LEARN_TIME                  10
  #define SET_SPEED_TIME              10
  #define SET_LIMIT_POINT_TIME        30
  
  // 学码、删码时间控制
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
  // 设置转速时间控制
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
    if(++Time_Power_On >=15)//上电10s内
    {
        Time_Power_On = 0;
        Flag.Power_on = 0;
    }
  }
  if(Time_Fail_Test != 0)
  {
      if(--Time_Fail_Test == 0)
      {
        LedStatusUpdate(LED_OFF);//退出产测
        Time_Power_On = 0;
        Flag.Power_on = 0;
      }
  }
  
  // 微调行程时间控制
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
  函数名:  Timer0_A1
  功能:    Timer0_A1中断
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
  函数名:   SystemTime
  功  能:   
 *********************************************************************************/
void SystemTime (void)
{
   if(Flag.Time_4ms == TRUE)
    {
      if (--Time_4ms_Cnt == NULL)                               /* 是否到4MS       */
      {
        Time_4ms_Cnt = TIME_4MS_VALUE;
        Uart_Byte_Cnt = NULL;				        // 清接收字节计数器
        Flag.Uart_Read_OK = TRUE;		                // 置接收完成标志
        Flag.Time_4ms = NULL;  
      }
    }
    if (--Time_10ms_Cnt == NULL)                               /* 是否到10MS       */
    {
      Time_10ms_Cnt = TIME_10MS_VALUE;
      Flag.Time_10ms = TRUE;                                   /* 置10MS标志       */
    }
    if (--Time_50ms_Cnt == NULL)                               /* 是否到50MS       */
    {
      Time_50ms_Cnt = TIME_50MS_VALUE;
      Flag.Time_50ms = TRUE;                                   /* 置50MS标志       */
    }
    if (--Time_100ms_Cnt == NULL)                              /* 是否到100MS      */
    {
      Time_100ms_Cnt = TIME_100MS_VALUE;
      Flag.Time_100ms = TRUE;                                  /* 置100MS标志      */
    }
    if (--Time_200ms_Cnt == NULL)                              /* 是否到200MS      */
    {
      Time_200ms_Cnt = TIME_200MS_VALUE;
      Flag.Time_200ms = TRUE;                                  /* 置200MS标志      */
    }
    if (--Time_500ms_Cnt == NULL)                              /* 是否到500MS      */
    {
      Time_500ms_Cnt = TIME_500MS_VALUE;
      Flag.Time_500ms = TRUE;                                  /* 置500MS标志      */
      if (--Time_1s_Cnt == NULL)                               /* 是否到1S         */
      {
        Time_1s_Cnt = TIME_1S_VALUE;
        Flag.Time_1s = TRUE;                                   /* 置1S标志         */
      }
    }
}