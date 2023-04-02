#include "gtest/gtest.h"
#include "db.h"
#include <thread>

/**
 * @brief 
 */
TEST(KVSQLite, put)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Options opt;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open(opt, "KVSQLite.db", &pDB);
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
    KVSQLite::Options opt;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open(opt, "KVSQLite.db", &pDB);
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
    KVSQLite::Options opt;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open(opt, "KVSQLite.db", &pDB);
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
    KVSQLite::Options opt;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open(opt, "KVSQLite.db", &pDB);
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
    KVSQLite::Options opt;

    KVSQLite::Status statusA = KVSQLite::DB<int, int>::open(opt, "KVSQLiteA.db", &pDBA);
    ASSERT_EQ(statusA.ok(), true);

    KVSQLite::Status statusB = KVSQLite::DB<int, int>::open(opt, "KVSQLiteB.db", &pDBB);
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

/**
 * @brief
 */
TEST(KVSQLite, Memory)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Options opt;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open(opt, ":memory:", &pDB);
    ASSERT_EQ(status.ok(), true);

    KVSQLite::WriteOptions options;
    options.sync = true;

    for(int i= 1; i <= 1000; i++)
    {
        status = pDB->put(options, i, i*100);
        EXPECT_EQ(status.ok(), true);
    }

    delete pDB;
}

/**
 * @brief
 */
TEST(KVSQLite, Generics)
{
    {
        KVSQLite::DB<std::string, double> * pDB = nullptr;
        KVSQLite::Options opt;

        KVSQLite::Status status = KVSQLite::DB<std::string, double>::open(opt, ":memory:", &pDB);
        ASSERT_EQ(status.ok(), true);

        KVSQLite::WriteOptions options;
        status = pDB->put(options, "3.14", 3.14);
        EXPECT_EQ(status.ok(), true);
        double val = 0;
        status = pDB->get("3.14", val);
        EXPECT_EQ(status.ok(), true);
        status = pDB->del(KVSQLite::WriteOptions(), "3.14");
        EXPECT_EQ(status.ok(), true);
        delete pDB;
    }
    {
        KVSQLite::DB<int64_t, double> * pDB = nullptr;
        KVSQLite::Options opt;
        KVSQLite::Status status = KVSQLite::DB<int64_t, double>::open(opt, ":memory:", &pDB);
        ASSERT_EQ(status.ok(), true);

        KVSQLite::WriteOptions options;
        status = pDB->put(options, 1, 3.14);
        EXPECT_EQ(status.ok(), true);
        double val = 0;
        status = pDB->get(1, val);
        EXPECT_EQ(status.ok(), true);
        status = pDB->del(KVSQLite::WriteOptions(), 1);
        EXPECT_EQ(status.ok(), true);
        delete pDB;
    }
}

/**
 * @brief
 */
TEST(KVSQLite, write)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Options opt;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open(opt, "KVSQLite.db", &pDB);
    ASSERT_EQ(status.ok(), true);

    KVSQLite::WriteOptions options;
    options.sync = true;
    KVSQLite::WriteBatch<int, int> batch;

    for(int i= 1; i <= 10000; i++)
    {
        batch.put(i, i+1);
    }

    status = pDB->write(options, &batch);
    EXPECT_EQ(status.ok(), true);
    delete pDB;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
