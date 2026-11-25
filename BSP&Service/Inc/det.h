#ifndef DET_H
#define DET_H

#include <stdint.h>
#include <stdbool.h>
#include "det_cfg.h"

typedef struct
{
    uint16_t ModuleId;
    uint8_t  InstanceId;
    uint8_t  ApiId;
    uint8_t  ErrorId;
    uint32_t Timestamp;
} Det_ErrorRecordType;

void Det_Init(void);

void Det_ReportError(
    uint16_t ModuleId,
    uint8_t InstanceId,
    uint8_t ApiId,
    uint8_t ErrorId
);

uint16_t Det_GetErrorCount(void);

bool Det_GetErrorRecord(
    uint16_t Index,
    Det_ErrorRecordType *Record
);

void Det_ClearErrorRecords(void);   

#endif