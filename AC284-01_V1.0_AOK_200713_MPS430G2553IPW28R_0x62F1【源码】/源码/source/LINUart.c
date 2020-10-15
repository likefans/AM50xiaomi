/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *
 *  LINUart.c file
 *  ��������: 
**********************************************************************************/
#include "Define.h" /* ȫ�ֺ궨�� ------*/
#include "Extern.h" /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define BAUD_RATE_VALUE(var) ((10000 / ((var) / 100)) * SYSTEM_CLOCK)
/*
#define UART_RX_BYTE           7
#define UART_TX_BYTE           10 + 2
#define EUSART_NO_RX_TIME      4

#define EUSART_DATA_HEAD       0x9a                         // �����ź�ͷ ------
#define EUSART_DATA_UP         0xdd                         // ������������ ----
#define EUSART_DATA_STOP       0xcc                         // ����ֹͣ���� ----
#define EUSART_DATA_DOWN       0xee                         // ������������ ----
#define EUSART_DATA_LEARN      0xaa                         // ����ѧϰ���� ----
#define EUSART_DATA_CLEAR      0xa6                         // ����������� ----
#define EUSART_DATA_ORDER      0x0a                         // ���������־ ----
#define EUSART_DATA_INQUIRE    0xcc                         // ���ڲ�ѯ��־ ----
                                                            //
#define INQUIRE_FUNCTION       0xCA                         // ��ѯ����״̬ ----
#define EUSART_DATA_GOTO_PCT   0xDD                         // ���е��ٷֱ� ----
#define EUSART_DATA_SET_SPEED  0xD9                         // �趨���ת�� ----
#define SET_DRY_CONTACT        0xD2                         // �趨�ֿط�ʽ ----
#define SET_HAND_DRAG          0xD5                         // �趨����������� 
#define SET_BAUD_RATE          0xDA                         // ���ڲ����� ------
                                                            //
#define EUSART_JIGGLE_UP       0x0d                         // �㶯�������� ----
#define EUSART_JIGGLE_DOWN     0x0e                         // �㶯�������� ----
#define EUSART_DATA_LIMIT_1    0x01                         // �м���λ��1 -----
#define EUSART_DATA_LIMIT_2    0x02                         // �м���λ��2 -----
#define EUSART_DATA_LIMIT_3    0x03                         // �м���λ��3 -----
#define EUSART_DATA_LIMIT_4    0x04                         // �м���λ��4 -----
#define SEND_DATA_HEAD         0xd8                         // �����ź�ͷ ------
*/
#define OUT_TEST_TIME 40

#define DATA_HEAD 0X9A
#define EUSART_NO_RX_TIME 4
/*********************************************�������*********************************************************/

#define COMMAND_TYPE_CONTROL 0X0A          //��״������»�
#define COMMAND_TYPE_PERCENT 0X0D          //ֻ���е����γɵĲ�Ʒ���ưٷֱ�
#define COMMAND_TYPE_SETTING0 0X11         //�û��ɲ��������ã��������ܡ��ٶȡ��㶯�Ƕ�
#define COMMAND_TYPE_SETTING1 0X12         //�û��ɲ��������ã����»���ɡ���ɡ�����ʱ��
#define COMMAND_TYPE_SETTING2 0X13         //�û��ɲ��������ã�������Ӧ�ȼ�
#define COMMAND_TYPE_USERDEBUG0 0X21       //��װ���������ã��������/�ȼ� �� �ֿط�ʽ �� ����ʱ��
#define COMMAND_TYPE_USERDEBUG1 0X22       //��װ���������ã���ѡ���� ����λ��
#define COMMAND_TYPE_CONFIGURE0 0X31       //���������ࣺ�̼��汾
#define COMMAND_TYPE_CONFIGURE1 0X32       //���������ࣺ���Ͳ���
#define COMMAND_TYPE_CONFIGURE2 0X33       //���������ࣺ�豸����
#define COMMAND_TYPE_CONFIGURE3 0X34       //���������ࣺ���к�
#define COMMAND_TYPE_CHANGENET0 0X41       //������ʱ��������л�ΪЭ����
#define COMMAND_TYPE_CHANGENET1 0X42       //������ʱ��������л�Ϊ·����
#define COMMAND_TYPE_CHANGENET2 0X43       //������ʱ�����������ң�����
#define COMMAND_TYPE_CHANGENET3 0X44       //������ʱ�������ɾ������ң����ԣ�����������
#define COMMAND_TYPE_RF_CONTROL0 0X51      //ң�������������������豸��5���ӣ�ң����������
#define COMMAND_TYPE_RF_CONTROL1 0X52      //ң�����������ֹͣ����豸��ң����������
#define COMMAND_TYPE_GETSTATUS 0XA1        //����״̬��ѯ
#define COMMAND_TYPE_GETPARA 0XA2          //����������ѯ�����»���ɡ���ɡ�����ʱ��
#define COMMAND_TYPE_GETCHANGEABLE 0XA3    // �û����޸ĳ��ù��ܲ�ѯ���������ܡ��ٶȡ��㶯�Ƕ�
#define COMMAND_TYPE_GETDRIERPARA 0XA4     //���»���ɡ���ɡ�����ʱ��
#define COMMAND_TYPE_GETUSERDEBUGINFO 0XA5 //�������/�Ǽǡ��ֿط�ʽ������ʱ��
#define COMMAND_TYPE_GETCONFIG0INFO 0XB1   //�����������ѯ���̼��汾
#define COMMAND_TYPE_GETCONFIG1INFO 0XB2   //�����������ѯ���ͺŲ���
#define COMMAND_TYPE_GETCONFIG2INFO 0XB3   //�����������ѯ���豸����
#define COMMAND_TYPE_GETCONFIG3INFO 0XB4   //�����������ѯ�����к�
#define COMMAND_TYPE_GETERRORINFO 0XA6     //����״̬��ѯ��������Ϣ
#define COMMAND_TYPE_WIFI 0XE1             //����/����
#define COMMAND_TYPE_PROTEST 0XE3

/********************************************************************************************************************************************************************************************/

/*************************************************���������*****************************************************/
//#define COMMAND_TYPE_CONTROL           0X0A //��״������»�
#define COMMAND_TYPE_CONTROL_LENGTH 1
#define COMMAND_TYPE_CONTROL_UP 0XDD        //����
#define COMMAND_TYPE_CONTROL_DDUP 0X0d      //�㶯�� ����Ҷ���Ͳ�Ʒ�ã�
#define COMMAND_TYPE_CONTROL_STOP 0Xcc      //ֹͣ
#define COMMAND_TYPE_CONTROL_DOWN 0Xee      //����
#define COMMAND_TYPE_CONTROL_DDDOWN 0X0e    //�㶯�� ����Ҷ���Ͳ�Ʒ�ã�
#define COMMAND_TYPE_CONTROL_LIGHT 0X2a     //����
#define COMMAND_TYPE_CONTROL_DRY 0X2b       //���
#define COMMAND_TYPE_CONTROL_BAKEOUT 0X2c   //���
#define COMMAND_TYPE_CONTROL_DISINFECT 0X2d //����
#define COMMAND_TYPE_CONTROL_MUSIC 0X2e     //����
#define COMMAND_TYPE_CONTROL_POWER 0X2f     //��Դ
#define COMMAND_TYPE_CONTROL_BEEP 0Xaa      //��ʾ�������������߷�������ʾ�������豸����Ӧ�ã�
/*******/
//#define COMMAND_TYPE_PERCENT           0X0D //ֻ���е����γɵĲ�Ʒ���ưٷֱ�
#define COMMAND_TYPE_PERCENT_LENGTH 3
/*******/
//#define COMMAND_TYPE_SETTING0          0X11 //�û��ɲ��������ã��������ܡ��ٶȡ��㶯�Ƕ�
#define COMMAND_TYPE_SETTING0_LENGTH 3
//data0:
#define COMMAND_TYPE_SETTING0_HANDLE 0X01    //0�������� 1��������
#define COMMAND_TYPE_SETTING0_DIRECTION 0X02 //0: Ĭ�Ϸ��� 1������
#define COMMAND_TYPE_SETTING0_INCH 0X04      //0: ���� 1���㶯
#define COMMAND_TYPE_SETTING0_RESTART 0X08   // 0:�޲���  1��ϵͳ����
#define COMMAND_TYPE_SETTING0_FREE0 0X10     //����
#define COMMAND_TYPE_SETTING0_FREE1 0X20     //����
#define COMMAND_TYPE_SETTING0_FREE2 0X40     //����
#define COMMAND_TYPE_SETTING0_FREE3 0X40     //����
//data1: �����ٶ�RPM
//data2:
/*�㶯ʱ�䣨0.01��
*���磺����3Ϊ255
�㶯ʱ��Ϊ255*0.1 = 2.55S
*/
/*******/
//#define COMMAND_TYPE_SETTING1          0X12 //�û��ɲ��������ã����»���ɡ���ɡ�����ʱ��
#define COMMAND_TYPE_SETTING1_LENGTH 3
//data0: ���ʱ�䣨���ӣ�
//data1: ���ʱ�䣨���ӣ�
//data2: ����ʱ�䣨���ӣ�
/*******/
//#define COMMAND_TYPE_SETTING2          0X13 //�û��ɲ��������ã�������Ӧ�ȼ�
#define COMMAND_TYPE_SETTING2_LENGTH 3
//data0: ��еȼ�0~9  0���ر� 1�������� 9�������
//data1: ��еȼ�0~9  0���ر� 1�������� 9�������
//data2: ��еȼ�0~9  0���ر� 1�������� 9�������
/*******/

//#define COMMAND_TYPE_USERDEBUG0       0X21            //��װ���������ã��������/�ȼ� �� �ֿط�ʽ �� ����ʱ��
#define COMMAND_TYPE_USERDEBUG0_LENGTH 5
//data0: ���������mA��
//data1: �ֿ��߷�ʽ��1������ 2����ͣ��ͣ����ѭ�� 3������ͣ�������л� 4������ͣ 5������ͣ
//data2: ���������� 01���������� 02:�������� 03����������
//data3~data4:�����ʱ�䣨�룩 data3��H  data4:L   0xfff0:��ʱ������
//data0~ data4 Ϊ0ʱ���������в���

//#define COMMAND_TYPE_USERDEBUG1       0X22            //��װ���������ã���ѡ���� ����λ��
#define COMMAND_TYPE_USERDEBUG1_LENGTH 3
//data0:
#define COMMAND_TYPE_USERDEBUG1_SLOWSTART 0X01      //0:�������� 1����������
#define COMMAND_TYPE_USERDEBUG1_SPACETOJOURNEY 0X02 //0:���г��м�϶ 1�����г̲�����϶
#define COMMAND_TYPE_USERDEBUG1_STOPTOJOURNEY 0X04  //0:���г�ֹͣ 1������ֹͣ
#define COMMAND_TYPE_USERDEBUG1_MEMORYJOURNEY 0X08  //0:�����г� 1���������г�
#define COMMAND_TYPE_USERDEBUG1_CONTACTSYNCHRO 0X10 //0:�ֿ���ͬ�� 1����ͬ��
#define COMMAND_TYPE_USERDEBUG1_FREE00 0X20         //0:����
#define COMMAND_TYPE_USERDEBUG1_FREE01 0X40         //0:����
#define COMMAND_TYPE_USERDEBUG1_FREE02 0X80         //0:����
//data1: (data1��data2 ����bit 0:�޲��� 1����Ч)
#define COMMAND_TYPE_USERDEBUG1_SETUPLIMIT 0X01   //��������λ
#define COMMAND_TYPE_USERDEBUG1_SETDOWNLIMIT 0X02 //��������λ
#define COMMAND_TYPE_USERDEBUG1_ADJUSTLIMIT 0X04  //΢����λ��
#define COMMAND_TYPE_USERDEBUG1_FREE10 0X08       //����
#define COMMAND_TYPE_USERDEBUG1_FREE11 0X10       //����
#define COMMAND_TYPE_USERDEBUG1_FREE12 0X20       //0:����
#define COMMAND_TYPE_USERDEBUG1_FREE13 0X40       //0:����
#define COMMAND_TYPE_USERDEBUG1_FREE14 0X80       //0:����
//data2:
#define COMMAND_TYPE_USERDEBUG1_RESETFACTORY 0X01     //��������λ
#define COMMAND_TYPE_USERDEBUG1_DELETEALLRFID 0X02    //��������λ
#define COMMAND_TYPE_USERDEBUG1_DELETEALLLIMIT 0X04   //΢����λ��
#define COMMAND_TYPE_USERDEBUG1_DELETEIDANDLIMIT 0X08 //����
#define COMMAND_TYPE_USERDEBUG1_FREE20 0X10           //����
#define COMMAND_TYPE_USERDEBUG1_FREE21 0X20           //0:����
#define COMMAND_TYPE_USERDEBUG1_FREE22 0X40           //0:����
#define COMMAND_TYPE_USERDEBUG1_FREE23 0X80           //0:����

/*******/
//#define COMMAND_TYPE_CONFIGURE0         0X31 //���������ࣺ�̼��汾
#define COMMAND_TYPE_CONFIGRE0_LENGTH 6
//data0~data5: ��λ��ASIC�� ����̼��汾160610  ����1����6����0����6����1����0��
/*******/
#define COMMAND_TYPE_CONFIGURE1 0X32     //���������ࣺ���Ͳ���
#define COMMAND_TYPE_CONFIGRE1_LENGTH 14 //N:������Ҫע������Ǽ��������翪������AM68-1/80-EM-P
/*******/
//#define COMMAND_TYPE_CONFIGURE2         0X33 //���������ࣺ�豸����
#define COMMAND_TYPE_CONFIGRE2_LENGTH 1
#define COMMAND_TYPE_CONFIGRE2_KAIHELIAN 0X01
#define COMMAND_TYPE_CONFIGRE2_KAICHUANGJI 0X02
#define COMMAND_TYPE_CONFIGRE2_DRIERLIGHT 0X03
#define COMMAND_TYPE_CONFIGRE2_DRIERPROFESSION 0X04
#define COMMAND_TYPE_CONFIGRE2_DRIERLUXURY 0X05
#define COMMAND_TYPE_CONFIGRE2_SCREEN 0X06           //ͶӰ��Ļ
#define COMMAND_TYPE_CONFIGRE2_SCREENDROPHANGER 0X07 //��Ļ����
#define COMMAND_TYPE_CONFIGRE2_AWNING 0X08           //������
#define COMMAND_TYPE_CONFIGRE2_BLIND 0X09            //��Ҷ�� (AM16 24 54 55��Ҷר�õ��)
#define COMMAND_TYPE_CONFIGRE2_FENGGUANGYU 0X0A      //����괫����
#define COMMAND_TYPE_CONFIGRE2_FENGGANQI 0X0B        //�����
#define COMMAND_TYPE_CONFIGRE2_WENSHI 0X0C           //��ʪ�ȴ�����
#define COMMAND_TYPE_CONFIGRE2_TIMEINGREMOTE 0X0D    //��ʱ������
#define COMMAND_TYPE_CONFIGRE2_HANDLEREMOTE 0X0E     //�ֳ�ʽ������
#define COMMAND_TYPE_CONFIGRE2_WALLREMOTE 0X0F       //ǽ����������
/*******/
//#define COMMAND_TYPE_CONFIGURE3         0X34 //���������ࣺ���к�  ������ˮ��
#define COMMAND_TYPE_CONFIGURE3_LENGTH 13 // N:ASIC��  �磺AM00002477700
/*******/
/*define COMMAND_TYPE_CHANGENET0          0X41 //������ʱ��������л�ΪЭ����
#define COMMAND_TYPE_CHANGENET1          0X42 //������ʱ��������л�Ϊ·����
#define COMMAND_TYPE_CHANGENET2          0X43 //������ʱ�����������ң�����
#define COMMAND_TYPE_CHANGENET3          0X44 //������ʱ�������ɾ������ң����ԣ�����������
#define COMMAND_TYPE_RF_CONTROL0         0X51 //ң�������������������豸��5���ӣ�ң����������      
#define COMMAND_TYPE_RF_CONTROL1         0X52 //ң�����������ֹͣ����豸��ң����������    */

/*#define COMMAND_TYPE_GETSTATUS             0XA1 //����״̬��ѯ
#define COMMAND_TYPE_GETPARA             0XA2 //����������ѯ�����»���ɡ���ɡ�����ʱ��
#define COMMAND_TYPE_GETCHANGEABLE       0XA3 // �û����޸ĳ��ù��ܲ�ѯ���������ܡ��ٶȡ��㶯�Ƕ�
#define COMMAND_TYPE_GETDRIERPARA        0XA4 //���»���ɡ���ɡ�����ʱ��
#define COMMAND_TYPE_GETUSERDEBUGINFO    0XA5 //�������/�Ǽǡ��ֿط�ʽ������ʱ��
#define COMMAND_TYPE_GETCONFIG0INFO       0XB1//�����������ѯ���̼��汾
#define COMMAND_TYPE_GETCONFIG1INFO       0XB2//�����������ѯ���̼��汾
#define COMMAND_TYPE_GETCONFIG2INFO       0XB3//�����������ѯ���̼��汾
#define COMMAND_TYPE_GETCONFIG3INFO       0XB4//�����������ѯ���̼��汾
#define COMMAND_TYPE_GETERRORINFO         0XA6//����״̬��ѯ��������Ϣ*/

/***************/
//#define COMMAND_TYPE_WIFI                 0XE1 //����/����
#define COMMAND_TYPE_WIFI_LENGTH 1
#define COMMAND_TYPE_WIFI_WIFIOUT 0X01
#define COMMAND_TYPE_WIFI_WIFIIN 0X02
#define CLR_NET_FINISHI_DATA 0x01 /* �����ɹ����� --- */
#define ADD_NET_FINISHI_DATA 0x02 /* �����ɹ����� --- */

#define COMMAND_TYPE_PAUSE 0 //��ͣ
#define COMMAND_TYPE_OPEN 1  //��
#define COMMAND_TYPE_CLOSE 2 //��

/* �ڲ����� ---------------------------------------------------------------------*/
void UartDataDisposal(void);
u8 JourneyPercentCount(void);
/* �ڲ����� ---------------------------------------------------------------------*/
static u8 Uart_TX_Byte_Cnt;            // �ֽڼ�����
static u8 Uart_Motor_Percentage = 100; //���ڷ��Ͱٷֱ�ֵ
static u8 Uart_Rx_Array[50];
static u8 Uart_Tx_Array[50];
//static u8 Uart_NO_Rx_Cnt;
//static u8 Inquire_Function_Flag;
static u8 TxDataLength;
u8 Status_Buffer; //������״̬
u8 PercentBuff;

const u16 Baud_Rate_Array[BAUD_RATE_NUM] =
    {
        1200, 2400, 4800, 9600, 19200};
/**********************************************************************************
  ������:   LinUartSetting
  ��  ��:   Uart ����
**********************************************************************************/
void LinUartInit(void)
{
    if (Uart_Baud_Rate >= BAUD_RATE_NUM)
    {
        Uart_Baud_Rate = BR19200;
    }

    UCA0BR0 = 138;     // BAUD_RATE_VALUE(115200) & 0xFF;
    UCA0BR1 = 0;       // BAUD_RATE_VALUE(115200) >> 8;
    UCA0MCTL = UCBRS1; // Modulation UCBRSx = 2

    P1SEL |= BIT1 + BIT2;  // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= BIT1 + BIT2; // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;  // SMCLK

    UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;      // Enable USCI_A0 RX interrupt

    RS485_RX();
}
/*********************************************************************************
  ������:   DataCRC
  ��  ��:   ѭ�����Ч��
  ��  ��:   ��Ч���������ַ��Ч�����ݵĸ���
  ��  ��:   ��    
  ��  ��:   ���Ľ��
  ��  ����
*********************************************************************************/
u8 DataCRC(u8 array[])
{
    if (strncmp((char *)array, "down set_properties", sizeof("down set_properties") - 1) == 0) //�Ƚ�
        return 1;
    else if (strncmp((char *)array, "down get_properties", sizeof("get_properties") - 1) == 0)
        return 2;
    else if (strncmp((char *)array, "down power_off", sizeof("down power_off") - 1) == 0)
        return 3;
    else if (strncmp((char *)array, "down MIIO_net_change cloud", sizeof("down MIIO_net_change cloud") - 1) == 0)
    {
        LedStatusUpdate(ADD_NET_FINISHI);
        return 0;
    }
    else if (strncmp((char *)array, "down MIIO_net_change local", sizeof("down MIIO_net_change cloud") - 1) == 0)
    {
        LedStatusUpdate(ADD_NET_FINISHI);
        return 0;
    }
    else
        return 0;
}
/**********************************************************************************
  ������:   MotorControl
  ��  ��:   
 *********************************************************************************/
u16 CountPercentPlace(u8 percent)
{
    u16 temp;
    u16 place;

    temp = Hall_Pulse_Amount - MIN_JOURNEY_VALUE;
    place = MIN_JOURNEY_VALUE;

    if (percent > 100)
    {
        percent = 100;
    }

    if (!(Motor_Array[0].Motor_Flag.Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction))
    {
        if (temp < 600)
        {
            place = Hall_Pulse_Amount - ((u16)((temp * percent) / 100));
        }
        else if (temp < 2400)
        {
            place = Hall_Pulse_Amount - ((u16)(((temp >> 2) * percent) / 25));
        }
        else if (temp < 6000)
        {
            place = Hall_Pulse_Amount - ((u16)(((temp / 10) * percent) / 10));
        }
        else
        {
            place = Hall_Pulse_Amount - ((u16)(((temp / 100) * percent)));
        }
    }
    else
    {

        if (temp < 600)
        {
            place += (u16)((temp * percent) / 100);
        }
        else if (temp < 2400)
        {
            place += (u16)(((temp >> 2) * percent) / 25);
        }
        else if (temp < 6000)
        {
            place += (u16)(((temp / 10) * percent) / 10);
        }
        else
        {
            place += (u16)(((temp / 100) * percent));
        }
    }

    return ((u16)place);
}
/**********************************************************************************
  ������:   MotorControl
  ��  ��:   
 *********************************************************************************/
u8 JourneyPercentCount(void)
{
    u8 percent;
    u16 i;
    u16 temp;

    temp = Hall_Pulse_Amount - MIN_JOURNEY_VALUE;
    i = Hall_Pulse_Amount_Cnt - MIN_JOURNEY_VALUE;

    if (temp < 100)
    {
        percent = (u8)((i * 100) / temp);
    }
    else if (temp < 12000)
    {
        percent = (u8)((i * 5) / (temp / 20));
    }
    else
    {
        percent = (u8)(i / (temp / 100));
    }

    if (percent > 100)
    {
        percent = 100;
    }

    if (!(Motor_Array[0].Motor_Flag.Direction ^ Motor_Array[0].Motor_Flag.Journey_Direction))
    {
        percent = 100 - percent;
    }

    return (percent);
}
/**********************************************************************************
  ������:   Buff_Handle
  ��  ��:   
 *********************************************************************************/
u8 Buff_Handle(u8 count, u8 date[], char str[])
{
    u8 i;
    for (i = 0; i < strlen(str); i++)
        date[count + i] = str[i];
    return i + count;
}
/*********************************************************************************
  ������:   UartStartupTX
  ��  ��:   ׼�����ݣ���������
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����
*********************************************************************************/
void UartStartupTX(void)
{
    u8 Date_Position;
    RS485_TX();

    Date_Position = 0;                               //��ʼ��λ��
    memset(Uart_Tx_Array, 0, sizeof(Uart_Tx_Array)); //��շ�������

    if (Flag.Reporting == TRUE) //��ʱ����״̬����
    {
        Flag.Reporting = NULL;

        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "properties_changed 2 1 0");
        if (Flag.model_Stat_Direction == TRUE)
        {
            if (Motor_Array[0].Motor_Flag.Direction == FALSE) //���͵����ת���
                Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 3 false");
            else
                Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 3 true");
            Flag.model_Stat_Direction = NULL;
        }

        if (Flag.modle_Stat_Percentage != NULL)
        {
            Flag.modle_Stat_Percentage = NULL;
            Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 4 ");
            do
            {
                if (Uart_Motor_Percentage == 100)
                {
                    Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "100");
                    break;
                }
                else if (Uart_Motor_Percentage >= 10)
                    Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage / 10 + '0';
                Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage % 10 + '0';
            } while (0);
            TxDataLength = Date_Position;
        }
        else if (Flag.modle_automatic_Percentage == TRUE)
        {
            Flag.modle_automatic_Percentage = NULL;
            Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 4 ");
            if (Motor_Array[0].Motor_Flag.Journey_Set_OK) //���г�
            {
                Uart_Motor_Percentage = 100 - JourneyPercentCount();
                do
                {
                    if (Uart_Motor_Percentage == 100)
                    {
                        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "100");
                        break;
                    }
                    else if (Uart_Motor_Percentage >= 10)
                        Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage / 10 + '0';
                    Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage % 10 + '0';
                } while (0);
            }
            else
            {
                do
                {
                    if (Uart_Motor_Percentage == 100)
                    {
                        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "100");
                        break;
                    }
                    else if (Uart_Motor_Percentage >= 10)
                        Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage / 10 + '0';
                    Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage % 10 + '0';
                } while (0);
            }
        }
        TxDataLength = Date_Position;
    }
    else if (Flag.polling == TRUE)
    {
        Flag.polling = NULL;
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "get_down"); //����get_down
        TxDataLength = Date_Position;
    }
    else if (Flag.model == TRUE)
    {
        Flag.model = NULL;
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "model aok98.curtain.qlam50"); //����model
        TxDataLength = Date_Position;
        Flag.Wifi_cnt2 = TRUE;
    }
    else if (Flag.VERSION == TRUE)
    {
        Flag.VERSION = NULL;
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "mcu_version 0001"); //��������
        TxDataLength = Date_Position;
    }
    else if (Flag.NetEvent == TRUE)
    {
        Flag.NetEvent = NULL;
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "restore"); //��������
        TxDataLength = Date_Position;
        LedStatusUpdate(CLR_NET_FINISHI);
    }
    else if (Flag.WifiTest == TRUE)
    {
        Flag.WifiTest = NULL;
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "factory"); //���볧��
        TxDataLength = Date_Position;
        LedStatusUpdate(CLR_NET_FINISHI);
    }

    Status_Buffer = Motor_Status;
    Uart_Tx_Array[TxDataLength] = 13; //���з�
    Uart_TX_Byte_Cnt = 0;             // �����ֽ�����0
    IE2 |= UCA0TXIE;                  // Enable USCI_A0 TX interrupt
    IE2 &= ~UCA0RXIE;                 // Disable USCI_A0 RX interrupt
    IFG1 |= UCA0TXIFG;                // �����жϱ�־�����뷢���жϳ���
}
/*********************************************************************************
  ������:   Uart_TX_Interrupt
  ��  ��:   ���ڷ����ж�
*********************************************************************************/
// USCI A0/B0 Transmit ISR
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
    UCA0TXBUF = Uart_Tx_Array[Uart_TX_Byte_Cnt]; // û���������
    Flag.Time_200ms = NULL;
    Time_200ms_Cnt = TIME_200MS_VALUE;
    if (++Uart_TX_Byte_Cnt > TxDataLength) // �ж��Ƿ��ѷ�����
    {
        Uart_TX_Byte_Cnt = 0; // ���ʹ�����0
        TxDataLength = 0;
        RS485_RX();
        IE2 &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
        IE2 |= UCA0RXIE;  // Enable USCI_A0 RX interrupt
    }
}
/*********************************************************************************
  ������:   Uart_RX_Interrupt
  ��  ��:   ���ڽ����ж�
*********************************************************************************/
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    if (!Flag.Uart_Read_OK)
    {
        Uart_Rx_Array[Uart_Byte_Cnt++] = UCA0RXBUF;
        Flag.Time_4ms = TRUE;
        Time_4ms_Cnt = TIME_4MS_VALUE;
    }

    //  Uart_NO_Rx_Cnt = 0;
}
/*********************************************************************************
  ������:   UartControl
  ��  ��:   
*********************************************************************************/
void UartControl(u8 data)
{
    switch (data)
    {
    case COMMAND_TYPE_PAUSE:
        Motor_Status = MOTOR_STOP;
        Motor_Array[0].Motor_Flag.Run_Status = NULL;
        break;
    case COMMAND_TYPE_OPEN:
        Motor_Status = MOTOR_UP;
        break;
    case COMMAND_TYPE_CLOSE:
        Motor_Status = MOTOR_DOWN;
        break;
    }

    if (Flag.Goto_PCT)
    {
        Flag.Goto_PCT = NULL;
        Motor_Array[0].Motor_Flag.StatusSwitch = TRUE; //����Ŀ��λ��
    }
}

void DisposalPercent(u8 percent)
{

    Motor_Array[0].Motor_Flag.StatusSwitch = TRUE; //����Ŀ��λ��

    Hall_Pulse_Amount_PCT = CountPercentPlace(percent);

    Motor_Status = MOTOR_UP;
    if (!(Motor_Array[0].Motor_Flag.Journey_Direction ^ Motor_Array[0].Motor_Flag.Direction))
    {
        if (Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount_PCT)
        {
            Motor_Status = MOTOR_DOWN;
        }
    }
    else
    {
        if (Hall_Pulse_Amount_Cnt < Hall_Pulse_Amount_PCT)
        {
            Motor_Status = MOTOR_DOWN;
        }
    }

    if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_PCT - 10)) && (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_PCT + 10)))
    {
        Motor_Array[0].Motor_Flag.Run_Status = NULL;
        Motor_Status = MOTOR_STOP;
        Flag.Goto_PCT = NULL;
    }
}

/*********************************************************************************
  ������:   UartDataDisposal
  ��  ��:   ���ڽ��յ����ݴ���
  ��  ��:   ���ڽ��յ����ݻ���������ַ
  ��  ��:   ��    
  ��  ��:   ��
  ��  ����
**********************************************************************************/
void UartDataDisposal(void)
{
    u8 Date_type, Date_Position;
    u8 count = 22;
    Date_type = DataCRC(Uart_Rx_Array); //�鿴��������

    Date_Position = 0;
    Flag.Time_50ms = 0;
    Time_50ms_Cnt = TIME_50MS_VALUE; //�ȴ�UP/DOWN�ȿ�����䣬Ȼ����MOTOR״̬
    memset(Uart_Tx_Array, 0, sizeof(Uart_Tx_Array));

    Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "result");
    TxDataLength = Date_Position;
    if (Date_type == 1)
    {
        do
        {
            switch (Uart_Rx_Array[count])
            {
            case '1':
                Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 1 0");
                TxDataLength = Date_Position;
                UartControl(Uart_Rx_Array[count + 2] - 48);
                break;
            case '2':
                Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 2 0");
                TxDataLength = Date_Position;
                UartControl(Uart_Rx_Array[count + 2] - 48);
                break;
            case '3':
                Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 3 0");
                TxDataLength = Date_Position;
                if (Uart_Rx_Array[count + 2] == 't')
                {
                    Motor_Array[0].Motor_Flag.Direction = TRUE; /*�����ת*/
                    count = count + 3;
                }
                else
                {
                    Motor_Array[0].Motor_Flag.Direction = FALSE; /*�����ת*/
                    count = count + 4;
                }
                Flag.model_Stat_Direction = TRUE;
                LedStatusUpdate(OPRATION_FINISH);
                FlashWriteData(0xee);
                break;
            case '4':
                Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 4 0");
                TxDataLength = Date_Position;
                Flag.modle_Stat_Percentage = TRUE;
                Uart_Motor_Percentage = 0;
                for (Date_type = 0; Date_type < 3; Date_type++) //��ȡ�ٷֱ�����
                    if (Uart_Rx_Array[24 + Date_type] <= '9' && Uart_Rx_Array[24 + Date_type] >= '0')
                    {
                        Uart_Motor_Percentage = Uart_Motor_Percentage * 10 + Uart_Rx_Array[24 + Date_type] - '0';
                    }
                    else
                        break;

                Flag.Goto_PCT = TRUE;
                if (Motor_Array[0].Motor_Flag.Journey_Set_OK) //�ж������г�
                {
                    Flag_Motor.PercentUsual = NULL;
                    DisposalPercent(100 - Uart_Motor_Percentage); //ִ�аٷֱ�
                }
                else
                {
                    PercentBuff = Uart_Motor_Percentage;
                    Flag_Motor.PercentUsual = TRUE;
                    if (!Motor_Array[0].Motor_Flag.Run_Status)
                    {
                        if (!(Motor_Array[0].Motor_Flag.Journey_Direction ^ Motor_Array[0].Motor_Flag.Direction))
                            Motor_Status = MOTOR_DOWN;
                        else
                            Motor_Status = MOTOR_UP;
                    }
                }
                break;
            default:
                break;
            }
            count = count + 6;
        } while (Uart_Rx_Array[count] != 0x00);
    }
    else if (Date_type == 2)
    {
        do
        {
            switch (Uart_Rx_Array[count])
            {
            case '1':
                Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 1 0 0");
                TxDataLength = Date_Position;
                break;
            case '3':
                if (Motor_Array[0].Motor_Flag.Direction == NULL)
                    Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 3 0 false");
                else
                    Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 3 0 true");
                Flag.modle_Stat_Percentage = TRUE;
                TxDataLength = Date_Position;
                break;
            case '4':
                Flag.modle_Stat_Percentage = TRUE; //�ϱ�����
                Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 4 0 ");
                if (Motor_Array[0].Motor_Flag.Journey_Set_OK) //���г�
                {
                    Uart_Motor_Percentage = 100 - JourneyPercentCount();
                    do
                    {
                        if (Uart_Motor_Percentage == 100) //���г�
                        {
                            Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "100");
                            break;
                        }
                        else if (Uart_Motor_Percentage >= 10)
                            Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage / 10 + '0';

                        Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage % 10 + '0';

                    } while (0);
                }
                else
                {
                    do
                    {
                        if (Uart_Motor_Percentage == 100) //���г�
                        {
                            Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "100");
                            break;
                        }
                        else if (Uart_Motor_Percentage >= 10)
                            Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage / 10 + '0';

                        Uart_Tx_Array[Date_Position++] = Uart_Motor_Percentage % 10 + '0';

                    } while (0);
                }
                TxDataLength = Date_Position;
                break;
            default:
                break;
            }
            count = count + 4;
        } while (Uart_Rx_Array[count] != 0x00);
    }
    else if (Date_type == 3)
    {
        memset(Uart_Tx_Array, 0, sizeof(Uart_Tx_Array));
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " error \"memory error\" -5003 ");
        TxDataLength = Date_Position;
    }
    else
    {
        memset(Uart_Rx_Array, 0, sizeof(Uart_Rx_Array));
        memset(Uart_Tx_Array, 0, sizeof(Uart_Tx_Array));
        Date_Position = NULL;
        return;
    }

    
    TxDataLength = Date_Position;
    memset(Uart_Rx_Array, 0, sizeof(Uart_Rx_Array));
    Uart_Tx_Array[TxDataLength] = 13; //���з�

    Flag.Uart_tx = NULL;
    Uart_TX_Byte_Cnt = 0;              //��������
    Time_200ms_Cnt = TIME_200MS_VALUE; //���÷���ʱ��
    Flag.Time_200ms = NULL;
    IE2 &= ~UCA0RXIE;  //�رս����ж�
    IE2 |= UCA0TXIE;   //�򿪷����ж�
    IFG1 |= UCA0TXIFG; // �����жϱ�־�����뷢���жϳ���
}
