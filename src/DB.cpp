#include "KVSQLite/DB.h"
#include <cstdio>
#include "sqlite3.h"

namespace KVSQLite
{

template<typename T>
struct mapping_traits
{
};

template<>
struct mapping_traits<int>
{
public:
    static int bind(sqlite3_stmt *stmt, const int &idx, const int &val)
    {
        return sqlite3_bind_int(stmt, idx, val);
    }
    static int getColumn(sqlite3_stmt *stmt, const int &idx)
    {
        return sqlite3_column_int(stmt, idx);
    }
};

template<>
struct mapping_traits<int64_t>
{
public:
    static int bind(sqlite3_stmt *stmt, const int &idx, const int64_t &val)
    {
        return sqlite3_bind_int64(stmt, idx, val);
    }
    static int64_t getColumn(sqlite3_stmt *stmt, const int &idx)
    {
        return sqlite3_column_int64(stmt, idx);
    }
};

template<>
struct mapping_traits<double>
{
public:
    static int bind(sqlite3_stmt *stmt, const int &idx, const double &val)
    {
        return sqlite3_bind_double(stmt, idx, val);
    }
    static double getColumn(sqlite3_stmt *stmt, const int &idx)
    {
        return sqlite3_column_double(stmt, idx);
    }
};

template<>
struct mapping_traits<std::string>
{
public:
    static int bind(sqlite3_stmt *stmt, const int &idx, const std::string &val)
    {
        return sqlite3_bind_text(stmt, idx, val.c_str(), val.length() + 1, SQLITE_TRANSIENT);
    }
    static std::string getColumn(sqlite3_stmt *stmt, const int &idx) 
    {
        const char * p = (char *)sqlite3_column_text(stmt, idx);
        return p ? p : "";
    }
};

template<typename K, typename V>
Status DB<K, V>::execSQL(sqlite3 * p, const std::string & sql)
{
    char *errmsg = nullptr;

    /*
     * If the 5th parameter to sqlite3_exec() is not NULL and no errors occur,
     * then sqlite3_exec() sets the pointer in its 5th parameter to NULL before
     * returning.
     */
    int sqlRet = sqlite3_exec(p, sql.c_str(), nullptr, nullptr, &errmsg);
    if(SQLITE_OK != sqlRet)
    {
        std::string databaseErr = "Fail to exec:" + sql;
        return Status(errmsg ? errmsg : "", databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }
    return Status();
}

template<typename K, typename V>
Status DB<K, V>::setSync(sqlite3 *p, bool sync)
{
    const std::string query = sync ? "PRAGMA synchronous = FULL;" : "PRAGMA synchronous = OFF;";
    return execSQL(p, query);
}

template<typename K, typename V>
Status DB<K, V>::open(const Options & options, const std::string & filename, DB ** ppDB)
{
    if(nullptr == ppDB)
    {
        return Status("", "Invalid argument, ppDB is null.", Status::InvalidArgument, "0");
    }

    int sqlRet = 0;
    Status status;

    DB * & pDB = *ppDB;
    pDB = new(std::nothrow) DB();
    if(nullptr == pDB)
    {
        return Status("", "Fail to new.", Status::UnknownError, "0");
    }

    do
    {
        if(options.error_if_exists)
        {
            /* if open success, then file exist */
            FILE * pF = fopen(filename.c_str(), "r");
            if(nullptr != pF)
            {
                fclose(pF);
                std::string databaseErr = "File already exist:" + filename;
                status = Status("", databaseErr, Status::IOError, "0");
                break;
            }

        }
        /* open or create database file.
         * If the filename is an empty string, then a private, temporary on-disk
         * database will be created. This private database will be automatically
         * deleted as soon as the database connection is closed.
         */
        int flags = SQLITE_OPEN_READWRITE;
        if(options.create_if_missing)
        {
            flags |= SQLITE_OPEN_CREATE;
        }
        sqlRet = sqlite3_open_v2(filename.c_str(), &pDB->m_db, flags, nullptr);
        if(SQLITE_OK != sqlRet)
        {
            std::string databaseErr = "Fail to open:" + filename;
            status = Status(sqlite3_errmsg(pDB->m_db), databaseErr, Status::IOError, std::to_string(sqlRet));
            break;
        }

        /* By default, write is asynchronous */
        status = pDB->setSync(pDB->m_db, false);
        if(!status.ok())
        {
            return status;
        }

        const std::string tableName = "KVTable";
        {
            char *errmsg = nullptr;
            const std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + "(key PRIMARY KEY, value)";

            /*
             * If the 5th parameter to sqlite3_exec() is not NULL and no errors occur,
             * then sqlite3_exec() sets the pointer in its 5th parameter to NULL before
             * returning.
             */
            sqlRet = sqlite3_exec(pDB->m_db, query.c_str(), nullptr, nullptr, &errmsg);
            if(SQLITE_OK != sqlRet)
            {
                std::string databaseErr = "Fail to exec:" + query;
                status = Status(errmsg ? errmsg : "", databaseErr, Status::UnknownError, std::to_string(sqlRet));
                break;
            }
        }

        {
            const std::string query = "INSERT OR REPLACE INTO " + tableName + "(key, value) VALUES (?, ?)";
            sqlRet = sqlite3_prepare_v2(pDB->m_db, query.c_str(), query.size(), &pDB->m_putSQL, nullptr);
            if(SQLITE_OK != sqlRet)
            {
                std::string databaseErr = "Fail to exec:" + query;
                status = Status(sqlite3_errmsg(pDB->m_db), databaseErr, Status::InvalidArgument, std::to_string(sqlRet));
                break;
            }
        }

        {
            const std::string query = "SELECT value FROM " + tableName + " WHERE key = ?";
            sqlRet = sqlite3_prepare_v2(pDB->m_db, query.c_str(), query.size(), &pDB->m_getSQL, nullptr);
            if(SQLITE_OK != sqlRet)
            {
                std::string databaseErr = "Fail to exec:" + query;
                status = Status(sqlite3_errmsg(pDB->m_db), databaseErr, Status::InvalidArgument, std::to_string(sqlRet));
                break;
            }
        }

        {
            const std::string query = "DELETE FROM " + tableName + " WHERE key = ?";
            sqlRet = sqlite3_prepare_v2(pDB->m_db, query.c_str(), query.size(), &pDB->m_delSQL, nullptr);
            if(SQLITE_OK != sqlRet)
            {
                std::string databaseErr = "Fail to exec:" + query;
                status = Status(sqlite3_errmsg(pDB->m_db), databaseErr, Status::InvalidArgument, std::to_string(sqlRet));
                break;
            }
        }
    }while(0);

    if(!status.ok())
    {
        delete pDB;
        pDB = nullptr;
    }
    return status;
}

template<typename K, typename V>
DB<K, V>::~DB()
{
    close();
}

template<typename K, typename V>
Status DB<K, V>::put(const WriteOptions & options, const K & key, const V & value)
{
    int sqlRet = 0;
    std::lock_guard<std::mutex> locker(m_mutex);

    if(m_syncWrite != options.sync)
    {
        m_syncWrite = options.sync;
        Status status = setSync(m_db, false);
        if(!status.ok())
        {
            return status;
        }
    }

    sqlRet= sqlite3_reset(m_putSQL);
    if(SQLITE_OK != sqlRet)
    {
        std::string databaseErr = "Fail to sqlite3_reset.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }

    sqlRet = mapping_traits<K>::bind(m_putSQL, 1, key);
    if(SQLITE_OK != sqlRet)
    {
        std::string databaseErr = "Fail to bind key.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }

    sqlRet = mapping_traits<V>::bind(m_putSQL, 2, value);
    if(SQLITE_OK != sqlRet)
    {
        std::string databaseErr = "Fail to bind value.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }

    sqlRet = sqlite3_step(m_putSQL);
    if(SQLITE_DONE != sqlRet)
    {
        std::string databaseErr = "Fail to sqlite3_step.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }

    return Status();
}

template<typename K, typename V>
Status DB<K, V>::get(const K & key, V & value)
{
    std::lock_guard<std::mutex> locker(m_mutex);

    int sqlRet= sqlite3_reset(m_getSQL);
    if(SQLITE_OK != sqlRet)
    {
        std::string databaseErr = "Fail to sqlite3_reset.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }

    sqlRet = mapping_traits<K>::bind(m_getSQL, 1, key);
    if(SQLITE_OK != sqlRet)
    {
        std::string databaseErr = "Fail to bind key.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }

    sqlRet = sqlite3_step(m_getSQL);

    if(SQLITE_ROW != sqlRet)
    {
        std::string databaseErr = "Not found.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::NotFound, std::to_string(sqlRet));
    }

    value = mapping_traits<V>::getColumn(m_getSQL, 0);
    return Status();
}

template<typename K, typename V>
Status DB<K, V>::del(const WriteOptions & options, const K & key)
{
    int sqlRet = 0;
    std::lock_guard<std::mutex> locker(m_mutex);

    if(m_syncWrite != options.sync)
    {
        m_syncWrite = options.sync;
        Status status = setSync(m_db, m_syncWrite);
        if(!status.ok())
        {
            return status;
        }
    }

    sqlRet= sqlite3_reset(m_delSQL);
    if(SQLITE_OK != sqlRet)
    {
        std::string databaseErr = "Fail to sqlite3_reset.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }

    sqlRet = mapping_traits<K>::bind(m_delSQL, 1, key);
    if(SQLITE_OK != sqlRet)
    {
        std::string databaseErr = "Fail to bind key.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }

    sqlRet = sqlite3_step(m_delSQL);
    if(SQLITE_DONE != sqlRet)
    {
        std::string databaseErr = "Fail to sqlite3_step.";
        return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
    }

    return Status();
}

template<typename K, typename V>
Status DB<K, V>::write(const WriteOptions & options, WriteBatch<K, V>* updates)
{
    Status status;
    int sqlRet = 0;
    std::lock_guard<std::mutex> locker(m_mutex);

    if(m_syncWrite != options.sync)
    {
        m_syncWrite = options.sync;
        status = setSync(m_db, m_syncWrite);
        if(!status.ok())
        {
            return status;
        }
    }

    {
        status = execSQL(m_db, "BEGIN");
        if(!status.ok())
        {
            return status;
        }
    }

    auto list = updates->getList();
    for(auto iter = list.begin(); iter != list.end(); ++iter)
    {
        if(WriteBatch<K, V>::NodeType::PUT == iter->type)
        {
            status = [&]()->Status {
                sqlRet= sqlite3_reset(m_putSQL);
                if(SQLITE_OK != sqlRet)
                {
                    std::string databaseErr = "Fail to sqlite3_reset.";
                    return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
                }

                sqlRet = mapping_traits<K>::bind(m_putSQL, 1, iter->key);
                if(SQLITE_OK != sqlRet)
                {
                    std::string databaseErr = "Fail to bind key.";
                    return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
                }

                sqlRet = mapping_traits<V>::bind(m_putSQL, 2, iter->value);
                if(SQLITE_OK != sqlRet)
                {
                    std::string databaseErr = "Fail to bind value.";
                    return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
                }

                sqlRet = sqlite3_step(m_putSQL);
                if(SQLITE_DONE != sqlRet)
                {
                    std::string databaseErr = "Fail to sqlite3_step.";
                    return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
                }
                return Status();
            }();
        }
        else if(WriteBatch<K, V>::NodeType::DEL == iter->type)
        {
            status = [&]()->Status {
                sqlRet= sqlite3_reset(m_delSQL);
                if(SQLITE_OK != sqlRet)
                {
                    std::string databaseErr = "Fail to sqlite3_reset.";
                    return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
                }

                sqlRet = mapping_traits<K>::bind(m_delSQL, 1, iter->key);
                if(SQLITE_OK != sqlRet)
                {
                    std::string databaseErr = "Fail to bind key.";
                    return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
                }

                sqlRet = sqlite3_step(m_delSQL);
                if(SQLITE_DONE != sqlRet)
                {
                    std::string databaseErr = "Fail to sqlite3_step.";
                    return Status(sqlite3_errmsg(m_db), databaseErr, Status::UnknownError, std::to_string(sqlRet));
                }
                return Status();
            }();
        }

        if(!status.ok())
        {
            break;
        }
    }

    if(!status.ok())
    {
        execSQL(m_db, "ROLLBACK");
        return status;
    }

    {
        status = execSQL(m_db, "COMMIT");
        if(!status.ok())
        {
            execSQL(m_db, "ROLLBACK");
            return status;
        }
    }
    return status;
}

template<typename K, typename V>
DB<K, V>::DB()
{
}

template<typename K, typename V>
void DB<K, V>::close()
{
    std::lock_guard<std::mutex> locker(m_mutex);

    if(m_getSQL)
    {
        sqlite3_finalize(m_getSQL);
        m_getSQL = nullptr;
    }
    if(m_putSQL)
    {
        sqlite3_finalize(m_putSQL);
        m_putSQL = nullptr;
    }
    if(m_delSQL)
    {
        sqlite3_finalize(m_delSQL);
        m_delSQL = nullptr;
    }
    if(m_db)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

/* Those stupid code in order to put template class implementation in .cpp file.
 * ref : https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
 */
template class DB<int, int>;
template class DB<int, int64_t>;
template class DB<int, double>;
template class DB<int, std::string>;

template class DB<int64_t, int>;
template class DB<int64_t, int64_t>;
template class DB<int64_t, double>;
template class DB<int64_t, std::string>;

template class DB<double, int>;
template class DB<double, int64_t>;
template class DB<double, double>;
template class DB<double, std::string>;

template class DB<std::string, int>;
template class DB<std::string, int64_t>;
template class DB<std::string, double>;
template class DB<std::string, std::string>;

}/* end of namespace KVSQLite */

