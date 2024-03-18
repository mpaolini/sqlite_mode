#include "modemath.h"
#include <string.h>
#include <math.h>
#include <bsd/stdlib.h>

#define MODE_INDEX_FOUND 4
#define MODE_INDEX_NOT_FOUND 5

int indexOf(ModeData data[static 1], double value, size_t idx[static 1])
{
    for (size_t i = 0; i < data->size; i++)
    {
        if (fabs(data->values[i] - value) <= __DBL_EPSILON__)
        {
            *idx = i;
            return MODE_INDEX_FOUND;
        }
    }
    return MODE_INDEX_NOT_FOUND;
}

int computeMode(ModeData *data, double *result, int *hasResult)
{
    ModeData summaryValues;
    ModeData summaryOccurrences;
    int status = MODE_OK;
    if ((status = modeDataInit(&summaryValues)) != MODE_OK)
    {
        return status;
    }
    if ((status = modeDataInit(&summaryOccurrences)) != MODE_OK)
    {
        modeDataFinish(&summaryValues);
        return status;
    }
    *hasResult = 0;
    for (size_t i = 0, idx; i < data->size; i++)
    {
        if (indexOf(&summaryValues, data->values[i], &idx) == MODE_INDEX_FOUND)
        {
            summaryOccurrences.values[idx]++;
        }
        else
        {
            if ((status = modeAddValue(&summaryValues, data->values[i])) != MODE_OK)
            {
                goto stop;
            }
            if ((status = modeAddValue(&summaryOccurrences, 1)) != MODE_OK)
            {
                goto stop;
            }
        }
    }
    size_t mode_occurrences;
    double mode_value;
    for (size_t i = 0; i < summaryOccurrences.size; i++)
    {
        // XXX is this the right way?
        if (!i || summaryOccurrences.values[i] > mode_occurrences)
        {
            mode_occurrences = summaryOccurrences.values[i];
            mode_value = summaryValues.values[i];
        }
    }
    *result = mode_value;
    *hasResult = 1;
stop:
    modeDataFinish(&summaryValues);
    modeDataFinish(&summaryOccurrences);
    return status;
}

int modeDataInit(ModeData *data)
{
    memset(data, 0, sizeof(ModeData));
    return MODE_OK;
}

void modeDataFinish(ModeData *data)
{
    free(data->values);
    memset(data, 0, sizeof(ModeData));
}

static int ensureCapacity(ModeData *data)
{
    if (data->size > data->capacity)
    {
        data->capacity = data->size * 2;
        data->values = (double *)reallocf(data->values, data->capacity * sizeof(double));
        if (!data->values)
        {
            return MODE_ERR;
        }
    }
    return MODE_OK;
}

int modeAddValue(ModeData *data, double value)
{
    data->size++;
    int result = ensureCapacity(data);
    if (result != MODE_OK)
    {
        return result;
    }
    data->values[data->size - 1] = value;
    return result;
}
