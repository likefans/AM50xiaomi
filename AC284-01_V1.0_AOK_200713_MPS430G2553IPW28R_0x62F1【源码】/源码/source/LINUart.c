/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *
 *  LINUart.c file
 *  功能描述: 
**********************************************************************************/
#include "Define.h" /* 全局宏定义 ------*/
#include "Extern.h" /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define BAUD_RATE_VALUE(var) ((10000 / ((var) / 100)) * SYSTEM_CLOCK)
/*
#define UART_RX_BYTE           7
#define UART_TX_BYTE           10 + 2
#define EUSART_NO_RX_TIME      4

#define EUSART_DATA_HEAD       0x9a                         // 接收信号头 ------
#define EUSART_DATA_UP         0xdd                         // 串口上行数据 ----
#define EUSART_DATA_STOP       0xcc                         // 串口停止数据 ----
#define EUSART_DATA_DOWN       0xee                         // 串口下行数据 ----
#define EUSART_DATA_LEARN      0xaa                         // 串口学习数据 ----
#define EUSART_DATA_CLEAR      0xa6                         // 串口清除数据 ----
#define EUSART_DATA_ORDER      0x0a                         // 串口命令标志 ----
#define EUSART_DATA_INQUIRE    0xcc                         // 串口查询标志 ----
                                                            //
#define INQUIRE_FUNCTION       0xCA                         // 查询功能状态 ----
#define EUSART_DATA_GOTO_PCT   0xDD                         // 运行到百分比 ----
#define EUSART_DATA_SET_SPEED  0xD9                         // 设定最高转速 ----
#define SET_DRY_CONTACT        0xD2                         // 设定手控方式 ----
#define SET_HAND_DRAG          0xD5                         // 设定手拉、方向等 
#define SET_BAUD_RATE          0xDA                         // 串口波特率 ------
                                                            //
#define EUSART_JIGGLE_UP       0x0d                         // 点动上行数据 ----
#define EUSART_JIGGLE_DOWN     0x0e                         // 点动下行数据 ----
#define EUSART_DATA_LIMIT_1    0x01                         // 中间限位点1 -----
#define EUSART_DATA_LIMIT_2    0x02                         // 中间限位点2 -----
#define EUSART_DATA_LIMIT_3    0x03                         // 中间限位点3 -----
#define EUSART_DATA_LIMIT_4    0x04                         // 中间限位点4 -----
#define SEND_DATA_HEAD         0xd8                         // 发送信号头 ------
*/
#define OUT_TEST_TIME 40

#define DATA_HEAD 0X9A
#define EUSART_NO_RX_TIME 4
/*********************************************命令分类*********************************************************/

#define COMMAND_TYPE_CONTROL 0X0A          //管状电机晾衣机
#define COMMAND_TYPE_PERCENT 0X0D          //只对有电子形成的产品控制百分比
#define COMMAND_TYPE_SETTING0 0X11         //用户可操作类设置：基本功能、速度、点动角度
#define COMMAND_TYPE_SETTING1 0X12         //用户可操作类设置：晾衣机风干、烘干、消毒时间
#define COMMAND_TYPE_SETTING2 0X13         //用户可操作类设置：风光雨感应等级
#define COMMAND_TYPE_USERDEBUG0 0X21       //安装调试类设置：遇阻电流/等级 、 手控方式 、 运行时间
#define COMMAND_TYPE_USERDEBUG1 0X22       //安装调试类设置：可选功能 、限位点
#define COMMAND_TYPE_CONFIGURE0 0X31       //出场配置类：固件版本
#define COMMAND_TYPE_CONFIGURE1 0X32       //出场配置类：机型参数
#define COMMAND_TYPE_CONFIGURE2 0X33       //出场配置类：设备类型
#define COMMAND_TYPE_CONFIGURE3 0X34       //出场配置类：序列号
#define COMMAND_TYPE_CHANGENET0 0X41       //无网关时控制命令：切换为协调器
#define COMMAND_TYPE_CHANGENET1 0X42       //无网关时控制命令：切换为路由器
#define COMMAND_TYPE_CHANGENET2 0X43       //无网关时控制命令：进入遥控配对
#define COMMAND_TYPE_CHANGENET3 0X44       //无网关时控制命令：删除所有遥控配对（单个窗帘）
#define COMMAND_TYPE_RF_CONTROL0 0X51      //遥控器控制命令：允许添加设备（5分钟，遥控器操作）
#define COMMAND_TYPE_RF_CONTROL1 0X52      //遥控器控制命令：停止添加设备（遥控器操作）
#define COMMAND_TYPE_GETSTATUS 0XA1        //工作状态查询
#define COMMAND_TYPE_GETPARA 0XA2          //工作参数查询，晾衣机风干、烘干、消毒时间
#define COMMAND_TYPE_GETCHANGEABLE 0XA3    // 用户可修改常用功能查询：基本功能、速度、点动角度
#define COMMAND_TYPE_GETDRIERPARA 0XA4     //晾衣机风干、烘干、消毒时间
#define COMMAND_TYPE_GETUSERDEBUGINFO 0XA5 //遇阻电流/登记、手控方式、运行时间
#define COMMAND_TYPE_GETCONFIG0INFO 0XB1   //出场配置类查询：固件版本
#define COMMAND_TYPE_GETCONFIG1INFO 0XB2   //出场配置类查询：型号参数
#define COMMAND_TYPE_GETCONFIG2INFO 0XB3   //出场配置类查询：设备类型
#define COMMAND_TYPE_GETCONFIG3INFO 0XB4   //出场配置类查询：序列号
#define COMMAND_TYPE_GETERRORINFO 0XA6     //鼓掌状态查询，保护信息
#define COMMAND_TYPE_WIFI 0XE1             //清网/加网
#define COMMAND_TYPE_PROTEST 0XE3

/********************************************************************************************************************************************************************************************/

/*************************************************子命令分类*****************************************************/
//#define COMMAND_TYPE_CONTROL           0X0A //管状电机晾衣机
#define COMMAND_TYPE_CONTROL_LENGTH 1
#define COMMAND_TYPE_CONTROL_UP 0XDD        //上行
#define COMMAND_TYPE_CONTROL_DDUP 0X0d      //点动上 （百叶类型产品用）
#define COMMAND_TYPE_CONTROL_STOP 0Xcc      //停止
#define COMMAND_TYPE_CONTROL_DOWN 0Xee      //下行
#define COMMAND_TYPE_CONTROL_DDDOWN 0X0e    //点动下 （百叶类型产品用）
#define COMMAND_TYPE_CONTROL_LIGHT 0X2a     //照明
#define COMMAND_TYPE_CONTROL_DRY 0X2b       //风干
#define COMMAND_TYPE_CONTROL_BAKEOUT 0X2c   //烘干
#define COMMAND_TYPE_CONTROL_DISINFECT 0X2d //消毒
#define COMMAND_TYPE_CONTROL_MUSIC 0X2e     //音乐
#define COMMAND_TYPE_CONTROL_POWER 0X2f     //电源
#define COMMAND_TYPE_CONTROL_BEEP 0Xaa      //提示命令（电机卫东或者蜂鸣器提示，例如设备在哪应用）
/*******/
//#define COMMAND_TYPE_PERCENT           0X0D //只对有电子形成的产品控制百分比
#define COMMAND_TYPE_PERCENT_LENGTH 3
/*******/
//#define COMMAND_TYPE_SETTING0          0X11 //用户可操作类设置：基本功能、速度、点动角度
#define COMMAND_TYPE_SETTING0_LENGTH 3
//data0:
#define COMMAND_TYPE_SETTING0_HANDLE 0X01    //0：有手拉 1：无手拉
#define COMMAND_TYPE_SETTING0_DIRECTION 0X02 //0: 默认方向 1：反向
#define COMMAND_TYPE_SETTING0_INCH 0X04      //0: 连动 1：点动
#define COMMAND_TYPE_SETTING0_RESTART 0X08   // 0:无操作  1：系统重启
#define COMMAND_TYPE_SETTING0_FREE0 0X10     //保留
#define COMMAND_TYPE_SETTING0_FREE1 0X20     //保留
#define COMMAND_TYPE_SETTING0_FREE2 0X40     //保留
#define COMMAND_TYPE_SETTING0_FREE3 0X40     //保留
//data1: 运行速度RPM
//data2:
/*点动时间（0.01）
*例如：数据3为255
点动时间为255*0.1 = 2.55S
*/
/*******/
//#define COMMAND_TYPE_SETTING1          0X12 //用户可操作类设置：晾衣机风干、烘干、消毒时间
#define COMMAND_TYPE_SETTING1_LENGTH 3
//data0: 风干时间（分钟）
//data1: 烘干时间（分钟）
//data2: 消毒时间（分钟）
/*******/
//#define COMMAND_TYPE_SETTING2          0X13 //用户可操作类设置：风光雨感应等级
#define COMMAND_TYPE_SETTING2_LENGTH 3
//data0: 风感等级0~9  0：关闭 1：最灵敏 9：最不灵敏
//data1: 光感等级0~9  0：关闭 1：最灵敏 9：最不灵敏
//data2: 雨感等级0~9  0：关闭 1：最灵敏 9：最不灵敏
/*******/

//#define COMMAND_TYPE_USERDEBUG0       0X21            //安装调试类设置：遇阻电流/等级 、 手控方式 、 运行时间
#define COMMAND_TYPE_USERDEBUG0_LENGTH 5
//data0: 遇阻电流（mA）
//data1: 手控线方式：1：正常 2：上停下停单键循环 3：上上停，上下切换 4：松手停 5：任意停
//data2: 遇阻灵敏度 01：高灵敏度 02:中灵敏度 03：低灵敏度
//data3~data4:最长运行时间（秒） data3：H  data4:L   0xfff0:无时间限制
//data0~ data4 为0时代表该项不进行操作

//#define COMMAND_TYPE_USERDEBUG1       0X22            //安装调试类设置：可选功能 、限位点
#define COMMAND_TYPE_USERDEBUG1_LENGTH 3
//data0:
#define COMMAND_TYPE_USERDEBUG1_SLOWSTART 0X01      //0:有慢启动 1：无慢启动
#define COMMAND_TYPE_USERDEBUG1_SPACETOJOURNEY 0X02 //0:到行程有间隙 1：到行程不留间隙
#define COMMAND_TYPE_USERDEBUG1_STOPTOJOURNEY 0X04  //0:到行程停止 1：遇阻停止
#define COMMAND_TYPE_USERDEBUG1_MEMORYJOURNEY 0X08  //0:记忆行程 1：不记忆行程
#define COMMAND_TYPE_USERDEBUG1_CONTACTSYNCHRO 0X10 //0:手控线同步 1：不同步
#define COMMAND_TYPE_USERDEBUG1_FREE00 0X20         //0:保留
#define COMMAND_TYPE_USERDEBUG1_FREE01 0X40         //0:保留
#define COMMAND_TYPE_USERDEBUG1_FREE02 0X80         //0:保留
//data1: (data1与data2 所有bit 0:无操作 1：有效)
#define COMMAND_TYPE_USERDEBUG1_SETUPLIMIT 0X01   //设置上限位
#define COMMAND_TYPE_USERDEBUG1_SETDOWNLIMIT 0X02 //设置下限位
#define COMMAND_TYPE_USERDEBUG1_ADJUSTLIMIT 0X04  //微调限位点
#define COMMAND_TYPE_USERDEBUG1_FREE10 0X08       //保留
#define COMMAND_TYPE_USERDEBUG1_FREE11 0X10       //保留
#define COMMAND_TYPE_USERDEBUG1_FREE12 0X20       //0:保留
#define COMMAND_TYPE_USERDEBUG1_FREE13 0X40       //0:保留
#define COMMAND_TYPE_USERDEBUG1_FREE14 0X80       //0:保留
//data2:
#define COMMAND_TYPE_USERDEBUG1_RESETFACTORY 0X01     //设置上限位
#define COMMAND_TYPE_USERDEBUG1_DELETEALLRFID 0X02    //设置下限位
#define COMMAND_TYPE_USERDEBUG1_DELETEALLLIMIT 0X04   //微调限位点
#define COMMAND_TYPE_USERDEBUG1_DELETEIDANDLIMIT 0X08 //保留
#define COMMAND_TYPE_USERDEBUG1_FREE20 0X10           //保留
#define COMMAND_TYPE_USERDEBUG1_FREE21 0X20           //0:保留
#define COMMAND_TYPE_USERDEBUG1_FREE22 0X40           //0:保留
#define COMMAND_TYPE_USERDEBUG1_FREE23 0X80           //0:保留

/*******/
//#define COMMAND_TYPE_CONFIGURE0         0X31 //出场配置类：固件版本
#define COMMAND_TYPE_CONFIGRE0_LENGTH 6
//data0~data5: 六位数ASIC码 例如固件版本160610  ：‘1’‘6’‘0’‘6’‘1’‘0’
/*******/
#define COMMAND_TYPE_CONFIGURE1 0X32     //出场配置类：机型参数
#define COMMAND_TYPE_CONFIGRE1_LENGTH 14 //N:这里需要注意具体是几个，例如开合帘：AM68-1/80-EM-P
/*******/
//#define COMMAND_TYPE_CONFIGURE2         0X33 //出厂配置类：设备类型
#define COMMAND_TYPE_CONFIGRE2_LENGTH 1
#define COMMAND_TYPE_CONFIGRE2_KAIHELIAN 0X01
#define COMMAND_TYPE_CONFIGRE2_KAICHUANGJI 0X02
#define COMMAND_TYPE_CONFIGRE2_DRIERLIGHT 0X03
#define COMMAND_TYPE_CONFIGRE2_DRIERPROFESSION 0X04
#define COMMAND_TYPE_CONFIGRE2_DRIERLUXURY 0X05
#define COMMAND_TYPE_CONFIGRE2_SCREEN 0X06           //投影银幕
#define COMMAND_TYPE_CONFIGRE2_SCREENDROPHANGER 0X07 //银幕吊架
#define COMMAND_TYPE_CONFIGRE2_AWNING 0X08           //遮阳篷
#define COMMAND_TYPE_CONFIGRE2_BLIND 0X09            //百叶帘 (AM16 24 54 55百叶专用电机)
#define COMMAND_TYPE_CONFIGRE2_FENGGUANGYU 0X0A      //风光雨传感器
#define COMMAND_TYPE_CONFIGRE2_FENGGANQI 0X0B        //风感器
#define COMMAND_TYPE_CONFIGRE2_WENSHI 0X0C           //温湿度传感器
#define COMMAND_TYPE_CONFIGRE2_TIMEINGREMOTE 0X0D    //定时发射器
#define COMMAND_TYPE_CONFIGRE2_HANDLEREMOTE 0X0E     //手持式发射器
#define COMMAND_TYPE_CONFIGRE2_WALLREMOTE 0X0F       //墙壁贴发射器
/*******/
//#define COMMAND_TYPE_CONFIGURE3         0X34 //出场配置类：序列号  配置流水号
#define COMMAND_TYPE_CONFIGURE3_LENGTH 13 // N:ASIC码  如：AM00002477700
/*******/
/*define COMMAND_TYPE_CHANGENET0          0X41 //无网关时控制命令：切换为协调器
#define COMMAND_TYPE_CHANGENET1          0X42 //无网关时控制命令：切换为路由器
#define COMMAND_TYPE_CHANGENET2          0X43 //无网关时控制命令：进入遥控配对
#define COMMAND_TYPE_CHANGENET3          0X44 //无网关时控制命令：删除所有遥控配对（单个窗帘）
#define COMMAND_TYPE_RF_CONTROL0         0X51 //遥控器控制命令：允许添加设备（5分钟，遥控器操作）      
#define COMMAND_TYPE_RF_CONTROL1         0X52 //遥控器控制命令：停止添加设备（遥控器操作）    */

/*#define COMMAND_TYPE_GETSTATUS             0XA1 //工作状态查询
#define COMMAND_TYPE_GETPARA             0XA2 //工作参数查询，晾衣机风干、烘干、消毒时间
#define COMMAND_TYPE_GETCHANGEABLE       0XA3 // 用户可修改常用功能查询：基本功能、速度、点动角度
#define COMMAND_TYPE_GETDRIERPARA        0XA4 //晾衣机风干、烘干、消毒时间
#define COMMAND_TYPE_GETUSERDEBUGINFO    0XA5 //遇阻电流/登记、手控方式、运行时间
#define COMMAND_TYPE_GETCONFIG0INFO       0XB1//出场配置类查询：固件版本
#define COMMAND_TYPE_GETCONFIG1INFO       0XB2//出场配置类查询：固件版本
#define COMMAND_TYPE_GETCONFIG2INFO       0XB3//出场配置类查询：固件版本
#define COMMAND_TYPE_GETCONFIG3INFO       0XB4//出场配置类查询：固件版本
#define COMMAND_TYPE_GETERRORINFO         0XA6//鼓掌状态查询，保护信息*/

/***************/
//#define COMMAND_TYPE_WIFI                 0XE1 //清网/加网
#define COMMAND_TYPE_WIFI_LENGTH 1
#define COMMAND_TYPE_WIFI_WIFIOUT 0X01
#define COMMAND_TYPE_WIFI_WIFIIN 0X02
#define CLR_NET_FINISHI_DATA 0x01 /* 清网成功命令 --- */
#define ADD_NET_FINISHI_DATA 0x02 /* 加网成功命令 --- */

#define COMMAND_TYPE_PAUSE 0 //暂停
#define COMMAND_TYPE_OPEN 1  //开
#define COMMAND_TYPE_CLOSE 2 //关

/* 内部函数 ---------------------------------------------------------------------*/
void UartDataDisposal(void);
u8 JourneyPercentCount(void);
/* 内部变量 ---------------------------------------------------------------------*/
static u8 Uart_TX_Byte_Cnt;            // 字节计数器
static u8 Uart_Motor_Percentage = 100; //串口发送百分比值
static u8 Uart_Rx_Array[50];
static u8 Uart_Tx_Array[50];
//static u8 Uart_NO_Rx_Cnt;
//static u8 Inquire_Function_Flag;
static u8 TxDataLength;
u8 Status_Buffer; //储存电机状态
u8 PercentBuff;

const u16 Baud_Rate_Array[BAUD_RATE_NUM] =
    {
        1200, 2400, 4800, 9600, 19200};
/**********************************************************************************
  函数名:   LinUartSetting
  功  能:   Uart 设置
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
  函数名:   DataCRC
  功  能:   循环异或效验
  输  入:   需效验的数组首址、效验数据的个数
  输  出:   空    
  返  回:   异或的结果
  描  述：
*********************************************************************************/
u8 DataCRC(u8 array[])
{
    if (strncmp((char *)array, "down set_properties", sizeof("down set_properties") - 1) == 0) //比较
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
  函数名:   MotorControl
  功  能:   
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
  函数名:   MotorControl
  功  能:   
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
  函数名:   Buff_Handle
  功  能:   
 *********************************************************************************/
u8 Buff_Handle(u8 count, u8 date[], char str[])
{
    u8 i;
    for (i = 0; i < strlen(str); i++)
        date[count + i] = str[i];
    return i + count;
}
/*********************************************************************************
  函数名:   UartStartupTX
  功  能:   准备数据，启动发送
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：
*********************************************************************************/
void UartStartupTX(void)
{
    u8 Date_Position;
    RS485_TX();

    Date_Position = 0;                               //初始化位置
    memset(Uart_Tx_Array, 0, sizeof(Uart_Tx_Array)); //清空发送数组

    if (Flag.Reporting == TRUE) //定时发送状态数据
    {
        Flag.Reporting = NULL;

        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "properties_changed 2 1 0");
        if (Flag.model_Stat_Direction == TRUE)
        {
            if (Motor_Array[0].Motor_Flag.Direction == FALSE) //发送电机反转情况
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
            if (Motor_Array[0].Motor_Flag.Journey_Set_OK) //有行程
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
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "get_down"); //发送get_down
        TxDataLength = Date_Position;
    }
    else if (Flag.model == TRUE)
    {
        Flag.model = NULL;
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "model aok98.curtain.qlam50"); //发送model
        TxDataLength = Date_Position;
        Flag.Wifi_cnt2 = TRUE;
    }
    else if (Flag.VERSION == TRUE)
    {
        Flag.VERSION = NULL;
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "mcu_version 0001"); //清网配网
        TxDataLength = Date_Position;
    }
    else if (Flag.NetEvent == TRUE)
    {
        Flag.NetEvent = NULL;
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "restore"); //清网配网
        TxDataLength = Date_Position;
        LedStatusUpdate(CLR_NET_FINISHI);
    }
    else if (Flag.WifiTest == TRUE)
    {
        Flag.WifiTest = NULL;
        Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, "factory"); //进入厂测
        TxDataLength = Date_Position;
        LedStatusUpdate(CLR_NET_FINISHI);
    }

    Status_Buffer = Motor_Status;
    Uart_Tx_Array[TxDataLength] = 13; //换行符
    Uart_TX_Byte_Cnt = 0;             // 发送字节数清0
    IE2 |= UCA0TXIE;                  // Enable USCI_A0 TX interrupt
    IE2 &= ~UCA0RXIE;                 // Disable USCI_A0 RX interrupt
    IFG1 |= UCA0TXIFG;                // 设置中断标志，进入发送中断程序
}
/*********************************************************************************
  函数名:   Uart_TX_Interrupt
  功  能:   串口发送中断
*********************************************************************************/
// USCI A0/B0 Transmit ISR
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
    UCA0TXBUF = Uart_Tx_Array[Uart_TX_Byte_Cnt]; // 没完继续发送
    Flag.Time_200ms = NULL;
    Time_200ms_Cnt = TIME_200MS_VALUE;
    if (++Uart_TX_Byte_Cnt > TxDataLength) // 判断是否已发送完
    {
        Uart_TX_Byte_Cnt = 0; // 发送次数清0
        TxDataLength = 0;
        RS485_RX();
        IE2 &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
        IE2 |= UCA0RXIE;  // Enable USCI_A0 RX interrupt
    }
}
/*********************************************************************************
  函数名:   Uart_RX_Interrupt
  功  能:   串口接收中断
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
  函数名:   UartControl
  功  能:   
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
        Motor_Array[0].Motor_Flag.StatusSwitch = TRUE; //更新目标位置
    }
}

void DisposalPercent(u8 percent)
{

    Motor_Array[0].Motor_Flag.StatusSwitch = TRUE; //更新目标位置

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
  函数名:   UartDataDisposal
  功  能:   串口接收的数据处理
  输  入:   串口接收的数据缓存数组首址
  输  出:   空    
  返  回:   空
  描  述：
**********************************************************************************/
void UartDataDisposal(void)
{
    u8 Date_type, Date_Position;
    u8 count = 22;
    Date_type = DataCRC(Uart_Rx_Array); //查看下行数据

    Date_Position = 0;
    Flag.Time_50ms = 0;
    Time_50ms_Cnt = TIME_50MS_VALUE; //先答复UP/DOWN等控制语句，然后反馈MOTOR状态
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
                    Motor_Array[0].Motor_Flag.Direction = TRUE; /*电机反转*/
                    count = count + 3;
                }
                else
                {
                    Motor_Array[0].Motor_Flag.Direction = FALSE; /*电机反转*/
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
                for (Date_type = 0; Date_type < 3; Date_type++) //获取百分比数字
                    if (Uart_Rx_Array[24 + Date_type] <= '9' && Uart_Rx_Array[24 + Date_type] >= '0')
                    {
                        Uart_Motor_Percentage = Uart_Motor_Percentage * 10 + Uart_Rx_Array[24 + Date_type] - '0';
                    }
                    else
                        break;

                Flag.Goto_PCT = TRUE;
                if (Motor_Array[0].Motor_Flag.Journey_Set_OK) //判断有无行程
                {
                    Flag_Motor.PercentUsual = NULL;
                    DisposalPercent(100 - Uart_Motor_Percentage); //执行百分比
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
                Flag.modle_Stat_Percentage = TRUE; //上报数据
                Date_Position = Buff_Handle(Date_Position, Uart_Tx_Array, " 2 4 0 ");
                if (Motor_Array[0].Motor_Flag.Journey_Set_OK) //有行程
                {
                    Uart_Motor_Percentage = 100 - JourneyPercentCount();
                    do
                    {
                        if (Uart_Motor_Percentage == 100) //无行程
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
                        if (Uart_Motor_Percentage == 100) //无行程
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
    Uart_Tx_Array[TxDataLength] = 13; //换行符

    Flag.Uart_tx = NULL;
    Uart_TX_Byte_Cnt = 0;              //发送清零
    Time_200ms_Cnt = TIME_200MS_VALUE; //重置发送时间
    Flag.Time_200ms = NULL;
    IE2 &= ~UCA0RXIE;  //关闭接收中断
    IE2 |= UCA0TXIE;   //打开发送中断
    IFG1 |= UCA0TXIFG; // 设置中断标志，进入发送中断程序
}
