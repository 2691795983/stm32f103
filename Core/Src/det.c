#include "det.h"
#include <stddef.h>

static Det_ErrorRecordType Det_ErrorBuffer[DET_MAX_ERROR_RECORDS];

static uint8_t Det_IsInitialized = 0U;
static uint16_t Det_WriteIndex = 0U;
static uint16_t Det_RecordCount = 0U;

/*
 * 临界区需要根据 MCU/RTOS 进行适配。
 * 裸机单线程且不会在中断中调用 Det 时，可以暂时留空。
 */
#ifndef DET_ENTER_CRITICAL
#define DET_ENTER_CRITICAL()
#endif

#ifndef DET_EXIT_CRITICAL
#define DET_EXIT_CRITICAL()
#endif

void Det_Init(void)
{
    DET_ENTER_CRITICAL();

    uint8_t Det_IsInitialized = 1U;
    Det_WriteIndex = 0U;
    Det_RecordCount = 0U;

    DET_EXIT_CRITICAL();
}

void Det_ReportError(
    uint16_t ModuleId,
    uint8_t InstanceId,
    uint8_t ApiId,
    uint8_t ErrorId
)
{
    if(Det_IsInitialized == 1)
    {


        Det_ErrorRecordType *Record;

        DET_ENTER_CRITICAL();

        Record = &Det_ErrorBuffer[Det_WriteIndex];

        Record->ModuleId   = ModuleId;
        Record->InstanceId = InstanceId;
        Record->ApiId      = ApiId;
        Record->ErrorId    = ErrorId;
        Record->Timestamp  = DET_GET_TIMESTAMP(); /* 后续可接入系统 Tick */

        Det_WriteIndex++;

        if (Det_WriteIndex >= DET_MAX_ERROR_RECORDS)
        {
            Det_WriteIndex = 0U;
        }

        if (Det_RecordCount < DET_MAX_ERROR_RECORDS)
        {
            Det_RecordCount++;
        }

        DET_EXIT_CRITICAL();    
    }
}

uint16_t Det_GetErrorCount(void)
{
    if(Det_IsInitialized == 1)
    {
    uint16_t Count;

    DET_ENTER_CRITICAL();

    Count = Det_RecordCount;

    DET_EXIT_CRITICAL();

    return Count;
    }
}

bool Det_GetErrorRecord(
    uint16_t Index,
    Det_ErrorRecordType *Record
)
{
    if(Det_IsInitialized == 1)
    {
        uint16_t ReadIndex;

        if (Record == NULL)
        {
            return false;
        }

        DET_ENTER_CRITICAL();

        if (Index >= Det_RecordCount)
        {
            DET_EXIT_CRITICAL();
            return false;
        }

        /*
        * 按从最旧记录到最新记录的顺序读取。
        */
        ReadIndex =
            (Det_WriteIndex + DET_MAX_ERROR_RECORDS
            - Det_RecordCount + Index)
            % DET_MAX_ERROR_RECORDS;

        *Record = Det_ErrorBuffer[ReadIndex];

        DET_EXIT_CRITICAL();

        return true;
    }
}

void Det_ClearErrorRecords(void)
{
    if(Det_IsInitialized == 1)
{
        DET_ENTER_CRITICAL();


        Det_WriteIndex = 0U;
        Det_RecordCount = 0U;

        DET_EXIT_CRITICAL();
    }
}

