/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *  
 *  MotorControl.c file
 *  功能描述: 
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define NO_SPEED_TIME              4000 / MOTOR_CONTROL_TIME     /* 无转速保护时间 --*/
#define MEET_PLUGB_NO_HAND_DRAG    800 / TIME_50MS_VALUE
#define JOURNEY_NO_HAND_DRAG       500  / TIME_50MS_VALUE

#define OVER_CURRENT_DETECT()      ((P3IN & BIT6)?0:1)

#define PULSE_SAMPLE_TIME          (RF_DECODE_TIME / 10)

//#define SPEED_COUNT(var) (46875/var)//(1000000us/(var*2*40us*16(光栅个数)))*60s

#ifdef HALL_DETECT
  #define SPEED_COUNT(var)                               \
    ((37500 + ((((var)*(PULSE_SAMPLE_TIME)) / 10) / 2))  \
    /(((var)*(PULSE_SAMPLE_TIME)) / 10))
#else  // 18750
  #define SPEED_COUNT(var)                               \
    ((18750 + ((((var)*(PULSE_SAMPLE_TIME)) / 10) / 2))  \
    /(((var)*(PULSE_SAMPLE_TIME)) / 10))
#endif


/* 内部函数 ---------------------------------------------------------------------*/
void MotorUp (void);
void MotorDown (void);
void MotorStop (void);
/* 内部变量 ---------------------------------------------------------------------*/
u8 no_hand_drag_cnt;
u8 Meet_Plugb_Event;
//static u16 Meet_plug_place_Buffer;
static u16 Meet_plug_place;
static u16 Target_Journey_Point;
/**********************************************************************************
  函数名:   MotorControl
  功  能:   
 *********************************************************************************/
void Duty_Add (u16 *p,u8 i)
{ if(Current_Current_mA > MAX_OUT_CURRENT) return;
  if(*p < MAX_DUTY)
  {
    *p += i;
  }
}
/**********************************************************************************
  函数名:   MotorControl
  功  能:   
 *********************************************************************************/
void Duty_Sub (u16 *p,u8 i)
{
  if (*p > i)
  {
    *p -= i;
  }
}
/**********************************************************************************
  函数名:   
  功  能:   
 *********************************************************************************/
void SetStopPWM (void)
{
  P2SEL = 0;
  P3SEL = 0;
  
  TA1CCR1 = 0;
  TA1CCR2 = 0;
  Motor_Duty = 0;
  
  IN1_H_0();
  IN1_L_0();
  IN2_H_0();
  IN2_L_0();
}
/**********************************************************************************
  函数名:   
  功  能:   
 *********************************************************************************/
void SetUpPWM (u16 duty)
{
  IN1_L_1();      // 关两个下管
  IN2_L_1();
  IN2_H_0();
  
  P3SEL = BIT3;   // P3.3
  P2SEL = 0;
  IN1_H_1();      // 开上桥臂
  TA1CCR2 = duty; //下桥臂PWM输出
}
/**********************************************************************************
  函数名:   
  功  能:   
 *********************************************************************************/
void SetDownPWM (u16 duty)
{
  IN1_L_1();      // 关两个下管
  IN2_L_1();
  IN1_H_0();
  
  P2SEL = BIT2;   // P2.2
  P3SEL = 0;
  IN2_H_1();      // 开上桥臂
  TA1CCR1 = duty; //下桥臂PWM输出
}
/**********************************************************************************
  函数名:   
  功  能:   
 *********************************************************************************/
void MotorStop (void)
{
  Motor_Array[0].Motor_Flag.Run_Status = FALSE;
  
  if (LED_Status == SPEED_HINT)
  {
    if (Opretion_Time_Cnt >= 3)
    {
      LedStatusUpdate(LED_OFF);
    }
  }
}
/**********************************************************************************
  函数名:   
  功  能:   
 *********************************************************************************/
void MotorUp (void)
{
  if (LED_Status == LED_OFF)
  {
    LedStatusUpdate(SPEED_HINT);
  }
  Motor_Array[0].Motor_Flag.Run_Status = TRUE;
  Motor_Array[0].Motor_Flag.Run_Direction = !Motor_Array[0].Motor_Flag.Direction;
}
/**********************************************************************************
  函数名:   
  功  能:   
 *********************************************************************************/
void MotorDown (void)
{
  if (LED_Status == LED_OFF)
  {
    LedStatusUpdate(SPEED_HINT);
  }
  Motor_Array[0].Motor_Flag.Run_Status = TRUE;
  Motor_Array[0].Motor_Flag.Run_Direction = Motor_Array[0].Motor_Flag.Direction;
}
/**********************************************************************************
  函数名:   JourneyDelete
  功  能:   
 *********************************************************************************/
void JourneyDelete (void)
{
  Hall_Pulse_Amount_Cnt = HALL_PULSE_MAX >> 1;   // 重新赋初始位置
  hall_pulse_buf = Hall_Pulse_Amount_Cnt;
  Hall_Pulse_Amount = 0;                         // 清除行程
  //Meet_plug_place_Buffer = 0;                    // 清除遇阻位置
    //Motor_Array[0].Motor_Flag.Hand_Drag_Status = 0;
  //Motor_Array[0].Motor_Flag.Direction = 0;
  Motor_Array[0].Motor_Flag.Journey_Direction = 0;
  Motor_Array[0].Motor_Flag.Single_Journey = NULL;              // 清行程标志
  Motor_Array[0].Motor_Flag.Journey_Set_OK = NULL;
  Flag_Motor.slowstop = NULL;                    // 取消减速，运行加速
}
/**********************************************************************************
  函数名:   JourneyCheck
  功  能:   
 *********************************************************************************/
void JourneyCheck (void)
{
  if (Motor_Array[0].Motor_Flag.Journey_Set_OK)
  {
    if (Hall_Pulse_Amount < (MIN_JOURNEY_VALUE + 50))
    {
      JourneyDelete();
    }
  }
}
/**********************************************************************************
  函数名:   JourneySeting
  功  能:   
 *********************************************************************************/
void JourneySeting (void)
{
  u8 Flag_Rebound = NULL;
  
    /*if (Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Direction) // 是否为上行(开) (不相等为上)
    {
      if ((Hall_Pulse_Amount_Cnt < (Meet_plug_place - 6))
      || (Hall_Pulse_Amount_Cnt > (Meet_plug_place + 6)))
      {
        Flag_Rebound = TRUE;
      }
    }*/
    
    if (!Motor_Array[0].Motor_Flag.Single_Journey)                                //是否未设定单边行程
    {
      Motor_Array[0].Motor_Flag.Single_Journey = TRUE;                            //设定单边行程设定OK
      if (Flag_Rebound)
      {
        Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE - 25;      //设定最小行程
      }
      else
      {
        Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;           // 设定最小行程
      }
      Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;    // 保存行程方向
    }
    else
    {
      if (Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
      {
        if (Flag_Rebound)
        {
          Hall_Pulse_Amount = Hall_Pulse_Amount_Cnt - 25;    //如果有反弹则再后退20
        }
        else
        {
          Hall_Pulse_Amount = Hall_Pulse_Amount_Cnt;         // 保存行程
        }
        if (Hall_Pulse_Amount < (MIN_JOURNEY_VALUE + 50))
        { if (Flag_Rebound)
          {
            Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE - 25;      //设定最小行程
          }
          else
          {
            Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;           // 设定最小行程
          }
          Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;    // 保存行程方向
        }
        else
        { Motor_Array[0].Motor_Flag.Journey_Set_OK = TRUE;                          // 行程设定OK
          Motor_Array[0].Motor_Flag.Single_Journey = NULL;                          //设定单边行程设定
          Flag_Motor.In_Journey_Place = TRUE;
        }
      }
      else
      {
        Motor_Array[0].Motor_Flag.Single_Journey = TRUE;                    //设定单边行程设定OK
        if (Flag_Rebound)
        {
          Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE - 25;    // 设定最小行程
        }
        else
        {
          Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;         // 设定最小行程
        }
        Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;  // 保存行程方向
      }
    }
    LedStatusUpdate(OPRATION_FINISH);
    //JourneyCheck();
    FlashWriteData(0xee);
}
/**********************************************************************************
  函数名:   MeetPlugb
  功  能:   
 *********************************************************************************/
void MeetPlugb (void)
{
  if (Flag_Motor.Meet_Plugb)
  {
    if (Flag_Motor.Adj_Limit_Point)      // 微调限位状态不处理遇阻
    {
      Flag_Motor.Wait_Set_Journey = NULL;
      return;
    }
    if ((Motor_Speed < MIN_SPEED) || (Motor_Array[0].Motor_Flag.Serpentine_Curtain)
    || (!(Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Direction)))
    {
      Flag_Motor.Meet_Plugb = NULL;         // 清置遇阻标志
      if (!Motor_Array[0].Motor_Flag.Journey_Set_OK)       // 无行程遇阻
      {
        JourneySeting();
      }
      else   // 有行程遇阻
      {
        if (!Flag_Motor.In_Journey_Place)  // 没有到过行程位置遇阻
        { if(!flag_yuzu_once)
          { flag_yuzu_once=1;
            Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;           // 设定最小行程
            Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;    // 保存行程方向
            LedStatusUpdate(OPRATION_FINISH);
            FlashWriteData(0xee);
          }
          else
          { 
            if (Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
            { 
              if (Hall_Pulse_Amount_Cnt < (MIN_JOURNEY_VALUE + 50))
              { Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;           // 设定最小行程
                Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;    // 保存行程方向
              }
              else
              { Hall_Pulse_Amount = Hall_Pulse_Amount_Cnt;
                Flag_Motor.In_Journey_Place = TRUE;
              }
              LedStatusUpdate(OPRATION_FINISH);
              FlashWriteData(0xee);
            }
            else
            { if(Hall_Pulse_Amount_Cnt < MIN_JOURNEY_VALUE)
              { Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;
                LedStatusUpdate(OPRATION_FINISH);
                //FlashWriteData(0xee);
              }
            }
          }
        }
        else
        { if(Hall_Pulse_Amount_Cnt<MIN_JOURNEY_VALUE-1)
          { Hall_Pulse_Amount += (MIN_JOURNEY_VALUE - Hall_Pulse_Amount_Cnt);
            Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;
            over_min_cnt = MIN_JOURNEY_VALUE;
            Flag_Motor.over_min_save=0;
            FlashWriteData(0xee);
            LedStatusUpdate(OPRATION_FINISH);
          }
          else if(Hall_Pulse_Amount_Cnt>Hall_Pulse_Amount+1)
          { Hall_Pulse_Amount = Hall_Pulse_Amount_Cnt;
            over_max_cnt = Hall_Pulse_Amount;
            Flag_Motor.over_max_save=0;
            FlashWriteData(0xee);
            LedStatusUpdate(OPRATION_FINISH);
          }
          else if(Meet_plug_place<MIN_JOURNEY_VALUE-1)
          { Flag_Motor.over_min_save=0;
            over_min_cnt = MIN_JOURNEY_VALUE;
          }
          else if(Meet_plug_place>Hall_Pulse_Amount+1)
          { Flag_Motor.over_max_save=0;
            over_max_cnt = Hall_Pulse_Amount;
          }
        }
      }
    }
    hall_pulse_buf = Hall_Pulse_Amount_Cnt;
    no_hand_drag_cnt = MEET_PLUGB_NO_HAND_DRAG;
  }
  else
  {
    Flag_Motor.Wait_Set_Journey = NULL;
  }
}
/**********************************************************************************
  函数名:   JourneyComper
  功  能:   
 *********************************************************************************/
void SlowStop (u16 target)
{
  static u16 slowstop_cnt;
  u8 temp;
  if ((Hall_Pulse_Amount_Cnt < (target + (Motor_Max_Speed >> 1)))
  && (Hall_Pulse_Amount_Cnt > (target - (Motor_Max_Speed>>1))))
  {
    Flag_Motor.slowstop = TRUE;
    if (Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
    {
      temp = target - Hall_Pulse_Amount_Cnt;
    }
    else
    {
      temp = Hall_Pulse_Amount_Cnt - target;
    }
    temp += 15;
    if (temp > Motor_Max_Speed)
    {
      temp = Motor_Max_Speed;
    }
    if (temp < MIN_SPEED)
    {
      temp = MIN_SPEED;
    }
    if (++slowstop_cnt >= 10)
    {
      slowstop_cnt = NULL;
      if (Target_Speed > temp)
      {
        Target_Speed--;
      }
      else if (Target_Speed < temp)
      {
        Target_Speed++;
      }
    }
  }
  else
  {
    Flag_Motor.slowstop = NULL;
  }
}
/**********************************************************************************
  函数名:   
  功  能:   
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：  
 *********************************************************************************/
void MotorRun (void)
{
  u8 flag_stop = FALSE;
  u8 meet_plug_stop = FALSE;
  
  static u8 reversal_cnt;
  static u16 motor_rum_time;
  static u16 Motor_SHUT_Cnt;
  static u8 Motor_Speed_Cnt;
  static u8 Motor_Speed_Buff;
  static u8 Hand_Drag_Direction_Open;
  static u8 Hand_Drag_Direction_SHUT;
  
  static u16 NoSpeed_Meet_Plug_Cnt;
  static u8 Speed_Unusual_Meet_Plug_Cnt;
  //static u8 Speed_Normal_Meet_Plug_Cnt;
  static u8 Place_Invariant_Meet_Plug_Cnt;
  static u8 Place_Normal_Meet_Plug_Cnt;
  
  static u16 Speed_Unusual_Hall_Pulse_Buf;
  //static u16 Speed_Normal_Hall_Pulse_Buf;
  static u16 Place_Invariant_Hall_Pulse_Buf;
  static u16 Reversal_Hall_Pulse_Buf;
  
  // 是否有边沿信号
  if (Flag_Motor.Hall_Pulse_edge)
  {
    Flag_Motor.Hall_Pulse_edge = FALSE;
    // 计算转速
    if (Hall_Pulse_Width < 234)
    {
      Motor_Speed = 200;
    }
    else if (Hall_Pulse_Width > 10000)
    {
     Motor_Speed = 0;
    }
    else
    {
      Motor_Speed = ((u8) SPEED_COUNT(Hall_Pulse_Width));   // 返回每分钟的转速
    }
    // 判断手拉方向
    if (Motor_Array[0].Motor_Flag.Fact_Direction)
    {
      Hand_Drag_Direction_Open++;
    }
    else
    {
      Hand_Drag_Direction_SHUT++;
    }
  }
  
  if (Motor_Array[0].Motor_Flag.Run_Status)  // 运行
  {
    Motor_SHUT_Cnt = NULL;
    reversal_cnt = NULL;
    Hand_Drag_Direction_Open = NULL;
    Hand_Drag_Direction_SHUT = NULL;
    hall_pulse_buf = Hall_Pulse_Amount_Cnt;
    
    // ***** 限位 **********************************************************
    if (Flag_Motor.Adj_Limit_Point) //是否在微调行程状态
    {
      Flag_Motor.slowstop = NULL;  // 不需要减速
      if (Flag_Motor.Meet_Plugb)   // 是否有遇阻
      {
        if (Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Run_Direction)//是否为遇阻的相反方向 
        {
          Flag_Motor.Meet_Plugb = FALSE;
        }
        else
        {
          flag_stop = TRUE;  // 遇阻后同方向不允许再走
        }
      }
    }
    else if (!Flag_Motor.over_limit_run&&(Motor_Array[0].Motor_Flag.Journey_Set_OK || Motor_Array[0].Motor_Flag.Single_Journey))
    {
      u8 space = 3;                              //  默认到行程预留3个信号的间隙
      if (Motor_Array[0].Motor_Flag.Serpentine_Curtain)         // 是否为蛇形帘
      {
        space = 0;                               // 蛇形帘间隙为0
      }
      
      // 有慢启动并且设置好行程以后才有慢停
      if ((!Motor_Array[0].Motor_Flag.No_Slow_Start) && (Motor_Array[0].Motor_Flag.Journey_Set_OK))
      {
        SlowStop (Target_Journey_Point);     // 接近目标行程点减速
      }
      
      // 位置比较
      if (Flag.Goto_PCT)
      {
        if (Motor_Array[0].Motor_Flag.Journey_Set_OK)
        {
          // 是否接近行程
          if ((Hall_Pulse_Amount_Cnt > (Target_Journey_Point - 2))
          && (Hall_Pulse_Amount_Cnt < (Target_Journey_Point + 2)))
          {
            flag_stop = TRUE;
          }
        }
      }
      else if (Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)//不相等
      {
       Flag_Motor.Shut_Margin = NULL;                        //清关行程界限点标志
       if (!Motor_Array[0].Motor_Flag.Single_Journey)
       {
        if (Flag_Motor.Open_Margin)
        {
          if (Hall_Pulse_Amount_Cnt  < (Target_Journey_Point - 8))// 是否接近行程
          {
            Flag_Motor.Open_Margin = NULL;                    //清开行程界限点标志
          }
          else
          {
            flag_stop = TRUE;        // 到行程电机停止                 
          }
        }
        if (Hall_Pulse_Amount_Cnt > (Target_Journey_Point - space)) // 是否接近行程
        {
          if (!Motor_Array[0].Motor_Flag.To_Journey_Meet_Plugb)         // 是否到行程不需要停止，遇阻才停
          {
            flag_stop = TRUE;
            Flag_Motor.Open_Margin = TRUE;                // 开行程界限点标志
            no_hand_drag_cnt = JOURNEY_NO_HAND_DRAG;
          
            Flag_Motor.Down_In_Journey_Place = TRUE;
            if (Flag_Motor.Up_In_Journey_Place)
            {
              Flag_Motor.In_Journey_Place = TRUE;
            }
            //if (!(Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Run_Direction))
            //{
              //Meet_plug_place_Buffer = NULL;                         /*走到行程清遇阻位置*/
            //}
          }
        }
       }
      }
      else 
      {
        Flag_Motor.Open_Margin = NULL;                             /*清开行程界限点标志*/
        if (Flag_Motor.Shut_Margin)
        {
          if (Hall_Pulse_Amount_Cnt > (Target_Journey_Point + 8))/* 是否已经到行程-*/
          {
            Flag_Motor.Shut_Margin = NULL;                        /*清关行程界限点标志*/
          }
          else
          {
            flag_stop = TRUE;
          }
        }
        if (Hall_Pulse_Amount_Cnt < (Target_Journey_Point + space)) /* 是否接近行程 --*/
        {
          if (!Motor_Array[0].Motor_Flag.To_Journey_Meet_Plugb)         // 是否到行程不需要停止，遇阻才停
          {
            flag_stop = TRUE;
            Flag_Motor.Shut_Margin = TRUE;                           /* 关行程界限点标志 */
            no_hand_drag_cnt = JOURNEY_NO_HAND_DRAG;
          
            Flag_Motor.Up_In_Journey_Place = TRUE;
            if (Flag_Motor.Down_In_Journey_Place)
            {
              Flag_Motor.In_Journey_Place = TRUE;
            }
            //if (!(Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Run_Direction))
            //{
              //Meet_plug_place_Buffer = NULL;                          /*走到行程清遇阻位置*/
            //}
          }
        }
      }
    }
    hall_pulse_buf = Hall_Pulse_Amount_Cnt;
    
    // **** 速度控制 ********************************************************
    
    if (Flag.Time_10ms)
    {
     Flag.Time_10ms = FALSE;
     if (!flag_stop)
     {
      if (motor_rum_time < (120 / MOTOR_CONTROL_TIME))
      {
        Motor_Duty = MIN_DUTY<<1;           // PWM占空比最小
        Flag_Motor.Runed = NULL;
        Target_Speed = MIN_SPEED;
        
        NoSpeed_Meet_Plug_Cnt = NULL;
        Speed_Unusual_Meet_Plug_Cnt = NULL;
        //Speed_Normal_Meet_Plug_Cnt = NULL;
        Place_Invariant_Meet_Plug_Cnt = NULL;
        Place_Normal_Meet_Plug_Cnt = NULL;
        
        Flag_Motor.slowstop = NULL;
  
        Speed_Unusual_Hall_Pulse_Buf = Hall_Pulse_Amount_Cnt;
        //Speed_Normal_Hall_Pulse_Buf = Hall_Pulse_Amount_Cnt;
        Place_Invariant_Hall_Pulse_Buf = Hall_Pulse_Amount_Cnt;
        
      }
      else
      {
        signed int En,En1;
        u8 flag;
        
        flag_rebound=1;
        
        if (Flag_Motor.Hand_Drag)
        {
          if (Motor_Speed >=60)
          {
            Motor_Duty = MIN_DUTY<<1;           // PWM占空比最小
            Flag_Motor.Runed = NULL;
            Target_Speed = MIN_SPEED << 1;
            //flag_rebound=0;
          }
          else
          {
            Flag_Motor.Hand_Drag = NULL;
          }
        }
        
        if (Flag_Motor.Runed)
        {
          // 速度控制
          En = Target_Speed - Motor_Speed;         // 得到当前速度误差 E(n)
          En1 = Target_Speed - Motor_Speed_Buff;   // 得到上次速度误差 E(n-1)
          En += En1;
          // 将误差转换成正数
          if (En >= 0)               // 当前增量是否为正数
          {
           flag = 1;                // 
          }
          else                       // 增量为负数
          {
            En = 0 - En;             // 转换成正数
            flag = 0;
          }
          
          if (flag == 0)                     // 增量为负数
          {
            if (En > 100)
            {
              Duty_Sub(&Motor_Duty,35);
            }
            else if (En > 80)
            {
              Duty_Sub(&Motor_Duty,10);
            }
            else if (En > 60)
            {
              Duty_Sub(&Motor_Duty,8);
            }
            else if (En > 40)
            {
              Duty_Sub(&Motor_Duty,5);
            }
            else if (En > 20)
            {
              Duty_Sub(&Motor_Duty,4);
            }
            else if (En > 10)
            {
              Duty_Sub(&Motor_Duty,2);
            }
            else if (En > 5)
            {
              Duty_Sub(&Motor_Duty,1);
            }
          }
          else                              // 增量为正数
          {
            if (!Flag_Motor.Hand_Drag)
            {
              En += (Motor_Max_Speed - Target_Speed) / 2;
            }
            
            if (En > 100)
            {
              Duty_Add(&Motor_Duty,30);
            }
            else if (En > 90)
            {
              Duty_Add(&Motor_Duty,15);
            }
            else if (En > 80)
            {
              Duty_Add(&Motor_Duty,10);
            }
            else if (En > 60)
            {
              Duty_Add(&Motor_Duty,8);
            }
            else if (En > 40)
            {
              Duty_Add(&Motor_Duty,5);
            }
            else if (En > 20)
            {
              Duty_Add(&Motor_Duty,4);
            }
            else if (En > 10)
            {
              Duty_Add(&Motor_Duty,2);
            }
            else if (En > 5)
            {
              Duty_Add(&Motor_Duty,1);
            }
          }
          
          if ((Motor_Speed == 0) || (Motor_Speed >= 200))    /* 转速异常 ---*/
          {
            if (++Speed_Unusual_Meet_Plug_Cnt >= (1000 / MOTOR_CONTROL_TIME))
            {
              Speed_Unusual_Meet_Plug_Cnt = NULL;
              //Speed_Normal_Meet_Plug_Cnt = NULL;
              if ((Hall_Pulse_Amount_Cnt < (Speed_Unusual_Hall_Pulse_Buf + 4))
              && (Hall_Pulse_Amount_Cnt > (Speed_Unusual_Hall_Pulse_Buf - 4)))
              {
                flag_stop = TRUE;
                meet_plug_stop = TRUE;
                Meet_Plugb_Event = 1;
              }
              Speed_Unusual_Hall_Pulse_Buf = Hall_Pulse_Amount_Cnt;
            }
          }
          else
          {
            Speed_Unusual_Meet_Plug_Cnt = NULL;
            /*if (++Speed_Normal_Meet_Plug_Cnt >= (2000 / MOTOR_CONTROL_TIME))
            {
              Speed_Normal_Meet_Plug_Cnt = NULL;
              if ((Hall_Pulse_Amount_Cnt < (Speed_Normal_Hall_Pulse_Buf + 4))
              && (Hall_Pulse_Amount_Cnt > (Speed_Normal_Hall_Pulse_Buf - 4)))
              {
                flag_stop = TRUE;
                meet_plug_stop = TRUE;
                Meet_Plugb_Event = 2;
              }
              Speed_Normal_Hall_Pulse_Buf = Hall_Pulse_Amount_Cnt;
            }*/
          }
          
          if (!Flag_Motor.slowstop)
          {
            //是否尚未设置行程
            if (!Motor_Array[0].Motor_Flag.Journey_Set_OK)
            {
              Target_Speed = 60;   //未设置行程，转速强制为60转，无慢起慢停
            }
            else
            {
              if (++Motor_Speed_Cnt > (40 / MOTOR_CONTROL_TIME))
              {
                Motor_Speed_Cnt = NULL;
                
                if (Target_Speed < Motor_Max_Speed)
                {
                  if (!Flag_Motor.Hand_Drag)
                  {
                    Target_Speed++;
                    if (Motor_Max_Speed > 65)
                    {
                      Target_Speed++;
                    }
                  }
                }
                else if (Target_Speed > Motor_Max_Speed)
                {
                  Target_Speed--;
                  if (Target_Speed > (Motor_Max_Speed + 20))
                  {
                    Target_Speed -= 2;
                  }
                }
              }
            }
          }
        }
        else
        {
          if (motor_rum_time > (1000 / MOTOR_CONTROL_TIME))
          { if(Current_Current_mA < MAX_OUT_CURRENT)
            Motor_Duty += 25;  //5
          }
          
          if (Motor_Speed != 0)
          {
            //if (++quicken_wait_time >= (100 / MOTOR_CONTROL_TIME))
           // {
              Motor_Speed = 0;
           //   quicken_wait_time = 0;
              Flag_Motor.Runed = TRUE;
              Flag_Motor.Run_OK = TRUE;                       /* 启动完成标志 ----*/
              Flag_Motor.Shut_OK = NULL;
           // }
          }
        }
        
        if (Motor_Duty < MIN_DUTY)
        {
          Motor_Duty = MIN_DUTY;
        }
        if (Motor_Duty > MAX_DUTY)
        {
          Motor_Duty = MAX_DUTY;
        }
        
        if (Motor_Array[0].Motor_Flag.Run_Direction)
        {
          SetUpPWM(Motor_Duty);
        }
        else
        {
          SetDownPWM(Motor_Duty);
        }
        
        if (motor_rum_time > (200 / MOTOR_CONTROL_TIME))
        {
          u8 meet_plug_time;
        
          meet_plug_time = (300 / MOTOR_CONTROL_TIME);
          /*if (Motor_Array[0].Motor_Flag.Journey_Set_OK && Flag_Motor.In_Journey_Place)
          {
            meet_plug_time = (600 / MOTOR_CONTROL_TIME);
          }*/
          
          //if (Motor_Speed < 10)
          if (Current_Current_mA > MEET_PLUG_CURRENT)
          {
              if (++Place_Invariant_Meet_Plug_Cnt > meet_plug_time)
              {
                Place_Invariant_Meet_Plug_Cnt = NULL;
                Place_Normal_Meet_Plug_Cnt = NULL;
                if ((Hall_Pulse_Amount_Cnt < (Place_Invariant_Hall_Pulse_Buf + 4))
                && (Hall_Pulse_Amount_Cnt > (Place_Invariant_Hall_Pulse_Buf - 4)))
                {
                  flag_stop = TRUE;
                  meet_plug_stop = TRUE;
                  Meet_Plugb_Event = 3;
                }
                Place_Invariant_Hall_Pulse_Buf = Hall_Pulse_Amount_Cnt;
              }
          }
          else
          {
            if (++Place_Normal_Meet_Plug_Cnt > 100)
            {
              Place_Invariant_Meet_Plug_Cnt = NULL;
              Place_Normal_Meet_Plug_Cnt = NULL;
            }
          }
          
          if (Motor_Speed == 0)    // 运行状态无转速
          {
            if (++NoSpeed_Meet_Plug_Cnt >= NO_SPEED_TIME)
            {
              // 速度低于10 并且持续3秒
              NoSpeed_Meet_Plug_Cnt = NULL;
              flag_stop = TRUE;
              //meet_plug_stop = TRUE; //不设行程
              //Meet_Plugb_Event = 4;
            }
          }
          else
          {
            NoSpeed_Meet_Plug_Cnt = NULL;
          }
        }
      }
      
      Motor_Speed_Buff = Motor_Speed;
      if (Motor_Array[0].Motor_Flag.No_Slow_Start)      // 是否没有慢启动
      {
        Target_Speed = Motor_Max_Speed;  // 一直为最高转速
      }
      if (motor_rum_time < NO_SPEED_TIME)
      {
        motor_rum_time++;
      }
      else if (!Flag_Motor.Runed)
      {
        flag_stop = TRUE;
        //meet_plug_stop = TRUE; //不设行程
        //Meet_Plugb_Event = 5;
      }
     }// end off if (!flag_stop)
    }// end off if (Flag.Time_10ms)
    
    if (flag_stop)
    {
      Motor_Duty = NULL;
      Flag_Motor.Run_OK = TRUE;
      Flag_Motor.Shut_OK = NULL;
      Motor_Status = MOTOR_STOP;
      Motor_Array[0].Motor_Flag.Run_Status = NULL;
      
      if(Motor_Array[0].Motor_Flag.Journey_Set_OK)Flag.Goto_PCT = NULL;
      if (meet_plug_stop)// && (Current_Current_mA > (MEET_PLUG_CURRENT >> 1)))
      {
        Flag_Motor.Meet_Plugb = TRUE;   // 置遇阻标志
        Flag_Motor.Meet_Plugb_Direction = Motor_Array[0].Motor_Flag.Run_Direction;
        Meet_plug_place = Hall_Pulse_Amount_Cnt;  // 记忆遇阻位置
        Flag_Motor.Wait_Set_Journey = TRUE;
      }
    }
  }
  else
  {
    static u8 motor_runed_cnt;
    static u8 runed_Latime_cnt;
    
    motor_rum_time = NULL;
    
    if (Motor_SHUT_Cnt < 50)  //50
    {
      Motor_Duty = 0;
      SetStopPWM();
      Hand_Drag_Direction_Open = NULL;
      Hand_Drag_Direction_SHUT = NULL;
      Flag_Motor.Hand_Drag = NULL;
      hall_pulse_buf = Hall_Pulse_Amount_Cnt;
      Motor_SHUT_Cnt++;
      reversal_cnt = 0;      // 反转持续时间
      runed_Latime_cnt = 0;  // 反转延迟时间
      motor_runed_cnt = 0;   // 反转次数
    }
    else
    {
      // // 是否为蛇形帘 或者为下行(合) (相等为下) 
      if ((Motor_Array[0].Motor_Flag.Serpentine_Curtain) || (!(Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Direction)))
      {
        MeetPlugb();                          // 蛇形帘提前设置行程
      }
      
      if (Motor_SHUT_Cnt < 2500)
      {
        Motor_SHUT_Cnt++;
      }
      else
      {
        if ((Flag_Motor.Meet_Plugb) && (!Flag_Motor.Adj_Limit_Point)) //是否不在微调行程状态
        {
          Flag_Motor.Wait_Set_Journey = NULL;
          Flag_Motor.Meet_Plugb = NULL;
        }
      }
      
      if (flag_rebound)
      {
        if (reversal_cnt == 0)
        {
          Reversal_Hall_Pulse_Buf = Hall_Pulse_Amount_Cnt;
        }
        if(reversal_cnt < 50)
        {
          if (!Motor_Array[0].Motor_Flag.Run_Direction)
          {
            SetUpPWM(MAX_DUTY - 200);
          }
          else
          {
            SetDownPWM(MAX_DUTY - 200);
          }
          reversal_cnt++;
        }
        else
        {
          hall_pulse_buf = Hall_Pulse_Amount_Cnt;
          Flag_Motor.Hand_Drag = NULL;
          Flag_Motor.Shut_OK = TRUE;
          Flag_Motor.Run_OK = NULL;
          
          if (runed_Latime_cnt == 0)
          {
            runed_Latime_cnt++;
            if ((Hall_Pulse_Amount_Cnt > (Reversal_Hall_Pulse_Buf - 2))
            && (Hall_Pulse_Amount_Cnt < (Reversal_Hall_Pulse_Buf + 2)))
            {
              // 输出头没有动 不用再反转
              flag_rebound=0;
              Flag_Motor.Runed = NULL;
              motor_runed_cnt = NULL;
              reversal_cnt = NULL;
              Motor_SHUT_Cnt = NULL;
            }
            /*else if (!(Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Direction)) // 是否为下行(合) (相等为下)
            {
                Flag_Motor.Runed = NULL;
                motor_runed_cnt = NULL;
                reversal_cnt = NULL;
                Motor_SHUT_Cnt = NULL;
            }*/
            else if (++motor_runed_cnt >= 10) // 电机反转10次
            {
              flag_rebound=0;
              Flag_Motor.Runed = NULL;
              motor_runed_cnt = NULL;
              reversal_cnt = NULL;
              Motor_SHUT_Cnt = NULL;
            }
            else if (Motor_Array[0].Motor_Flag.Journey_Set_OK && Flag_Motor.In_Journey_Place)
            { if (Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
              { if(Hall_Pulse_Amount_Cnt<MIN_JOURNEY_VALUE+(Hall_Pulse_Amount-MIN_JOURNEY_VALUE)/2)
                { flag_rebound=0;
                  Flag_Motor.Runed = NULL;
                  motor_runed_cnt = NULL;
                  reversal_cnt = NULL;
                  Motor_SHUT_Cnt = NULL;
                }
              }
              else
              { if(Hall_Pulse_Amount_Cnt>MIN_JOURNEY_VALUE+(Hall_Pulse_Amount-MIN_JOURNEY_VALUE)/2)
                { flag_rebound=0;
                  Flag_Motor.Runed = NULL;
                  motor_runed_cnt = NULL;
                  reversal_cnt = NULL;
                  Motor_SHUT_Cnt = NULL;
                }
              }
            }
          }
          else
          {
            if (!Motor_Array[0].Motor_Flag.Journey_Set_OK)
            {
              reversal_cnt = NULL;
              runed_Latime_cnt = NULL; // 没行程时连续反转
            }
            else
            {
              Motor_Duty = 0;
              SetStopPWM();
              if (++runed_Latime_cnt > 250) // 反转等待时间 500mS
              {
                reversal_cnt = NULL;
                runed_Latime_cnt = NULL;
              }
            }
          }
        }
      }
      else
      {
        Flag_Motor.Shut_OK = TRUE;
        Flag_Motor.Run_OK = NULL;
        MeetPlugb();
        
          // 微调行程时不允许手拉启动
          if ((!Motor_Array[0].Motor_Flag.Hand_Drag_Status) && (no_hand_drag_cnt == NULL) && (!Flag_Motor.Adj_Limit_Point))
          {
            if ((Hall_Pulse_Amount_Cnt > (hall_pulse_buf + 10))
            || (Hall_Pulse_Amount_Cnt < (hall_pulse_buf - 10)))
            {
              if (Motor_Status == MOTOR_STOP)
              {
                Flag_Motor.Hand_Drag_Startup = FALSE;
                Flag_Motor.Hand_Drag_Direction = FALSE;
                if (Hand_Drag_Direction_Open > Hand_Drag_Direction_SHUT)
                {
                  Flag_Motor.Hand_Drag_Direction = TRUE;
                }
                // 置手拉启动标志
                Flag_Motor.Hand_Drag = TRUE;
                Motor_Status = MOTOR_UP;
                if (!(Flag_Motor.Hand_Drag_Direction ^ Motor_Array[0].Motor_Flag.Direction))
                {
                  Motor_Status = MOTOR_DOWN;
                }
                Hand_Drag_Direction_Open = NULL;
                Hand_Drag_Direction_SHUT = NULL;
                hall_pulse_buf = Hall_Pulse_Amount_Cnt;
                
                if(Motor_Array[0].Motor_Flag.Journey_Set_OK)
                { if(Hall_Pulse_Amount_Cnt<MIN_JOURNEY_VALUE-5||Hall_Pulse_Amount_Cnt>Hall_Pulse_Amount+5)
                  { Flag_Motor.over_limit_run=1;
                  }
                }
                else if(Motor_Array[0].Motor_Flag.Single_Journey)
                { if(Hall_Pulse_Amount_Cnt<MIN_JOURNEY_VALUE-5) Flag_Motor.over_limit_run=1;
                }
              }
            }
          }
          else
          {
            Hand_Drag_Direction_Open = NULL;
            Hand_Drag_Direction_SHUT = NULL;
            hall_pulse_buf = Hall_Pulse_Amount_Cnt;
            Flag_Motor.Hand_Drag = FALSE;
          }

          if((Flag_Motor.PercentUsual) && (LED_Status <= LEARN_STATUS))             //if(Motor_Array[0].Motor_Flag.Journey_Set_OK)
          {
            if(Motor_Array[0].Motor_Flag.Journey_Set_OK)//if(Flag_Motor.PercentUsual)
            {
              if(Flag.Goto_PCT)
              {
                //Flag.Goto_PCT = NULL;
                LedStatusUpdate(LED_OFF);
                //UartDataDisposal();
                DisposalPercent(PercentBuff);//设置百分比时如果没有行程，先自动开合一次设定行程，然后跑到百分比位置
                //PercentBuff = 0;
                Flag_Motor.PercentUsual = NULL;
                Flag.Time_100ms = NULL;
                Time_100ms_Cnt = TIME_100MS_VALUE;
                
                Flag.Uart_tx = TRUE;
              }
            }
            else if(Motor_Array[0].Motor_Flag.Single_Journey)
            {
              if ((Motor_Array[0].Motor_Flag.Journey_Direction ^ Motor_Array[0].Motor_Flag.Direction))
                Motor_Status = MOTOR_DOWN;
              else
                Motor_Status = MOTOR_UP;
            }
          }
      }
    }
  }
}
/**********************************************************************************
  函数名:   
  功  能:   
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：  
 *********************************************************************************/
void MotorControl (Motor_Control_TypeDef * status)
{
  if (no_hand_drag_cnt != NULL)
  {
    no_hand_drag_cnt--;
    hall_pulse_buf = Hall_Pulse_Amount_Cnt;
  }
  
    if (LED_Status > SPEED_HINT)
    {
      if(!Flag_Motor.PercentUsual)
      {
        *status = MOTOR_STOP; 
        Motor_Array[0].Motor_Status_Buffer = MOTOR_STOP;
      }
    }
    if (!Motor_Array[0].Motor_Flag.StatusSwitch_Delay)
    {
      if (*status == Motor_Array[0].Motor_Status_Buffer)
      {
        if (++Motor_Array[0].RunTime_Cnt > MOTOR_RUN_CNT_VALUE)
        {
          Motor_Array[0].RunTime_Cnt = NULL;
          *status = MOTOR_STOP;
        }
        if(Motor_Array[0].RunTime_Cnt == MOTOR_RUN_5S_VALUE )
        {
//           Flag.AutoReport = 0;
//           Flag.Uart_tx = 0;
        }
      }
      else
      {
        if (Flag_Motor.Wait_Set_Journey)
        {
          *status = MOTOR_STOP;
          Motor_Array[0].Motor_Status_Buffer = MOTOR_STOP;
        }
        else
        {
            if ((Flag_Motor.Run_OK) || (Flag_Motor.Shut_OK))
            {
              Flag_Motor.Run_OK = NULL;
              Flag_Motor.Shut_OK = NULL;
              
              Set_Limit_Point_Cnt = NULL;
              
              Motor_Array[0].RunTime_Cnt = NULL;
              if (3 == (*status | Motor_Array[0].Motor_Status_Buffer))
              {
                Motor_Array[0].Motor_Flag.StatusSwitch = TRUE;
                Motor_Array[0].Motor_Flag.StatusSwitch_Delay = TRUE;
                Motor_Array[0].Motor_Status_Buffer = MOTOR_STOP;
                //Flag_Motor.Runed = FALSE;
                Flag_Motor.Hand_Drag = FALSE;
              }
              else
              {
                Motor_Speed = 0;
                Motor_Array[0].Motor_Status_Buffer = *status;
                Motor_Array[0].Motor_Flag.StatusSwitch = TRUE;
//                Flag.AutoReport = TRUE;
//                Flag.Uart_tx = TRUE;
              }
            }
        }
      }
      (*Motor_Array[0].Motor_Diver_Array[Motor_Array[0].Motor_Status_Buffer])();
      
      if (Motor_Array[0].Motor_Flag.StatusSwitch)
      {
        Motor_Array[0].Motor_Flag.StatusSwitch = FALSE;
        
        if (!Motor_Array[0].Motor_Flag.Manual_Control_Synchro)  // 是否需要手控同步
        {
          Key_Out_Status = Motor_Array[0].Motor_Status_Buffer; 
          Flag.Key_Signal_Out = TRUE;            // 按键同步输出信号
        }
        
        if ((Motor_Array[0].Motor_Flag.Journey_Set_OK) || (Motor_Array[0].Motor_Flag.Single_Journey))
        {
          // 确定目标位置
          if (Flag.Goto_PCT)
          {
            Target_Journey_Point = Hall_Pulse_Amount_PCT;
          }
          else
          {
            if (Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
            {
              Target_Journey_Point = Hall_Pulse_Amount;
            }
            else
            {
              Target_Journey_Point = MIN_JOURNEY_VALUE;
            }
          }
        }
      }
    }
    else if (++Motor_Array[0].RunTime_Cnt > MOTOR_SWITCH_LATIME)
    {
      Motor_Array[0].RunTime_Cnt = NULL;
      Motor_Array[0].Motor_Flag.StatusSwitch_Delay = FALSE;
    }
}