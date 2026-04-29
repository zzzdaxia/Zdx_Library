/**
  ******************************************************************************
  * Copyright (C) 2026 xxx xxx Technology Co, LTD. All Rights Reserved.
  ******************************************************************************
  * @file    common.h
  * @brief   常用宏定义
  * @version V1.1
  * @author  周大侠
  * @email   zzzdaxia@qq.com
  * @date    2026-04-29 23:03:59
  ******************************************************************************
  * @remark
    Default encoding UTF-8
  ******************************************************************************
  */
#ifndef _COMMON_H_
#define _COMMON_H_


#define HARDWARE_VERSION                    "VER123T"
#define SOFTWARE_VERSION                    "220916_ZDX"
#if (TEST_MODE ) || ( OPEN_WATCH_DOG)
#define PHASE_VERSION                       "_Test"
#else
#define PHASE_VERSION                       "_Beta"
#endif
#define VERSION_NAME_MAX_SIZE               32U
//版本号    = 硬件版本号 + 软件版本号 + 阶段版本号
//const char SysVersion[VERSION_NAME_MAX_SIZE] = HARDWARE_VERSION SOFTWARE_VERSION PHASE_VERSION;

//typedef signed long long                    int64_t;
typedef unsigned long long                  uint64_t;
typedef unsigned long                       size_t;
typedef signed int                          int32_t;
typedef unsigned int                        uint32_t;
typedef signed short                        int16_t;
typedef unsigned short                      uint16_t;
typedef signed char                         int8_t;
typedef unsigned char                       uint8_t;

#ifndef TRUE
#define TRUE    (1)
#define FALSE   (0)
#endif

#ifndef ON
#define ON      (1)
#define OFF     (0)
#endif

// 宏求大小
#define MY_SIZEOF(_T_)          ((uint32_t)((typeof(_T_) *)0 + 1))

// 求结构体成员偏移值
#define OFFSETOF(TYPE, MEMBER)  ((long)(&(((TYPE *)0)->MEMBER)))

// 求数组成员个数
#define ARRAY_LEN(ARRAY_NAME) \
    (sizeof(ARRAY_NAME) / sizeof(ARRAY_NAME[0]))

// 获取数组结尾地址，  注意类型
#define GET_ARRAY_TAIL_ADDR(ARRAY_NAME) \
    ((void *)((char *)(&(ARRAY_NAME) + 1) - 1))

// 获取数组最后一个元素地址
#define GET_ARRAY_LAST_ELEMENT_ADDR(ARRAY_NAME, _TYPE_) \
    (((_TYPE_ *)(&(ARRAY_NAME) + 1) - 1))

// 求整形差 unsigned 自动回绕
#define UINT32DIFF(start, end) ((uint32_t)((end) - (start)))

#define UINT16DIFF(start, end) ((uint16_t)((end) - (start)))

#ifndef min
#define min(a, b) \
    ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a < _b ? _a : _b; })
#endif
#ifndef max
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })
#endif

#ifndef LIMITER
#define LIMITER(v, n, m) (((v) < (n)) ? (n) : ((v) > (m) ? ((m) > (n) ? (m) : (v)) : (v)))
#endif

/******************************************************************************
 *  对齐相关
 *****************************************************************************/

/**
 * 计算容纳 size 所需的 unit_size 数量（向上取整）
 * @unit_size: 单个容器单位大小
 */
#ifndef RoundUpUnits
#define RoundUpUnits(size, unit_size) \
    (((size) + (unit_size) - 1) / (unit_size))
#endif

/**
 * 按 2 的幂向下对齐（比 ALIGN_DOWN_ANY 更快）
 * @align_size: 对齐单位，应为 2 的幂，例如 2、4、8 ...
 */
#ifndef ALIGN_DOWN_POWER2
#define ALIGN_DOWN_POWER2(size, align_size) ((size) & (~((align_size) - 1)))
#endif

/**
 * 按 2 的幂向上对齐（比 ALIGN_UP_ANY 更快）
 * @align_size: 对齐单位，应为 2 的幂，例如 2、4、8 ...
 */
#ifndef ALIGN_UP_POWER2
#define ALIGN_UP_POWER2(size, align_size) \
    (((size) + (align_size) - 1) & (~((align_size) - 1)))
#endif

/**
 * 按任意对齐单位向下对齐
 */
#ifndef ALIGN_DOWN_ANY
#define ALIGN_DOWN_ANY(size, align_size) \
    (((size) / (align_size)) * (align_size))
#endif

/**
 * 按任意对齐单位向上对齐
 */
#ifndef ALIGN_UP_ANY
#define ALIGN_UP_ANY(size, align_size) \
    (RoundUpUnits(size, align_size) * (align_size))
#endif

/**
 * 向上对齐到 4 字节边界
 */
#ifndef ALIGN_UP_4BYTES
#define ALIGN_UP_4BYTES(size) ALIGN_UP_POWER2(size, 4)
#endif

/**
 * 判断是否 4 字节对齐
 */
#define Is4BytesAligned(value)      (0 == (((uint32_t)value) & 0x3))

/******************************************************************************
 *  其他
 *****************************************************************************/

/**
 * 编译期断言：condition 为真时编译报错，为假时无任何影响
 * 用途：在编译阶段拦截非法配置，零运行时开销
 * 示例：BUILD_BUG_ON(sizeof(MyStruct) != 16);  // 结构体大小不是16字节则报错
 */
#define BUILD_BUG_ON(condition)     ((void)sizeof(char[1 - 2*!!(condition)]))

/**
 * 编译期断言：e 为真时编译报错，e 为假时值为 0（整数）
 * 与 BUILD_BUG_ON 的区别：可以内嵌到表达式中使用，因为有返回值 0
 * 示例：int x = BUILD_BUG_ON_ZERO(N < 0) + N;  // N<0 则报错，否则 x = 0 + N
 */
#define BUILD_BUG_ON_ZERO(e)        (sizeof(struct { int:-!!(e); }))

/**
 * 编译期断言：e 为真时编译报错，e 为假时值为 NULL（void* 指针）
 * 与 BUILD_BUG_ON_ZERO 的区别：返回值为指针类型，可内嵌到需要指针的表达式中
 * 示例：void *p = BUILD_BUG_ON_NULL(N < 0);  // N<0 则报错，否则 p = NULL
 */
#define BUILD_BUG_ON_NULL(e)        ((void *)sizeof(struct { int:-!!(e); }))

#define SEC_TO_MS(_sec_)            ((_sec_) * 1000U)
#define MIN_TO_MS(_min_)            ((_min_) * 60U * 1000U)
#define HOUR_TO_MS(_hour_)          ((_hour_) * 60U * 60U * 1000U)

#define SET_BIT(_Val_, _Bit_)     	((_Val_) |= (1U << (_Bit_)))
#define CLEAN_BIT(_Val_, _Bit_)   	((_Val_) &= (~(1U << (_Bit_))))
#define GET_BIT(_Val_, _Bit_)     	(((_Val_) >> (_Bit_)) & 1U)

#define SET_BIT_64(_Val_, _Bit_)    ((_Val_) |= ((uint64_t)1 << (_Bit_)))
#define CLEAN_BIT_64(_Val_, _Bit_)  ((_Val_) &= (~((uint64_t)1 << (_Bit_))))
#define GET_BIT_64(_Val_, _Bit_)    (((_Val_) >> (_Bit_)) & (uint64_t)1)

#define BigLittleSwap16(_16t_)      ((((uint16_t)(_16t_) & 0xff00) >> 8) | \
                                     (((uint16_t)(_16t_) & 0x00ff) << 8))

#define BigLittleSwap32(_32t_)      ((((uint32_t)(_32t_) & 0xff000000) >> 24) | \
                                     (((uint32_t)(_32t_) & 0x00ff0000) >> 8) |  \
                                     (((uint32_t)(_32t_) & 0x0000ff00) << 8) |  \
                                     (((uint32_t)(_32t_) & 0x000000ff) << 24))

#define BigLittleSwap64(_64t_)      ((((uint64_t)(_64t_) & (uint64_t)0x00000000000000ffULL) << 56) | \
                                     (((uint64_t)(_64t_) & (uint64_t)0x000000000000ff00ULL) << 40) | \
                                     (((uint64_t)(_64t_) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
                                     (((uint64_t)(_64t_) & (uint64_t)0x00000000ff000000ULL) << 8) |  \
                                     (((uint64_t)(_64t_) & (uint64_t)0x000000ff00000000ULL) >> 8) |  \
                                     (((uint64_t)(_64t_) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
                                     (((uint64_t)(_64t_) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
                                     (((uint64_t)(_64t_) & (uint64_t)0xff00000000000000ULL) >> 56))

#define TEST_TASK_PERIOD            (10U)
#define TEST_TASK_DELAY_MS(_ms_)    (((_ms_) > TEST_TASK_PERIOD) ? ((_ms_) / TEST_TASK_PERIOD - 1) : 0)

#endif
