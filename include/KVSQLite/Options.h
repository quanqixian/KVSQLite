#ifndef _KVSQLITE_OPTIONS_H_
#define _KVSQLITE_OPTIONS_H_
#include "Export.h"

namespace KVSQLite
{

/**
 * Options to control the behavior of a database (passed to DB::Open)
 */
struct KVSQLITE_EXPORT Options
{
    /* Create an Options object with default values for all fields. */
    Options() = default;

    /* If true, the database will be created if it is missing. */
    bool create_if_missing = true;

    /* If true, an error is raised if the database already exists. */
    bool error_if_exists = false;
};

/* Options that control write operations */
struct KVSQLITE_EXPORT WriteOptions
{
    WriteOptions() = default;

    /* If true, the write will be flushed from the operating system
     * buffer cache  before the write is considered complete.
     * If this flag is true, writes will be slower.
     */
    bool sync = false;
};

};

#endif

