/**
  ******************************************************************************
  * @file    Library 头文件
  * @author  周大侠
  * @version V1.0
  * @date    2020-8-15 14:58:34
  * @brief   周大侠 常用库
  ******************************************************************************
  * @attention

  ******************************************************************************
  */
#ifndef _ZDX_LIBRARY_H_
#define _ZDX_LIBRARY_H_


#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>


#define int32_t                             signed int      
#define uint32_t                            unsigned int    
#define int16_t                             signed short    
#define uint16_t                            unsigned short  
#define int8_t                              signed char     
#define uint8_t                             unsigned char   

#ifndef TRUE
#define TRUE                                (1)
#define FALSE                               (0)
#endif                                             

#define ZDX_TASK            //无操作系统下的分时调度
#define ZDX_QUEUE           //队列管理
#define ZDX_RING_REDIS      //环形缓存

#define DEBUG_PRINT(...)                         //printf


#define _OFFSETOF(TYPE, MEMBER)             				((long)(&(((TYPE *)0)->MEMBER)))
#define ARRAY_LEN(ARRAY_NAME)               				(sizeof(ARRAY_NAME) / sizeof(ARRAY_NAME[0]))//求数组成员个数
#define GET_ARRAY_TAIL_ADDR(ARRAY_NAME)                     ((void*)((char*)(&(ARRAY_NAME) + 1 ) - 1))//获取数组结尾地址，  注意类型
#define GET_ARRAY_LAST_ELEMENT_ADDR(ARRAY_NAME,_TYPE_)      (((_TYPE_*)(&(ARRAY_NAME) + 1 ) - 1))//获取数组最后一个元素地址



#define SET_BIT(_Val_,_Bit_)                ((_Val_) |= (1 << (_Bit_)))
#define CLEAN_BIT(_Val_,_Bit_)              ((_Val_) &= (~(1 << (_Bit_))))
#define GET_BIT(_Val_,_Bit_)                (((_Val_) >> (_Bit_)) & 0x0001)

#define BigLittleSwap16(_16t_)              ((((uint16_t)(_16t_) & 0xff00) >> 8) | \
                                              (((uint16_t)(_16t_) & 0x00ff) << 8))

#define BigLittleSwap32(_32t_)              ((((uint32_t)(_32t_) & 0xff000000) >> 24) | \
                                              (((uint32_t)(_32t_) & 0x00ff0000) >> 8) | \
                                              (((uint32_t)(_32t_) & 0x0000ff00) << 8) | \
                                              (((uint32_t)(_32t_) & 0x000000ff) << 24))





#ifdef ZDX_TASK

#define TASK_AMOUNT_MAX                     (10U)      // 最大任务数量
#define TASK_NAME_LEN_MAX                   (20U)      // 任务名最大长度


#define TEST_TASK_PERIOD                    (10U)
#define TEST_TASK_DELAY_MS(_ms_)            (((_ms_) > TEST_TASK_PERIOD) ? ((_ms_) / TEST_TASK_PERIOD -1) : 0)


typedef uint32_t                            task_t;//任务控制块


typedef enum
{
    TASK_IDLE = 0,//空闲  
    TASK_Resume = 1,//运行中
    TASK_ready = 2,//准备中
    TASK_Suspend = 3,//挂起  
}cmTask_status;//任务状态


typedef struct 
{
	uint32_t timeOutCnt;//任务计数重载值
	uint32_t timeOut;//任务计数值
	void* (*Task_func)(void *arg);//任务函数指针
	void *par ;//任务函数参数
	cmTask_status status;// 任务状态
    char name[TASK_NAME_LEN_MAX];
}ScmTask_info;//单任务信息



typedef struct 
{
    uint32_t Task_now;
    ScmTask_info Task_queue[TASK_AMOUNT_MAX];
}Sys_Task;//任务表


int Task_create(char* TaskName ,task_t *task ,void *(*start_routine)(void *arg) ,void *par ,uint32_t period); 
void Task_cancel(task_t* task);
void Task_reckon_time(void);
void  Task_scheduling (void);

#endif


#ifdef ZDX_QUEUE

#define QUEUE_DATA_LEN_MAX                  (128U)  //队列数据最大长度
#define QUEUE_AMOUNT_MAX                    (32U)   //队列最大长度


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
int Queue_add(ScmQueue_info *p_Queue,char* data,uint32_t len);
int Queue_del(ScmQueue_info *p_Queue);
uint32_t Queue_get(ScmQueue_info *p_Queue,char ** data);


#endif



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




















#endif

