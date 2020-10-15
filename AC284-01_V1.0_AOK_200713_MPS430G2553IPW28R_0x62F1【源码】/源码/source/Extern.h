/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *
 *  Extern.h file
 *  ��������: �ⲿ��������������
**********************************************************************************/
#ifndef Extern_h
#define Extern_h

/* ȫ��λ�������� -----------------------------------------------------------*/
extern struct
{
    u8 Time_200us : 1; /* 200usʱ���־ ---*/
    u8 Time_2ms : 1;   /* 2msʱ���־ -----*/
    u8 Time_4ms : 1;   /* 4msʱ���־ ----*/
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

    u8 Power_on : 1;                   //�ϵ��־
    u8 WifiTest : 1;                   //����
    u8 polling : 1;                    /* ��ѯ --------*/
    u8 Reporting : 1;                  /* �ϱ� --------*/
    u8 model : 1;                      /* model --------*/
    u8 model_Stat_Direction : 1;       /* �����תת̬�ϱ� --------*/
    u8 modle_Stat_Percentage : 1;      /* ����ٷֱ�ת̬�ϱ� --------*/
    u8 modle_automatic_Percentage : 1; /* �Զ�����ٷֱ�ת̬�ϱ� --------*/
    u8 Wifi_cnt : 1;                   /* ��ʱ --------*/
    u8 Wifi_cnt2 : 1;                  /* ��ʱ --------*/
    u8 VERSION : 1;                    /* ��ʱ --------*/
} Flag;

extern struct
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

extern struct
{
    u8 Signal_Status : 1; /* RF��ƽ״̬��־ --*/
    u8 Signal_Head : 1;   /* RF�ź�ͷ��־ ----*/
    u8 Raise_Edge : 1;    /* RF�����ر�־ ----*/
    u8 Rx_Data : 1;       /* RF�������ݱ�־ --*/
    u8 Rx_Finish : 1;     /* RF�������ݱ�־ --*/
    u8 Data_Disposal : 1; /* RF���ݴ����־ --*/

} Flag_RF;

/* ȫ�ֱ������� ---------------------------------------------------------------*/
extern u8 Time_4ms_Cnt;
extern u8 Time_10ms_Cnt;
extern u8 Time_50ms_Cnt;
extern u8 Time_100ms_Cnt;
extern u8 Time_200ms_Cnt;
extern u8 Time_500ms_Cnt;
extern u8 Time_1s_Cnt;
extern u8 Time_Power_On;
extern u8 Time_Fail_Test;

extern u8 Uart_Byte_Cnt; // �ֽڼ�����
extern u8 WiFi_Cnt;

extern u8 test_data;
extern u8 learn_key_cnt;
extern u8 No_learn_key_cnt;
extern u8 RF_High_Cnt; /* RF�ߵ�ƽʱ�� ----*/
extern u8 RF_Low_Cnt;  /* RF�׵�ƽʱ�� ----*/
extern u8 Current_Voltage;
extern u8 Current_Temperature;
extern u16 Current_Current_mA;

extern u8 Opretion_Time_Cnt;
extern u8 Learn_Time_Cnt;
extern u8 Set_Speed_Time_Cnt;
extern u8 Set_Limit_Point_Cnt;

extern Led_Display_TypeDef LED_Status;

extern u16 Motor_Duty;
extern u8 Motor_Speed; /* ���ת�� --------*/
extern u8 Target_Speed;
extern Motor_Control_TypeDef Motor_Status; /* ���״̬ --------*/
extern u16 Hall_Pulse_Amount_PCT;          /* �������������� */

extern u8 Motor_Max_Speed;
extern u16 Hall_Pulse_Width; /* ������������� */
extern u16 Hall_Pulse_Amount;
extern __no_init u16 Hall_Pulse_Amount_Cnt; /* �������������� */
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
/* �������� -------------------------------------------------------------------*/
extern void Initialize(void); /* ��ʼ������ ------*/
extern void GPIOSetting(void);
extern void CLKSetting(void);
extern void TIMXSetting(void);
extern void Delay(register unsigned int i);
extern void KeyManage(void);        /* ����ɨ�輰���� --*/
extern void LearnDelCodeTime(void); /* ѧ��ɾ��ʱ����� */
extern void SystemTime(void);

extern void VUartDataDisposal(void);
extern void VUartStartupTX(void);
extern void VUartRX(void);
extern void VUartTX(void);
extern void VUartInit(void);

// ADC ����
extern void ADCSetting(void);
extern void ADSamping(void); /* AD�������� ------*/
extern void VoltageProtect(void);
// RF ����
extern void RFStatusInspect(void);
extern u8 RFSignalDecode(u16 High_Time, u16 Low_Time); /* RF���뺯�� ------*/
extern u8 RFDataVerify(void);                          /* RF����У�� ------*/
extern void RFDataDisposal(void);                      /* RF������ ------*/
// ID ������
extern u8 CompID(u8 array[]);                      /* �Ƚ��Ƿ�����ͬID */
extern void SaveID(u8 array[]);                    /* �����µ�ID ------*/
extern void DeleteAllID(u8 check);                 /* ɾ�����е�ID ----*/
extern void DeleteChannel(u8 array[], u8 ch_addr); /* ɾ��ͨ�� --------*/
extern void SaveChannel(u8 array[], u8 ch_addr);   /* ����ͨ�� --------*/
// Flash ��д����
extern void FlashReadData(void);
extern void FlashWriteData(u8 check);
extern void FlashWritePlace(void);
extern void FlashReadPlace(void);
// ������ƺ���
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

// �г� ����
extern void JourneyDelete(void);
extern void JourneyCheck(void);
extern void DisposalPercent(u8 percent);
// Uart ������
extern void LinUartInit(void);
extern void UartDataDisposal(void);
extern void UartStartupTX(void);
extern void UartControl(u8 data);

// LED ָʾ
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
  ���⺯��
  */
extern void Enter_test(u8 *RF_data, u8 exe_conditon);
extern void Under_Test(Protest_type indicateType);
extern void Test_FAIL(Protest_type indicateType);
extern void Test_Succeed(Protest_type indicateType);
extern u8 Out_test(u8 out_time);
#endif
