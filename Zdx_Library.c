/**
  ******************************************************************************
  * @file    Library 常用库
  * @author  周大侠
  * @version V1.0
  * @date    2020-8-15 14:58:34
  ******************************************************************************
  * @attention

  ******************************************************************************
  */
#include "Zdx_Library.h"



//版本号  =  硬件版本号 + 软件版本号 + 阶段版本号  
const char SysVersion[VERSION_NAME_MAX_SIZE] = HARDWARE_VERSION SOFTWARE_VERSION PHASE_VERSION;



#ifdef ZDX_TASK

volatile static Sys_Task Task_info_all;

// 无操作系统下的分时调度
/********************************************************
  * @Description：   创建一个任务
  * @Arguments	：
			    TaskName        任务名称，指向ASCII字符串的指针
			    task            任务控制块指针
			    start_routine   任务函数
			    par             传递给任务函数的参数  
			    period          任务调度周期
  * @Returns	：
                0   创建成功
                -1  创建失败
  * @author     : 周大侠     2020-8-15 15:17:49
 *******************************************************/
int Task_create(char* TaskName ,task_t *task ,void *(*start_routine)(void *arg) ,void *par ,uint32_t period) 
{  
    uint32_t i = 0;
    uint8_t Task_full_falg = TRUE;

    if(NULL != task && NULL != start_routine)
    {
        for(i = 0;i < TASK_AMOUNT_MAX;i++)
        {
            if(NULL == Task_info_all.Task_queue[i].Task_func)
            {
                strncpy((char*)&Task_info_all.Task_queue[i].name,TaskName,TASK_NAME_LEN_MAX-1);
                Task_info_all.Task_queue[i].timeOutCnt = period;
                Task_info_all.Task_queue[i].Task_func = start_routine;
                Task_info_all.Task_queue[i].par = par;
                Task_info_all.Task_queue[i].status = TASK_IDLE;
                *task = i;
                Task_full_falg = FALSE; 
                break;
            }
        } 
        if(FALSE == Task_full_falg)
        {
            DEBUG_PRINT("Task :\"%s\" Creating  successful!\n",Task_info_all.Task_queue[i].name);
            return 0;
        } 
        else
            DEBUG_PRINT("Unable to add task,Task full!!\n");
    }
    else
        DEBUG_PRINT("Unable to add task,parameter error!!\n");
        
    return -1;
}


/********************************************************
  * @Description：删除一个任务
  * @Arguments	：
			    task   任务控制块指针
  * @Returns	：
  * @author     : 周大侠         2020-8-15 15:23:37  
 *******************************************************/
void Task_cancel(task_t* task) 
{
    if(NULL == task)//结束当前线程
        memset((char*)&Task_info_all.Task_queue[Task_info_all.Task_now],00,sizeof(ScmTask_info));
    else
        memset((char*)&Task_info_all.Task_queue[*(task)],00,sizeof(ScmTask_info));
}


/********************************************************
  * @Description：任务计时
  * @Arguments	：
  * @Returns	：
  * @author     : 周大侠    2020-8-15 15:54:49
  * @remark     ：定时器1mS 执行一次
 *******************************************************/
void Task_reckon_time(void)
{
    uint32_t i = 0;
    
    for(i = 0 ;i < TASK_AMOUNT_MAX;i++)
     {
        if(TASK_IDLE == Task_info_all.Task_queue[i].status && NULL != Task_info_all.Task_queue[i].Task_func)
        {
            Task_info_all.Task_queue[i].timeOut++;
            if(Task_info_all.Task_queue[i].timeOut >= Task_info_all.Task_queue[i].timeOutCnt)
            {
                Task_info_all.Task_queue[i].timeOut = 0;
                Task_info_all.Task_queue[i].status = TASK_ready;
            }
        }
     }
}



/********************************************************
  * @Description：任务调度
  * @Arguments	：
                无
  * @Returns	：
                无 
  * @author     : 周大侠     
 *******************************************************/
void  Task_scheduling (void)
{
    uint32_t i = 0;
    
    for(i = 0 ;i < TASK_AMOUNT_MAX;i++)
    {
        if(TASK_ready == Task_info_all.Task_queue[i].status && NULL != Task_info_all.Task_queue[i].Task_func)
        {
            Task_info_all.Task_queue[i].status = TASK_Resume;
            Task_info_all.Task_now = i;
            Task_info_all.Task_queue[i].Task_func(Task_info_all.Task_queue[i].par);
            if(TASK_Suspend != Task_info_all.Task_queue[Task_info_all.Task_now].status)
            {
                Task_info_all.Task_queue[i].status = TASK_IDLE;
            }    
        }
    }
}

#endif





#ifdef ZDX_QUEUE

//队列管理
/********************************************************
  * @Description：队列初始化
  * @Arguments	：
			    p_Queue  队列表
  * @Returns	：
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
  * @Description：增加队列成员---  入队
  * @Arguments	：
			    p_Queue  队列表
			    data 数据
			    len 数据长度
  * @Returns	：
                -1 fall
                0 succeed
  * @author     : 周大侠                   
 *******************************************************/
int Queue_add(ScmQueue_info *p_Queue,char* data,uint32_t len)
{
    uint32_t point_to_end = 0;
    int result = 0;
    
	//xSemaphoreTake( p_Queue->clock, portMAX_DELAY );  
	
    if(p_Queue->Queue_sum < QUEUE_AMOUNT_MAX && NULL != data && len <= QUEUE_DATA_LEN_MAX)
    {
		point_to_end = p_Queue->Queue_new + p_Queue->Queue_sum;
		point_to_end = (point_to_end >= QUEUE_AMOUNT_MAX) ? ( point_to_end -= QUEUE_AMOUNT_MAX) : (point_to_end);

		memcpy(&p_Queue->List[point_to_end].data[0],data,len);
		p_Queue->List[point_to_end].len = len;
		p_Queue->Queue_sum++;     
    }
    else
    {
        DEBUG_PRINT("Queue_add fill.\n");
        result = -1;
    } 
	//xSemaphoreGive( p_Queue->clock );    

	return result;
}

/********************************************************
  * @Description：删除队列成员---出队
  * @Arguments	：
			    p_Queue  队列表
  * @Returns	：
                -1 fall
                0 succeed   
  * @author     : 周大侠                   
 *******************************************************/
int Queue_del(ScmQueue_info *p_Queue)
{
    uint32_t point_to_head = p_Queue->Queue_new;
    int result = 0;

	//xSemaphoreTake( p_Queue->clock, portMAX_DELAY );  
	
    if(p_Queue->Queue_new < QUEUE_AMOUNT_MAX && p_Queue->Queue_sum >= 1)
    {
        memset(&p_Queue->List[p_Queue->Queue_new].data[0],00,QUEUE_DATA_LEN_MAX);
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
        DEBUG_PRINT("FUN Queue_del err.\n");
        result = -1;
    } 
	
	//xSemaphoreGive( p_Queue->clock ); 
	return result;
}


/********************************************************
  * @Description：读取队列头--读头元素
  * @Arguments	：
			    p_Queue  队列表
			    data  要存放的数据的指针的地址
  * @Returns	：
                0或许是失败
                其余表示数据长度
  * @author     : 周大侠                   
 *******************************************************/
uint32_t Queue_get(ScmQueue_info *p_Queue,char ** data)
{
    uint32_t len = 0;
    
	//xSemaphoreTake( p_Queue->clock, portMAX_DELAY ); 
	if(NULL != p_Queue && NULL != data)
	{
	    if(0 < p_Queue->Queue_sum && NULL != p_Queue->List[p_Queue->Queue_new].data)
	    {
	        *data = (char*)p_Queue->List[p_Queue->Queue_new].data;
	        len = p_Queue->List[p_Queue->Queue_new].len;
	    }
	    else
	    {
            *data = NULL;
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
  * @Arguments	：
			    pRing  环形缓冲区结构指针
			    size   设置环形缓冲区大小
  * @Returns	：
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
  * @Arguments	：
                pRing   环形缓冲区结构指针
			    buffer   写入的数据指针
			    addLen   写入的数据长度
  * @Returns	：
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
  * @Arguments	：
                pRing   环形缓冲区结构指针
			    buffer   接收数据缓存
			    addLen   将要接收的数据长度
  * @Returns	：
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
  * @Arguments	：
                pRing   环形缓冲区结构指针
  * @Returns	：
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
  * @Arguments	：
                required_bytes   要申请内存字节大小 (Byte)
                alignment 字节对齐值 必须为2的n次方
  * @Returns	：
                0   申请失败
                其它：得到的地址
  * @author     : 周大侠     2021-3-13 11:28:07
 *******************************************************/
void* aligned_malloc(size_t         required_bytes, size_t alignment)
{
    void* res = nullptr;
    size_t   offset = alignment - 1 + sizeof(void*);//需要增加调整空间和存放实际地址的空间
    void* p = (void*)malloc(required_bytes+offset);// p 为实际malloc的地址
    
    if (nullptr != p)
    {
        res = (void*)(((size_t  )(p)+offset)&~(alignment-1));//a &~(b-1) 可以理解为b在a范围中的最大整数倍的值    ，b必须为2的n次方，如：100&~(8-1) = 96
        void** tmp = (void**)res;//tmp是一个存放指针数据的地址
        tmp[-1] = p;//地址的前一个位置存放指针p
    }        
     
    return res;
}
 
/********************************************************
  * @Description：字节对齐的 内存释放
  * @Arguments	：
                r 要释放的地址值
  * @Returns	：
  * @author     : 周大侠     2021-3-13 11:28:07
 *******************************************************/
void aligned_free(void* r)
{
    if (r != nullptr)
    {
        void** tmp = (void**)r;
        free(tmp[-1]);
    }
 
}

#endif

