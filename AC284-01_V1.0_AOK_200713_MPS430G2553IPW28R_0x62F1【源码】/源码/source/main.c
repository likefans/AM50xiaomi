/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *  ��������:    
 *  оƬѡ��:    MSP430G2533
 *  ��Ƶ��:    �ڲ� RC12.0M
 *  Watchdog:    Enable
 *  MAIN.C file  
**********************************************************************************/
#include "Define.h" /* ȫ�ֺ궨�� ------*/
#include "Extern.h" /* ȫ�ֱ��������� --*/
/**********************************************************************************
  ������:  main
  ��  ��:  
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

                /* ���ڷ��� ---------------------------------------------------------------*/
                if (Flag.Uart_tx)
                {
                    Flag.Uart_tx = FALSE;
                    UartStartupTX();
                }
                /* ���ڽ��� ---------------------------------------------------------------*/
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
                // ���ֹͣ��ű���ת��
                if (Flag_Motor.Save_Speed)
                {
                    Flag_Motor.Save_Speed = 0;
                    FlashWriteData(0xee); // ����ת��
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
            /* RF���� -------------------------------------------------------------------*/
            if (Flag_RF.Raise_Edge) /* �Ƿ��յ�RF���� --*/
            {
                Flag_RF.Raise_Edge = FALSE;

                if (RFSignalDecode(RF_High_Cnt, RF_Low_Cnt)) /* RF���� ----------*/
                {
                    if (RFDataVerify()) /* �����Ƿ���ȷ ----*/
                    {
                        RFDataDisposal(); /* RF���ݴ��� ------*/
                    }
                }
            }
            /* 50ms ---------------------------------------------------------------------*/
            if (Flag.Time_50ms)
            {
                Flag.Time_50ms = FALSE;
                MotorControl(&Motor_Status); /* ������� --------*/
                RFStatusInspect();
            }
            /* 200ms ��ѯģ��--------------------------------------------------------------------*/
            if (Flag.Time_200ms)
            {
                if (WiFi_Cnt-- == NULL)
                {
                    if (Flag.Wifi_cnt == TRUE)
                    {
                        Flag.Wifi_cnt = NULL;
                        Flag.model = TRUE;   //Flag.CLR_Net_Oder = TRUE;
                        Flag.Uart_tx = TRUE; // ���ڷ�������������ʾ
                        WiFi_Cnt = TIME_1S_WiFi;
                    }
                    else if (Flag.Wifi_cnt2 == TRUE)
                    {
                        Flag.Wifi_cnt2 = NULL;
                        Flag.VERSION = TRUE; //Flag.CLR_Net_Oder = TRUE;
                        Flag.Uart_tx = TRUE; // ���ڷ�������������ʾ
                        WiFi_Cnt = TIME_1S_WiFi;
                    }
                    else
                    {
                        WiFi_Cnt = TIME_1S_WiFi;
                        Flag.Reporting = TRUE; //�ϱ����λ��
                        Flag.Uart_tx = TRUE;
                    }
                }
                else
                {
                    Flag.polling = TRUE; //��ȡģ����������
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