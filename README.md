# Zdx_Library
裸机环境上实用的库

 目录

1. TASK 任务调度

2. QUEUE 队列管理 

3. RING_REDIS 环形缓存

4. MEMORY 内存管理

5. TIME 时间转换

## 1. TASK 任务调度：#define ZDX_TASK   ##

无RTOS(裸机环境)下实现分时调度法，非常适用于单片机环境下的一般应用场景

RTOS优点不用多说了，但对于一些简单的业务需求，移植一个操作系统又显得非常麻烦，且非常占用系统资源。

通常的做法是在一个大while下执行各个业务函数，通过多个定时器去控制各个函数的执行周期，利用几个全局变量关联各个模块功能。

但这样的做法在多轮开发下，会造成 代码非常臃肿，模块关系混乱，不利于阅读。

为了解决这个问题，手动码了一个简单的多任务调度，封装成接口供大家使用！

实现方式：创建任务时，设定好任务的调度周期和任务函数，通过定时器来计算任务是否达到执行周期，在主循环上执行任务函数。

**优点：** 

1、占用代码量超小

2、移植超简单

3、程序架构清晰、模块化

**管理方式：**

1、链表形式(推荐)，任务数量无限，通过修改宏TASK_MODE_SELECT为TASK_MODE_LINKED进行条件编译

2、数组形式，任务数量由宏定义TASK_AMOUNT_MAX决定，通过修改宏TASK_MODE_SELECT为TASK_MODE_ARRAY进行条件编译

**接口说明:**

1、Task_create 创建一个任务，需要传入任务函数指针和执行周期

2、Task_cancel 取消一个任务

3、Task_reckon_time 任务计数，通常放在1mS的定时器中断中执行

4、Task_scheduling 任务调度，主函数上循环调用即可


例：
```c
//任务句柄变量
task_t gTestHandle,gLedHandle,gRtcHandle;

void main(void)
{
    Drive_init();//时钟、外设、IO 初始化
    Time_init();//设定1mS中断一次的定时器

    Task_create(&gTestHandle,"test_task_1",Task_1_fun,NULL,5);//创建一个Task_1_fun 任务 5mS执行一次
    Task_create(&gLedHandle,"test_led",Led_fun,NULL,100);//创建一个Led_fun 任务 100mS执行一次
    Task_create(&gRtcHandle,"test_RTC",RTC_fun,NULL,1000);//创建RTC_fun 任务 1000mS执行一次

    start_time();//启动定时器
    
    while(1)
    {
        Task_scheduling(); //死循环，任务调度
    }
}

//定时器中断回调函数 1mS一次中断 
void CallBack_time_it(void)
{
    Task_reckon_time();//任务周期计算
}
```




## 2. QUEUE  队列管理：#define ZDX_QUEUE    ##

使用方法：根据业务场景，修改宏定义QUEUE_DATA_LEN_MAX和QUEUE_AMOUNT_MAX ，创建静态或动态的队列缓存。

**接口说明：**

 Queue_init：队列初始化

 Queue_add：入队

 Queue_del：出队

 Queue_get：读取队列头数据




## 3. RING_REDIS  环形缓存：#define ZDX_RING_REDIS    ##


一、环形缓冲区的特性

1、先进先出        
2、当缓冲区被使用完，且又有新的数据需要存储时，丢掉历史最久的数据，保存最新数据
现实中的存储介质都是线性的，因此我们需要做一下处理，才能在功能上实现环形缓冲区
![](https://img-blog.csdn.net/20180823161741219?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L21hb3dlbnRhbzA0MTY=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)


![](https://img-blog.csdn.net/20180823161009879?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L21hb3dlbnRhbzA0MTY=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)




## 4. MEMORY  内存管理：#define ZDX_MEMORY    ##

编写支持对其分配的malloc和free函数，分配内存时，malloc函数返回的地址必须能被2的n次方整除。


void* aligned_malloc(size_t required_bytes, size_t alignment)    
void aligned_free(void* r)

内存模型：
![](https://img-blog.csdnimg.cn/202103131542419.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM3ODMwNzU3,size_16,color_FFFFFF,t_70)



## 5. TIME  时间转换：#define TIME_CONVERSION    ##

**接口说明：**

1.Time_strTimeToUtime   时间格式转化成时间戳

2.Time_uTimeToStrTime  时间戳转换成时间格式结构体

3.Time_checkFormatIsLegal 判断时间是否合法