/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *  
 *  Samping.c file
 *  功能描述: AD及电机速度采样
**********************************************************************************/
#include "Define.h" /* 全局宏定义 ------*/
#include "Extern.h" /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define VOLTAGE_RECKON(ad_val) (((ad_val) / 28) + ((ad_val) / 55)) //((((ad_val)*60)) / 28)    //31
#define CURRENT_RECKON(ad_val) ((u16)((ad_val) << 4))              //  电流放大4倍

/* 内部变量 ---------------------------------------------------------------------*/
static u8 ADC_VOL_Cnt;
static u16 ADC_VOL_Buffer;

static u8 ADC_CUR_Cnt;
static u16 ADC_CUR_Buffer;
static u16 ADC_CUR_Array[4];
static u8 ADC_CUR_Array_Index;
/* 内部函数 ---------------------------------------------------------------------*/
void VoltageProtect(void);
/**********************************************************************************
  函数名:   ADCSetting
  功  能:   ADC 设置
**********************************************************************************/
void ADCSetting(void)
{
    P1SEL |= BIT4 + BIT5;
    //ADC10CTL0 = ADC10SHT_2 + ADC10ON;              // 8 * ADC10CLKs  ADC10ON  vcc

    ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;
    //ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON;   // 1.5

    ADC10CTL1 = INCH_5; // input A5
    ADC10AE0 |= BIT5;   // P1.5 ADC option select

    ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
}
/**********************************************************************************
  函数名:   SampleAverage
  功  能:   AD值求平均
  输  入:   数组，数组的大小
  输  出:   空
  返  回:   平均值
  描  述：  去最大和最小值，中值求平均
 *********************************************************************************/
u16 SampleAverage(u16 array[], u8 num)
{
    u8 max; //应该为u16
    u8 min; //应该为u16
    u8 i;
    u8 j;
    u16 value;

    j = 0;
    max = 0;
    min = 0;
    value = 0;

    for (i = num - 1; i > 0; i--)
    {
        j++;
        if (array[j] > array[max])
        {
            max = j;
        }
        if (array[j] < array[min])
        {
            min = j;
        }
    }
    for (i = NULL; i < num; i++)
    {
        if ((i != max) && (i != min))
        {
            value += array[i];
        }
    }

    if (max == min)
    {
        value = array[max];
    }
    else
    {
        value = value / (num - 2);
    }
    return (value);
}
/**********************************************************************************
  函数名:   ADSamping
  功  能:   AD采样
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：  
 *********************************************************************************/
void ADSamping(void)
{
    u16 buffer;

    if (!(ADC10CTL1 & ADC10BUSY))
    {
        buffer = ADC10MEM;

        if (ADC10CTL1 == INCH_5) // 电压
        {
            ADC_VOL_Buffer += buffer;
            ADC_VOL_Cnt++;
            if (ADC_VOL_Cnt >> 4)
            {
                Current_Voltage = VOLTAGE_RECKON(ADC_VOL_Buffer >> 4);
                ADC_VOL_Buffer = NULL;
                ADC_VOL_Cnt = NULL;
            }
            ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;
            ADC10CTL1 = INCH_4; // input A7
            ADC10AE0 |= BIT4;
        }
        else if (ADC10CTL1 == INCH_4) // 电流
        {
            ADC_CUR_Buffer += buffer;
            ADC_CUR_Cnt++;
            if (ADC_CUR_Cnt >> 2)
            {
                ADC_CUR_Cnt = NULL;
                ADC_CUR_Array[ADC_CUR_Array_Index] = ADC_CUR_Buffer >> 2;
                ADC_CUR_Buffer = NULL;

                ADC_CUR_Array_Index++;
                if (ADC_CUR_Array_Index >> 2)
                {
                    u16 temp;
                    temp = SampleAverage(ADC_CUR_Array, ADC_CUR_Array_Index);

                    Current_Current_mA = CURRENT_RECKON(temp);
                    ADC_CUR_Array_Index = NULL;

                    if (Current_Current_mA > MAX_OUT_CURRENT)
                    {
                        Duty_Sub(&Motor_Duty, ((Current_Current_mA - MEET_PLUG_CURRENT) >> 3));
                        //Duty_Sub(&Motor_Duty,50);
                        if (Motor_Array[0].Motor_Flag.Run_Direction)
                        {
                            SetUpPWM(Motor_Duty);
                        }
                        else
                        {
                            SetDownPWM(Motor_Duty);
                        }
                    }
                }
            }
            ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;
            ADC10CTL1 = INCH_5; // input A6
            ADC10AE0 |= BIT5;   // P1.6 ADC option select
        }

        ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
    }
}
/**********************************************************************************
  函数名:   VoltageProtect
  功  能:   
 *********************************************************************************/
void PowerDownSave(void)
{
    if ((Motor_Array[0].Motor_Flag.SaveEE) && ((Motor_Array[0].Motor_Flag.Journey_Set_OK) || (Motor_Array[0].Motor_Flag.Single_Journey)))
    {
        if (Motor_Array[0].Motor_Flag.PowerDown_Remember) // 断电是否需要记忆
        {
            FlashWritePlace();
        }
    }
    Motor_Array[0].Motor_Flag.SaveEE = NULL;
}
/**********************************************************************************
  函数名:   VoltageProtect
  功  能:   
 *********************************************************************************/
void VoltageProtect(void)
{
#define DETECT_VOL_TIME 2

    static u16 High_VOL_PROTECT_Cnt;
    static u16 VOL_OK_Cnt;
    static u16 PowerDown_Time_Cnt;

    if (Current_Voltage < 3)
    {
        VOL_OK_Cnt = NULL;
        High_VOL_PROTECT_Cnt = NULL;
        SetStopPWM();
        Motor_Status = MOTOR_STOP;
        //Flag_Motor.Runed = FALSE;
        Motor_Array[0].Motor_Flag.Run_Status = NULL;

        //Flag_Motor.Up_In_Journey_Place = FALSE;
        //Flag_Motor.Down_In_Journey_Place = FALSE;
        //Flag_Motor.In_Journey_Place = FALSE;
        //flag_yuzu_once=0;

        if (LED_Status != LED_OFF)
        {
            LedStatusUpdate(LED_OFF); // 欠压保护提示
        }
    }
    else if (Current_Voltage < 18)
    {
        High_VOL_PROTECT_Cnt = NULL;
        VOL_OK_Cnt = NULL;

        SetStopPWM();
        Motor_Status = MOTOR_STOP;
        //Flag_Motor.Runed = FALSE;
        Motor_Array[0].Motor_Flag.Run_Status = NULL;

        //Flag_Motor.Up_In_Journey_Place = FALSE;
        //Flag_Motor.Down_In_Journey_Place = FALSE;
        //Flag_Motor.In_Journey_Place = FALSE;
        //flag_yuzu_once=0;

        if (LED_Status != UNDERVOLTAGE_PROTECT)
        {
            LedStatusUpdate(UNDERVOLTAGE_PROTECT); // 欠压保护提示
        }

        Flag.Low_VOL_Protect = TRUE;
    }
    else if (Current_Voltage > 28)
    {
        if (++High_VOL_PROTECT_Cnt > (2000 / DETECT_VOL_TIME)) // 2S
        {
            High_VOL_PROTECT_Cnt = NULL;
            VOL_OK_Cnt = NULL;
            SetStopPWM();
            Motor_Status = MOTOR_STOP;
            Flag.High_VOL_Protect = TRUE;

            if (LED_Status != OVERVOLTAGE_PROTECT)
            {
                LedStatusUpdate(OVERVOLTAGE_PROTECT); // 过压保护提示
            }
        }
    }
    else //if ((Current_Voltage >= 20) && (Current_Voltage <= 26))
    {
        if (++VOL_OK_Cnt > (500 / DETECT_VOL_TIME)) // 2S
        {
            VOL_OK_Cnt = NULL;
            Flag.High_VOL_Protect = NULL;

            if (LED_Status >= OVERVOLTAGE_PROTECT)
            {
                LedStatusUpdate(LED_OFF); // 过压保护提示
            }

            High_VOL_PROTECT_Cnt = NULL;

            if (Flag.Low_VOL_Protect)
            {
                RF_ENABLE();
            }
            Flag.Low_VOL_Protect = NULL;
        }
    }

    if (Flag.Low_VOL_Protect)
    {
        RF_DISABLE();

        if (PowerDown_Time_Cnt == (20 / DETECT_VOL_TIME)) // 200MS
        {
            PowerDownSave();
        }
        /*else if (PowerDown_Time_Cnt == (500 / DETECT_VOL_TIME))   // 500MS
    {
      PowerDownSave();
    }
    else if (PowerDown_Time_Cnt == (1000 / DETECT_VOL_TIME))   // 1S
    {
      PowerDownSave();
    }*/

        if (PowerDown_Time_Cnt < 60000)
        {
            PowerDown_Time_Cnt++;
        }
    }
    else
    {
        PowerDown_Time_Cnt = NULL;
    }
}
