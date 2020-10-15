/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *  
 *  MotorControl.c file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define NO_SPEED_TIME              4000 / MOTOR_CONTROL_TIME     /* ��ת�ٱ���ʱ�� --*/
#define MEET_PLUGB_NO_HAND_DRAG    800 / TIME_50MS_VALUE
#define JOURNEY_NO_HAND_DRAG       500  / TIME_50MS_VALUE

#define OVER_CURRENT_DETECT()      ((P3IN & BIT6)?0:1)

#define PULSE_SAMPLE_TIME          (RF_DECODE_TIME / 10)

//#define SPEED_COUNT(var) (46875/var)//(1000000us/(var*2*40us*16(��դ����)))*60s

#ifdef HALL_DETECT
  #define SPEED_COUNT(var)                               \
    ((37500 + ((((var)*(PULSE_SAMPLE_TIME)) / 10) / 2))  \
    /(((var)*(PULSE_SAMPLE_TIME)) / 10))
#else  // 18750
  #define SPEED_COUNT(var)                               \
    ((18750 + ((((var)*(PULSE_SAMPLE_TIME)) / 10) / 2))  \
    /(((var)*(PULSE_SAMPLE_TIME)) / 10))
#endif


/* �ڲ����� ---------------------------------------------------------------------*/
void MotorUp (void);
void MotorDown (void);
void MotorStop (void);
/* �ڲ����� ---------------------------------------------------------------------*/
u8 no_hand_drag_cnt;
u8 Meet_Plugb_Event;
//static u16 Meet_plug_place_Buffer;
static u16 Meet_plug_place;
static u16 Target_Journey_Point;
/**********************************************************************************
  ������:   MotorControl
  ��  ��:   
 *********************************************************************************/
void Duty_Add (u16 *p,u8 i)
{ if(Current_Current_mA > MAX_OUT_CURRENT) return;
  if(*p < MAX_DUTY)
  {
    *p += i;
  }
}
/**********************************************************************************
  ������:   MotorControl
  ��  ��:   
 *********************************************************************************/
void Duty_Sub (u16 *p,u8 i)
{
  if (*p > i)
  {
    *p -= i;
  }
}
/**********************************************************************************
  ������:   
  ��  ��:   
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
  ������:   
  ��  ��:   
 *********************************************************************************/
void SetUpPWM (u16 duty)
{
  IN1_L_1();      // �������¹�
  IN2_L_1();
  IN2_H_0();
  
  P3SEL = BIT3;   // P3.3
  P2SEL = 0;
  IN1_H_1();      // �����ű�
  TA1CCR2 = duty; //���ű�PWM���
}
/**********************************************************************************
  ������:   
  ��  ��:   
 *********************************************************************************/
void SetDownPWM (u16 duty)
{
  IN1_L_1();      // �������¹�
  IN2_L_1();
  IN1_H_0();
  
  P2SEL = BIT2;   // P2.2
  P3SEL = 0;
  IN2_H_1();      // �����ű�
  TA1CCR1 = duty; //���ű�PWM���
}
/**********************************************************************************
  ������:   
  ��  ��:   
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
  ������:   
  ��  ��:   
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
  ������:   
  ��  ��:   
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
  ������:   JourneyDelete
  ��  ��:   
 *********************************************************************************/
void JourneyDelete (void)
{
  Hall_Pulse_Amount_Cnt = HALL_PULSE_MAX >> 1;   // ���¸���ʼλ��
  hall_pulse_buf = Hall_Pulse_Amount_Cnt;
  Hall_Pulse_Amount = 0;                         // ����г�
  //Meet_plug_place_Buffer = 0;                    // �������λ��
    //Motor_Array[0].Motor_Flag.Hand_Drag_Status = 0;
  //Motor_Array[0].Motor_Flag.Direction = 0;
  Motor_Array[0].Motor_Flag.Journey_Direction = 0;
  Motor_Array[0].Motor_Flag.Single_Journey = NULL;              // ���г̱�־
  Motor_Array[0].Motor_Flag.Journey_Set_OK = NULL;
  Flag_Motor.slowstop = NULL;                    // ȡ�����٣����м���
}
/**********************************************************************************
  ������:   JourneyCheck
  ��  ��:   
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
  ������:   JourneySeting
  ��  ��:   
 *********************************************************************************/
void JourneySeting (void)
{
  u8 Flag_Rebound = NULL;
  
    /*if (Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Direction) // �Ƿ�Ϊ����(��) (�����Ϊ��)
    {
      if ((Hall_Pulse_Amount_Cnt < (Meet_plug_place - 6))
      || (Hall_Pulse_Amount_Cnt > (Meet_plug_place + 6)))
      {
        Flag_Rebound = TRUE;
      }
    }*/
    
    if (!Motor_Array[0].Motor_Flag.Single_Journey)                                //�Ƿ�δ�趨�����г�
    {
      Motor_Array[0].Motor_Flag.Single_Journey = TRUE;                            //�趨�����г��趨OK
      if (Flag_Rebound)
      {
        Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE - 25;      //�趨��С�г�
      }
      else
      {
        Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;           // �趨��С�г�
      }
      Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;    // �����г̷���
    }
    else
    {
      if (Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
      {
        if (Flag_Rebound)
        {
          Hall_Pulse_Amount = Hall_Pulse_Amount_Cnt - 25;    //����з������ٺ���20
        }
        else
        {
          Hall_Pulse_Amount = Hall_Pulse_Amount_Cnt;         // �����г�
        }
        if (Hall_Pulse_Amount < (MIN_JOURNEY_VALUE + 50))
        { if (Flag_Rebound)
          {
            Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE - 25;      //�趨��С�г�
          }
          else
          {
            Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;           // �趨��С�г�
          }
          Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;    // �����г̷���
        }
        else
        { Motor_Array[0].Motor_Flag.Journey_Set_OK = TRUE;                          // �г��趨OK
          Motor_Array[0].Motor_Flag.Single_Journey = NULL;                          //�趨�����г��趨
          Flag_Motor.In_Journey_Place = TRUE;
        }
      }
      else
      {
        Motor_Array[0].Motor_Flag.Single_Journey = TRUE;                    //�趨�����г��趨OK
        if (Flag_Rebound)
        {
          Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE - 25;    // �趨��С�г�
        }
        else
        {
          Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;         // �趨��С�г�
        }
        Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;  // �����г̷���
      }
    }
    LedStatusUpdate(OPRATION_FINISH);
    //JourneyCheck();
    FlashWriteData(0xee);
}
/**********************************************************************************
  ������:   MeetPlugb
  ��  ��:   
 *********************************************************************************/
void MeetPlugb (void)
{
  if (Flag_Motor.Meet_Plugb)
  {
    if (Flag_Motor.Adj_Limit_Point)      // ΢����λ״̬����������
    {
      Flag_Motor.Wait_Set_Journey = NULL;
      return;
    }
    if ((Motor_Speed < MIN_SPEED) || (Motor_Array[0].Motor_Flag.Serpentine_Curtain)
    || (!(Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Direction)))
    {
      Flag_Motor.Meet_Plugb = NULL;         // ���������־
      if (!Motor_Array[0].Motor_Flag.Journey_Set_OK)       // ���г�����
      {
        JourneySeting();
      }
      else   // ���г�����
      {
        if (!Flag_Motor.In_Journey_Place)  // û�е����г�λ������
        { if(!flag_yuzu_once)
          { flag_yuzu_once=1;
            Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;           // �趨��С�г�
            Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;    // �����г̷���
            LedStatusUpdate(OPRATION_FINISH);
            FlashWriteData(0xee);
          }
          else
          { 
            if (Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)
            { 
              if (Hall_Pulse_Amount_Cnt < (MIN_JOURNEY_VALUE + 50))
              { Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;           // �趨��С�г�
                Motor_Array[0].Motor_Flag.Journey_Direction = Motor_Array[0].Motor_Flag.Run_Direction;    // �����г̷���
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
  ������:   JourneyComper
  ��  ��:   
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
  ������:   
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
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
  
  // �Ƿ��б����ź�
  if (Flag_Motor.Hall_Pulse_edge)
  {
    Flag_Motor.Hall_Pulse_edge = FALSE;
    // ����ת��
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
      Motor_Speed = ((u8) SPEED_COUNT(Hall_Pulse_Width));   // ����ÿ���ӵ�ת��
    }
    // �ж���������
    if (Motor_Array[0].Motor_Flag.Fact_Direction)
    {
      Hand_Drag_Direction_Open++;
    }
    else
    {
      Hand_Drag_Direction_SHUT++;
    }
  }
  
  if (Motor_Array[0].Motor_Flag.Run_Status)  // ����
  {
    Motor_SHUT_Cnt = NULL;
    reversal_cnt = NULL;
    Hand_Drag_Direction_Open = NULL;
    Hand_Drag_Direction_SHUT = NULL;
    hall_pulse_buf = Hall_Pulse_Amount_Cnt;
    
    // ***** ��λ **********************************************************
    if (Flag_Motor.Adj_Limit_Point) //�Ƿ���΢���г�״̬
    {
      Flag_Motor.slowstop = NULL;  // ����Ҫ����
      if (Flag_Motor.Meet_Plugb)   // �Ƿ�������
      {
        if (Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Run_Direction)//�Ƿ�Ϊ������෴���� 
        {
          Flag_Motor.Meet_Plugb = FALSE;
        }
        else
        {
          flag_stop = TRUE;  // �����ͬ������������
        }
      }
    }
    else if (!Flag_Motor.over_limit_run&&(Motor_Array[0].Motor_Flag.Journey_Set_OK || Motor_Array[0].Motor_Flag.Single_Journey))
    {
      u8 space = 3;                              //  Ĭ�ϵ��г�Ԥ��3���źŵļ�϶
      if (Motor_Array[0].Motor_Flag.Serpentine_Curtain)         // �Ƿ�Ϊ������
      {
        space = 0;                               // ��������϶Ϊ0
      }
      
      // ���������������ú��г��Ժ������ͣ
      if ((!Motor_Array[0].Motor_Flag.No_Slow_Start) && (Motor_Array[0].Motor_Flag.Journey_Set_OK))
      {
        SlowStop (Target_Journey_Point);     // �ӽ�Ŀ���г̵����
      }
      
      // λ�ñȽ�
      if (Flag.Goto_PCT)
      {
        if (Motor_Array[0].Motor_Flag.Journey_Set_OK)
        {
          // �Ƿ�ӽ��г�
          if ((Hall_Pulse_Amount_Cnt > (Target_Journey_Point - 2))
          && (Hall_Pulse_Amount_Cnt < (Target_Journey_Point + 2)))
          {
            flag_stop = TRUE;
          }
        }
      }
      else if (Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction)//�����
      {
       Flag_Motor.Shut_Margin = NULL;                        //����г̽��޵��־
       if (!Motor_Array[0].Motor_Flag.Single_Journey)
       {
        if (Flag_Motor.Open_Margin)
        {
          if (Hall_Pulse_Amount_Cnt  < (Target_Journey_Point - 8))// �Ƿ�ӽ��г�
          {
            Flag_Motor.Open_Margin = NULL;                    //�忪�г̽��޵��־
          }
          else
          {
            flag_stop = TRUE;        // ���г̵��ֹͣ                 
          }
        }
        if (Hall_Pulse_Amount_Cnt > (Target_Journey_Point - space)) // �Ƿ�ӽ��г�
        {
          if (!Motor_Array[0].Motor_Flag.To_Journey_Meet_Plugb)         // �Ƿ��г̲���Ҫֹͣ�������ͣ
          {
            flag_stop = TRUE;
            Flag_Motor.Open_Margin = TRUE;                // ���г̽��޵��־
            no_hand_drag_cnt = JOURNEY_NO_HAND_DRAG;
          
            Flag_Motor.Down_In_Journey_Place = TRUE;
            if (Flag_Motor.Up_In_Journey_Place)
            {
              Flag_Motor.In_Journey_Place = TRUE;
            }
            //if (!(Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Run_Direction))
            //{
              //Meet_plug_place_Buffer = NULL;                         /*�ߵ��г�������λ��*/
            //}
          }
        }
       }
      }
      else 
      {
        Flag_Motor.Open_Margin = NULL;                             /*�忪�г̽��޵��־*/
        if (Flag_Motor.Shut_Margin)
        {
          if (Hall_Pulse_Amount_Cnt > (Target_Journey_Point + 8))/* �Ƿ��Ѿ����г�-*/
          {
            Flag_Motor.Shut_Margin = NULL;                        /*����г̽��޵��־*/
          }
          else
          {
            flag_stop = TRUE;
          }
        }
        if (Hall_Pulse_Amount_Cnt < (Target_Journey_Point + space)) /* �Ƿ�ӽ��г� --*/
        {
          if (!Motor_Array[0].Motor_Flag.To_Journey_Meet_Plugb)         // �Ƿ��г̲���Ҫֹͣ�������ͣ
          {
            flag_stop = TRUE;
            Flag_Motor.Shut_Margin = TRUE;                           /* ���г̽��޵��־ */
            no_hand_drag_cnt = JOURNEY_NO_HAND_DRAG;
          
            Flag_Motor.Up_In_Journey_Place = TRUE;
            if (Flag_Motor.Down_In_Journey_Place)
            {
              Flag_Motor.In_Journey_Place = TRUE;
            }
            //if (!(Flag_Motor.Meet_Plugb_Direction ^ Motor_Array[0].Motor_Flag.Run_Direction))
            //{
              //Meet_plug_place_Buffer = NULL;                          /*�ߵ��г�������λ��*/
            //}
          }
        }
      }
    }
    hall_pulse_buf = Hall_Pulse_Amount_Cnt;
    
    // **** �ٶȿ��� ********************************************************
    
    if (Flag.Time_10ms)
    {
     Flag.Time_10ms = FALSE;
     if (!flag_stop)
     {
      if (motor_rum_time < (120 / MOTOR_CONTROL_TIME))
      {
        Motor_Duty = MIN_DUTY<<1;           // PWMռ�ձ���С
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
            Motor_Duty = MIN_DUTY<<1;           // PWMռ�ձ���С
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
          // �ٶȿ���
          En = Target_Speed - Motor_Speed;         // �õ���ǰ�ٶ���� E(n)
          En1 = Target_Speed - Motor_Speed_Buff;   // �õ��ϴ��ٶ���� E(n-1)
          En += En1;
          // �����ת��������
          if (En >= 0)               // ��ǰ�����Ƿ�Ϊ����
          {
           flag = 1;                // 
          }
          else                       // ����Ϊ����
          {
            En = 0 - En;             // ת��������
            flag = 0;
          }
          
          if (flag == 0)                     // ����Ϊ����
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
          else                              // ����Ϊ����
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
          
          if ((Motor_Speed == 0) || (Motor_Speed >= 200))    /* ת���쳣 ---*/
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
            //�Ƿ���δ�����г�
            if (!Motor_Array[0].Motor_Flag.Journey_Set_OK)
            {
              Target_Speed = 60;   //δ�����г̣�ת��ǿ��Ϊ60ת����������ͣ
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
              Flag_Motor.Run_OK = TRUE;                       /* ������ɱ�־ ----*/
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
          
          if (Motor_Speed == 0)    // ����״̬��ת��
          {
            if (++NoSpeed_Meet_Plug_Cnt >= NO_SPEED_TIME)
            {
              // �ٶȵ���10 ���ҳ���3��
              NoSpeed_Meet_Plug_Cnt = NULL;
              flag_stop = TRUE;
              //meet_plug_stop = TRUE; //�����г�
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
      if (Motor_Array[0].Motor_Flag.No_Slow_Start)      // �Ƿ�û��������
      {
        Target_Speed = Motor_Max_Speed;  // һֱΪ���ת��
      }
      if (motor_rum_time < NO_SPEED_TIME)
      {
        motor_rum_time++;
      }
      else if (!Flag_Motor.Runed)
      {
        flag_stop = TRUE;
        //meet_plug_stop = TRUE; //�����г�
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
        Flag_Motor.Meet_Plugb = TRUE;   // �������־
        Flag_Motor.Meet_Plugb_Direction = Motor_Array[0].Motor_Flag.Run_Direction;
        Meet_plug_place = Hall_Pulse_Amount_Cnt;  // ��������λ��
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
      reversal_cnt = 0;      // ��ת����ʱ��
      runed_Latime_cnt = 0;  // ��ת�ӳ�ʱ��
      motor_runed_cnt = 0;   // ��ת����
    }
    else
    {
      // // �Ƿ�Ϊ������ ����Ϊ����(��) (���Ϊ��) 
      if ((Motor_Array[0].Motor_Flag.Serpentine_Curtain) || (!(Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Direction)))
      {
        MeetPlugb();                          // ��������ǰ�����г�
      }
      
      if (Motor_SHUT_Cnt < 2500)
      {
        Motor_SHUT_Cnt++;
      }
      else
      {
        if ((Flag_Motor.Meet_Plugb) && (!Flag_Motor.Adj_Limit_Point)) //�Ƿ���΢���г�״̬
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
              // ���ͷû�ж� �����ٷ�ת
              flag_rebound=0;
              Flag_Motor.Runed = NULL;
              motor_runed_cnt = NULL;
              reversal_cnt = NULL;
              Motor_SHUT_Cnt = NULL;
            }
            /*else if (!(Motor_Array[0].Motor_Flag.Run_Direction ^ Motor_Array[0].Motor_Flag.Direction)) // �Ƿ�Ϊ����(��) (���Ϊ��)
            {
                Flag_Motor.Runed = NULL;
                motor_runed_cnt = NULL;
                reversal_cnt = NULL;
                Motor_SHUT_Cnt = NULL;
            }*/
            else if (++motor_runed_cnt >= 10) // �����ת10��
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
              runed_Latime_cnt = NULL; // û�г�ʱ������ת
            }
            else
            {
              Motor_Duty = 0;
              SetStopPWM();
              if (++runed_Latime_cnt > 250) // ��ת�ȴ�ʱ�� 500mS
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
        
          // ΢���г�ʱ��������������
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
                // ������������־
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
                DisposalPercent(PercentBuff);//���ðٷֱ�ʱ���û���г̣����Զ�����һ���趨�г̣�Ȼ���ܵ��ٷֱ�λ��
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
  ������:   
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
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
        
        if (!Motor_Array[0].Motor_Flag.Manual_Control_Synchro)  // �Ƿ���Ҫ�ֿ�ͬ��
        {
          Key_Out_Status = Motor_Array[0].Motor_Status_Buffer; 
          Flag.Key_Signal_Out = TRUE;            // ����ͬ������ź�
        }
        
        if ((Motor_Array[0].Motor_Flag.Journey_Set_OK) || (Motor_Array[0].Motor_Flag.Single_Journey))
        {
          // ȷ��Ŀ��λ��
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