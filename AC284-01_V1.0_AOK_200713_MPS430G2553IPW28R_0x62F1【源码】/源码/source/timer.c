/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *
 *  .c file
 *  功能描述: 
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define TIME0_INT_NUMBER       3                             // Time0中断次数
#define MS_2_TIME              (2000 / (RF_DECODE_TIME * TIME0_INT_NUMBER))
#define TIME0_INT_NUMBER       3                             // Time0中断次数
#define HALL1                  0
#define HALL2                  1
#define HALL_SR1               BIT3
#define HALL_SR2               BIT4
#define HALL1_IN()            (P2IN & HALL_SR1)              /* P2.3 ------------*/
#define HALL2_IN()            (P2IN & HALL_SR2)              /* P2.4 ------------*/
#define RF_SIGNAL_SAMPLE()    (P2IN & BIT5)                  /* P2.5 ------------*/
/* 内部变量 ---------------------------------------------------------------------*/
u8 Time_200uS_Cnt = TIME_200US_VALUE;

u8 Time2_Int_Cnt = TIME0_INT_NUMBER - 2;
u8 Ms_2_Cnt = MS_2_TIME - 1;

u8 RF_Signal_Sample1;                                 /* 数据电平采样 ----*/
u8 RF_Signal_Sample2;                                 /* 数据电平采样 ----*/
u8 RF_High_Cnt_Buffer;                                /* 数据时间缓存 ----*/
u8 RF_Low_Cnt_Buffer;                                 /* 数据时间缓存 ----*/
/*********************************************************************************
  函数名:    DetectHallSignal
  功  能:   检测霍尔信号
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
  
  // 霍尔1为基准，是否有边沿触发
  if (Hall_Status_Buffer != hall_value)
  {
    // 连续3次防止误动作
    if (++hall_change_cnt >= 3)
    {
      hall_change_cnt = 0;
      
      // 确定运行方向
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
      
      // 读取脉冲宽度
      Hall_Pulse_Width = Hall_Time_Cnt;
      Hall_Time_Cnt = 0;
      Flag_Motor.Hall_Pulse_edge = TRUE;  
      
      Motor_Array[0].Motor_Flag.SaveEE = TRUE;
      if (Motor_Array[0].Motor_Flag.Fact_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
      {
        // 是否在最大范围内
        if (Hall_Pulse_Amount_Cnt < HALL_PULSE_MAX)
        {
          ++Hall_Pulse_Amount_Cnt;
        }
      }
      else
      {
        // 是否在最大范围内
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
  函数名:  Timer0_A0
  功能:    Timer0_A0中断
**********************************************************************************/
// Timer_A3 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
  TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;    // 40 uS 掉电后480us
  
  if (RF_SIGNAL_SAMPLE())                                    /* 第一次采样 ------*/
  {
    RF_Signal_Sample1++;
  }
  DetectHallSignal();
  /*-----------------------------------------------------------------------------*/
  if (RF_SIGNAL_SAMPLE())                                    /* 第二次采样 ------*/
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
      if (RF_SIGNAL_SAMPLE())                                    /* 第三次采样 ------*/
      {
        RF_Signal_Sample1++;
      }
      /*-----------------------------------------------------------------------------*/
      if (RF_Signal_Sample1 >> 1)                                /* 是否两次为高 ----*/
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
            Flag_RF.Raise_Edge = TRUE;                           // 置RF上升沿标志  
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
} //中断结束并返回
