/**
  ******************************************************************************
  * @file    Zdx_Library.h
  * @brief   裸机下常用库头文件
  * @version V1.1
  * @author  周大侠
  * @email   zzzdaxia@qq.com
  * @date    2022-09-16 10:51:48
  ******************************************************************************
  * @remark
    Default encoding UTF-8
  ******************************************************************************
  */
#ifndef _ZDX_LIBRARY_H_
#define _ZDX_LIBRARY_H_

/*-----------------------------------------------------------------------------
 * Include file
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"


//任务调度
#define ZDX_TASK

//队列管理
#define ZDX_QUEUE

//环形缓存
#define ZDX_RING_REDIS

//内存管理
#define ZDX_MEMORY

//时间转换
#define TIME_CONVERSION
    

#define DEBUG_OUT(...)              //printf


/*-----------------------------------------------------------------------------
 * 任务调度
-----------------------------------------------------------------------------*/
#ifdef ZDX_TASK

#define TASK_MODE_LINKED            1 //链表形式，无限任务
#define TASK_MODE_ARRAY             0 //数组形式，任务数由宏决定

#define TASK_MODE_SELECT            TASK_MODE_LINKED
#define TASK_NAME_LEN_MAX           (10U) // 任务名最大长度

typedef enum
{
    TASK_IDLE = 0,//空闲  
    TASK_RESUME,//运行态
    TASK_READY,//准备态
    TASK_DEL,//删除态
}cmTask_status;//任务状态

typedef struct 
{
    uint32_t uPeriod;//任务执行周期
    uint32_t uCnt;//任务计数值
    void* (*Task_func)(void*);//任务函数指针
    void* par ;//任务函数参数
    cmTask_status status;// 任务状态
    char name[TASK_NAME_LEN_MAX];
}ScmTask_Info;//任务信息

#if (TASK_MODE_SELECT == TASK_MODE_LINKED)

typedef struct ScmTask_Node
{
    ScmTask_Info taskInfo;
    struct ScmTask_Node* nextTask;
}task_t;//任务控制块

typedef struct 
{
    uint32_t taskSum;
    task_t* taskNow;
    task_t* taskHead;
}Sys_Task;

#elif (TASK_MODE_SELECT == TASK_MODE_ARRAY)

#define TASK_AMOUNT_MAX             (20U)//最大任务数量
typedef uint16_t                    task_t;//任务控制块

typedef struct 
{
    task_t taskSum;
    task_t taskNow;
    ScmTask_Info taskList[TASK_AMOUNT_MAX];
}Sys_Task;//任务表

#else
    #error "Please select the correct task pattern!"
#endif//#if(TASK_MODE_SELECT == xxx)

int Task_create(task_t* pTaskHandle, char* sTaskName, 
                   void* (*pRoutine)(void*), void* par, uint32_t uPeriod);
int Task_cancel(task_t* pTaskHandle);
void Task_reckon_time(void);
void  Task_scheduling (void);

#endif

/*-----------------------------------------------------------------------------
 * 队列管理
-----------------------------------------------------------------------------*/
#ifdef ZDX_QUEUE

#define QUEUE_DATA_LEN_MAX              (128U) //队列数据最大长度
#define QUEUE_AMOUNT_MAX                (32U)  //队列最大长度


typedef struct
{
    char data[QUEUE_DATA_LEN_MAX];
    uint32_t len;
}ScmQueue_data;

typedef struct
{
    //mutex_t clock;    //互斥锁
    uint32_t Queue_sum;
    uint32_t Queue_new;
    ScmQueue_data List[QUEUE_AMOUNT_MAX];
}ScmQueue_info;


//队列管理
int Queue_init(ScmQueue_info *p_Queue);
int Queue_add(ScmQueue_info *p_Queue, void* pData, uint32_t uSize);
int Queue_del(ScmQueue_info *p_Queue);
uint32_t Queue_get(ScmQueue_info *p_Queue, char** pData);

#endif

/*-----------------------------------------------------------------------------
 * 环形缓存
-----------------------------------------------------------------------------*/
#ifdef ZDX_RING_REDIS

typedef struct
{
    //mutex_t clock;    //互斥锁
    uint32_t validLen;//已使用的数据长度
    uint32_t RingSize;//缓冲区总大小
    char* pHead;//环形存储区的首地址
    char* pTail;//环形存储区的结尾地址
    char* pValid;//已使用的缓冲区的首地址
    char* pValidTail;//已使用的缓冲区的尾地址
}ScmRingBuff;

int initRingbuffer(ScmRingBuff* pRing ,uint32_t size);
int wirteRingbuffer(ScmRingBuff* pRing,char* buffer,uint32_t addLen);
int readRingbuffer(ScmRingBuff* pRing,char* buffer,uint32_t len);
int releaseRingbuffer(ScmRingBuff* pRing);

#endif

/*-----------------------------------------------------------------------------
 * 内存管理
-----------------------------------------------------------------------------*/
#ifdef ZDX_MEMORY

void* aligned_malloc(size_t required_bytes, size_t alignment);
void aligned_free(void* r);

#endif

/*-----------------------------------------------------------------------------
 * 时间转换
-----------------------------------------------------------------------------*/
#ifdef TIME_CONVERSION

typedef struct
{
    uint16_t year; //[1970,2100]
    uint8_t month; //[1,12]
    uint8_t day;   //[1,31]
    uint8_t hour;  //[0,23]
    uint8_t minte; //[0,59]
    uint8_t second;//[0,59]
    uint16_t msec; //[0,999]
}TimeStruct;

uint32_t Time_strTimeToUtime(TimeStruct* pStrTime);
void Time_uTimeToStrTime(uint32_t uTime, TimeStruct* pStrTime);
int Time_checkFormatIsLegal(uint16_t year, uint8_t month, uint8_t day, 
                                  uint8_t hour, uint8_t minte, uint8_t second);
#endif

#endif

