#define MODE_ERR 1
#define MODE_OK 0
#include <stdlib.h>

typedef struct ModeData
{
    size_t size;
    size_t capacity;
    double *values;
} ModeData;

int computeMode(ModeData *data, double *result, int *hasResult);

void modeDataInit(ModeData *data);

void modeDataFinish(ModeData *data);

int modeAddValue(ModeData *data, double value);