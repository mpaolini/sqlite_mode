#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "modemath.h"

typedef struct AggregateData
{
    int init;
    ModeData modeData;
} AggregateData;

static AggregateData *
getData(sqlite3_context *context)
{
    AggregateData *data = (AggregateData *)sqlite3_aggregate_context(context, sizeof(AggregateData));
    if (!data)
    {
        sqlite3_result_error_nomem(context);
    }
    if (!data->init)
    {
        if (modeDataInit(&data->modeData) != MODE_OK)
        {
            char *err = "mode data init error";
            sqlite3_result_error(context, err, strlen(err));
            return NULL;
        }
        data->init = 1;
    }
    return data;
}

static void modeStep(
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv)
{
    assert(argc == 1);
    AggregateData *data = getData(context);
    if (!data)
    {
        return;
    }
    switch (sqlite3_value_type(argv[0]))
    {
    case SQLITE_INTEGER:
    // TODO: support ints natively, without float conversion
    case SQLITE_FLOAT:
    {
        double value = sqlite3_value_double(argv[0]);
        if (modeAddValue(&data->modeData, value) != MODE_OK)
        {
            char *err = "mode calculation error in step";
            sqlite3_result_error(context, err, strlen(err));
            goto stop;
        }
    }
    break;
    }
    return;
stop:
    modeDataFinish(&data->modeData);
}

static void modeFinal(
    sqlite3_context *context)
{
    AggregateData *data = getData(context);
    if (!data)
    {
        return;
    }
    double result;
    int hasResult;
    if (computeMode(&data->modeData, &result, &hasResult) != MODE_OK)
    {
        char *err = "mode calculation error in final";
        sqlite3_result_error(context, err, strlen(err));
        goto finish;
    }
    if (hasResult)
    {
        sqlite3_result_double(context, result);
    }
    else
    {
        sqlite3_result_null(context);
    }
finish:
    modeDataFinish(&data->modeData);
}

int sqlite3_mode_init(
    sqlite3 *db,
    char **pzErrMsg,
    const sqlite3_api_routines *pApi)
{
    int rc = SQLITE_OK;
    SQLITE_EXTENSION_INIT2(pApi);
    // // register as auto extension
    // rc = sqlite3_auto_extension((void (*)(void))mode_EntryPoint);

    // // set error message
    // char *errStr = "Hey!";
    // *pzErrMsg = (char *)sqlite3_malloc64(strlen(errStr));
    // if (*pzErrMsg)
    // {
    //     strcpy(*pzErrMsg, errStr);
    // }

    // // register scalar function
    // rc = sqlite3_create_function(db, "one", 1,
    //                              SQLITE_UTF8 | SQLITE_DIRECTONLY | SQLITE_DETERMINISTIC,
    //                              0, one, 0, 0);
    // register aggregate function
    if (rc == SQLITE_OK)
    {
        rc = sqlite3_create_function(db, "mode", 1,
                                     SQLITE_UTF8 | SQLITE_DIRECTONLY | SQLITE_DETERMINISTIC,
                                     0, 0, modeStep, modeFinal);
    }
    // // is this ok?
    // if (rc == SQLITE_OK)
    //     rc = SQLITE_OK_LOAD_PERMANENTLY;
    return rc;
}
