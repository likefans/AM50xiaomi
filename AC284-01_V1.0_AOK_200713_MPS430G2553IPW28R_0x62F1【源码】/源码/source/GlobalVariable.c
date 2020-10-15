/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *
 *  GlobalVariable.c file
 *  ��������: ȫ�ֱ�������
**********************************************************************************/
#include "Define.h" /* ȫ�ֺ궨�� ------*/
/* ȫ��λ�������� ---------------------------------------------------------------*/
struct
{
    u8 Time_200us : 1; /* 200usʱ���־ ---*/
    u8 Time_2ms : 1;   /* 2msʱ���־ -----*/
    u8 Time_4ms : 1;   /* 10msʱ���־ ----*/
    u8 Time_10ms : 1;  /* 10msʱ���־ ----*/
    u8 Time_50ms : 1;  /* 50msʱ���־ ----*/
    u8 Time_100ms : 1; /* 100msʱ���־ ---*/
    u8 Time_200ms : 1; /* 200msʱ���־ ---*/
    u8 Time_500ms : 1; /* 500msʱ���־ ---*/
    u8 Time_1s : 1;    /* 1sʱ���־ ------*/

    u8 Learn_Key_ON : 1;    /* ѧϰ������ ------*/
    u8 Electrify_Reset : 1; /* �ϵ縴λ��־ ----*/

    u8 Uart_Read_OK : 1;  /* ���ڽ���OK��־ --*/
    u8 Uart_rx_OK : 1;    /* ���ڽ���OK��־ --*/
    u8 Uart_tx : 1;       /* ���������ͱ�־ --*/
    u8 Uart_Disposal : 1; /* �������ݴ����־ */
    u8 Uart_ACK : 1;
    u8 Goto_PCT : 1; /* �����־ --------*/

    u8 High_VOL_Protect : 1;
    u8 Low_VOL_Protect : 1;

    u8 VUart_Read_OK : 1;
    u8 VUart_Tx : 1;
    u8 NetStatus : 1;
    u8 NetEvent : 1;
    u8 Vuart_Status : 1;
    u8 Vuart_Raise_Edge : 1;
    u8 Key_Signal_Out : 1;
    u8 AutoReport : 1;
    u8 AutoOpenCloseDone : 1; //�Զ�����
    u8 Learn_Ok : 1;
    u8 Uart_Answer : 1;

    u8 Power_on : 1; //�ϵ��־
    u8 WifiTest : 1;
    u8 polling : 1;                    /* ��ѯ --------*/
    u8 Reporting : 1;                  /* �ϱ� --------*/
    u8 model : 1;                      /* model --------*/
    u8 model_Stat_Direction : 1;       /* �����תת̬�ϱ� --------*/
    u8 modle_Stat_Percentage : 1;      /* ��������ٷֱ�ת̬�ϱ� --------*/
    u8 modle_automatic_Percentage : 1; /* �Զ�����ٷֱ�ת̬�ϱ� --------*/
    u8 Wifi_cnt : 1;                   /* ��ʱ --------*/
    u8 Wifi_cnt2 : 1;                  /* ��ʱ2 --------*/
    u8 VERSION : 1;                    /* �汾�� --------*/
} Flag;

struct
{
    u8 Hand_Drag_Status : 1; /* ����״̬��־ ----*/
    u8 Hand_Drag_Startup : 1;
    u8 Hand_Drag : 1;           /* ������־ --------*/
    u8 Hand_Drag_Direction : 1; /* ���������־ ----*/

    u8 Adj_Limit_Point : 1;        /* ΢����λ���־ --*/
    u8 Adj_Limit_Point_Status : 1; /* ΢����λ��״̬ --*/

    u8 Rebound : 1;
    u8 Rebound_Direction : 1;
    u8 Meet_Plugb : 1;
    u8 Meet_Plugb_Direction : 1;

    u8 Hall_Pulse_edge : 1; /* ����������ر�־ */

    u8 Runed : 1;
    u8 Wait_Set_Journey : 1;
    u8 Shut_OK : 1;
    u8 Run_OK : 1;
    u8 slowstop : 1;

    u8 Shut_Margin : 1;
    u8 Open_Margin : 1;
    u8 Down_In_Journey_Place : 1;
    u8 Up_In_Journey_Place : 1;
    u8 In_Journey_Place : 1;

    u8 Save_Speed : 1;

    u8 over_limit_run : 1;
    u8 over_min_save : 1;
    u8 over_max_save : 1;

    u8 PercentUsual : 1;
} Flag_Motor;

struct
{
    u8 Signal_Status : 1; /* RF��ƽ״̬��־ --*/
    u8 Signal_Head : 1;   /* RF�ź�ͷ��־ ----*/
    u8 Raise_Edge : 1;    /* RF�����ر�־ ----*/
    u8 Rx_Data : 1;       /* RF�������ݱ�־ --*/
    u8 Rx_Finish : 1;     /* RF�������ݱ�־ --*/
    u8 Data_Disposal : 1; /* RF���ݴ����־ --*/
} Flag_RF;

/* ȫ�ֱ������� -----------------------------------------------------------------*/
u8 Time_4ms_Cnt = TIME_4MS_VALUE;
u8 Time_10ms_Cnt = TIME_10MS_VALUE;
u8 Time_50ms_Cnt = TIME_50MS_VALUE;
u8 Time_100ms_Cnt = TIME_100MS_VALUE;
u8 Time_200ms_Cnt = TIME_100MS_VALUE;
u8 Time_500ms_Cnt = TIME_500MS_VALUE;
u8 Time_1s_Cnt = TIME_1S_VALUE;
u8 Time_Power_On;
u8 Time_Fail_Test;

u8 Uart_Byte_Cnt; // �ֽڼ�����
u8 WiFi_Cnt;

u8 test_data;
u8 learn_key_cnt;
u8 No_learn_key_cnt;
u8 RF_High_Cnt; /* RF�ߵ�ƽʱ�� ----*/
u8 RF_Low_Cnt;  /* RF�׵�ƽʱ�� ----*/
u8 Current_Voltage;
u8 Current_Temperature;
u16 Current_Current_mA;

u8 Opretion_Time_Cnt;
u8 Learn_Time_Cnt;
u8 Set_Speed_Time_Cnt;
u8 Set_Limit_Point_Cnt;

Led_Display_TypeDef LED_Status;

u16 Motor_Duty;
u8 Motor_Speed; /* ���ת�� --------*/
u8 Target_Speed;

Motor_Control_TypeDef Motor_Status; /* ���״̬ --------*/

u8 Motor_Max_Speed = MAX_SPEED_100;
u16 Hall_Pulse_Width; /* ������������� */
u16 Hall_Pulse_Amount;
__no_init u16 Hall_Pulse_Amount_Cnt; /* �������������� */
u16 hall_pulse_buf;
u8 Dry_Contact_Control_Mode;
Uart_Baud_Rate_TypeDef Uart_Baud_Rate = BR2400;

__no_init u8 Hall_Status_Buffer;
u8 hall_change_cnt;
u16 Hall_Time_Cnt;

u8 Uart_ID;
u8 Uart_CH_L;
u8 Uart_CH_H;

Motor Motor_Array[MOTOR_NUM];

u16 Hall_Pulse_Amount_PCT = HALL_PULSE_MAX >> 1; /* �������������� */
Motor_Control_TypeDef Key_Out_Status;

u8 Room_Card_Buf;
u8 Room_Card_Cnt;

u16 over_min_cnt = HALL_PULSE_MAX;
u16 over_max_cnt = HALL_PULSE_MIN;
__no_init u16 RAM_PSWD;
u8 flag_yuzu_once, flag_rebound;

u8 ProductType = 1; //������
u8 HardWareVersion[6];

u8 ProductPara[MAX_PRODUCT_PARA_BYTES] = {'A', 'M', '5', '0', '-', '1', '.', '2', '/', '1', '0', '0', '-', 'E', 'M', '-', 'P', 'W'};