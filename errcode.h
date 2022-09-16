/**
  ******************************************************************************
  * @file    errcode.h
  * @brief   错误码定义
  * @version V1.0
  * @author  周大侠
  * @email   zzzdaxia@qq.com
  * @date    2022-09-16 10:27:19
  ******************************************************************************
  * @remark
    Default encoding UTF-8
  ******************************************************************************
  */
#ifndef _ERRCODE_H_
#define _ERRCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error codes
 * Usage: return negative value with errcode, such as 'return -ERR_NOT_PERMIT'
 */
typedef enum
{
    /* System errors */
    SUCCESS = 0,                  /* Success */
    ERR_NOT_PERMIT,               /* Operation not permitted */
    ERR_LONG_PARAM,               /* Parameter too long */
    ERR_DEVICE_BUSY,              /* Device or resource busy */
    ERR_NO_DEVICE,                /* No such device */
    ERR_INVAL_PARAM,              /* Invalid external user parmeter */
    ERR_INVAL_ARGU,               /* Invalid internal function argument */
    ERR_BUF_FULL,                 /* Buffer is full */
    ERR_BUF_EMPTY,                /* Buffer is empty */
    ERR_BUF_OVERFLOW,             /* Buffer is overflow */
    ERR_BUF_BUSY,                 /* Buffer is in busy (in writing or reading) */
    ERR_BUF_IDLE,                 /* Buffer is in idle */
    ERR_NOT_IMPLEMENTED,          /* Function not implemented */
    ERR_NO_DATA,                  /* No data available */
    ERR_VAL_OVERFLOW,             /* Value overflow */
    ERR_VAL_UNDERFLOW,            /* Value underflow */
    ERR_PROTO_NOT_SUPPORT,        /* Protocol not supported */
    ERR_NO_MEMORY,                /* No memory */
    ERR_ALREADY_INITIALIZED,      /* Already initialized */
    ERR_NOT_INITIALIZED,          /* Not initialized */
    ERR_FEATURE_NOT_SUPPORT,      /* Feature not supported */
    ERR_TIMEOUT,                  /* Timeout */
    ERR_NO_MORE,                  /* No more */
    ERR_VERSION_MISMATCH,         /* Version mismatch */
    ERR_NOT_READY,                /* Not ready yet */
    ERR_INVALID_MODE,             /* Invalid mode */
    ERR_UPGRADE_FAILED,           /* Upgrade failed */
    ERR_REPEAT_MESSAGE,           /* Repeat message */
    
    /* Length errors */
    ERR_FRAME_LEN,                /* Frame length error */
    ERR_PACKET_LEN,               /* Packet length error */
    ERR_APPDATA_LEN,              /* Application data length error */

    /* Format errors */
    ERR_FRAME_FORMAT,             /* Frame format error */
    ERR_PACKET_FORMAT,            /* Packet format error */
    ERR_APPDATA_FORMAT,           /* Application data format error */

    /* Data validation */
    ERR_DUPLICATE_DATA,           /* Duplicate data */
    ERR_FILLED_DATA,              /* Filled data */
    ERR_CRC,                      /* CRC checking failed */
    ERR_XOR,                      /* XOR checking failed */
    ERR_SUM,                      /* Sum checking failed */
    ERR_2_OUTOF_3,                /* 2 out of 3 checking failed */
    ERR_NOT_2BYTES_ALIGN_ADDR,    /* Not 2-bytes aligned address */
    ERR_NOT_4BYTES_ALIGN_ADDR,    /* Not 4-bytes aligned address */

    /* Hardware errors */
    ERR_SPI_SEND,                 /* SPI sending error */
    ERR_SPI_RECV,                 /* SPI receiving error */
    ERR_I2C_SEND,                 /* I2C sending error */
    ERR_I2C_RECV,                 /* I2C receiving error */
    ERR_UART_SEND,                /* UART sending error */
    ERR_UART_RECV,                /* UART receiving error */
    ERR_CAN_SEND,                 /* CAN sending error */
    ERR_CAN_RECV,                 /* CAN receiving error */
    ERR_CAN_NODE,                 /* CAN node unknown error */
    ERR_CAN_FRAME_SEQ,            /* CAN frame sequence error */
    ERR_FIFO_WRITE,               /* FIFO writing error */
    ERR_FIFO_READ,                /* FIFO reading error */
    ERR_NAND_ERASE,               /* NAND erasing error */
    ERR_NAND_WRITE,               /* NAND writing error */
    ERR_NAND_READ,                /* NAND reading error */
    ERR_FERAM_WRITE,              /* FeRAM writing error */
    ERR_FERAM_READ,               /* FeRAM reading error */

    /* 'ERR_CODE_MAX' should always be the last */
    ERR_CODE_MAX
} errCode;

#ifdef __cplusplus
}
#endif

#endif
