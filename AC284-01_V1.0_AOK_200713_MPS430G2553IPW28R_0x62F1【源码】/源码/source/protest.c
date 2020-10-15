#include "Define.h" /* 全局宏定义 ------*/
#include "Extern.h" /* 全局变量及函数 --*/

//typedef enum {
//  LED_INDICATE      (u8)0x01,
//  BUZZER_INDICATE   (u8)0x02,
//  INCHING_INDICATE  (u8)0x03,
//}Protest_type;
/**********************************************************************************
  函数名:   
  功  能:   进入产测
  输  入:   空
  输  出:   空    
  返  回:	空
  描  述：  
**********************************************************************************/
void Enter_test(u8 *RF_data, u8 exe_conditon)
{
    if (exe_conditon) //确定执行
    {
        {
            if (RF_LEARN == RF_data[6]) //背面键
            {
                Flag_RF.Data_Disposal = TRUE;
                Time_Power_On = 0;

                No_learn_key_cnt = 0;
                if (++learn_key_cnt >= 3) //按键三次
                {
                    learn_key_cnt = 0;
                    Flag.Power_on = 0;
                    Time_Power_On = 0;
                    Flag.WifiTest = 1; //开始测试wifi
                    Flag.Uart_tx = 1;
                }
            }
        }
    }
}
/**********************************************************************************
  函数名:   
  功  能:   退出产测
  输  入:   空
  输  出:   空    
  返  回:	空
  描  述：  
**********************************************************************************/
u8 Out_test(u8 out_time)
{
    if (out_time != 0)
    {
        out_time--;
    }
    return out_time;
}
/**********************************************************************************
  函数名:   
  功  能:   产测中，指示灯快闪（蜂鸣器快响）
  输  入:   空
  输  出:   空    
  返  回:	空
  描  述：  
**********************************************************************************/
void Under_Test(Protest_type indicateType)
{
    switch (indicateType)
    {
    case LED_INDICATE:
        LedStatusUpdate(CLR_NET_FINISHI); //红蓝交替
        break;
    case BUZZER_INDICATE:;
        break;
    case INCHING_INDICATE:;
        break;
    }
}
/**********************************************************************************
  函数名:   
  功  能:   产测完成成功
  输  入:   空
  输  出:   空    
  返  回:	空
  描  述：  
**********************************************************************************/
void Test_Succeed(Protest_type indicateType)
{
    switch (indicateType)
    {
    case LED_INDICATE:
        LedStatusUpdate(DELETE_FINISH); //蓝灯长亮2S
        break;
    case BUZZER_INDICATE:;
        break;
    case INCHING_INDICATE:;
        break;
    }
}
/**********************************************************************************
  函数名:   
  功  能:   产测完成失败
  输  入:   空
  输  出:   空    
  返  回:	空
  描  述：  
**********************************************************************************/
void Test_FAIL(Protest_type indicateType)
{
    switch (indicateType)
    {
    case LED_INDICATE:
        LedStatusUpdate(LED_OFF); //灯mie
        break;
    case BUZZER_INDICATE:;
        break;
    case INCHING_INDICATE:;
        break;
    }
}