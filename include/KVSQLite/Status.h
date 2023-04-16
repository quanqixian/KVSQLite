#ifndef _KVSQLITE_STATUS_H_
#define _KVSQLITE_STATUS_H_

#include <string>
#include "Export.h"

namespace KVSQLite
{

class StatusPrivate;
class KVSQLITE_EXPORT Status
{
public:
    enum ErrorType {
        NoError,
        NotFound,
        InvalidArgument,
        IOError,
        UnknownError
    };
    Status();
    Status(const std::string &driverText,
              const std::string &databaseText,
              ErrorType type,
              const std::string &errorCode);
    Status(const Status& other);
    Status& operator=(const Status& other);
    bool operator==(const Status& other) const;
    bool operator!=(const Status& other) const;
    ~Status();

    std::string driverText() const;
    std::string databaseText() const;
    ErrorType type() const;
    std::string nativeErrorCode() const;
    std::string toString() const;
    bool ok() const;

private:
    StatusPrivate *d = nullptr;
};

}/* end of namespace KVSQLite */

#endif
