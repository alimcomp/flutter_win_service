#ifndef HEADER_RESULT_STRUCT
#define HEADER_RESULT_STRUCT
#include <stdbool.h>
typedef struct ResultStruct
{
    bool status;
    size_t code;
    wchar_t *message;
} ResultStruct;
#endif