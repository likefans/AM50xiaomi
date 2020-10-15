/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 
 *  Initialize.C file
 *  功能描述: 系统初始化
 *********************************************************************************/
#include "Define.h" /* 全局宏定义 ------*/
#include "Extern.h" /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define ID_MAX_AMOUNT 10     /*可保存ID的最大数量*/
#define ID_LENGTH 6          /* 数据长度 --------*/
#define ID_SEGMENT 0x1000    /* 数据长度 --------*/
#define DATA_SEGMENT 0x1040  /* 数据长度 --------*/
#define PLACE_SEGMENT 0x1080 /* 数据长度 --------*/
/* 内部变量 ---------------------------------------------------------------------*/
u8 Flash_Buffer[64];
/* 内部函数 ---------------------------------------------------------------------*/
void FlashReadSegment(volatile u8 *buf, u16 addr);
void FlashWriteSegment(volatile u8 *buf, u16 addr);
/**********************************************************************************
  函数名:  CompID
  功  能:   
**********************************************************************************/
u8 CompID(u8 data[])
{
    u8 i;
    u8 flag;
    u8 place;

    place = 1;
    flag = NULL;

    FlashReadSegment(Flash_Buffer, ID_SEGMENT); // 读数据

    for (i = 0; i < 64;)
    {
        if (Flash_Buffer[i] == data[0])
        {
            if (Flash_Buffer[i + 1] == data[1])
            {
                if (Flash_Buffer[i + 2] == data[2])
                {
                    if (Flash_Buffer[i + 3] == data[3])
                    {
                        flag = place; /*当前ID所在的序号 -*/
                        /* 两个通道中有1个字节中的1位同时为1时说明已经对号码*/
                        if ((Flash_Buffer[i + 4] & data[4]) || (Flash_Buffer[i + 5] & data[5]))
                        {
                            flag |= 0x80; /* 有相同通道标志 --*/
                            break;
                        }
                    }
                }
            }
        }
        i += ID_LENGTH;
        place++;
    }

    return (flag);
}
/**********************************************************************************
  函数名:   SEAVE_ID
  功  能:   
**********************************************************************************/
void SaveID(u8 data[])
{
    u8 i;
    u8 flag;
    u8 place;

    place = NULL;
    flag = NULL;

    FlashReadSegment(Flash_Buffer, ID_SEGMENT); // 读数据

    // 比较是否已存
    for (i = 0; i < 60;) // 扫描整个ID区域
    {
        if (RF_HEAD != Flash_Buffer[i])
        {
            place = i; //  保存当前位置
            flag = TRUE;
            break; // 没有保存过退出
        }
        i += ID_LENGTH;
    }
    if (flag == TRUE)
    {
        for (i = 0; i < 6; i++)
        {
            Flash_Buffer[place++] = data[i];
        }
        FlashWriteSegment(Flash_Buffer, ID_SEGMENT); // 写数据
    }
    else if (i == 60)
    {
        place = 60 - ID_LENGTH; // 覆盖
        for (i = 0; i < 6; i++)
        {
            Flash_Buffer[place++] = data[i];
        }
        FlashWriteSegment(Flash_Buffer, ID_SEGMENT); // 写数据
    }
}
/**********************************************************************************
  函数名:  	SaveChannel
  功  能:   
**********************************************************************************/
void SaveChannel(u8 array[], u8 ch_addr)
{
    u8 i;
    u8 j;

    j = ch_addr & 0x0f;

    if (j > 10)
    {
        return;
    }

    FlashReadSegment(Flash_Buffer, ID_SEGMENT); // 读数据

    i = (j * ID_LENGTH) - 2; // 得到通道位置
    Flash_Buffer[i] |= array[4];
    Flash_Buffer[i + 1] |= array[5]; // 添加通道地址

    FlashWriteSegment(Flash_Buffer, ID_SEGMENT); // 写数据
}
/**********************************************************************************
  函数名:  	DeleteSingleChannel
  功  能:   
**********************************************************************************/
void DeleteChannel(u8 array[], u8 ch_addr)
{
    u8 i;
    u8 j;

    j = ch_addr & 0x0f;

    if (j > 10)
    {
        return;
    }

    FlashReadSegment(Flash_Buffer, ID_SEGMENT); // 读数据

    i = (j * ID_LENGTH) - 2; // 得到通道位置
    Flash_Buffer[i] &= ~array[4];
    Flash_Buffer[i + 1] &= ~array[5]; // 删除通道地址

    if ((Flash_Buffer[i] == 0) && (Flash_Buffer[i + 1] == 0)) // 通道是否等于0
    {
        i -= 4;
        for (j = 0; j < ID_LENGTH; j++)
        {
            Flash_Buffer[i++] = 0xff;
        }
    }

    FlashWriteSegment(Flash_Buffer, ID_SEGMENT); // 写数据
}
/**********************************************************************************
  函数名:  	DeleteAllID
  功  能:   
**********************************************************************************/
void DeleteAllID(u8 check)
{
    unsigned char *Flash_ptr; // Flash pointer

    if (check == 0xee)
    {
        WDTCTL = WDT_ARST_16;
        _DINT(); // 关总中断
        WDTCTL = WDTPW + WDTHOLD;

        Flash_ptr = (unsigned char *)(ID_SEGMENT); // Initialize Flash pointer D
        FCTL1 = FWKEY + ERASE;                     // Set Erase bit
        FCTL3 = FWKEY;
        *Flash_ptr = 0; // Dummy write to erase Flash segment

        JourneyDelete();
        Motor_Array[0].Motor_Flag.Direction = 0;
        FlashWriteData(0xee);

        Flash_ptr = (unsigned char *)(PLACE_SEGMENT); // Initialize Flash pointer D
        FCTL1 = FWKEY + ERASE;                        // Set Erase bit
        FCTL3 = FWKEY;
        *Flash_ptr = 0; // Dummy write to erase Flash segment

        FCTL1 = FWKEY;        // Clear WRT bit
        FCTL3 = FWKEY + LOCK; // Set LOCK bit

        _EINT(); // 开总中断
    }
}
/**********************************************************************************
  函数名:  FlashReadSegment
  功  能:   
  输  入:  空
  输  出:  空
  返  回:  空
  描  述：  
**********************************************************************************/
void FlashReadSegment(volatile u8 *buf, u16 addr)
{
    unsigned char *Flash_ptr; // Flash pointer
    unsigned char i;

    Flash_ptr = (unsigned char *)(addr); // 取页面地址
    for (i = 0; i < 64; i++)
    {
        *buf++ = *Flash_ptr++;
    }
}
/**********************************************************************************
  函数名:  FlashWriteSegment
  功  能:   
**********************************************************************************/
void FlashWriteSegment(volatile u8 *buf, u16 addr)
{
    unsigned char *Flash_ptr; // Flash pointer
    unsigned char i;

    WDTCTL = WDT_ARST_16;
    _DINT(); // 关总中断
    WDTCTL = WDTPW + WDTHOLD;

    Flash_ptr = (unsigned char *)(addr); // Initialize Flash pointer D
    FCTL1 = FWKEY + ERASE;               // Set Erase bit
    FCTL3 = FWKEY;
    *Flash_ptr = 0; // Dummy write to erase Flash segment

    FCTL1 = FWKEY + WRT; // Set WRT bit for write operation
    for (i = 0; i < 64; i++)
    {
        WDTCTL = WDT_ARST_16;
        *Flash_ptr++ = *buf++; // Write value to flash
    }

    FCTL1 = FWKEY;        // Clear WRT bit
    FCTL3 = FWKEY + LOCK; // Set LOCK bit

    _EINT(); // 开总中断
}
/**********************************************************************************
  函数名:  FlashWriteData
  功  能:   
**********************************************************************************/
void FlashWriteData(u8 check)
{
    u8 i;
    Bit_TypeDef System;
    Bit_TypeDef Journey;

    if (check == 0xee)
    {
        // 保存系统标志位
        System.Flag_Bit.b0 = Motor_Array[0].Motor_Flag.Direction;
        System.Flag_Bit.b1 = Motor_Array[0].Motor_Flag.Hand_Drag_Status;
        System.Flag_Bit.b2 = 0;
        System.Flag_Bit.b3 = Motor_Array[0].Motor_Flag.No_Slow_Start;
        System.Flag_Bit.b4 = Motor_Array[0].Motor_Flag.Serpentine_Curtain;
        System.Flag_Bit.b5 = Motor_Array[0].Motor_Flag.To_Journey_Meet_Plugb;
        System.Flag_Bit.b6 = Motor_Array[0].Motor_Flag.PowerDown_Remember;
        System.Flag_Bit.b7 = Motor_Array[0].Motor_Flag.Manual_Control_Synchro;
        // 保存行程标志位
        Journey.Flag_Bit.b0 = Motor_Array[0].Motor_Flag.Journey_Direction;
        Journey.Flag_Bit.b1 = Motor_Array[0].Motor_Flag.Journey_Set_OK;
        Journey.Flag_Bit.b2 = Motor_Array[0].Motor_Flag.Single_Journey;
        Journey.Flag_Bit.b3 = 0;
        Journey.Flag_Bit.b4 = 0;
        Journey.Flag_Bit.b5 = 0;
        Journey.Flag_Bit.b6 = 0;
        Journey.Flag_Bit.b7 = 0;

        Flash_Buffer[0] = System.Flag_Byte;
        Flash_Buffer[1] = Journey.Flag_Byte;
        Flash_Buffer[2] = Hall_Pulse_Amount >> 8;
        Flash_Buffer[3] = Hall_Pulse_Amount;

        Flash_Buffer[4] = Uart_ID;
        Flash_Buffer[5] = Uart_CH_L;
        Flash_Buffer[6] = Uart_CH_H;
        Flash_Buffer[7] = Dry_Contact_Control_Mode;
        Flash_Buffer[8] = Motor_Max_Speed;
        Flash_Buffer[9] = Uart_Baud_Rate;

        //固件版本
        Flash_Buffer[10] = HardWareVersion[0];
        Flash_Buffer[11] = HardWareVersion[1];
        Flash_Buffer[12] = HardWareVersion[2];
        Flash_Buffer[13] = HardWareVersion[3];
        Flash_Buffer[14] = HardWareVersion[4];
        Flash_Buffer[15] = HardWareVersion[5];

        Flash_Buffer[16] = ProductType;

        for (i = 0; i < MAX_PRODUCT_PARA_BYTES; i++)
            Flash_Buffer[17 + i] = ProductPara[i];

        Flash_Buffer[17 + MAX_PRODUCT_PARA_BYTES] = 0x5d;
        Flash_Buffer[18 + MAX_PRODUCT_PARA_BYTES] = 0;

        for (i = 0; i < (18 + MAX_PRODUCT_PARA_BYTES); i++)
        {
            Flash_Buffer[18 + MAX_PRODUCT_PARA_BYTES] ^= Flash_Buffer[i];
        }
        FlashWriteSegment(Flash_Buffer, DATA_SEGMENT); // 写数据
    }
}
/**********************************************************************************
  函数名:  FlashReadData
  功  能:   
**********************************************************************************/
void FlashReadData(void)
{
    u8 i;
    u8 j = NULL;
    Bit_TypeDef System;
    Bit_TypeDef Journey;

    FlashReadSegment(Flash_Buffer, DATA_SEGMENT); // 读数据

    for (i = 0; i < 19 + MAX_PRODUCT_PARA_BYTES; i++)
    {
        j ^= Flash_Buffer[i];
    }

    if (NULL == j)
    {
        if (0x5d == Flash_Buffer[17 + MAX_PRODUCT_PARA_BYTES])
        {
            System.Flag_Byte = Flash_Buffer[0];
            Journey.Flag_Byte = Flash_Buffer[1];

            Motor_Array[0].Motor_Flag.Direction = System.Flag_Bit.b0;
            Motor_Array[0].Motor_Flag.Hand_Drag_Status = System.Flag_Bit.b1;
            Motor_Array[0].Motor_Flag.No_Slow_Start = System.Flag_Bit.b3;
            Motor_Array[0].Motor_Flag.Serpentine_Curtain = System.Flag_Bit.b4;
            Motor_Array[0].Motor_Flag.To_Journey_Meet_Plugb = System.Flag_Bit.b5;
            Motor_Array[0].Motor_Flag.PowerDown_Remember = System.Flag_Bit.b6;
            Motor_Array[0].Motor_Flag.Manual_Control_Synchro = System.Flag_Bit.b7;

            if (Motor_Array[0].Motor_Flag.PowerDown_Remember) // 断电是否需要记忆
            {
                Motor_Array[0].Motor_Flag.Journey_Direction = Journey.Flag_Bit.b0;
                Motor_Array[0].Motor_Flag.Journey_Set_OK = Journey.Flag_Bit.b1;
                Motor_Array[0].Motor_Flag.Single_Journey = Journey.Flag_Bit.b2;

                Hall_Pulse_Amount = Flash_Buffer[2];
                Hall_Pulse_Amount <<= 8;
                Hall_Pulse_Amount |= Flash_Buffer[3];
            }

            Uart_ID = Flash_Buffer[4];
            Uart_CH_L = Flash_Buffer[5];
            Uart_CH_H = Flash_Buffer[6];
            Dry_Contact_Control_Mode = Flash_Buffer[7];
            Motor_Max_Speed = Flash_Buffer[8];
            Uart_Baud_Rate = (Uart_Baud_Rate_TypeDef)Flash_Buffer[9];

            //固件版本
            HardWareVersion[0] = Flash_Buffer[10];
            HardWareVersion[1] = Flash_Buffer[11];
            HardWareVersion[2] = Flash_Buffer[12];
            HardWareVersion[3] = Flash_Buffer[13];
            HardWareVersion[4] = Flash_Buffer[14];
            HardWareVersion[5] = Flash_Buffer[15];

            ProductType = Flash_Buffer[16];

            for (i = 0; i < MAX_PRODUCT_PARA_BYTES; i++)
                ProductPara[i] = Flash_Buffer[17 + i];
        }
    }
}
/**********************************************************************************
  函数名:  FlashWritePlace
  功  能:   
**********************************************************************************/
void FlashWritePlace(void)
{
    u8 i;
    u8 temp = 0;

    Flash_Buffer[0] = Hall_Status_Buffer;
    Flash_Buffer[1] = Hall_Pulse_Amount_Cnt >> 8;
    Flash_Buffer[2] = Hall_Pulse_Amount_Cnt;
    Flash_Buffer[3] = 0xa5;

    for (i = 0; i < 4; i++)
    {
        temp ^= Flash_Buffer[i];
    }

    Flash_Buffer[4] = temp;

    FlashWriteSegment(Flash_Buffer, PLACE_SEGMENT); // 写数据
}
/**********************************************************************************
  函数名:  FlashReadPlace
  功  能:   
**********************************************************************************/
void FlashReadPlace(void)
{
    u8 i;
    u8 j = NULL;

    FlashReadSegment(Flash_Buffer, PLACE_SEGMENT); // 读数据
    for (i = 0; i < 5; i++)
    {
        j ^= Flash_Buffer[i];
    }

    if (NULL == j)
    {
        if (0xa5 == Flash_Buffer[3])
        {
            Hall_Status_Buffer = Flash_Buffer[0];

            Hall_Pulse_Amount_Cnt = Flash_Buffer[1];
            Hall_Pulse_Amount_Cnt <<= 8;
            Hall_Pulse_Amount_Cnt |= Flash_Buffer[2];
        }
    }
}