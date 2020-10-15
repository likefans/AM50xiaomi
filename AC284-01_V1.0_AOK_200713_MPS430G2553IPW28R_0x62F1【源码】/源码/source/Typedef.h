

typedef signed long  s32;
typedef signed short s16;
typedef signed char  s8;

typedef signed long  const 	sc32;            /* Read Only */
typedef signed short const 	sc16;            /* Read Only */
typedef signed char  const  sc8;             /* Read Only */

typedef volatile signed long  vs32;
typedef volatile signed short vs16;
typedef volatile signed char  vs8;

//typedef volatile signed long  const  vsc32;  /* Read Only */
//ypedef volatile signed short const  vsc16;  /* Read Only */
//typedef volatile signed char  const  vsc8;   /* Read Only */

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef unsigned long  const  uc32;           /* Read Only */
typedef unsigned short const  uc16;           /* Read Only */
typedef unsigned char  const  c8;  	          /* Read Only */

typedef volatile unsigned long   vu32;
typedef volatile unsigned short  vu16;
typedef volatile unsigned char   vu8;


//typedef volatile unsigned long  const  vuc32; /* Read Only */
//typedef volatile unsigned short  const vuc16; /* Read Only */
//typedef volatile unsigned char  const  vuc8;  /* Read Only */

typedef struct
{
  u8 vol_status                : 1;
  u8 edge                      : 1;
  u8 edge_change               : 1;
  u8 const HALL;
  u8 sample_value;
  u8 sample_cnt;
  u16 time_cnt;
}Hall;

typedef enum
{
  FALSE = 0,
  TRUE = !FALSE
}bool;
typedef enum
{
  RESET = 0,
  SET = !RESET
}FlagStatus, ITStatus, BitStatus;
typedef enum 
{
  DISABLE = 0,
  ENABLE = !DISABLE
}FunctionalState;

typedef union
{
  u8 Flag_Byte;
  struct
  {
    u8 b0 :1;
    u8 b1 :1;
    u8 b2 :1;
    u8 b3 :1;
    u8 b4 :1;
    u8 b5 :1;
    u8 b6 :1;
    u8 b7 :1;
  }Flag_Bit;
}Bit_TypeDef;

typedef enum {
  RF_HEAD                      = (u8)0xA3,    /* RFͷ�� ----------*/
  RF_UP                        = (u8)0x0b,    /* RF�������� ------*/
  RF_STOP                      = (u8)0x23,    /* RFֹͣ���� ------*/
  RF_STOP_2S                   = (u8)0x5a,    /* RFֹͣ����2S ----*/
  RF_JIGGLE_STOP               = (u8)0x24,    /* RF�㶯ͣ���� ----*/
  RF_DOWN                      = (u8)0x43,    /* RF�������� ------*/
  RF_LEARN                     = (u8)0x53,    /* RFѧϰ���� ------*/
  RF_CLEAR                     = (u8)0x2b,    /* RF������� ------*/
  RF_AWOKEN                    = (u8)0x50,    /* RF�������� ------*/
  RF_UP_DOWN_1S                = (u8)0x4a,    /*  ------*/
  RF_UP_DOWN_5S                = (u8)0x3a,    /*  ------*/
  RF_UP_STOP                   = (u8)0x1a,    /*  ------*/
  RF_DOWN_STOP                 = (u8)0x2a,    /*  ------*/
  RF_ACCESS_LEARN              = (u8)0x55,    /*  ------*/
  
} RF_Command_TypeDef;

#define LED_HINT_NUM           14
typedef void (*LedHint) (void);
typedef enum {
  LED_OFF                      = (u8)0x00,
  SET_SPEED_STATUS             = (u8)0x01,
  SPEED_HINT                   = (u8)0x02,
  LEARN_STATUS                 = (u8)0x03,
  DELETE_STATUS                = (u8)0x04,
  OPRATION_FINISH              = (u8)0x05,
  DELETE_FINISH                = (u8)0x06,
  CLR_NET_STATUS               = (u8)0x07,
  CLR_NET_FINISHI              = (u8)0x08,
  ADD_NET_FINISHI              = (u8)0x09,
  BLUE_LED_HINT                = (u8)0x0A,
  
  OVERTEMPERATURE_PROTECT      = (u8)0x0B,
  OVERVOLTAGE_PROTECT          = (u8)0x0C,
  UNDERVOLTAGE_PROTECT         = (u8)0x0D,
} Led_Display_TypeDef;

typedef enum {
  LED_INDICATE     = (u8)0x00,
  BUZZER_INDICATE  = (u8)0x01,
  INCHING_INDICATE = (u8)0x02,
}Protest_type;

typedef enum {
  NORMAL                       = (u8)0x00,                   /* ����ģʽ --------*/
  STXT_LOOP                    = (u8)0x01,                   /* ��ͣ��ͣѭ�� ----*/
  SST_XXT_SXQH                 = (u8)0x02,                   /* ����ͣ����ͣ --- */
  SONG_SHOU_TING               = (u8)0x03,                   /* ����ͣ ----------*/
  REN_YI_JIAN_TING             = (u8)0x04,                   /* ����ͣ ----------*/
} Dry_Contact_TypeDef;

#define BAUD_RATE_NUM          5
typedef enum {
  BR1200                       = (u8)0x00,
  BR2400                       = (u8)0x01,
  BR4800                       = (u8)0x02,
  BR9600                       = (u8)0x03,
  BR19200                      = (u8)0x04,
} Uart_Baud_Rate_TypeDef;

typedef enum {
  MOTOR_STOP                   = (u8)0x00,                   /* ֹͣ ------------*/
  MOTOR_UP                     = (u8)0x01,                   /* ���� ------------*/
  MOTOR_DOWN                   = (u8)0x02,                   /* ���� ------------*/
  MOTOR_UP_INCHING             = (u8)0x04,                   /* �㶯�� ----------*/
  MOTOR_DOWN_INCHING           = (u8)0x08,                   /* �㶯�� ----------*/
} Motor_Control_TypeDef;

typedef struct 
{
  u8 Direction                 : 1;                          /* ��������־ ----*/
  u8 Run_Status                : 1;                          /* �������״̬ ----*/
  u8 Run_Direction             : 1;                          /* ������з����־ */
  u8 Fact_Direction            : 1;                          /* ���ʵ�ʷ����־ */
  
  u8 Inching_Status            : 1;                          /* �㶯״̬��־ ----*/
  u8 StatusSwitch              : 1;                          /* ״̬�л��ӳٱ�־ */
  u8 StatusSwitch_Delay        : 1;                          /* ״̬�л��ӳٱ�־ */
  
  u8 Single_Journey            : 1;                          /* �����г̱�־ ----*/
  u8 Journey_Set_OK            : 1;                          /* �г��趨OK��־ --*/
  u8 Journey_Direction         : 1;                          /* �г̷����־ ----*/  
  
  u8 SaveEE                    : 1;                          /* ת�ٲ�����־ ----*/
  
  u8 Hand_Drag_Status          : 1;                          /* ����״̬��־ ----*/
  u8 No_Slow_Start             : 1;                           /* ����������־ ---*/
  u8 Serpentine_Curtain        : 1;                           /* ��������־ -----*/
  u8 To_Journey_Meet_Plugb     : 1;                           /* ���г������־ -*/
  u8 PowerDown_Remember        : 1;                           /* �ϵ�����־ ---*/
  u8 Manual_Control_Synchro    : 1;                           /* �ֿ�ͬ����־ ---*/
  
}Flag_M;
typedef void (*Motor_Diver) (void);
typedef struct
{
  Flag_M Motor_Flag;
  Motor_Control_TypeDef  Motor_Status_Buffer;
  u16 RunTime_Cnt;
  Motor_Diver Motor_Diver_Array[3];
}Motor;


typedef enum {
  START                         = (u8)0x00,
  DATA                          = (u8)0x01,
  STOP                          = (u8)0x02,
  END                           = (u8)0x03
} VUart_Event_TypeDef;
typedef struct
{
  u8 Read_Write                 : 1;   // ��д��־
  u8 Start_RX                   : 1;   // ��ʼ���ձ�־
  u8 Bit_Data                   : 1;   // ����λ
	
  u8 Bit_Time_Cnt;                     // BITʱ�������
  u8 Bit_High_Cnt;                     // �ߵ�ƽ������
  u8 RW_Bit_Cnt;                       // BIT������  
  u8 RW_Byte_Cnt;                      // BYTE������
  u8 RXTime_Cnt;                       // ����ʱ�������
	
  VUart_Event_TypeDef RW_Event;        // ��д�¼�
  u8 *Data_Buffer;                     // ���ݻ���
	
} VUart_TypeDef;

typedef struct
{
  u8 Control_Finish            : 1;
  u8 const VALUE;
  u8 Old_Value;
  u8 In_Cnt;
  u8 N0_In_Cnt;
  
  Motor_Control_TypeDef  Control_Command;
}Power_In_TypeDef;