#ifndef __KVSQLITE_DB_H__
#define __KVSQLITE_DB_H__

#include <string>
#include "sqlite3.h"
#include "Status.h"

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
class DB
{
public:
    static Status open(const std::string & filename, DB ** ppDB)
    {
        if(nullptr == ppDB)
        {
            return Status("", "Invalid argument, ppDB is null.", Status::InvalidArgument, "0");
        }

        int sqlRet = 0;
        Status status;

        DB * & pDB = *ppDB;
        pDB = new(std::nothrow) DB();

        do
        {
            /* open or create database file.
             * If the filename is an empty string, then a private, temporary on-disk
             * database will be created. This private database will be automatically
             * deleted as soon as the database connection is closed.
             */
            int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
            sqlRet = sqlite3_open_v2(filename.c_str(), &pDB->m_db, flags, nullptr);
            if(SQLITE_OK != sqlRet)
            {
                std::string databaseErr = "Fail to open:" + filename;
                status = Status(sqlite3_errmsg(pDB->m_db), databaseErr, Status::IOError, std::to_string(sqlRet));
                break;
            }

            const std::string tableName = "KVTable";
            {
                char *errmsg = nullptr;
                const std::string query = ("CREATE TABLE IF NOT EXISTS " + tableName + "(key PRIMARY KEY, value)");

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
                sqlRet = sqlite3_prepare(pDB->m_db, query.c_str(), query.size(), &pDB->m_putSQL, nullptr);
                if(SQLITE_OK != sqlRet)
                {
                    std::string databaseErr = "Fail to exec:" + query;
                    status = Status(sqlite3_errmsg(pDB->m_db), databaseErr, Status::InvalidArgument, std::to_string(sqlRet));
                    break;
                }
            }

            {
                const std::string query = "SELECT value FROM " + tableName + " WHERE key = ?";
                sqlRet = sqlite3_prepare(pDB->m_db, query.c_str(), query.size(), &pDB->m_getSQL, nullptr);
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
    virtual ~DB()
    {
        close();
    };

    Status put(const K & key, const V & value)
    {
        int sqlRet= sqlite3_reset(m_putSQL);
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
    };

    Status get(const K & key, V & value)
    {
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
        if(SQLITE_DONE == sqlRet)
        {
            std::string databaseErr = "Key is not found.";
            return Status(sqlite3_errmsg(m_db), databaseErr, Status::NotFound, std::to_string(sqlRet));
        }
        else
        {
            value = mapping_traits<V>::getColumn(m_getSQL, 0);
        }

        return Status();
    }
private:
    DB()
    {
    }
    void close()
    {
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
        if(m_db)
        {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }
private:
    DB(const DB&) = delete;
    DB& operator=(const DB&) = delete;
private:
    sqlite3 *m_db = nullptr;
    sqlite3_stmt *m_putSQL = nullptr;
    sqlite3_stmt *m_getSQL = nullptr;
};

}/* end of namespace KVSQLite */

#endif
