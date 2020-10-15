/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *  功能描述:    
 *  芯片选型:    MSP430G2533
 *  振荡频率:    内部 RC12.0M
 *  Watchdog:    Enable
 *  MAIN.C file  
**********************************************************************************/
#include "Define.h" /* 全局宏定义 ------*/
#include "Extern.h" /* 全局变量及函数 --*/
/**********************************************************************************
  函数名:  main
  功  能:  
**********************************************************************************/
int main(void)
{

    Initialize();
    while (1)
    {
        WDTCTL = WDT_ARST_16;

        /* 2ms ----------------------------------------------------------------------*/
        if (Flag.Time_2ms)
        {
            Flag.Time_2ms = FALSE;
            WDTCTL = WDT_ARST_16;
            SystemTime();
            ADSamping();
            if (Flag.Electrify_Reset)
            {
                VoltageProtect();
            }

            if (Motor_Array[0].Motor_Flag.Journey_Set_OK && Flag_Motor.In_Journey_Place)
            {
                if (Hall_Pulse_Amount_Cnt < MIN_JOURNEY_VALUE - 1)
                {
                    if (over_min_cnt > Hall_Pulse_Amount_Cnt)
                    {
                        over_min_cnt = Hall_Pulse_Amount_Cnt;
                        Flag_Motor.over_min_save = 1;
                    }
                }
                else if (Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount + 1)
                {
                    if (over_max_cnt < Hall_Pulse_Amount_Cnt)
                    {
                        over_max_cnt = Hall_Pulse_Amount_Cnt;
                        Flag_Motor.over_max_save = 1;
                    }
                }
                else
                    Flag_Motor.over_limit_run = 0;
            }

            if (!Flag.Low_VOL_Protect)
            {
                if (Motor_Status == MOTOR_STOP && Motor_Status != Status_Buffer)
                {
                    Flag.modle_automatic_Percentage = TRUE;
                }
                Status_Buffer = Motor_Status;
                KeyManage();
                MotorRun();

                /* 串口发送 ---------------------------------------------------------------*/
                if (Flag.Uart_tx)
                {
                    Flag.Uart_tx = FALSE;
                    UartStartupTX();
                }
                /* 串口接收 ---------------------------------------------------------------*/
                if (Flag.Uart_Read_OK)
                {
                    UartDataDisposal();
                    Flag.Uart_Read_OK = FALSE;
                }
            }
        }

        /* 100ms --------------------------------------------------------------------*/
        if (Flag.Time_100ms)
        {
            Flag.Time_100ms = NULL;
            LedDisplay();

            if (Motor_Status == MOTOR_STOP)
            {
                Flag_Motor.over_limit_run = 0;
                // 电机停止后才保存转速
                if (Flag_Motor.Save_Speed)
                {
                    Flag_Motor.Save_Speed = 0;
                    FlashWriteData(0xee); // 保存转速
                }

                if (Motor_Speed == 0 && !Flag_Motor.Meet_Plugb)
                {
                    if (Flag_Motor.over_min_save)
                    {
                        if (Hall_Pulse_Amount_Cnt > over_min_cnt + 2)
                        {
                            Hall_Pulse_Amount += (MIN_JOURNEY_VALUE + 1 - over_min_cnt);
                            Hall_Pulse_Amount_Cnt += (MIN_JOURNEY_VALUE + 1 - over_min_cnt);
                            over_min_cnt = MIN_JOURNEY_VALUE;
                            Flag_Motor.over_min_save = 0;
                            //hall_pulse_buf = Hall_Pulse_Amount_Cnt;
                            FlashWriteData(0xee);
                            //LedStatusUpdate(OPRATION_FINISH);
                        }
                    }
                    else if (Flag_Motor.over_max_save)
                    {
                        if (Hall_Pulse_Amount_Cnt < over_max_cnt - 2)
                        {
                            Hall_Pulse_Amount = over_max_cnt + 1;
                            Flag_Motor.over_max_save = 0;
                            //hall_pulse_buf = Hall_Pulse_Amount_Cnt;
                            FlashWriteData(0xee);
                            //LedStatusUpdate(OPRATION_FINISH);
                        }
                    }
                }
            }
        }

        if (!Flag.Low_VOL_Protect)
        {
            /* RF解码 -------------------------------------------------------------------*/
            if (Flag_RF.Raise_Edge) /* 是否收到RF数据 --*/
            {
                Flag_RF.Raise_Edge = FALSE;

                if (RFSignalDecode(RF_High_Cnt, RF_Low_Cnt)) /* RF解码 ----------*/
                {
                    if (RFDataVerify()) /* 数据是否正确 ----*/
                    {
                        RFDataDisposal(); /* RF数据处理 ------*/
                    }
                }
            }
            /* 50ms ---------------------------------------------------------------------*/
            if (Flag.Time_50ms)
            {
                Flag.Time_50ms = FALSE;
                MotorControl(&Motor_Status); /* 电机控制 --------*/
                RFStatusInspect();
            }
            /* 200ms 轮询模块--------------------------------------------------------------------*/
            if (Flag.Time_200ms)
            {
                if (WiFi_Cnt-- == NULL)
                {
                    if (Flag.Wifi_cnt == TRUE)
                    {
                        Flag.Wifi_cnt = NULL;
                        Flag.model = TRUE;   //Flag.CLR_Net_Oder = TRUE;
                        Flag.Uart_tx = TRUE; // 串口发送清网命令提示
                        WiFi_Cnt = TIME_1S_WiFi;
                    }
                    else if (Flag.Wifi_cnt2 == TRUE)
                    {
                        Flag.Wifi_cnt2 = NULL;
                        Flag.VERSION = TRUE; //Flag.CLR_Net_Oder = TRUE;
                        Flag.Uart_tx = TRUE; // 串口发送清网命令提示
                        WiFi_Cnt = TIME_1S_WiFi;
                    }
                    else
                    {
                        WiFi_Cnt = TIME_1S_WiFi;
                        Flag.Reporting = TRUE; //上报电机位置
                        Flag.Uart_tx = TRUE;
                    }
                }
                else
                {
                    Flag.polling = TRUE; //获取模块下行数据
                    Flag.Uart_tx = TRUE;
                }
                Flag.Time_200ms = NULL;
            }
            /* 1s -----------------------------------------------------------------------*/
            if (Flag.Time_1s)
            {
                Flag.Time_1s = FALSE;
                Flag.Electrify_Reset = TRUE;
                LearnDelCodeTime();
            }
        }
    }
}