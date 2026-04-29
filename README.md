# Zdx_Library

裸机（无 RTOS）环境下常用的工具库，包含任务调度、队列、环形缓存、内存管理和时间转换五个模块，按需通过宏开关启用。

---

## 目录

1. [TASK — 任务调度](#1-task--任务调度)
2. [QUEUE — 队列管理](#2-queue--队列管理)
3. [RING — 环形缓存](#3-ring--环形缓存)
4. [MEMORY — 内存管理](#4-memory--内存管理)
5. [TIME — 时间转换](#5-time--时间转换)

---

## 1. TASK — 任务调度

> 启用：`#define ZDX_TASK`

在裸机大循环中实现轻量级分时调度，无需移植 RTOS，适合逻辑简单、对实时性要求不严格的场景。

**工作原理：** 1 ms 定时器中断调用 `Task_reckonTime()` 累计计数，主循环调用 `Task_scheduling()` 轮询执行到期任务。

**管理模式（修改 `TASK_MODE_SELECT` 切换）：**

| 模式 | 宏值 | 特点 |
|------|------|------|
| 链表（推荐） | `TASK_MODE_LINKED` | 任务数量不限 |
| 数组 | `TASK_MODE_ARRAY` | 任务数量由 `TASK_AMOUNT_MAX`（默认 20）决定 |

**接口：**

| 函数 | 说明 |
|------|------|
| `Task_create(handle, name, func, par, period)` | 创建任务，`period` 单位 ms |
| `Task_cancel(handle)` | 删除任务，传 NULL 删除当前任务 |
| `Task_reckonTime()` | 放在 1 ms 定时器中断中调用 |
| `Task_scheduling()` | 放在主循环中调用 |

**示例：**

```c
task_t gTestHandle, gLedHandle, gRtcHandle;

void main(void)
{
    Drive_init();   // 时钟、外设、IO 初始化
    Timer_init();   // 配置 1 ms 定时器

    Task_create(&gTestHandle, "task1",  Task_1_fun, NULL,  5);   //  5 ms 执行一次
    Task_create(&gLedHandle,  "led",    Led_fun,    NULL,  100);  // 100 ms 执行一次
    Task_create(&gRtcHandle,  "rtc",    RTC_fun,    NULL,  1000); //  1  s 执行一次

    Timer_start();

    while (1)
    {
        Task_scheduling();
    }
}

// 1 ms 定时器中断回调
void Timer_IRQHandler(void)
{
    Task_reckonTime();
}
```

---

## 2. QUEUE — 队列管理

> 启用：`#define ZDX_QUEUE`

固定大小的先进先出队列，适合模块间消息传递。

**容量配置（在 `Zdx_Library.h` 中修改）：**

| 宏 | 默认值 | 说明 |
|----|--------|------|
| `QUEUE_DATA_LEN_MAX` | 128 B | 单条数据最大长度 |
| `QUEUE_AMOUNT_MAX` | 32 | 队列最大条目数 |

**接口：**

| 函数 | 说明 |
|------|------|
| `Queue_init(q)` | 初始化队列 |
| `Queue_add(q, data, size)` | 入队 |
| `Queue_del(q)` | 出队（移除队头） |
| `Queue_get(q, &pData)` | 读取队头数据指针，不出队，返回数据长度 |

---

## 3. RING — 环形缓存

> 启用：`#define ZDX_RING_REDIS`

动态分配的环形缓冲区。缓冲区写满时自动覆盖最旧数据，始终保留最新数据。

**接口：**

| 函数 | 说明 |
|------|------|
| `Ring_init(ring, size)` | 初始化，`size` 为字节数（内部 malloc） |
| `Ring_write(ring, buf, len)` | 写入数据，`len > size` 时返回 -2 |
| `Ring_read(ring, buf, len)` | 读出数据，读后数据被消耗，返回实际读取字节数 |
| `Ring_release(ring)` | 释放缓冲区内存 |

**示例：**

```c
ScmRingBuff ring = {0};

Ring_init(&ring, 256);           // 分配 256 字节缓冲区

Ring_write(&ring, txBuf, 10);    // 写入 10 字节
Ring_read(&ring, rxBuf, 10);     // 读出 10 字节

Ring_release(&ring);             // 释放
```

---

## 4. MEMORY — 内存管理

> 启用：`#define ZDX_MEMORY`

支持字节对齐的 `malloc` / `free`，`alignment` 必须是 2 的幂（如 4、8、16）。

**接口：**

| 函数 | 说明 |
|------|------|
| `aligned_malloc(size, alignment)` | 分配 `size` 字节，返回地址按 `alignment` 对齐 |
| `aligned_free(ptr)` | 释放 `aligned_malloc` 分配的内存 |

**示例：**

```c
void *p = aligned_malloc(100, 8); // 分配 100 字节，8 字节对齐
// 使用 p ...
aligned_free(p);
```

---

## 5. TIME — 时间转换

> 启用：`#define TIME_CONVERSION`

北京时间（UTC+8）与 Unix 时间戳互转，支持范围 1970～2100 年。

时间结构体 `TimeStruct`：

```c
typedef struct {
    uint16_t year;   // [1970, 2100]
    uint8_t  month;  // [1, 12]
    uint8_t  day;    // [1, 31]
    uint8_t  hour;   // [0, 23]
    uint8_t  minute; // [0, 59]
    uint8_t  second; // [0, 59]
    uint16_t msec;   // [0, 999]
} TimeStruct;
```

**接口：**

| 函数 | 说明 |
|------|------|
| `Time_getWeek(year, month, day)` | 返回星期值 1～7（1=周一） |
| `Time_checkLeapYear(year)` | 闰年判断，1=闰年，0=平年 |
| `Time_strTimeToUtime(pTime)` | 北京时间结构体 → Unix 时间戳 |
| `Time_uTimeToStrTime(utime, pTime)` | Unix 时间戳 → 北京时间结构体 |
| `Time_checkFormatIsLegal(y,mo,d,h,mi,s)` | 时间合法性校验，0=合法 |