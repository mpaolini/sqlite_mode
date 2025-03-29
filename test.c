#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int mode_cb(void *, int, char **, char **);

int main()
{
    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    rc = sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 1, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Can't enable load extension: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    char *err = NULL;
    rc = sqlite3_load_extension(db, "mode", NULL, &err);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Can't load extension: %s\n", err);
        sqlite3_free(err);
        return rc;
    }
    rc = sqlite3_exec(db, "create table people (name varchar(10), age numeric); insert into people values ('me', 1), ('you', 2), ('them', 2)", NULL, NULL, &err);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Can't exec statement: %s\n", err);
        sqlite3_free(err);
        return rc;
    }
    rc = sqlite3_exec(db, "select mode(age) from people;", mode_cb, NULL, &err);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Can't exec statement: %s\n", err);
        sqlite3_free(err);
        return rc;
    }
    sqlite3_close(db);
    fprintf(stdout, "Test Passed\n");
    return 0;
}

int mode_cb(void *arg, int cols, char **col_text, char **col_name)
{
    assert(cols == 1);
    assert(strcmp(col_text[0], "2.0") == 0);
    return 0;
}
