/**
  ******************************************************************************
  * @file    common.h
  * @brief   常用宏定义
  * @version V1.0
  * @author  周大侠
  * @email   zzzdaxia@qq.com
  * @date    2022-09-16 10:29:16
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
#define PHASE_VERSION                       "_Test";
#else
#define PHASE_VERSION                       "_Beta";
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
#define TRUE                                (1)
#define FALSE                               (0)
#endif

//宏求大小
#define MY_SIZEOF(_T_)                      ((uint32_t)((typeof(_T_)*)0 + 1))

//求结构体成员偏移值
#define OFFSETOF(TYPE, MEMBER)              ((long)(&(((TYPE *)0)->MEMBER)))

//求数组成员个数
#define ARRAY_LEN(ARRAY_NAME)   \
    (sizeof(ARRAY_NAME) / sizeof(ARRAY_NAME[0]))

//获取数组结尾地址，  注意类型
#define GET_ARRAY_TAIL_ADDR(ARRAY_NAME) \
        ((void*)((char*)(&(ARRAY_NAME) + 1 ) - 1))

//获取数组最后一个元素地址
#define GET_ARRAY_LAST_ELEMENT_ADDR(ARRAY_NAME,_TYPE_)  \
    (((_TYPE_*)(&(ARRAY_NAME) + 1 ) - 1))

//求整形差，避免溢出
#define UINT32DIFF(start, end)  \
        (((end) >= (start))? ((end) - (start)) : ((uint32_t)(-1) - (start) + (end)))


#define SET_BIT(_Val_,_Bit_)                ((_Val_) |= (1 << (_Bit_)))
#define CLEAN_BIT(_Val_,_Bit_)              ((_Val_) &= (~(1 << (_Bit_))))
#define GET_BIT(_Val_,_Bit_)                (((_Val_) >> (_Bit_)) & 0x0001)

#define BigLittleSwap16(_16t_)              ((((uint16_t)(_16t_) & 0xff00) >> 8) | \
                                            (((uint16_t)(_16t_) & 0x00ff) << 8))

#define BigLittleSwap32(_32t_)              ((((uint32_t)(_32t_) & 0xff000000) >> 24) | \
                                            (((uint32_t)(_32t_) & 0x00ff0000) >> 8) | \
                                            (((uint32_t)(_32t_) & 0x0000ff00) << 8) | \
                                            (((uint32_t)(_32t_) & 0x000000ff) << 24))

#define TEST_TASK_PERIOD                    (10U)
#define TEST_TASK_DELAY_MS(_ms_)            (((_ms_) > TEST_TASK_PERIOD) ? ((_ms_) / TEST_TASK_PERIOD -1) : 0)

#endif

