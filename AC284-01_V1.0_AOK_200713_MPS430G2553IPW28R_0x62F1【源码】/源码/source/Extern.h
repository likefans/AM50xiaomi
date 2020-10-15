/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *
 *  Extern.h file
 *  功能描述: 外部变量、函数声明
**********************************************************************************/
#ifndef Extern_h
#define Extern_h

/* 全局位变量声明 -----------------------------------------------------------*/
extern struct
{
    u8 Time_200us : 1; /* 200us时间标志 ---*/
    u8 Time_2ms : 1;   /* 2ms时间标志 -----*/
    u8 Time_4ms : 1;   /* 4ms时间标志 ----*/
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

    u8 Power_on : 1;                   //上电标志
    u8 WifiTest : 1;                   //厂测
    u8 polling : 1;                    /* 轮询 --------*/
    u8 Reporting : 1;                  /* 上报 --------*/
    u8 model : 1;                      /* model --------*/
    u8 model_Stat_Direction : 1;       /* 电机反转转态上报 --------*/
    u8 modle_Stat_Percentage : 1;      /* 电机百分比转态上报 --------*/
    u8 modle_automatic_Percentage : 1; /* 自动电机百分比转态上报 --------*/
    u8 Wifi_cnt : 1;                   /* 记时 --------*/
    u8 Wifi_cnt2 : 1;                  /* 记时 --------*/
    u8 VERSION : 1;                    /* 记时 --------*/
} Flag;

extern struct
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

extern struct
{
    u8 Signal_Status : 1; /* RF电平状态标志 --*/
    u8 Signal_Head : 1;   /* RF信号头标志 ----*/
    u8 Raise_Edge : 1;    /* RF上升沿标志 ----*/
    u8 Rx_Data : 1;       /* RF接收数据标志 --*/
    u8 Rx_Finish : 1;     /* RF接收数据标志 --*/
    u8 Data_Disposal : 1; /* RF数据处理标志 --*/

} Flag_RF;

/* 全局变量声明 ---------------------------------------------------------------*/
extern u8 Time_4ms_Cnt;
extern u8 Time_10ms_Cnt;
extern u8 Time_50ms_Cnt;
extern u8 Time_100ms_Cnt;
extern u8 Time_200ms_Cnt;
extern u8 Time_500ms_Cnt;
extern u8 Time_1s_Cnt;
extern u8 Time_Power_On;
extern u8 Time_Fail_Test;

extern u8 Uart_Byte_Cnt; // 字节计数器
extern u8 WiFi_Cnt;

extern u8 test_data;
extern u8 learn_key_cnt;
extern u8 No_learn_key_cnt;
extern u8 RF_High_Cnt; /* RF高电平时间 ----*/
extern u8 RF_Low_Cnt;  /* RF底电平时间 ----*/
extern u8 Current_Voltage;
extern u8 Current_Temperature;
extern u16 Current_Current_mA;

extern u8 Opretion_Time_Cnt;
extern u8 Learn_Time_Cnt;
extern u8 Set_Speed_Time_Cnt;
extern u8 Set_Limit_Point_Cnt;

extern Led_Display_TypeDef LED_Status;

extern u16 Motor_Duty;
extern u8 Motor_Speed; /* 电机转速 --------*/
extern u8 Target_Speed;
extern Motor_Control_TypeDef Motor_Status; /* 电机状态 --------*/
extern u16 Hall_Pulse_Amount_PCT;          /* 霍尔输出脉冲计数 */

extern u8 Motor_Max_Speed;
extern u16 Hall_Pulse_Width; /* 霍尔输出脉冲宽度 */
extern u16 Hall_Pulse_Amount;
extern __no_init u16 Hall_Pulse_Amount_Cnt; /* 霍尔输出脉冲计数 */
extern u16 hall_pulse_buf;
extern u8 Dry_Contact_Control_Mode;
extern Uart_Baud_Rate_TypeDef Uart_Baud_Rate;

extern u8 Uart_ID;
extern u8 Uart_CH_L;
extern u8 Uart_CH_H;
extern Motor Motor_Array[MOTOR_NUM];
extern u8 PercentBuff;

extern __no_init u8 Hall_Status_Buffer;
extern u8 hall_change_cnt;
extern u16 Hall_Time_Cnt;

extern Motor_Control_TypeDef Key_Out_Status;

extern u8 Room_Card_Buf;
extern u8 Room_Card_Cnt;

extern u16 over_min_cnt;
extern u16 over_max_cnt;
extern __no_init u16 RAM_PSWD;
extern u8 flag_yuzu_once, flag_rebound;

extern u8 Status_Buffer;
/* 函数声明 -------------------------------------------------------------------*/
extern void Initialize(void); /* 初始化函数 ------*/
extern void GPIOSetting(void);
extern void CLKSetting(void);
extern void TIMXSetting(void);
extern void Delay(register unsigned int i);
extern void KeyManage(void);        /* 按键扫描及处理 --*/
extern void LearnDelCodeTime(void); /* 学码删码时间控制 */
extern void SystemTime(void);

extern void VUartDataDisposal(void);
extern void VUartStartupTX(void);
extern void VUartRX(void);
extern void VUartTX(void);
extern void VUartInit(void);

// ADC 处理
extern void ADCSetting(void);
extern void ADSamping(void); /* AD采样函数 ------*/
extern void VoltageProtect(void);
// RF 函数
extern void RFStatusInspect(void);
extern u8 RFSignalDecode(u16 High_Time, u16 Low_Time); /* RF解码函数 ------*/
extern u8 RFDataVerify(void);                          /* RF数据校验 ------*/
extern void RFDataDisposal(void);                      /* RF处理函数 ------*/
// ID 处理函数
extern u8 CompID(u8 array[]);                      /* 比较是否有相同ID */
extern void SaveID(u8 array[]);                    /* 保存新的ID ------*/
extern void DeleteAllID(u8 check);                 /* 删除所有的ID ----*/
extern void DeleteChannel(u8 array[], u8 ch_addr); /* 删除通道 --------*/
extern void SaveChannel(u8 array[], u8 ch_addr);   /* 保存通道 --------*/
// Flash 读写函数
extern void FlashReadData(void);
extern void FlashWriteData(u8 check);
extern void FlashWritePlace(void);
extern void FlashReadPlace(void);
// 电机控制函数
extern void Duty_Add(u16 *p, u8 i);
extern void Duty_Sub(u16 *p, u8 i);
extern void SetStopPWM(void);
extern void SetUpPWM(u16 duty);
extern void SetDownPWM(u16 duty);
extern void MotorRun(void);
extern void MotorControl(Motor_Control_TypeDef *status);

extern void MotorUp(void);
extern void MotorDown(void);
extern void MotorStop(void);

//extern u8 DataCRC (u8 array[],u8 amount);
extern u8 DataCRC(u8 array[]);
extern u8 JourneyPercentCount(void);
extern u16 CountPercentPlace(u8 percent);

// 行程 处理
extern void JourneyDelete(void);
extern void JourneyCheck(void);
extern void DisposalPercent(u8 percent);
// Uart 处理函数
extern void LinUartInit(void);
extern void UartDataDisposal(void);
extern void UartStartupTX(void);
extern void UartControl(u8 data);

// LED 指示
extern void LedStatusUpdate(Led_Display_TypeDef status);
extern void LedDisplay(void);

extern void DetectPowerIn(u8 hall_num);
extern void PowerInScan(void);
extern void TemperatureDetect(void);

extern void PowerDownSave(void);

extern u8 HardWareVersion[6];
extern u8 ProductType;
extern u8 ProductPara[MAX_PRODUCT_PARA_BYTES];

/*
  产测函数
  */
extern void Enter_test(u8 *RF_data, u8 exe_conditon);
extern void Under_Test(Protest_type indicateType);
extern void Test_FAIL(Protest_type indicateType);
extern void Test_Succeed(Protest_type indicateType);
extern u8 Out_test(u8 out_time);
#endif
