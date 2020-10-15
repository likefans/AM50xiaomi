/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *
 *  Signal_Sampling.c file
 *  功能描述: 信号采样
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define RF_MAX_BYTE_VALUE      8                             // 接收事件最大值
#define DATA_BIT_AMOUNT        8                             // 数据BIT数量
#define AD_TIME                RF_DECODE_TIME * 3
#define DIRECTION_AMOUNT       4
#define RF_NO_RX_TIME          250 / TIME_50MS_VALUE
// 信号头最大值50us*3*106=16MS
#define HEAD_LONG_MAX          ((6500 + ((AD_TIME) / 2)) / (AD_TIME))
#define HEAD_LONG_MIN          ((3500 + ((AD_TIME) / 2)) / (AD_TIME))
#define HEAD_SHORT_MAX         ((1000 + ((AD_TIME) / 2)) / (AD_TIME))
#define HEAD_SHORT_MIN         ((400  + ((AD_TIME) / 2)) / (AD_TIME))
#define SIGNAL_LONG_MAX        ((1000 + ((AD_TIME) / 2)) / (AD_TIME))
#define SIGNAL_LONG_MIN        ((450  + ((AD_TIME) / 2)) / (AD_TIME))
#define SIGNAL_SHORT_MAX       ((400  + ((AD_TIME) / 2)) / (AD_TIME))
#define SIGNAL_SHORT_MIN       ((120  + ((AD_TIME) / 2)) / (AD_TIME))
#define SIGNAL_BIT_MAX         ((1400 + ((AD_TIME) / 2)) / (AD_TIME))
#define SIGNAL_BIT_MIN         ((450  + ((AD_TIME) / 2)) / (AD_TIME))
/* 内部变量 ---------------------------------------------------------------------*/
u8 RF_Bit_cnt;                                               /* Bit计数器 -------*/
u8 RF_Byte_Cnt;                                              /* Byte计数器 ------*/
u8 RF_KeyUnloosen_Cnt;                                       /* 松键计数器 ------*/
u8 RF_RxData[RF_MAX_BYTE_VALUE];                             /* RF接收数据缓存 --*/
u8 * Rx_Data_P;                                              /* RF数据接收指针 --*/
u8 RF_NO_Rx_Cnt;
/* 内部函数 ---------------------------------------------------------------------*/
u8 RFDataVerify (void);
void ErrorDisposal (void);
void RFDataDisposal (void);
/*********************************************************************************
  函数名:   
  功  能:   
*********************************************************************************/
void RFStatusInspect (void)
{
  if (Flag_RF.Rx_Finish)
  {
    Flag_RF.Rx_Finish = NULL;
    RF_NO_Rx_Cnt = NULL;
  }
  else
  {
    if (++RF_NO_Rx_Cnt >= RF_NO_RX_TIME)
    {
      RF_NO_Rx_Cnt = NULL;
      Flag_RF.Data_Disposal = NULL;
    }
  }
}
/**********************************************************************************
  函数名:    ErrorDisposal
  功  能:   清除接收寄存器
**********************************************************************************/
void ErrorDisposal (void)
{
    RF_Bit_cnt = NULL;                                       /* 清BIT计数器 -----*/
    RF_Byte_Cnt = NULL;
    Flag_RF.Signal_Head = NULL;                              /* RF信号头标志---- */
}
/**********************************************************************************
  函数名:   RFDataVerify
  功  能:   效验收到的数据是否正确
  输  入:   空
  输  出:   空
  返  回:   数据是否正确标志
  描  述：  
**********************************************************************************/
u8 RFDataVerify (void)
{
    u8 flag = NULL;
    u8 i;
    
    if (RF_RxData[0] == RF_HEAD)                        /* 头码是否正确 ----*/
    {
        i = RF_RxData[1] + RF_RxData[2] + RF_RxData[3] 
            + RF_RxData[4] + RF_RxData[5] + RF_RxData[6];
        
        if (RF_RxData[7] == i)                               /* 校验和是否相等 --*/
        {
            flag = TRUE;   
        }
    }
  
  return (flag);
}
/**********************************************************************************
  函数名:  	RFDataDisposal
  功  能:   RF数据处理
  输  入:   空
  输  出:   空    
  返  回:	空
  描  述：  
**********************************************************************************/
void RFDataDisposal (void)
{
  u8 ch_addr;
  
  if ((!Flag_RF.Data_Disposal) && (!Flag.Learn_Key_ON) && (LED_Status <= LEARN_STATUS))
  {
    ch_addr = CompID(RF_RxData);                             /* 比较ID ----------*/
    Enter_test(RF_RxData,Flag.Power_on);
    /* 是否有相同的ID和通道，并且不在删码状态 -----------------------------------*/
    if ((ch_addr & 0x80))
    {
      Flag_RF.Rx_Finish = TRUE;
      Flag_Motor.PercentUsual = NULL;
      Flag.Goto_PCT = NULL;
      /* 是在学习状态 ----*/
      if (LED_Status == LEARN_STATUS)
      {
        Flag_RF.Data_Disposal = TRUE;
        switch (RF_RxData[6])                                /* 是什么命令 ------*/
        {
          case RF_UP: // 上行 学码（已有ID，学通道）
            LedStatusUpdate(OPRATION_FINISH);
//            Flag.Uart_tx = 1;
//            Flag.Learn_Ok  = 1;
            SaveChannel(RF_RxData,ch_addr);
          break;
          case RF_STOP: // 停止 换向
            Motor_Array[0].Motor_Flag.Direction = !Motor_Array[0].Motor_Flag.Direction;
            Flag.model_Stat_Direction = TRUE;
            LedStatusUpdate(OPRATION_FINISH);
            FlashWriteData(0xee);
          break;
          case RF_DOWN: // 下行 删单码
            LedStatusUpdate(OPRATION_FINISH);
            DeleteChannel(RF_RxData,ch_addr);
          break;
          case RF_LEARN: // 学码
            Learn_Time_Cnt = NULL;
          break;
          case RF_CLEAR: //  删全码
            LedStatusUpdate(DELETE_FINISH);
            DeleteAllID(0xee);
          break;
        }
      }
      else                                                   /* 不在学习状态 ----*/
      {
        Set_Limit_Point_Cnt = NULL;
        if (Flag.Goto_PCT)
        {
          Flag.Goto_PCT = NULL;
          Motor_Array[0].Motor_Flag.StatusSwitch = TRUE;  //更新目标位置
        }
        switch (RF_RxData[6])                                /* 是什么命令 -------*/
        {
          case RF_UP: // 上行
            Motor_Status = MOTOR_UP;
          break;
          case RF_STOP: // 停止
            Motor_Status = MOTOR_STOP;
          break;
          case RF_DOWN: // 下行
            Motor_Status = MOTOR_DOWN;
          break;
          case RF_AWOKEN: //  唤醒
            LedStatusUpdate(LEARN_STATUS);
            Motor_Status = MOTOR_STOP;
          break;
          
          /*case RF_LEARN: //  进入限位点设置状态
          if (!Flag_RF.Data_Disposal)
          {
            if ((Motor_Array[0].Motor_Flag.Journey_Set_OK) || (Motor_Array[0].Motor_Flag.Single_Journey))   // 是否已经有行程了
            {
              Flag_RF.Data_Disposal = TRUE;
            
              if (Motor_Status != MOTOR_STOP)
              {
                Motor_Status = MOTOR_STOP;
                Motor_Array[0].Motor_Flag.Run_Status = FALSE;
                break;
              }
              
              if (Flag_Motor.Adj_Limit_Point)                                 // 是否已经在微调状态
              {
                if (Flag_Motor.Adj_Limit_Point_Status)                       // 微调哪个行程
                {
                  Hall_Pulse_Amount = Hall_Pulse_Amount_Cnt;
                }
                else
                {
                  Hall_Pulse_Amount += MIN_JOURNEY_VALUE - Hall_Pulse_Amount_Cnt;
                  Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;
                }
                Flag_Motor.Meet_Plugb = FALSE;      // 清遇阻标志
                Flag_Motor.Adj_Limit_Point = FALSE; // 退出微调状态
                LedStatusUpdate(OPRATION_FINISH);   // LED指示
                JourneyCheck();                     // 校验行程
                FlashWriteData(0xee);                   // 保存行程
              }
              else
              {
                if ((Hall_Pulse_Amount_Cnt > (MIN_JOURNEY_VALUE - 40))         // 当前是否在最小行程位置 左右必须留一定的范围
                && (Hall_Pulse_Amount_Cnt < (MIN_JOURNEY_VALUE + 40)))
                {
                  LedStatusUpdate(OPRATION_FINISH);
                  Flag_Motor.Adj_Limit_Point = TRUE;
                  Flag_Motor.Adj_Limit_Point_Status = FALSE;
                }
                else if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount - 40))   // 当前是否在最大行程位置
                && (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount + 40)))
                {
                  LedStatusUpdate(OPRATION_FINISH);
                  Flag_Motor.Adj_Limit_Point = TRUE;
                  Flag_Motor.Adj_Limit_Point_Status = TRUE;
                }
              }
            }
          }
          break;*/
        }
      }
    }
    else
    {
      if (LED_Status == LEARN_STATUS)                     /* 是否在学习状态 --*/
      {
        if (RF_RxData[6] == RF_UP)                      /* 是否上行键 ------*/
        {
          Flag_RF.Data_Disposal = TRUE;
          LedStatusUpdate(OPRATION_FINISH);
          if (NULL != ch_addr)                          /* 是否有相同的ID --*/
          {
            SaveChannel(RF_RxData,ch_addr);
          }
          else
          {
            SaveID(RF_RxData);                          /* 学习ID ----------*/
          }
//          Flag.Uart_tx = 1;
//          Flag.Learn_Ok  = 1;
        }
      }
    }
  }
}
/**********************************************************************************
  函数名:  	RFSignalDecode
  功  能:   RF信号解码
  输  入:   空
  输  出:   空
  返  回:	  是否收到正确数据标志
  描  述：  
**********************************************************************************/
u8 RFSignalDecode (u16 High_Time,u16 Low_Time)
{
  u8 flag = NULL;
  u8 temp = High_Time + Low_Time;

  if (!Flag_RF.Signal_Head)                                  /* 是否没有信号头 --*/
  {
    //if ((Low_Time <= HEAD_LONG_MAX) && (Low_Time >= HEAD_LONG_MIN))
    if ((High_Time <= HEAD_LONG_MAX) && (High_Time >= HEAD_LONG_MIN))
    {
    //  if ((High_Time <= HEAD_SHORT_MAX) && (High_Time >= HEAD_SHORT_MIN))
      if ((Low_Time <= HEAD_SHORT_MAX) && (Low_Time >= HEAD_SHORT_MIN))
      {
        Flag_RF.Signal_Head = TRUE;                          /* 取信号头标志 ----*/
        Rx_Data_P = RF_RxData;                               /* 接收指针复位 ----*/
        RF_Bit_cnt = NULL;                                   /* 清BIT计数器 -----*/
        RF_Byte_Cnt = NULL;
      }
    }
  }
  else                                                       /* 已有信号头 ------*/
  {
    if ((temp <= SIGNAL_BIT_MAX) && (temp >= SIGNAL_BIT_MIN))
    {
      if ((High_Time >= SIGNAL_SHORT_MIN) && (Low_Time >= SIGNAL_SHORT_MIN))
      {
        *Rx_Data_P <<= 1;
        if (High_Time > Low_Time)
        {
          *Rx_Data_P |= 0x01;                                /* 接收1bit --------*/
        }
        if (++RF_Bit_cnt >= DATA_BIT_AMOUNT)                 /* 一个字节是否收完 */
        {
          RF_Bit_cnt = NULL;
          ++Rx_Data_P;                             
          if (++RF_Byte_Cnt >= RF_MAX_BYTE_VALUE)            /* 数据是否接收完成 */
          {
            flag = TRUE;                                     /* 置处理标志 ------*/
            RF_KeyUnloosen_Cnt = NULL;	                     /* 清松键计数器 ----*/
            RF_Byte_Cnt = NULL;
            ErrorDisposal();
          }
        }
      }
      else
      {
        ErrorDisposal();
      }
    } 
    else
    {
      ErrorDisposal();
    }    
  }
  return (flag);
}