#ifndef HEADER_RESULT_STRUCT
#define HEADER_RESULT_STRUCT
#include <stdbool.h>
typedef struct ResultStruct
{
    bool status;
    size_t code;
    wchar_t *message;
} ResultStruct;

typedef struct ServiceStatusStruct
{
    bool status;
    size_t code;
    wchar_t *message;
    size_t dwServiceType;
    size_t dwCurrentState;
    size_t dwControlsAccepted;
    size_t dwWin32ExitCode;
    size_t dwServiceSpecificExitCode;
    size_t dwCheckPoint;
    size_t dwWaitHint;
} ServiceStatusStruct;
#endif