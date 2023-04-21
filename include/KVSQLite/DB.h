/**
 * @file DB.h
 * @brief The DB class implements.
 */

#ifndef __KVSQLITE_DB_H__
#define __KVSQLITE_DB_H__

#include <string>
#include "Export.h"
#include "Status.h"
#include "Options.h"
#include "WriteBatch.h"

namespace KVSQLite
{

class DBImpl;
/**
 * @brief The DB class implements the database operation interface.
 */
template<typename K, typename V>
class KVSQLITE_EXPORT DB
{
public:
    /**
     * @brief      Open a database
     * @param[in]  options : options to control the behavior of a database. see @ref Options for details. 
     * @param[in]  filename : database path
     * @param[out] ppDB : pointer to a database pointer
     * @return     Status : on success Status::ok() is true. See @ref Status for details. 
     */
    static Status open(const Options & options, const std::string & filename, DB ** ppDB);
    virtual ~DB();

    /**
     * @brief      Set the database entry for "key" to "value".
     * @param[in]  options : Options that control write operations. see @ref WriteOptions for details. 
     * @param[in]  key : key of data
     * @param[in]  value : value of data
     * @return     Status : on success Status::ok() is true and false on error. See @ref Status for details. 
     */
    Status put(const WriteOptions & options, const K & key, const V & value);

    /**
     * @brief      If the database contains an entry for "key" store the corresponding value in value.
     * @param[in]  key : key of data
     * @param[in]  value : value of data
     * @return     Status : on success Status::ok() is true and false on error. See @ref Status for details. 
     */
    Status get(const K & key, V & value);

    /**
     * @brief      Remove the database entry (if any) for "key". It is not an error if "key" did not exist in the database.
     * @param[in]  options : Options that control write operations. see @ref WriteOptions for details. 
     * @param[in]  key : key of data
     * @return     Status : on success Status::ok() is true and false on error. See @ref Status for details. 
     */
    Status del(const WriteOptions & options, const K & key);

    /**
     * @brief      
     * @param[in]  options : Options that control write operations. see @ref WriteOptions for details. 
     * @param[in]  updates : 
     * @return     Status : on success Status::ok() is true and false on error. See @ref Status for details. 
     */
    Status write(const WriteOptions & options, WriteBatch<K, V>* updates);
private:
    DB();
    void close();
private:
    DB(const DB&) = delete;
    DB& operator=(const DB&) = delete;
private:
    DBImpl* m_DBImpl = nullptr;
};

}/* end of namespace KVSQLite */

#endif
