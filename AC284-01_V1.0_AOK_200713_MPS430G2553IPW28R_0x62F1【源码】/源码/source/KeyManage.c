/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *
 *  KeyManage.c file
 *  ��������: ����ɨ�輰����
**********************************************************************************/
#include "Define.h" /* ȫ�ֺ궨�� ------*/
#include "Extern.h" /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define KEY_DIS_SHAKE_TIME 40 / SYSTEM_REFERENCE_TIME
#define LONG_KEY_TIME 500 / SYSTEM_REFERENCE_TIME
#define SET_KEY_TIME (3000 / (SYSTEM_REFERENCE_TIME * (KEY_DIS_SHAKE_TIME)))
#define LEARN_TIME (500 / (SYSTEM_REFERENCE_TIME * (KEY_DIS_SHAKE_TIME)))
#define DELETE_TIME (3000 / (SYSTEM_REFERENCE_TIME * (KEY_DIS_SHAKE_TIME)))
#define CLR_TIME (2000 / (SYSTEM_REFERENCE_TIME * (KEY_DIS_SHAKE_TIME)))
#define HAND_DRAG_SELECT_TIME (7000 / (SYSTEM_REFERENCE_TIME * (KEY_DIS_SHAKE_TIME)))

#define KEY_NUM 4
#define UP_KEY BIT6
#define DOWN_KEY BIT7
#define STOP_KEY UP_KEY + DOWN_KEY
#define NO_KEY 0X00
#define EX_KEY UP_KEY + DOWN_KEY

#define LEARN_KEY BIT7
#define LEARN_KEY_IN() (((P1IN)&LEARN_KEY) ? 0 : 1)

#define KEY_OUT_TIME 150 / SYSTEM_REFERENCE_TIME
#define KEY_IN_VALUE 0
#define KEY_OUT_VALUE UP_KEY + DOWN_KEY
//#define Key_IO_Set(var)        (P1DIR = ((P1DIR & (~KEY_OUT_VALUE)) | (var)))
#define UP_KEY_IO 6
#define DOWN_KEY_IO 7
#define OUT_UP_KEY() (P2OUT &= ~(1 << UP_KEY_IO))
#define OUT_DOWN_KEY() (P2OUT &= ~(1 << DOWN_KEY_IO))
#define NOOUT_UP_KEY() (P2OUT |= (1 << UP_KEY_IO))
#define NOOUT_DOWN_KEY() (P2OUT |= (1 << DOWN_KEY_IO))
/* �ڲ����� ---------------------------------------------------------------------*/
void KeyControl(Motor_Control_TypeDef command);
void NoKey(Motor_Control_TypeDef command);
/* �ڲ����� ---------------------------------------------------------------------*/
static u8 Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
static u8 Key_Old_Value;

static u8 Learn_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
static u8 Learn_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
static u16 Learn_Key_Cnt;
static u8 Set_Speed_Cnt;
static u8 Key_Status_Cnt;

static u8 Set_Speed_Cnt;

static u8 CLR_Net_Key_Num;
static u16 CLR_Net_Key_Time_Out_Cnt;

u8 const MOTOR_SPEED_TAB[3] = {MAX_SPEED_100, MAX_SPEED_80, MAX_SPEED_130};
Motor_Control_TypeDef const KeyLoopArray[4] =
    {
        MOTOR_UP,
        MOTOR_STOP,
        MOTOR_DOWN,
        MOTOR_STOP,
};

struct
{
    u8 Change_Direction : 1;
    u8 CLR_Net_Disposal : 1;
    u8 CLR_Net_Status : 1;
    u8 Learn_Key_Dispose : 1;
    u8 Set_Key_Dispose : 1;
} KeyFlag;

typedef struct
{
    u8 const KEY_VALUE;
    Motor_Control_TypeDef const COMMAND;
    void (*KeyDisposal)(Motor_Control_TypeDef command);
} Key;
Key const KeyArray[KEY_NUM] =
    {
        {UP_KEY, MOTOR_UP, KeyControl},
        {DOWN_KEY, MOTOR_DOWN, KeyControl},
        {STOP_KEY, MOTOR_STOP, KeyControl},
        {NO_KEY, MOTOR_STOP, NoKey}};

/**********************************************************************************
  ������:   KeyIN
  ��  ��:   
 *********************************************************************************/
u8 KeyIN(void)
{
    u8 temp;

    temp = (~P2IN) & UP_KEY;
    temp |= (~P2IN) & DOWN_KEY;

    return (temp);
}
/**********************************************************************************
  ������:   KeyControl
  ��  ��:   
 *********************************************************************************/
void KeyControl(Motor_Control_TypeDef command)
{
    switch (Dry_Contact_Control_Mode)
    {
    case NORMAL:
        Motor_Status = command;
        break;
    case STXT_LOOP:
        Motor_Status = KeyLoopArray[Key_Status_Cnt];
        if (++Key_Status_Cnt >= 4)
        {
            Key_Status_Cnt = 0;
        }
        break;
    case SST_XXT_SXQH:
        if (Motor_Status == command)
        {
            Motor_Status = MOTOR_STOP;
        }
        else
        {
            Motor_Status = command;
        }
        break;
    case SONG_SHOU_TING:
        Motor_Status = command;
        break;
    case REN_YI_JIAN_TING:
        if (Motor_Status != MOTOR_STOP)
        {
            Motor_Status = MOTOR_STOP;
        }
        else
        {
            Motor_Status = command;
        }
        break;
    default:
        Dry_Contact_Control_Mode = NORMAL;
        break;
    }

    //if (Flag.Goto_PCT)
    {
        Flag.Goto_PCT = NULL;
        Flag_Motor.PercentUsual = NULL;
        Motor_Array[0].Motor_Flag.StatusSwitch = TRUE; //����Ŀ��λ��
    }
}
/**********************************************************************************
  ������:   NoKey
  ��  ��:   
 *********************************************************************************/
void NoKey(Motor_Control_TypeDef command)
{
    if (Dry_Contact_Control_Mode == SONG_SHOU_TING)
    {
        if ((Key_Old_Value == UP_KEY) || (Key_Old_Value == DOWN_KEY))
        {
            Motor_Status = MOTOR_STOP;
        }
    }
}
/**********************************************************************************
  ������:   KeyScan
  ��  ��:   ɨ�谴��
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void KeyManage(void)
{
    u8 i;
    u8 new_key_value;

    if (Flag.Key_Signal_Out) // �Ƿ���������ź�
    {
        static u16 key_out_cnt;
        static u16 key_out_cnt2;
        static u8 Key_Out_Status_buffer;

        P2DIR |= EX_KEY; // ����IO��Ϊ���
        _NOP();

        if (Key_Out_Status_buffer != Key_Out_Status)
        {
            NOOUT_UP_KEY();
            NOOUT_DOWN_KEY();
            key_out_cnt = NULL;
            if (++key_out_cnt2 >= KEY_OUT_TIME)
            {
                key_out_cnt2 = NULL;
                Key_Out_Status_buffer = Key_Out_Status;
            }
        }
        else
        {
            switch (Key_Out_Status_buffer) // �������ʲô״̬
            {
            case MOTOR_UP: // ��
                OUT_UP_KEY();
                NOOUT_DOWN_KEY();
                break;
            case MOTOR_STOP: // ͣ
                OUT_UP_KEY();
                OUT_DOWN_KEY();
                break;
            case MOTOR_DOWN: // ��
                OUT_DOWN_KEY();
                NOOUT_UP_KEY();
                break;
            }
            if (++key_out_cnt >= KEY_OUT_TIME)
            {
                Flag.Key_Signal_Out = NULL; // ֹͣ��������ź�
                key_out_cnt = NULL;
                NOOUT_UP_KEY();
                NOOUT_DOWN_KEY();
            }
        }
    }
    else
    {
        //Key_IO_Set(KEY_IN_VALUE);                                // ����IO��Ϊ����
        P2DIR = 0x07; // ����IO��Ϊ����
        _NOP();

        new_key_value = KeyIN();            /* �������ֵ ------*/
        if (new_key_value != Key_Old_Value) /* �Ƿ����µİ��� --*/
        {
            if (--Key_ON_Cnt == 0)
            {
                Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
                for (i = 0; i < KEY_NUM; i++)
                {
                    if (new_key_value == KeyArray[i].KEY_VALUE)
                    {
                        (*KeyArray[i].KeyDisposal)(KeyArray[i].COMMAND);
                        Key_Old_Value = new_key_value; /* ���µ�ǰ��ֵ ----*/
                        break;
                    }
                }
            }
        }
        else
        {
            Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
        }
    }

    // ѧ���
    if (LEARN_KEY_IN())
    {
        if (--Learn_Key_ON_Cnt == NULL)
        {
            Learn_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
            Learn_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
            if (!KeyFlag.Learn_Key_Dispose)
            {
                Flag.Learn_Key_ON = TRUE;
                if (LED_Status == SET_SPEED_STATUS)
                {
                    if (++Set_Speed_Cnt >= 3)
                    {
                        Set_Speed_Cnt = 0; // ��λָ��
                    }

                    Set_Speed_Time_Cnt = NULL; // �趨ת��ʱ������
                    KeyFlag.Learn_Key_Dispose = TRUE;
                    Motor_Max_Speed = MOTOR_SPEED_TAB[Set_Speed_Cnt]; // ȡת��ֵ

                    Flag_Motor.Save_Speed = TRUE;
                }
                else if (++Learn_Key_Cnt == LEARN_TIME && (!KeyFlag.Change_Direction) && (!KeyFlag.CLR_Net_Status) && (CLR_Net_Key_Num < 3))
                {
                    if (Flag_Motor.PercentUsual)
                    {
                        Flag.Goto_PCT = NULL;
                        Flag_Motor.PercentUsual = NULL;
                    }
                    CLR_Net_Key_Num = 0;
                    LedStatusUpdate(LEARN_STATUS);
                }
                else if (Learn_Key_Cnt >= HAND_DRAG_SELECT_TIME) // ����7����������ѡ��
                {
                    CLR_Net_Key_Num = 0;
                    LedStatusUpdate(OPRATION_FINISH);
                    KeyFlag.Learn_Key_Dispose = TRUE;
                    Motor_Array[0].Motor_Flag.Hand_Drag_Status = !Motor_Array[0].Motor_Flag.Hand_Drag_Status; // ��������״̬�л�
                    FlashWriteData(0xee);                                                                     // ������������״̬
                }
                else if (Learn_Key_Cnt >= DELETE_TIME) // ����3��ɾ��
                {
                    if (KeyFlag.CLR_Net_Status) // �Ƿ�����
                    {
                        CLR_Net_Key_Num = 0;
                        KeyFlag.CLR_Net_Status = FALSE;
                        //            Flag.model = TRUE;               //Flag.CLR_Net_Oder = TRUE;
                        //            Flag.Uart_tx = TRUE;             // ���ڷ�������������ʾ
                        Flag.NetEvent = TRUE;            //Flag.CLR_Net_Oder = TRUE;
                        Flag.Uart_tx = TRUE;             // ���ڷ�������������ʾ
                        LedStatusUpdate(CLR_NET_STATUS); // �����һ����ʾ
                        KeyFlag.Learn_Key_Dispose = TRUE;
                    }
                    else if (KeyFlag.Change_Direction) // �Ƿ��ֶ�����
                    {
                        KeyFlag.Learn_Key_Dispose = TRUE;
                        KeyFlag.Change_Direction = 0;
                        Motor_Array[0].Motor_Flag.Direction = !Motor_Array[0].Motor_Flag.Direction;
                        Flag.model_Stat_Direction = TRUE;
                        LedStatusUpdate(OPRATION_FINISH);
                        FlashWriteData(0xee);
                    }
                    else if (CLR_Net_Key_Num == 3) // �Ƿ��ֶ������һ�γ���
                    {
                        //CLR_Net_Key_Num = 0;
                        KeyFlag.Change_Direction = TRUE;
                        LedStatusUpdate(BLUE_LED_HINT);
                        KeyFlag.Learn_Key_Dispose = TRUE;
                    }
                    else if (LED_Status == LEARN_STATUS)
                    {
                        LedStatusUpdate(DELETE_STATUS);
                    }
                }
                else if (LED_Status == DELETE_STATUS)
                {
                    CLR_Net_Key_Num = 0;
                    LedStatusUpdate(DELETE_FINISH);
                    KeyFlag.Learn_Key_Dispose = TRUE;
                    DeleteAllID(0xee);
                }
                else if ((!KeyFlag.CLR_Net_Disposal) && (!KeyFlag.Change_Direction))
                {
                    KeyFlag.CLR_Net_Disposal = TRUE;
                    CLR_Net_Key_Time_Out_Cnt = 0;
                    if (++CLR_Net_Key_Num >= 4)
                    {
                        KeyFlag.CLR_Net_Status = TRUE;
                    }
                    LedStatusUpdate(BLUE_LED_HINT);
                }
            }
        }
    }
    else if (--Learn_Key_OFF_Cnt == NULL)
    {
        Learn_Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
        Learn_Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
        KeyFlag.Learn_Key_Dispose = FALSE;
        Learn_Key_Cnt = NULL;
        Flag.Learn_Key_ON = FALSE;

        KeyFlag.CLR_Net_Disposal = FALSE;

        if (++CLR_Net_Key_Time_Out_Cnt >= CLR_TIME)
        {
            KeyFlag.CLR_Net_Status = FALSE;
            KeyFlag.Change_Direction = FALSE;
            CLR_Net_Key_Num = 0;
            CLR_Net_Key_Time_Out_Cnt = 0;
        }
    }

#ifdef KEY_CARD_CONTROL
    RS485_RX();
    if (Key_Old_Value != STOP_KEY) /* �Ƿ��� --------*/
    {
        u8 temp;
        temp = P1IN & BIT1;
        if (temp != Room_Card_Buf)
        {
            if (++Room_Card_Cnt >= 80)
            {
                Room_Card_Cnt = NULL;
                Room_Card_Buf = temp;
                if (temp == NULL)
                {
                    Motor_Status = MOTOR_UP;
                }
                else
                {
                    Motor_Status = MOTOR_DOWN;
                }
            }
        }
        else
        {
            Room_Card_Cnt = NULL;
        }
    }
#endif
}