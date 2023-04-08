#include "KVSQLite/db.h"
#include <string>
#include <iostream>

void example01()
{
    KVSQLite::DB<std::string, std::string> * pDB = nullptr;
    KVSQLite::Status status = KVSQLite::DB<std::string, std::string>::open(KVSQLite::Options(), "KVSQLite.db", &pDB);
    if(!status.ok())
    {
        std::cout << status.toString() << std::endl;
        return;
    }

    do
    {
        std::string value;
        status = pDB->put(KVSQLite::WriteOptions(),  "key1", "value1");
        if(!status.ok())
        {
            std::cout << status.toString() << std::endl;
            break;
        }

        KVSQLite::Status s = pDB->get("key1", value);
        if(!status.ok())
        {
            std::cout << status.toString() << std::endl;
            break;
        }

        status = pDB->del(KVSQLite::WriteOptions(), "key1");
        if(!status.ok())
        {
            std::cout << status.toString() << std::endl;
            break;
        }
    }while(0);

    delete pDB;
}

int main(int argc, const char *argv[])
{
    example01();
    return 0;
}
