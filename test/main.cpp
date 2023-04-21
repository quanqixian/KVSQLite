#include "gtest/gtest.h"
#include "KVSQLite/DB.h"
#include "KVSQLite/Slice.h"
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
TEST(KVSQLite, delNoExist)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Options opt;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open(opt, "KVSQLite.db", &pDB);
    ASSERT_EQ(status.ok(), true);

    status = pDB->del(KVSQLite::WriteOptions(), 99999);
    EXPECT_EQ(status.ok(), true);

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

    /* If the following code runs slow like a synchronous way, it may be a bug */
    for(int i= 1; i <= 1000; i++)
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
        KVSQLite::DB<int, int> * pDB = nullptr;
        KVSQLite::Options opt;

        KVSQLite::Status status = KVSQLite::DB<int, int>::open(opt, ":memory:", &pDB);
        ASSERT_EQ(status.ok(), true);

        KVSQLite::WriteOptions options;
        status = pDB->put(options, 1, 2);
        EXPECT_EQ(status.ok(), true);
        int val = 0;
        status = pDB->get(1, val);
        EXPECT_EQ(status.ok(), true);
        status = pDB->del(KVSQLite::WriteOptions(), 1);
        EXPECT_EQ(status.ok(), true);
        delete pDB;
    }
    {
        KVSQLite::DB<int, int64_t> * pDB = nullptr;
        KVSQLite::Options opt;

        KVSQLite::Status status = KVSQLite::DB<int, int64_t>::open(opt, ":memory:", &pDB);
        ASSERT_EQ(status.ok(), true);

        KVSQLite::WriteOptions options;
        status = pDB->put(options, 1, 2);
        EXPECT_EQ(status.ok(), true);
        int64_t val = 0;
        status = pDB->get(1, val);
        EXPECT_EQ(status.ok(), true);
        status = pDB->del(KVSQLite::WriteOptions(), 1);
        EXPECT_EQ(status.ok(), true);
        delete pDB;
    }
    {
        KVSQLite::DB<int, double> * pDB = nullptr;
        KVSQLite::Options opt;

        KVSQLite::Status status = KVSQLite::DB<int, double>::open(opt, ":memory:", &pDB);
        ASSERT_EQ(status.ok(), true);

        KVSQLite::WriteOptions options;
        status = pDB->put(options, 1, 2.0);
        EXPECT_EQ(status.ok(), true);
        double val = 0;
        status = pDB->get(1, val);
        EXPECT_EQ(status.ok(), true);
        status = pDB->del(KVSQLite::WriteOptions(), 1);
        EXPECT_EQ(status.ok(), true);
        delete pDB;
    }
    {
        KVSQLite::DB<int, std::string> * pDB = nullptr;
        KVSQLite::Options opt;

        KVSQLite::Status status = KVSQLite::DB<int, std::string>::open(opt, ":memory:", &pDB);
        ASSERT_EQ(status.ok(), true);

        KVSQLite::WriteOptions options;
        status = pDB->put(options, 1, "2");
        EXPECT_EQ(status.ok(), true);
        std::string val;
        status = pDB->get(1, val);
        EXPECT_EQ(status.ok(), true);
        status = pDB->del(KVSQLite::WriteOptions(), 1);
        EXPECT_EQ(status.ok(), true);
        delete pDB;
    }

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

    for(int i= 1; i <= 200; i++)
    {
        batch.put(i, i+1);
    }
    for(int i= 1; i <= 200; i++)
    {
        if(i & 0x01)
        {
            batch.del(i);
        }
    }

    status = pDB->write(options, &batch);
    EXPECT_EQ(status.ok(), true);
    delete pDB;
}

/**
 * @brief
 */
TEST(KVSQLite, Slice)
{
    {
        KVSQLite::DB<int64_t, KVSQLite::Slice> * pDB = nullptr;
        KVSQLite::Options opt;
        KVSQLite::Status status = KVSQLite::DB<int64_t, KVSQLite::Slice>::open(opt, ":memory:", &pDB);
        ASSERT_EQ(status.ok(), true);

        KVSQLite::WriteOptions options;
        char buf[8] = {0x01, 0x02, 0x03, 0x00, 0x01};
        status = pDB->put(options, 1, KVSQLite::Slice(buf, sizeof(buf)));
        EXPECT_EQ(status.ok(), true);
        KVSQLite::Slice val;
        status = pDB->get(1, val);
        EXPECT_EQ(status.ok(), true);

        ASSERT_EQ(val.size(), sizeof(buf));
        for(int i = 0; i < val.size(); i++)
        {
            EXPECT_EQ(val[i], buf[i]);
        }
        status = pDB->del(KVSQLite::WriteOptions(), 1);
        EXPECT_EQ(status.ok(), true);
        delete pDB;
    }

    {
        KVSQLite::DB<KVSQLite::Slice, KVSQLite::Slice> * pDB = nullptr;
        KVSQLite::Options opt;
        KVSQLite::Status status = KVSQLite::DB<KVSQLite::Slice, KVSQLite::Slice>::open(opt, ":memory:", &pDB);
        ASSERT_EQ(status.ok(), true);

        KVSQLite::WriteOptions options;
        char keyBuf[8] = {0x01, 0x02, 0x03, 0x00, 0x01};
        char valBuf[8] = {0x03, 0x02, 0x01, 0x00, 0x04};
		KVSQLite::Slice key = KVSQLite::Slice(keyBuf, sizeof(keyBuf));
		KVSQLite::Slice val = KVSQLite::Slice(valBuf, sizeof(valBuf));

        status = pDB->put(options, key, val);
        EXPECT_EQ(status.ok(), true);
        KVSQLite::Slice valGet;
        status = pDB->get(key, valGet);
        EXPECT_EQ(status.ok(), true);

        ASSERT_EQ(valGet.size(), val.size());
        for(int i = 0; i < valGet.size(); i++)
        {
            EXPECT_EQ(valGet[i], val[i]);
        }
        status = pDB->del(KVSQLite::WriteOptions(), key);
        EXPECT_EQ(status.ok(), true);
        delete pDB;
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
