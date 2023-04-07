#include "KVSQLite/Status.h"

namespace KVSQLite
{
class StatusPrivate
{
public:
    std::string driverError;
    std::string databaseError;
    Status::ErrorType errorType;
    std::string errorCode;
};

Status::Status()
{

}

Status::Status(const std::string &driverText, const std::string &databaseText,
                     ErrorType type, const std::string &code)
{
    d = new StatusPrivate;

    d->driverError = driverText;
    d->databaseError = databaseText;
    d->errorType = type;
    d->errorCode = code;
}


Status::Status(const Status& other)
{
    if(nullptr != other.d)
    {
        d = new StatusPrivate;
        *d = *other.d;
    }
}

Status& Status::operator=(const Status& other)
{
    if(nullptr == other.d)
    {
        if(nullptr != d)
        {
            delete d;
        }
        d = nullptr;
    }
    else
    {
        if(nullptr == d)
        {
            d = new StatusPrivate;
        }
        *d = *other.d;
    }

    return *this;
}

bool Status::operator==(const Status& other) const
{
    int flag = ((nullptr != d) << 1)|(nullptr != other.d);

    switch(flag)
    {
    case 0: /* both NULL */
        {
            return true;
        }
    case 1: /* one is NULL */
    case 2:
        {
            return false;
        }
    case 3: /* neither NULL */
        {
            return (d->errorType == other.d->errorType);
        }
    default:
        break;
    }

    return true;
}

bool Status::operator!=(const Status& other) const
{
    return !(*this == other);
}

Status::~Status()
{
    delete d;
}

std::string Status::driverText() const
{
    return d ? d->driverError : "";
}

std::string Status::databaseText() const
{
    return d ? d->databaseError : "";
}

Status::ErrorType Status::type() const
{
    return d ? d->errorType : NoError;
}

std::string Status::nativeErrorCode() const
{
    return d ? d->errorCode : 0;
}

std::string Status::toString() const
{
    std::string result;
    if(d)
    {
        result= d->databaseError;
        result += ' ';
        result += d->driverError;
    }

    return result;
}

bool Status::ok() const
{
    return d ? (d->errorType == NoError) : true;
}

}/* end of namespace KVSQLite */

