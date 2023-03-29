#include "gtest/gtest.h"
#include "db.h"

/**
 * @brief 
 */
TEST(KVSQLite, write)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open("KVSQLite.db", &pDB);
    ASSERT_EQ(status.ok(), true);

    for(int i= 1; i <= 100; i++)
    {
        pDB->put(i, i*100);
        EXPECT_EQ(status.ok(), true);
    }

    delete pDB;
}

/**
 * @brief 
 */
TEST(KVSQLite, read)
{
    KVSQLite::DB<int, int> * pDB = nullptr;
    KVSQLite::Status status = KVSQLite::DB<int, int>::open("KVSQLite.db", &pDB);
    ASSERT_EQ(status.ok(), true);

    for(int i= 1; i <= 100; i++)
    {
        int val = 0;
        pDB->get(i, val);
        EXPECT_EQ(status.ok(), true);
        EXPECT_EQ(val, i*100);
    }

    delete pDB;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
