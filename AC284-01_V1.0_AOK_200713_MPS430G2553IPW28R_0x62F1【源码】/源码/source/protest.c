#include "Define.h" /* ȫ�ֺ궨�� ------*/
#include "Extern.h" /* ȫ�ֱ��������� --*/

//typedef enum {
//  LED_INDICATE      (u8)0x01,
//  BUZZER_INDICATE   (u8)0x02,
//  INCHING_INDICATE  (u8)0x03,
//}Protest_type;
/**********************************************************************************
  ������:   
  ��  ��:   �������
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:	��
  ��  ����  
**********************************************************************************/
void Enter_test(u8 *RF_data, u8 exe_conditon)
{
    if (exe_conditon) //ȷ��ִ��
    {
        {
            if (RF_LEARN == RF_data[6]) //�����
            {
                Flag_RF.Data_Disposal = TRUE;
                Time_Power_On = 0;

                No_learn_key_cnt = 0;
                if (++learn_key_cnt >= 3) //��������
                {
                    learn_key_cnt = 0;
                    Flag.Power_on = 0;
                    Time_Power_On = 0;
                    Flag.WifiTest = 1; //��ʼ����wifi
                    Flag.Uart_tx = 1;
                }
            }
        }
    }
}
/**********************************************************************************
  ������:   
  ��  ��:   �˳�����
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:	��
  ��  ����  
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
  ������:   
  ��  ��:   �����У�ָʾ�ƿ��������������죩
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:	��
  ��  ����  
**********************************************************************************/
void Under_Test(Protest_type indicateType)
{
    switch (indicateType)
    {
    case LED_INDICATE:
        LedStatusUpdate(CLR_NET_FINISHI); //��������
        break;
    case BUZZER_INDICATE:;
        break;
    case INCHING_INDICATE:;
        break;
    }
}
/**********************************************************************************
  ������:   
  ��  ��:   ������ɳɹ�
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:	��
  ��  ����  
**********************************************************************************/
void Test_Succeed(Protest_type indicateType)
{
    switch (indicateType)
    {
    case LED_INDICATE:
        LedStatusUpdate(DELETE_FINISH); //���Ƴ���2S
        break;
    case BUZZER_INDICATE:;
        break;
    case INCHING_INDICATE:;
        break;
    }
}
/**********************************************************************************
  ������:   
  ��  ��:   �������ʧ��
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:	��
  ��  ����  
**********************************************************************************/
void Test_FAIL(Protest_type indicateType)
{
    switch (indicateType)
    {
    case LED_INDICATE:
        LedStatusUpdate(LED_OFF); //��mie
        break;
    case BUZZER_INDICATE:;
        break;
    case INCHING_INDICATE:;
        break;
    }
}