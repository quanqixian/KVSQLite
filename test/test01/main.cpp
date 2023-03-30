#include "gtest/gtest.h"
#include "db.h"
#include <thread>

/**
 * @brief 
 */
TEST(KVSQLite, put)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open("KVSQLite.db", &pDB);
    ASSERT_EQ(status.ok(), true);

    KVSQLite::WriteOptions options;

    for(int i= 1; i <= 100; i++)
    {
        status = pDB->put(options, i, i*100);
        EXPECT_EQ(status.ok(), true);
    }

    delete pDB;
}

/**
 * @brief 
 */
TEST(KVSQLite, get)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open("KVSQLite.db", &pDB);
    ASSERT_EQ(status.ok(), true);

    for(int i= 1; i <= 100; i++)
    {
        int val = 0;
        status = pDB->get(i, val);
        EXPECT_EQ(status.ok(), true);
        EXPECT_EQ(val, i*100);
    }

    delete pDB;
}

/**
 * @brief 
 */
TEST(KVSQLite, get_notFound)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open("KVSQLite.db", &pDB);
    ASSERT_EQ(status.ok(), true);

    int val = 0;
    status = pDB->get(99999, val);
    EXPECT_EQ(status.ok(), false);
    EXPECT_EQ(status.type(), KVSQLite::Status::NotFound);

    delete pDB;
}

/**
 * @brief 
 */
TEST(KVSQLite, del)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open("KVSQLite.db", &pDB);
    ASSERT_EQ(status.ok(), true);

    for(int i= 1; i <= 10; i++)
    {
        status = pDB->del(KVSQLite::WriteOptions(), i);
        EXPECT_EQ(status.ok(), true);

        int val = 0;
        status = pDB->get(i, val);
        EXPECT_EQ(status.ok(), false);
        EXPECT_EQ(status.type(), KVSQLite::Status::NotFound);
    }

    delete pDB;
}

/**
 * @brief 
 */
TEST(KVSQLite, multiDB)
{
    KVSQLite::DB<int, int> * pDBA = nullptr;
    KVSQLite::DB<int, int> * pDBB = nullptr;

    KVSQLite::Status statusA = KVSQLite::DB<int, int>::open("KVSQLiteA.db", &pDBA);
    ASSERT_EQ(statusA.ok(), true);

    KVSQLite::Status statusB = KVSQLite::DB<int, int>::open("KVSQLiteB.db", &pDBB);
    ASSERT_EQ(statusB.ok(), true);


    KVSQLite::WriteOptions optionsB;
    optionsB.sync = false;
    statusB = pDBB->put(optionsB, 0, 0);

    KVSQLite::WriteOptions optionsA;
    optionsA.sync = true;
    statusA = pDBA->put(optionsA, 0, 0);


    for(int i= 1; i <= 10000; i++)
    {
        statusB = pDBB->put(optionsB, i, i*100);
        EXPECT_EQ(statusB.ok(), true);
    }

    delete pDBA;
    delete pDBB;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
