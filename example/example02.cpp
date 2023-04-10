#include "KVSQLite/DB.h"
#include <string>
#include <iostream>

void example()
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
        KVSQLite::WriteBatch<std::string, std::string> batch;

        batch.put("key1", "val1");
        batch.put("key2", "val2");
        batch.del("key3");
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
    example();
    return 0;
}
