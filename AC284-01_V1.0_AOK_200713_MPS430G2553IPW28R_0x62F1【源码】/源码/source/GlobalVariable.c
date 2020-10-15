/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *
 *  GlobalVariable.c file
 *  功能描述: 全局变量定义
**********************************************************************************/
#include "Define.h" /* 全局宏定义 ------*/
/* 全局位变量定义 ---------------------------------------------------------------*/
struct
{
    u8 Time_200us : 1; /* 200us时间标志 ---*/
    u8 Time_2ms : 1;   /* 2ms时间标志 -----*/
    u8 Time_4ms : 1;   /* 10ms时间标志 ----*/
    u8 Time_10ms : 1;  /* 10ms时间标志 ----*/
    u8 Time_50ms : 1;  /* 50ms时间标志 ----*/
    u8 Time_100ms : 1; /* 100ms时间标志 ---*/
    u8 Time_200ms : 1; /* 200ms时间标志 ---*/
    u8 Time_500ms : 1; /* 500ms时间标志 ---*/
    u8 Time_1s : 1;    /* 1s时间标志 ------*/

    u8 Learn_Key_ON : 1;    /* 学习键按下 ------*/
    u8 Electrify_Reset : 1; /* 上电复位标志 ----*/

    u8 Uart_Read_OK : 1;  /* 串口接收OK标志 --*/
    u8 Uart_rx_OK : 1;    /* 串口接收OK标志 --*/
    u8 Uart_tx : 1;       /* 串口正发送标志 --*/
    u8 Uart_Disposal : 1; /* 串口数据处理标志 */
    u8 Uart_ACK : 1;
    u8 Goto_PCT : 1; /* 方向标志 --------*/

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
    u8 AutoOpenCloseDone : 1; //自动开合
    u8 Learn_Ok : 1;
    u8 Uart_Answer : 1;

    u8 Power_on : 1; //上电标志
    u8 WifiTest : 1;
    u8 polling : 1;                    /* 轮询 --------*/
    u8 Reporting : 1;                  /* 上报 --------*/
    u8 model : 1;                      /* model --------*/
    u8 model_Stat_Direction : 1;       /* 电机反转转态上报 --------*/
    u8 modle_Stat_Percentage : 1;      /* 反馈电机百分比转态上报 --------*/
    u8 modle_automatic_Percentage : 1; /* 自动电机百分比转态上报 --------*/
    u8 Wifi_cnt : 1;                   /* 记时 --------*/
    u8 Wifi_cnt2 : 1;                  /* 记时2 --------*/
    u8 VERSION : 1;                    /* 版本号 --------*/
} Flag;

struct
{
    u8 Hand_Drag_Status : 1; /* 手拉状态标志 ----*/
    u8 Hand_Drag_Startup : 1;
    u8 Hand_Drag : 1;           /* 手拉标志 --------*/
    u8 Hand_Drag_Direction : 1; /* 手拉方向标志 ----*/

    u8 Adj_Limit_Point : 1;        /* 微调限位点标志 --*/
    u8 Adj_Limit_Point_Status : 1; /* 微调限位点状态 --*/

    u8 Rebound : 1;
    u8 Rebound_Direction : 1;
    u8 Meet_Plugb : 1;
    u8 Meet_Plugb_Direction : 1;

    u8 Hall_Pulse_edge : 1; /* 霍尔脉冲边沿标志 */

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
    u8 Signal_Status : 1; /* RF电平状态标志 --*/
    u8 Signal_Head : 1;   /* RF信号头标志 ----*/
    u8 Raise_Edge : 1;    /* RF上升沿标志 ----*/
    u8 Rx_Data : 1;       /* RF接收数据标志 --*/
    u8 Rx_Finish : 1;     /* RF接收数据标志 --*/
    u8 Data_Disposal : 1; /* RF数据处理标志 --*/
} Flag_RF;

/* 全局变量定义 -----------------------------------------------------------------*/
u8 Time_4ms_Cnt = TIME_4MS_VALUE;
u8 Time_10ms_Cnt = TIME_10MS_VALUE;
u8 Time_50ms_Cnt = TIME_50MS_VALUE;
u8 Time_100ms_Cnt = TIME_100MS_VALUE;
u8 Time_200ms_Cnt = TIME_100MS_VALUE;
u8 Time_500ms_Cnt = TIME_500MS_VALUE;
u8 Time_1s_Cnt = TIME_1S_VALUE;
u8 Time_Power_On;
u8 Time_Fail_Test;

u8 Uart_Byte_Cnt; // 字节计数器
u8 WiFi_Cnt;

u8 test_data;
u8 learn_key_cnt;
u8 No_learn_key_cnt;
u8 RF_High_Cnt; /* RF高电平时间 ----*/
u8 RF_Low_Cnt;  /* RF底电平时间 ----*/
u8 Current_Voltage;
u8 Current_Temperature;
u16 Current_Current_mA;

u8 Opretion_Time_Cnt;
u8 Learn_Time_Cnt;
u8 Set_Speed_Time_Cnt;
u8 Set_Limit_Point_Cnt;

Led_Display_TypeDef LED_Status;

u16 Motor_Duty;
u8 Motor_Speed; /* 电机转速 --------*/
u8 Target_Speed;

Motor_Control_TypeDef Motor_Status; /* 电机状态 --------*/

u8 Motor_Max_Speed = MAX_SPEED_100;
u16 Hall_Pulse_Width; /* 霍尔输出脉冲宽度 */
u16 Hall_Pulse_Amount;
__no_init u16 Hall_Pulse_Amount_Cnt; /* 霍尔输出脉冲计数 */
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

u16 Hall_Pulse_Amount_PCT = HALL_PULSE_MAX >> 1; /* 霍尔输出脉冲计数 */
Motor_Control_TypeDef Key_Out_Status;

u8 Room_Card_Buf;
u8 Room_Card_Cnt;

u16 over_min_cnt = HALL_PULSE_MAX;
u16 over_max_cnt = HALL_PULSE_MIN;
__no_init u16 RAM_PSWD;
u8 flag_yuzu_once, flag_rebound;

u8 ProductType = 1; //开合帘
u8 HardWareVersion[6];

u8 ProductPara[MAX_PRODUCT_PARA_BYTES] = {'A', 'M', '5', '0', '-', '1', '.', '2', '/', '1', '0', '0', '-', 'E', 'M', '-', 'P', 'W'};