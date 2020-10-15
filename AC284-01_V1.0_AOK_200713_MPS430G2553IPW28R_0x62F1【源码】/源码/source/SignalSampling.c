/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *
 *  Signal_Sampling.c file
 *  ��������: �źŲ���
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define RF_MAX_BYTE_VALUE      8                             // �����¼����ֵ
#define DATA_BIT_AMOUNT        8                             // ����BIT����
#define AD_TIME                RF_DECODE_TIME * 3
#define DIRECTION_AMOUNT       4
#define RF_NO_RX_TIME          250 / TIME_50MS_VALUE
// �ź�ͷ���ֵ50us*3*106=16MS
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
/* �ڲ����� ---------------------------------------------------------------------*/
u8 RF_Bit_cnt;                                               /* Bit������ -------*/
u8 RF_Byte_Cnt;                                              /* Byte������ ------*/
u8 RF_KeyUnloosen_Cnt;                                       /* �ɼ������� ------*/
u8 RF_RxData[RF_MAX_BYTE_VALUE];                             /* RF�������ݻ��� --*/
u8 * Rx_Data_P;                                              /* RF���ݽ���ָ�� --*/
u8 RF_NO_Rx_Cnt;
/* �ڲ����� ---------------------------------------------------------------------*/
u8 RFDataVerify (void);
void ErrorDisposal (void);
void RFDataDisposal (void);
/*********************************************************************************
  ������:   
  ��  ��:   
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
  ������:    ErrorDisposal
  ��  ��:   ������ռĴ���
**********************************************************************************/
void ErrorDisposal (void)
{
    RF_Bit_cnt = NULL;                                       /* ��BIT������ -----*/
    RF_Byte_Cnt = NULL;
    Flag_RF.Signal_Head = NULL;                              /* RF�ź�ͷ��־---- */
}
/**********************************************************************************
  ������:   RFDataVerify
  ��  ��:   Ч���յ��������Ƿ���ȷ
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   �����Ƿ���ȷ��־
  ��  ����  
**********************************************************************************/
u8 RFDataVerify (void)
{
    u8 flag = NULL;
    u8 i;
    
    if (RF_RxData[0] == RF_HEAD)                        /* ͷ���Ƿ���ȷ ----*/
    {
        i = RF_RxData[1] + RF_RxData[2] + RF_RxData[3] 
            + RF_RxData[4] + RF_RxData[5] + RF_RxData[6];
        
        if (RF_RxData[7] == i)                               /* У����Ƿ���� --*/
        {
            flag = TRUE;   
        }
    }
  
  return (flag);
}
/**********************************************************************************
  ������:  	RFDataDisposal
  ��  ��:   RF���ݴ���
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:	��
  ��  ����  
**********************************************************************************/
void RFDataDisposal (void)
{
  u8 ch_addr;
  
  if ((!Flag_RF.Data_Disposal) && (!Flag.Learn_Key_ON) && (LED_Status <= LEARN_STATUS))
  {
    ch_addr = CompID(RF_RxData);                             /* �Ƚ�ID ----------*/
    Enter_test(RF_RxData,Flag.Power_on);
    /* �Ƿ�����ͬ��ID��ͨ�������Ҳ���ɾ��״̬ -----------------------------------*/
    if ((ch_addr & 0x80))
    {
      Flag_RF.Rx_Finish = TRUE;
      Flag_Motor.PercentUsual = NULL;
      Flag.Goto_PCT = NULL;
      /* ����ѧϰ״̬ ----*/
      if (LED_Status == LEARN_STATUS)
      {
        Flag_RF.Data_Disposal = TRUE;
        switch (RF_RxData[6])                                /* ��ʲô���� ------*/
        {
          case RF_UP: // ���� ѧ�루����ID��ѧͨ����
            LedStatusUpdate(OPRATION_FINISH);
//            Flag.Uart_tx = 1;
//            Flag.Learn_Ok  = 1;
            SaveChannel(RF_RxData,ch_addr);
          break;
          case RF_STOP: // ֹͣ ����
            Motor_Array[0].Motor_Flag.Direction = !Motor_Array[0].Motor_Flag.Direction;
            Flag.model_Stat_Direction = TRUE;
            LedStatusUpdate(OPRATION_FINISH);
            FlashWriteData(0xee);
          break;
          case RF_DOWN: // ���� ɾ����
            LedStatusUpdate(OPRATION_FINISH);
            DeleteChannel(RF_RxData,ch_addr);
          break;
          case RF_LEARN: // ѧ��
            Learn_Time_Cnt = NULL;
          break;
          case RF_CLEAR: //  ɾȫ��
            LedStatusUpdate(DELETE_FINISH);
            DeleteAllID(0xee);
          break;
        }
      }
      else                                                   /* ����ѧϰ״̬ ----*/
      {
        Set_Limit_Point_Cnt = NULL;
        if (Flag.Goto_PCT)
        {
          Flag.Goto_PCT = NULL;
          Motor_Array[0].Motor_Flag.StatusSwitch = TRUE;  //����Ŀ��λ��
        }
        switch (RF_RxData[6])                                /* ��ʲô���� -------*/
        {
          case RF_UP: // ����
            Motor_Status = MOTOR_UP;
          break;
          case RF_STOP: // ֹͣ
            Motor_Status = MOTOR_STOP;
          break;
          case RF_DOWN: // ����
            Motor_Status = MOTOR_DOWN;
          break;
          case RF_AWOKEN: //  ����
            LedStatusUpdate(LEARN_STATUS);
            Motor_Status = MOTOR_STOP;
          break;
          
          /*case RF_LEARN: //  ������λ������״̬
          if (!Flag_RF.Data_Disposal)
          {
            if ((Motor_Array[0].Motor_Flag.Journey_Set_OK) || (Motor_Array[0].Motor_Flag.Single_Journey))   // �Ƿ��Ѿ����г���
            {
              Flag_RF.Data_Disposal = TRUE;
            
              if (Motor_Status != MOTOR_STOP)
              {
                Motor_Status = MOTOR_STOP;
                Motor_Array[0].Motor_Flag.Run_Status = FALSE;
                break;
              }
              
              if (Flag_Motor.Adj_Limit_Point)                                 // �Ƿ��Ѿ���΢��״̬
              {
                if (Flag_Motor.Adj_Limit_Point_Status)                       // ΢���ĸ��г�
                {
                  Hall_Pulse_Amount = Hall_Pulse_Amount_Cnt;
                }
                else
                {
                  Hall_Pulse_Amount += MIN_JOURNEY_VALUE - Hall_Pulse_Amount_Cnt;
                  Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;
                }
                Flag_Motor.Meet_Plugb = FALSE;      // �������־
                Flag_Motor.Adj_Limit_Point = FALSE; // �˳�΢��״̬
                LedStatusUpdate(OPRATION_FINISH);   // LEDָʾ
                JourneyCheck();                     // У���г�
                FlashWriteData(0xee);                   // �����г�
              }
              else
              {
                if ((Hall_Pulse_Amount_Cnt > (MIN_JOURNEY_VALUE - 40))         // ��ǰ�Ƿ�����С�г�λ�� ���ұ�����һ���ķ�Χ
                && (Hall_Pulse_Amount_Cnt < (MIN_JOURNEY_VALUE + 40)))
                {
                  LedStatusUpdate(OPRATION_FINISH);
                  Flag_Motor.Adj_Limit_Point = TRUE;
                  Flag_Motor.Adj_Limit_Point_Status = FALSE;
                }
                else if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount - 40))   // ��ǰ�Ƿ�������г�λ��
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
      if (LED_Status == LEARN_STATUS)                     /* �Ƿ���ѧϰ״̬ --*/
      {
        if (RF_RxData[6] == RF_UP)                      /* �Ƿ����м� ------*/
        {
          Flag_RF.Data_Disposal = TRUE;
          LedStatusUpdate(OPRATION_FINISH);
          if (NULL != ch_addr)                          /* �Ƿ�����ͬ��ID --*/
          {
            SaveChannel(RF_RxData,ch_addr);
          }
          else
          {
            SaveID(RF_RxData);                          /* ѧϰID ----------*/
          }
//          Flag.Uart_tx = 1;
//          Flag.Learn_Ok  = 1;
        }
      }
    }
  }
}
/**********************************************************************************
  ������:  	RFSignalDecode
  ��  ��:   RF�źŽ���
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:	  �Ƿ��յ���ȷ���ݱ�־
  ��  ����  
**********************************************************************************/
u8 RFSignalDecode (u16 High_Time,u16 Low_Time)
{
  u8 flag = NULL;
  u8 temp = High_Time + Low_Time;

  if (!Flag_RF.Signal_Head)                                  /* �Ƿ�û���ź�ͷ --*/
  {
    //if ((Low_Time <= HEAD_LONG_MAX) && (Low_Time >= HEAD_LONG_MIN))
    if ((High_Time <= HEAD_LONG_MAX) && (High_Time >= HEAD_LONG_MIN))
    {
    //  if ((High_Time <= HEAD_SHORT_MAX) && (High_Time >= HEAD_SHORT_MIN))
      if ((Low_Time <= HEAD_SHORT_MAX) && (Low_Time >= HEAD_SHORT_MIN))
      {
        Flag_RF.Signal_Head = TRUE;                          /* ȡ�ź�ͷ��־ ----*/
        Rx_Data_P = RF_RxData;                               /* ����ָ�븴λ ----*/
        RF_Bit_cnt = NULL;                                   /* ��BIT������ -----*/
        RF_Byte_Cnt = NULL;
      }
    }
  }
  else                                                       /* �����ź�ͷ ------*/
  {
    if ((temp <= SIGNAL_BIT_MAX) && (temp >= SIGNAL_BIT_MIN))
    {
      if ((High_Time >= SIGNAL_SHORT_MIN) && (Low_Time >= SIGNAL_SHORT_MIN))
      {
        *Rx_Data_P <<= 1;
        if (High_Time > Low_Time)
        {
          *Rx_Data_P |= 0x01;                                /* ����1bit --------*/
        }
        if (++RF_Bit_cnt >= DATA_BIT_AMOUNT)                 /* һ���ֽ��Ƿ����� */
        {
          RF_Bit_cnt = NULL;
          ++Rx_Data_P;                             
          if (++RF_Byte_Cnt >= RF_MAX_BYTE_VALUE)            /* �����Ƿ������� */
          {
            flag = TRUE;                                     /* �ô����־ ------*/
            RF_KeyUnloosen_Cnt = NULL;	                     /* ���ɼ������� ----*/
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