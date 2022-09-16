/**
  ******************************************************************************
  * @file    Zdx_Library.h
  * @brief   裸机下常用库
  * @version V1.1
  * @author  周大侠
  * @email   zzzdaxia@qq.com
  * @date    2022-09-16 10:52:04
  ******************************************************************************
  * @remark
    Default encoding UTF-8
  ******************************************************************************
  */
#include "Zdx_Library.h"


#ifdef ZDX_TASK
// 无操作系统下的分时调度

#if (TASK_MODE_SELECT == TASK_MODE_LINKED)

volatile static Sys_Task Sys_TaskMange = {0, NULL, NULL};

/********************************************************
  * @Description：创建一个任务
  * @Arguments  ：
                pTaskHandle[IN] 任务控制块指针
                sTaskName[IN]   任务名称，指向ASCII字符串的指针
                pRoutine[IN]    任务函数
                par[IN]         传递给任务函数的参数指针 
                uPeriod[IN]     任务调度周期-毫秒
  * @Returns    ：
                0  创建成功
                -1 创建失败，参数错误
  * @author     : 周大侠  2022-09-15 15:32:04
 *******************************************************/
int Task_create(task_t* pTaskHandle, char* sTaskName, 
    void* (*pRoutine)(void*), void* par, uint32_t uPeriod) 
{      
    if(NULL == pTaskHandle || NULL == pRoutine || NULL == sTaskName)
    {
        DEBUG_OUT("Unable to add task,parameter error!\n");
        return -1;
    }
    
    pTaskHandle->taskInfo.uPeriod = uPeriod;
    pTaskHandle->taskInfo.uCnt = 0;
    pTaskHandle->taskInfo.Task_func = pRoutine;
    pTaskHandle->taskInfo.par = par;
    pTaskHandle->taskInfo.status = TASK_IDLE;
    strncpy((char*)&pTaskHandle->taskInfo.name,sTaskName,TASK_NAME_LEN_MAX-1);
    pTaskHandle->nextTask = NULL;

    if(NULL != Sys_TaskMange.taskHead)
    {
        task_t* tailNode;
        
        tailNode = Sys_TaskMange.taskHead;
        for(;NULL != tailNode->nextTask;tailNode = tailNode->nextTask);
        tailNode->nextTask = pTaskHandle;
    }
    else
    {
        Sys_TaskMange.taskHead = pTaskHandle;
    }
    Sys_TaskMange.taskSum++;
    
    return 0;
}

/********************************************************
  * @Description：删除一个任务
  * @Arguments  ：
                pTaskHandle[IN]  要删除的任务的控制块指针,NULL为当前任务
  * @Returns    ：
                0：成功
                -1 无任务
                -2 未找到需要删除的任务
  * @author     : 周大侠      2022-09-15 15:33:06
 *******************************************************/
int Task_cancel(task_t* pTaskHandle) 
{    
    if(NULL == pTaskHandle)//结束当前任务
    {
        pTaskHandle = Sys_TaskMange.taskNow;
    }
    
    if(NULL == Sys_TaskMange.taskHead || NULL == pTaskHandle || 1 > Sys_TaskMange.taskSum)
    {
        DEBUG_OUT("Unable to cancel task,parameter error!\n");
        return -1;
    }

    if(Sys_TaskMange.taskHead == pTaskHandle)
    {
        Sys_TaskMange.taskHead = pTaskHandle->nextTask;
        pTaskHandle->nextTask = NULL;
        pTaskHandle->taskInfo.status = TASK_DEL;
        Sys_TaskMange.taskSum--;
    }
    else
    {
        task_t* taskNode = Sys_TaskMange.taskHead;
                
        for(; NULL != taskNode->nextTask; taskNode = taskNode->nextTask)
        {
            if(taskNode->nextTask == pTaskHandle)
            {
                taskNode->nextTask = pTaskHandle->nextTask;
                pTaskHandle->nextTask = NULL;
                pTaskHandle->taskInfo.status = TASK_DEL;
                Sys_TaskMange.taskSum--;
                return 0;
            }
        }
    }
    DEBUG_OUT("Unable to find the task of cancel!\n");
    return -2;
}

/********************************************************
  * @Description：任务计时
  * @Arguments  ：无
  * @Returns    ：无
  * @author     : 周大侠 2022-09-14 16:27:02
  * @remark     ：需要定时器1mS 调用一次
 *******************************************************/
void Task_reckon_time(void)
{
    task_t* taskNode = Sys_TaskMange.taskHead;
    
    for(; NULL != taskNode; taskNode = taskNode->nextTask)
     {
        if(TASK_IDLE == taskNode->taskInfo.status)
        {
            taskNode->taskInfo.uCnt++;
            if(taskNode->taskInfo.uCnt >= taskNode->taskInfo.uPeriod)
            {
                taskNode->taskInfo.uCnt = 0;
                taskNode->taskInfo.status = TASK_READY;
            }
        }
     }
}

/********************************************************
  * @Description：任务调度
  * @Arguments  ：无
  * @Returns    ：无
  * @author     : 周大侠     2022-09-14 16:27:11  
 *******************************************************/
void  Task_scheduling (void)
{
    task_t* taskNode = Sys_TaskMange.taskHead;
    
    for(; NULL != taskNode; taskNode = taskNode->nextTask)
    {
        if(TASK_READY == taskNode->taskInfo.status && NULL != taskNode->taskInfo.Task_func)
        {
            taskNode->taskInfo.status = TASK_RESUME;
            Sys_TaskMange.taskNow = taskNode;
            taskNode->taskInfo.Task_func(taskNode->taskInfo.par);
            Sys_TaskMange.taskNow = NULL;
            if(TASK_RESUME == taskNode->taskInfo.status)
                taskNode->taskInfo.status = TASK_IDLE;
        }
    }
}

#elif (TASK_MODE_SELECT == TASK_MODE_ARRAY)

volatile static Sys_Task Sys_TaskMange = {0, 0,{0}};

/********************************************************
  * @Description：创建一个任务
  * @Arguments  ：
                pTaskHandle[OUT]任务控制块指针
                sTaskName[IN]   任务名称，指向ASCII字符串的指针
                pRoutine[IN]    任务函数
                par[IN]         传递给任务函数的参数指针 
                uPeriod[IN]     任务调度周期-毫秒
  * @Returns    ：
                0  创建成功
                -1 创建失败，参数错误
                -2 创建失败，任务池已满 
  * @author     : 周大侠  2022-09-15 15:32:04
 *******************************************************/

int Task_create(task_t* pTaskHandle, char* sTaskName, 
    void* (*pRoutine)(void*), void* par, uint32_t uPeriod) 
{  
    uint16_t i = 0;

    if(NULL == pTaskHandle || NULL == pRoutine || NULL == sTaskName)
    {
        DEBUG_OUT("Unable to add task,parameter error!\n");
        return -1;
    }
    
    for(i = 0; i < TASK_AMOUNT_MAX; i++)
    {
        if(NULL == Sys_TaskMange.taskList[i].Task_func)
        {
            Sys_TaskMange.taskList[i].uPeriod = uPeriod;
            Sys_TaskMange.taskList[i].uCnt = 0;
            Sys_TaskMange.taskList[i].Task_func = pRoutine;
            Sys_TaskMange.taskList[i].par = par;
            Sys_TaskMange.taskList[i].status = TASK_IDLE;
            strncpy((char*)&Sys_TaskMange.taskList[i].name,sTaskName,TASK_NAME_LEN_MAX-1);
            *pTaskHandle = i;
            Sys_TaskMange.taskSum++;
            return 0;
        }
    } 

    DEBUG_OUT("The task pool is full and the failure is created!\n");
    return -2;
}

/********************************************************
  * @Description：删除一个任务
  * @Arguments  ：
                pTaskHandle[IN]  要删除的任务的控制块指针,NULL为当前任务
  * @Returns    ：
                0：成功
                -1 超出任务池
  * @author     : 周大侠      2022-09-15 15:33:06
 *******************************************************/

int Task_cancel(task_t* pTaskHandle) 
{
    task_t delTask;

    if(NULL == pTaskHandle)//结束当前线程
        delTask = Sys_TaskMange.taskNow;
    else
        delTask = *pTaskHandle;

    if(delTask < TASK_AMOUNT_MAX)
    {
        memset((char*)&Sys_TaskMange.taskList[delTask],00,sizeof(ScmTask_Info));
        Sys_TaskMange.taskList[delTask].status = TASK_DEL;
        Sys_TaskMange.taskSum--;
        return 0;
    }
    else
        return -1;
}

/********************************************************
  * @Description：任务计时
  * @Arguments  ：无
  * @Returns    ：无
  * @author     : 周大侠 2022-09-14 16:27:02
  * @remark     ：需要定时器1mS 调用一次
 *******************************************************/
void Task_reckon_time(void)
{
    task_t i = 0;
    
    for(i = 0 ;i < TASK_AMOUNT_MAX;i++)
     {
        if(TASK_IDLE == Sys_TaskMange.taskList[i].status && NULL != Sys_TaskMange.taskList[i].Task_func)
        {
            Sys_TaskMange.taskList[i].uCnt++;
            if(Sys_TaskMange.taskList[i].uCnt >= Sys_TaskMange.taskList[i].uPeriod)
            {
                Sys_TaskMange.taskList[i].uCnt = 0;
                Sys_TaskMange.taskList[i].status = TASK_READY;
            }
        }
     }
}

/********************************************************
  * @Description：任务调度
  * @Arguments  ：无
  * @Returns    ：无
  * @author     : 周大侠     2022-09-14 16:27:11  
 *******************************************************/
void  Task_scheduling (void)
{
    task_t i = 0;
    
    for(i = 0 ;i < TASK_AMOUNT_MAX;i++)
    {
        if(TASK_READY == Sys_TaskMange.taskList[i].status && NULL != Sys_TaskMange.taskList[i].Task_func)
        {
            Sys_TaskMange.taskNow = i;
            Sys_TaskMange.taskList[i].status = TASK_RESUME;
            Sys_TaskMange.taskList[i].Task_func(Sys_TaskMange.taskList[i].par);
            if(TASK_RESUME == Sys_TaskMange.taskList[i].status)
                Sys_TaskMange.taskList[i].status = TASK_IDLE;
        }
    }
}
#endif//#if(TASK_MODE_SELECT == xxx)

#endif//#ifdef ZDX_TASK



#ifdef ZDX_QUEUE

//队列管理
/********************************************************
  * @Description：队列初始化
  * @Arguments   ：
                p_Queue:[IN]队列句柄
  * @Returns    ：
                -1 fall
                0 succeed
  * @author     : 周大侠                   
 *******************************************************/
int Queue_init(ScmQueue_info *p_Queue)
{
    if(NULL != p_Queue)
    {
        memset((char*)p_Queue,00,sizeof(ScmQueue_info));
        //p_Queue->clock  init
        return 0;
    }
    return -1;
}

/********************************************************
  * @Description：队列-入队
  * @Arguments  ：
                p_Queue:[IN]队列句柄
                pData:  [IN]:数据指针
                uSize:  [IN]:数据大小
  * @Returns    ：
                -1 fall
                0 succeed
  * @author     : 周大侠                   
 *******************************************************/
int Queue_add(ScmQueue_info *p_Queue, void* pData, uint32_t uSize)
{
    uint32_t point_to_end = 0;
    int result = 0;
    
    //xSemaphoreTake( p_Queue->clock, portMAX_DELAY );  
    if(p_Queue->Queue_sum < QUEUE_AMOUNT_MAX && NULL != pData && uSize <= QUEUE_DATA_LEN_MAX)
    {
        point_to_end = p_Queue->Queue_new + p_Queue->Queue_sum;
        point_to_end = (point_to_end >= QUEUE_AMOUNT_MAX) ? ( point_to_end -= QUEUE_AMOUNT_MAX) : (point_to_end);

        memcpy(&p_Queue->List[point_to_end].data[0],(uint8_t*)pData,uSize);
        p_Queue->List[point_to_end].len = uSize;
        p_Queue->Queue_sum++;     
    }
    else
    {
        DEBUG_OUT("Queue_add fill.\n");
        result = -1;
    } 
    //xSemaphoreGive( p_Queue->clock );    

    return result;
}

/********************************************************
  * @Description：队列-出队
  * @Arguments  ：
                p_Queue:[IN]队列句柄
  * @Returns    ：
                -1 fall
                0 succeed   
  * @author     : 周大侠    
  * @notice     : 移除一个头部成员
 *******************************************************/
int Queue_del(ScmQueue_info *p_Queue)
{
    uint32_t point_to_head = p_Queue->Queue_new;
    int result = 0;

    //xSemaphoreTake( p_Queue->clock, portMAX_DELAY );  
    if(p_Queue->Queue_new < QUEUE_AMOUNT_MAX && p_Queue->Queue_sum >= 1)
    {
        //memset(&p_Queue->List[p_Queue->Queue_new].data[0],00,QUEUE_DATA_LEN_MAX);
        p_Queue->List[p_Queue->Queue_new].len = 0;
        p_Queue->Queue_sum--;
        if(p_Queue->Queue_sum > 0)
        {
            point_to_head++;
            (point_to_head >= QUEUE_AMOUNT_MAX) ? ( point_to_head = 0) : (point_to_head);
            p_Queue->Queue_new = point_to_head;
        }  
    }
    else
    {
        DEBUG_OUT("FUN Queue_del err.\n");
        result = -1;
    } 
    //xSemaphoreGive( p_Queue->clock ); 
    
    return result;
}

/********************************************************
  * @Description：队列-读取队列头成员
  * @Arguments  ：
                p_Queue: [IN]队列句柄
                pData[OUT]: 存放头部成员数据的指针
  * @Returns    ：
                [OUT]数据大小
  * @author     : 周大侠                   
 *******************************************************/
uint32_t Queue_get(ScmQueue_info *p_Queue, char** pData)
{
    uint32_t len = 0;
    
    //xSemaphoreTake( p_Queue->clock, portMAX_DELAY ); 
    if(NULL != p_Queue && NULL != pData)
    {
        if(0 < p_Queue->Queue_sum && NULL != p_Queue->List[p_Queue->Queue_new].data)
        {
            *pData = (char*)p_Queue->List[p_Queue->Queue_new].data;
            len = p_Queue->List[p_Queue->Queue_new].len;
        }
        else
        {
            *pData = NULL;
            len = 0;
        }
    }
    //xSemaphoreGive( p_Queue->clock )
    
    return len;
}


#endif
 
 
#ifdef ZDX_RING_REDIS
//环形缓存

/********************************************************
  * @Description：   初始化环形缓冲区
  * @Arguments  ：
                pRing  环形缓冲区结构指针
                size   设置环形缓冲区大小
  * @Returns    ：
                0   成功
                -1  失败
 *******************************************************/
int initRingbuffer(ScmRingBuff* pRing ,uint32_t size)
{
    int  result = -1;
 
    if(NULL != pRing)
    {
        if(pRing->pHead == NULL)
        {
            pRing->pHead = (char*) malloc(size);
            if(NULL != pRing->pHead)
            {
                pRing->pValid = pRing->pValidTail = pRing->pHead;
                pRing->pTail = pRing->pHead + size;
                pRing->validLen = 0;
                pRing->RingSize = size;
                //pRing->clock  init
                result = 0;
            }
        }
    }
    return result;
}
 
 /********************************************************
  * @Description：   向缓冲区写入数据
  * @Arguments  ：
                pRing   环形缓冲区结构指针
                buffer   写入的数据指针
                addLen   写入的数据长度
  * @Returns    ：
                0   成功
                -1:缓冲区没有初始化
                -2:写入长度过大
 *******************************************************/
int wirteRingbuffer(ScmRingBuff* pRing,char* buffer,uint32_t addLen)
{
    int  result = -1;
    
    if(NULL != pRing  && NULL != buffer)
    {
        //xSemaphoreTake( pRing->clock, portMAX_DELAY ); 
        if(NULL != pRing->pHead && NULL != pRing->pTail && NULL != pRing->pValid && NULL != pRing->pValidTail)
        {
            if(addLen <= pRing->RingSize)
            {
                //将要存入的数据copy到pValidTail处
                if(pRing->pValidTail + addLen > pRing->pTail)//需要分成两段copy
                {
                    uint32_t len1 = pRing->pTail - pRing->pValidTail;
                    uint32_t len2 = addLen - len1;
                    memcpy( pRing->pValidTail, buffer, len1);
                    memcpy( pRing->pHead, buffer + len1, len2);
                    pRing->pValidTail = pRing->pHead + len2;//新的有效数据区结尾指针
                }
                else
                {
                    memcpy( pRing->pValidTail, buffer, addLen);
                    pRing->pValidTail += addLen;//新的有效数据区结尾指针
                }
     
                //需重新计算已使用区的起始位置
                if(pRing->validLen + addLen > pRing->RingSize)
                {
                    int moveLen = pRing->validLen + addLen - pRing->RingSize;//有效指针将要移动的长度
                    if(pRing->pValid + moveLen > pRing->pTail)//需要分成两段计算
                    {
                        uint32_t len1 = pRing->pTail - pRing->pValid;
                        uint32_t len2 = moveLen - len1;
                        pRing->pValid = pRing->pHead + len2;
                    }
                    else
                    {
                        pRing->pValid = pRing->pValid + moveLen;
                    }
                    
                    pRing->validLen = pRing->RingSize;
                }
                else
                {
                    pRing->validLen += addLen;
                }
                result = 0;    
            }
            else
                result = -2;
        }
        //xSemaphoreGive( pRing->clock );    
    }
    return result;
}
 
/********************************************************
  * @Description：   向缓冲区读出数据
  * @Arguments  ：
                pRing   环形缓冲区结构指针
                buffer  接收数据缓存
                len     将要接收的数据长度
  * @Returns    ：
                大于0  实际读取的长度
                -1:缓冲区没有初始化
 *******************************************************/
int readRingbuffer(ScmRingBuff* pRing,char* buffer,uint32_t len)
{
    int  result = -1;
    
    if(NULL != pRing  && NULL != buffer)
    {    
        //xSemaphoreTake( pRing->clock, portMAX_DELAY ); 
        if(NULL != pRing->pHead && NULL != pRing->pTail && NULL != pRing->pValid && NULL != pRing->pValidTail)
        {
            if(0 < pRing->validLen)
            {
                if( len >= pRing->validLen) 
                    len = pRing->validLen;
                    
                if(pRing->pValid + len > pRing->pTail)//需要分成两段copy
                {
                    uint32_t len1 = pRing->pTail - pRing->pValid;
                    uint32_t len2 = len - len1;
                    memcpy( buffer, pRing->pValid, len1);//第一段
                    memcpy( buffer+len1, pRing->pHead, len2);//第二段，绕到整个存储区的开头
     
                    //注释此语句，则读取的数据不会被清除
                    pRing->pValid = pRing->pHead + len2;//更新已使用缓冲区的起始
                }
                else
                {
                    memcpy( buffer, pRing->pValid, len);
                    //注释此语句，则读取的数据不会被清除
                    pRing->pValid = pRing->pValid +len;//更新已使用缓冲区的起始
                }
                //注释此语句，则读取的数据不会被清除
                pRing->validLen -= len;//更新已使用缓冲区的长度
                result = len;
            }
            else
                result = 0;
        }
        //xSemaphoreGive( pRing->clock );    
        
    }    
    return result;
}
 
/********************************************************
  * @Description：   释放环形缓冲区
  * @Arguments  ：
                pRing   环形缓冲区结构指针
  * @Returns    ：
                0   成功
                -1  失败
 *******************************************************/
int releaseRingbuffer(ScmRingBuff* pRing)
{
    int  result = -1;
 
    if(NULL != pRing  && NULL != pRing->pHead)
    {
        //xSemaphoreTake( pRing->clock, portMAX_DELAY ); 
        free(pRing->pHead);
        //xSemaphoreGive( pRing->clock ); 
        memset((char*)pRing,00,sizeof(ScmRingBuff));
        result = 0;
    }
    return result;
}

#endif


#ifdef ZDX_MEMORY

/********************************************************
  * @Description：字节对齐 malloc
  * @Arguments  ：
                required_bytes   要申请内存字节大小 (Byte)
                alignment 字节对齐值 必须为2的n次方
  * @Returns    ：
                0   申请失败
                其它：得到的地址
  * @author     : 周大侠     2021-3-13 11:28:07
 *******************************************************/
void* aligned_malloc(size_t required_bytes, size_t alignment)
{
    void* res = NULL;
    size_t offset = alignment - 1 + sizeof(void*);//需要增加调整空间和存放实际地址的空间
    void* p = (void*)malloc(required_bytes+offset);// p 为实际malloc的地址

    if (NULL != p)
    {
        res = (void*)(((size_t  )(p)+offset)&~(alignment-1));//a &~(b-1) 可以理解为b在a范围中的最大整数倍的值    ，b必须为2的n次方，如：100&~(8-1) = 96
        void** tmp = (void**)res;//tmp是一个存放指针数据的地址
        tmp[-1] = p;//地址的前一个位置存放指针p
    }
    
    return res;
}

/********************************************************
  * @Description：字节对齐的 内存释放
  * @Arguments  ：
                r 要释放的地址值
  * @Returns    ：
  * @author     : 周大侠     2021-3-13 11:28:07
 *******************************************************/
void aligned_free(void* r)
{
    if (r != NULL)
    {
        void** tmp = (void**)r;
        free(tmp[-1]);
    }
}

#endif

#ifdef TIME_CONVERSION

/********************************************************
  * @Description：闰年判断
  * @Arguments  ：
                year[IN] 判断的年份
  * @Returns    ：
                0：平年
                1： 闰年 
  * @author     : 周大侠     2022-8-1 20:19:29
 *******************************************************/
static uint8_t Time_checkLeapYear(uint16_t uYear)
{
    return (((uYear) % 4) == 0 && (((uYear) % 100) != 0 || ((uYear) % 400) == 0));
}

/********************************************************
  * @Description：时间格式转化成时间戳
  * @Arguments  ：
                pStrTime[IN] 时间结构体
  * @Returns    ：
                从1970年起的时间戳
  * @author     : 周大侠     2022-8-1 20:19:29
 *******************************************************/
uint32_t Time_strTimeToUtime(TimeStruct* pStrTime)
{
    const uint16_t mon_yday[2][12] = 
    {
        {0,31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0,31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
    };
    uint32_t ret;
    int i = 0;

    if(NULL == pStrTime || 0 == pStrTime->day ||
        0 == pStrTime->month || 12 < pStrTime->month || 1970 > pStrTime->year )
    {
        return 0;
    }
    
    // 以平年时间计算的秒数
    ret = (pStrTime->year - 1970) * 365 * 24 * 3600;
    ret += (mon_yday[Time_checkLeapYear(pStrTime->year)][pStrTime->month - 1] + pStrTime->day - 1) * 24 * 3600;
    ret += pStrTime->hour * 3600 + pStrTime->minte * 60 + pStrTime->second;
    // 加上闰年的秒数
    for(i = 1970; i < pStrTime->year; i++)
    {
        if(Time_checkLeapYear(i))
        {
            ret += 24 * 3600;
        }
    }
    if(ret > 4107715199U)//2100-02-29 23:59:59
    { 
        ret += 24 * 3600;
    }
    return(ret);
}

/********************************************************
  * @Description：时间戳转换成时间格式结构体
  * @Arguments  ：
                uTime[IN] 时间戳
                pStrTime[OUT] 输出的时间结构体指针
  * @Returns    ：
                NULL
  * @author     : 周大侠     2022-8-1 20:19:29
 *******************************************************/
void Time_uTimeToStrTime(uint32_t uTime, TimeStruct* pStrTime)
{
    const char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    uint32_t Pass4year;
    int hours_per_year;

    pStrTime->msec = 0;
    //取秒时间
    pStrTime->second=(int)(uTime % 60);
    uTime /= 60;
    //取分钟时间
    pStrTime->minte=(int)(uTime % 60);
    uTime /= 60;
    //取过去多少个四年，每四年有 1461*24 小时
    Pass4year = uTime / (1461L * 24L);
    //计算年份
    pStrTime->year=(Pass4year << 2) + 1970;
    //四年中剩下的小时数
    uTime %= 1461L * 24L;
    //校正闰年影响的年份，计算一年中剩下的小时数
    while(1)
    {
        //一年的小时数
        hours_per_year = 365 * 24;
        //判断闰年，是闰年，一年则多24小时，即一天
        if ((pStrTime->year & 3) == 0) hours_per_year += 24;

        if (uTime < hours_per_year) break;

        pStrTime->year++;
        uTime -= hours_per_year;
    }
    //小时数
    pStrTime->hour=(int)(uTime % 24);
    //一年中剩下的天数
    uTime /= 24;
    //假定为闰年
    uTime++;
    //校正闰年的误差，计算月份，日期
    if((pStrTime->year & 3) == 0)
    {
        if (uTime > 60) 
        {
            uTime--;
        } 
        else 
        {
            if (uTime == 60) 
            {
                pStrTime->month = 2;
                pStrTime->day = 29;
                return ;
            }
        }
    }
    //计算月日
    for (pStrTime->month = 1; Days[pStrTime->month - 1] < uTime;pStrTime->month++)
    {
        uTime -= Days[pStrTime->month - 1];
    }

    pStrTime->day = (uint8_t)(uTime);

    return;
}

/********************************************************
  * @Description：判断时间是否合法
  * @Arguments  ：
                year[IN]  年
                month[IN] 月
                day[IN]   日
                hour[IN]  时
                minte[IN] 分
                second[IN]秒
  * @Returns    ：
                0 :合法
                其它：非法
  * @author     : 周大侠     2022-8-1 20:19:29
 *******************************************************/
int Time_checkFormatIsLegal(uint16_t year, uint8_t month, uint8_t day, 
                                  uint8_t hour, uint8_t minte, uint8_t second)
{
    if ((year < 1970) || (year > 2100) || (month == 0) || (month > 12) || 
    (day == 0) || (hour > 24) || (minte > 59) || (second > 59)) //数值非法
    {
        return -1;
    }

    switch (month)  //日数是否超限
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:    if (day > 31) return 0;break;   //一个月31天
        case 4:
        case 6:
        case 9:
        case 11:    if (day > 30) return 0;break;   //一个月30天
        case 2:
            if (((year % 4) == 0 && (year % 100) != 0) || ((year % 400) == 0))  //是否是闰年
            {
                if (day > 29)	//闰年2月29天
                    return -1;
                break;
            }
            else
            {
                if (day > 28)	//非闰年2月28天
                    return -1;
                break;
            }
    }
    return 0;
}

#endif