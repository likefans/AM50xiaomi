/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *
 *  Define.h file
 *  功能描述: 全局宏定义
**********************************************************************************/
#ifndef Define_h
#define Define_h

/* 头文件 -------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "Typedef.h" /* 类型定义头文件 --*/
#include "msp430g2553.h"
/* 全局宏定义 ---------------------------------------------------------------*/
//    #define NULL                   0
//    #define TRUE                   1

#define HALL_DETECT
#define RF_DECODE_TIME 40

#define MOTOR_NUM 1
#define SYSTEM_CLOCK 16
#define SYSTEM_REFERENCE_TIME 2 // 系统基准时间
#define SYSTEM_TIME_VALUE SYSTEM_REFERENCE_TIME * 1000

//#define DEFAULT_UNREMEMBER     // 默认不记忆行程
//#define GE_NING_SI_DING_ZHI    // 是否格宁斯定制
//#define KEY_CARD_CONTROL       // 房卡控制

// 最大电流
#define MEET_PLUG_CURRENT 1000
#define MAX_OUT_CURRENT 1300

// 最高转速
#define MAX_SPEED_130 130
#define MAX_SPEED_100 100
#define MAX_SPEED_80 80
// 最大最小圈数
#define HALL_PULSE_MAX 65000
#define HALL_PULSE_MIN 350
#define MIN_JOURNEY_VALUE 10000
// 最大转速与占空比
#define MAX_SPEED 130
#define MIN_SPEED 30
#define MAX_DUTY ((SYSTEM_CLOCK * 625) / 10)
#define MIN_DUTY 150

// 系统时间
#define MOTOR_CONTROL_TIME 26

#define TIME_200US_VALUE (400 / RF_DECODE_TIME)
#define TIME_4MS_VALUE (4 / SYSTEM_REFERENCE_TIME)
#define TIME_10MS_VALUE (MOTOR_CONTROL_TIME / SYSTEM_REFERENCE_TIME)
#define TIME_50MS_VALUE (45 / SYSTEM_REFERENCE_TIME)
#define TIME_100MS_VALUE (122 / SYSTEM_REFERENCE_TIME)
#define TIME_500MS_VALUE (500 / SYSTEM_REFERENCE_TIME)
#define TIME_1S_VALUE (1000 / 500)

/*WIfi 时间控制*/
#define TIME_200MS_VALUE (200 / SYSTEM_REFERENCE_TIME)
#define TIME_1S_WiFi 5
/* 电机运行时间 ----*/
#define MOTOR_RUN_TIME 5 /* 最长运行 5分钟 --*/
#define MOTOR_RUN_CNT_VALUE (MOTOR_RUN_TIME * (60000 / (TIME_50MS_VALUE * SYSTEM_REFERENCE_TIME)))
#define MOTOR_RUN_5S_VALUE (MOTOR_RUN_TIME * (1000 / (TIME_50MS_VALUE * SYSTEM_REFERENCE_TIME)))
#define MOTOR_SWITCH_LATIME (300 / (TIME_50MS_VALUE * SYSTEM_REFERENCE_TIME))

/* IO 端口定义 --------------------------------------------------------------*/
#define RS485 0    /* P1.0-------------*/
#define BLUE_LED 6 /* P3.6-------------*/
#define RED_LED 7  /* P3.7-------------*/
#define RF_EN 5    /* P3.5-------------*/

#define IN1_H 1 /* P2.1-------------*/
#define IN1_L 2 /* P2.2-------------*/
#define IN2_H 2 /* P3.2-------------*/
#define IN2_L 3 /* P3.3-------------*/

#define IN1_H_1() (P2OUT |= (1 << IN1_H))
#define IN1_H_0() (P2OUT &= ~(1 << IN1_H))
#define IN1_L_1() (P2OUT |= (1 << IN1_L))
#define IN1_L_0() (P2OUT &= ~(1 << IN1_L))

#define IN2_H_1() (P3OUT |= (1 << IN2_H))
#define IN2_H_0() (P3OUT &= ~(1 << IN2_H))
#define IN2_L_1() (P3OUT |= (1 << IN2_L))
#define IN2_L_0() (P3OUT &= ~(1 << IN2_L))

#define RS485_TX() (P1OUT |= (1 << RS485))
#define RS485_RX() (P1OUT &= ~(1 << RS485))
#define BLUE_LED_OFF() (P3OUT |= (1 << BLUE_LED))
#define BLUE_LED_ON() (P3OUT &= ~(1 << BLUE_LED))
#define BLUE_LED_BLINK() (P3OUT ^= (1 << BLUE_LED))
#define RED_LED_OFF() (P3OUT |= (1 << RED_LED))
#define RED_LED_ON() (P3OUT &= ~(1 << RED_LED))
#define RED_LED_BLINK() (P3OUT ^= (1 << RED_LED))
#define RF_ENABLE() (P3OUT |= (1 << RF_EN))
#define RF_DISABLE() (P3OUT &= ~(1 << RF_EN))

#define DEFAULT_UNREMEMBER

#define MAX_PRODUCT_PARA_BYTES 18

#endif
