#include "gtest/gtest.h"
#include "db.h"

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

    for(int i= 1; i <= 10; i++)
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
    status = pDB->get(1000, val);
    EXPECT_EQ(status.ok(), false);
    EXPECT_EQ(status.type(), KVSQLite::Status::NotFound);

    delete pDB;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
