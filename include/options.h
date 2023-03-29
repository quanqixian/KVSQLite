#ifndef _KVSQLITE_OPTIONS_H_
#define _KVSQLITE_OPTIONS_H_
#include "export.h"

namespace KVSQLite
{

/* Options that control write operations */
struct Q_SQL_EXPORT WriteOptions
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

