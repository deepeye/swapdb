#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>
#include "SSDB_client.h"
#include "gtest/gtest.h"
#include "ssdb_test.h"
using namespace std;

class HashTest : public SSDBTest
{
    public:
        ssdb::Status s;
        std::vector<std::string> list, keys, kvs2;
        std::map<std::string, std::string> kvs;
        string key, val, getVal, field;
        uint16_t keysNum;
        int64_t ret;
};

TEST_F(HashTest, Test_hash_hset) {
#define OKHset s = client->hset(key, field, val);\
    ASSERT_TRUE(s.ok())<<"fail to hset key!"<<endl;\
    s = client->hget(key, field, &getVal);\
    ASSERT_TRUE(s.ok()&&(val == getVal))<<"fail to hget key val!"<<s.code()<<key<<endl;

#define FalseHset s = client->hset(key, field, val);\
    ASSERT_TRUE(s.error())<<"this key should set fail!"<<s.code()<<endl;

    // Some special keys
    for(vector<string>::iterator it = Keys.begin(); it != Keys.end(); it++)
    {
        key = *it;
        field = GetRandomField_();
        val = GetRandomVal_();
        s = client->multi_del(key);
        OKHset

        // set exsit key
        field = GetRandomField_();
        val = GetRandomVal_();
        OKHset
        s = client->multi_del(key);
    }

    // Some random keys
    keysNum = 100;
    val = "";
    key = GetRandomKey_();
    field = GetRandomField_();
    s = client->multi_del(key);
    for(int n = 0; n < keysNum; n++)
    {
        field = field+itoa(n);
        OKHset
    }
    s = client->hsize(key, &ret);
    ASSERT_EQ(keysNum, ret);
    s = client->multi_del(key);

    //other types key
    field = GetRandomField_();
    val = GetRandomVal_();

    client->multi_del(key);
    s = client->set(key, val);
    FalseHset

    client->multi_del(key);
    client->sadd(key, val);
    FalseHset

    client->multi_del(key);
    client->qpush_front(key, val);
    FalseHset

    client->multi_del(key);
    client->zset(key, field, 1.0);
    FalseHset
    client->multi_del(key);
}

TEST_F(HashTest, Test_hash_hget) {
#define NotFoundHget s = client->hget(key, field, &getVal);\
    ASSERT_TRUE(s.not_found())<<"this key should be not found!"<<endl;

    // Some random keys
    for(int n = 0; n < 5; n++)
    {
        key = GetRandomKey_();
        val = GetRandomVal_();
        field = GetRandomField_();
        s = client->multi_del(key);
        NotFoundHget
    }

    keysNum = 100;
    for(int n = 0; n < keysNum; n++)
    {
        field = field+itoa(n);
        s = client->hset(key, field, val);
    }
    field = field+itoa(keysNum);
    NotFoundHget
    s = client->multi_del(key);
}

TEST_F(HashTest, Test_hash_hdel) {
#define OKHdel s = client->multi_hdel(key, field);\
    ASSERT_TRUE(s.ok())<<"fail to delete key!"<<key<<endl;\
    s = client->hget(key, field, &getVal);\
    ASSERT_TRUE(s.not_found())<<"this key should be deleted!"<<key<<endl;

#define NotFoundHdel s = client->multi_hdel(key, field);\
    ASSERT_TRUE(s.not_found())<<"this key should be not found!"<<endl;
    //Some special keys
    for(vector<string>::iterator it = Keys.begin(); it != Keys.end(); it++)
    {
        key = *it;
        client->multi_del(key);
        val = GetRandomVal_();
        field = GetRandomField_();
        s = client->hset(key, field, val);
        OKHdel
        // NotFoundHdel
    }

    keysNum = 100;
    val = "";
    for(int n = 0; n < keysNum; n++)
    {
        field = field+itoa(n);
        s = client->hset(key, field, val);
        OKHdel
        // NotFoundHdel
    }
}

TEST_F(HashTest, Test_hash_hincrby) {
#define OKHincr incr = GetRandomInt64_();\
    s = client->multi_del(key);\
    s = client->hincr(key, field, incr, &ret);\
    ASSERT_TRUE(s.ok());\
    s = client->hget(key, field, &getVal);\
    ASSERT_EQ(to_string(incr), getVal);\
    \
    s = client->hincr(key, field, n, &ret);\
    ASSERT_TRUE(s.ok());\
    s = client->hget(key, field, &getVal);\
    ASSERT_EQ(to_string(incr+n), getVal);\
    s = client->multi_hdel(key, field);

#define FalseHincr s = client->hincr(key, field, 1, &ret);\
    ASSERT_TRUE(s.error())<<"this key should hincr fail!"<<endl;

    int64_t incr, ret, n = 0;
    //Some special keys
    for(vector<string>::iterator it = Keys.begin(); it != Keys.end(); it++)
    {
        n++;
        key = *it;
        field = GetRandomField_();
        OKHincr
        client->multi_del(key);
    }

    //Some random keys
    keysNum = 10;
    val = "";
    for(n = 0; n < keysNum; n++)
    {
        key = GetRandomKey_();
        field = GetRandomField_();
        OKHincr
        client->multi_del(key);
    }

    s = client->multi_del(key);
    s = client->hincr(key, field, MAX_INT64, &ret);
    s = client->hget(key, field, &getVal);
    ASSERT_EQ(i64toa(MAX_INT64), getVal);
    s = client->hincr(key, field, 1, &ret);
    ASSERT_TRUE(s.error());
    s = client->hget(key, field, &getVal);
    ASSERT_EQ(i64toa(MAX_INT64), getVal);

    s = client->multi_del(key);
    s = client->hincr(key, field, MIN_INT64, &ret);
    ASSERT_EQ((MIN_INT64), ret);

    s = client->hget(key, field, &getVal);
    ASSERT_EQ(i64toa(MIN_INT64), getVal);
    s = client->hincr(key, field, -1, &ret);
    ASSERT_TRUE(s.error());
    s = client->hget(key, field, &getVal);
    ASSERT_EQ(i64toa(MIN_INT64), getVal);
    s = client->multi_hdel(key, field);

    //other types key
    field = GetRandomField_();
    val = GetRandomVal_();

    client->multi_del(key);
    s = client->set(key, val);
    FalseHincr

    client->multi_del(key);
    client->sadd(key, val);
    FalseHincr

    client->multi_del(key);
    client->qpush_front(key, val);
    FalseHincr

    client->multi_del(key);
    client->zset(key, field, 1.0);
    FalseHincr
    client->multi_del(key);
}

//hdecr removed
//TEST_F(HashTest, Test_hash_hdecrby) {
//#define OKHdecr decr = GetRandomInt64_();\
//    s = client->multi_del(key);\
//    s = client->hdecr(key, field, decr, &ret);\
//    ASSERT_TRUE(s.ok());\
//    s = client->hget(key, field, &getVal);\
//    ASSERT_EQ(to_string(-1*decr), getVal);\
//    \
//    s = client->hdecr(key, field, n, &ret);\
//    ASSERT_TRUE(s.ok());\
//    s = client->hget(key, field, &getVal);\
//    ASSERT_EQ(to_string(-1*(decr+n)), getVal);\
//    s = client->multi_hdel(key, field);
//
//    int64_t decr, ret, n = 0;
//    //Some special keys
//    for(vector<string>::iterator it = Keys.begin(); it != Keys.end(); it++)
//    {
//        n++;
//        key = *it;
//        field = GetRandomField_();
//        OKHdecr
//        client->multi_del(key);
//    }
//}

TEST_F(HashTest, Test_hash_hgetall) {
#define NotExsitHgetall s = client->hgetall(key, &list);\
    ASSERT_TRUE(s.ok());\
    ASSERT_EQ(0, list.size())<<"get list should be empty!"<<endl;

    // EXPECT_TRUE(s.not_found())<<"this key should be not found!"<<endl;

    key = GetRandomKey_();
    val = GetRandomVal_();
    field = GetRandomField_();
    s = client->multi_del(key);
    NotExsitHgetall
    keysNum = 10;
    for(int n = 0; n < keysNum; n++)
    {
        field = field+itoa(n);
        val = val+itoa(n);
		kvs.insert(std::make_pair(field, val));
        s = client->hset(key, field, val);
    }
    s = client->hgetall(key, &list);
    for(int n = 0; n < keysNum; n += 2)
    {
        EXPECT_EQ(kvs[list[n]], list[n+1]);
    }
    s = client->multi_del(key);
}

TEST_F(HashTest, Test_hash_hsize) {
#define OKHsize(num) s = client->hsize(key, &ret);\
    ASSERT_EQ(ret, num)<<"fail to hsize key!"<<key<<endl;\
    ASSERT_TRUE(s.ok())<<"hsize key not ok!"<<endl;

    // Some special keys
    for(vector<string>::iterator it = Keys.begin(); it != Keys.end(); it++)
    {
        key = *it;
        field = GetRandomField_();
        val = GetRandomVal_();
        s = client->multi_del(key);
        OKHsize(0)
        s = client->hset(key, field, val);
        OKHsize(1)
        s = client->multi_hdel(key, field);
    }

    key = GetRandomKey_();
    field = GetRandomField_();
    val = GetRandomVal_();
    s = client->multi_del(key);
    for(int n = 0; n < 10; n++)
    {
        field = field+itoa(n);
        val = val+itoa(n);
        client->hset(key, field, val);
        OKHsize(n+1)
    }
    s = client->multi_del(key);
}
//use del instead of hclear,and hclear cannot clear 100 elements hash key now.
/*
TEST_F(HashTest, Test_hash_hclear) {
#define OKHclear(num) s = client->hclear(key, &ret);\
    ASSERT_EQ(ret , num)<<"fail to hclear key!"<<key<<endl;\
    s = client->hsize(key, &ret);\
    ASSERT_EQ(ret, 0)<<"key is not null!"<<key<<endl;

    // Some special keys
    for(vector<string>::iterator it = Keys.begin(); it != Keys.end(); it++)
    {
        key = *it;
        field = GetRandomField_();
        val = GetRandomVal_();
        s = client->multi_del(key);
        OKHclear(0)
        s = client->hset(key, field, val);
        OKHclear(1)
    }

    key = GetRandomKey_();
    field = GetRandomField_();
    val = GetRandomVal_();
    s = client->multi_del(key);
    for(int n = 0; n < 100; n++)
    {
        field = field+itoa(n);
        val = val+itoa(n);
        client->hset(key, field, val);
    }
    OKHclear(100)
} */


TEST_F(HashTest, Test_hash_hkeys) {
    key = GetRandomKey_();
    s = client->hkeys(key, "", "", 5, &list);
    ASSERT_TRUE(s.ok() && list.size() == 0);
    list.clear();
    client->hset(key, "000000001","");
    client->hset(key, "000000002","");
    client->hset(key, "000000003","");
    //TODO HKEYS command changed
    s = client->hkeys(key, "000000000", "000000002", 5, &list);
    ASSERT_TRUE(s.ok() && list.size() == 3);
    ASSERT_EQ("000000001", list[0]);
    ASSERT_EQ("000000002", list[1]);
    list.clear();
    s = client->hkeys(key, "000000000", "000000003", 5, &list);
    ASSERT_TRUE(s.ok() && list.size() == 3);
    ASSERT_EQ("000000003", list[2]);
    list.clear();
    s = client->hkeys(key, "000000000", "000000003", 2, &list);
    ASSERT_TRUE(s.ok() && list.size() == 3);
    s = client->multi_del(key);
}

TEST_F(HashTest, DISABLED_Test_hash_hscan) {
    key = GetRandomKey_();
    s = client->hscan(key, "", "", 2, &list);
    ASSERT_TRUE(s.ok() && list.size() <= 4);
    list.clear();
    client->multi_del("key");
    client->hset("key", "00000000f1","v1");
    client->hset("key", "00000000f2","v2");
    s = client->hscan("key", "00000000f0", "00000000f2", 2, &list);
    ASSERT_TRUE(s.ok() && list.size() == 4);
    ASSERT_EQ("00000000f1", list[0]);
    ASSERT_EQ("v1", list[1]);
    ASSERT_EQ("00000000f2", list[2]);
    ASSERT_EQ("v2", list[3]);
    list.clear();
    s = client->hscan("key", "00000000f2", "00000000f0", 2, &list);
    ASSERT_EQ(0, list.size());
    s = client->multi_del("key");
    s = client->hscan("key", "00000000f0", "00000000f2", 2, &list);
    ASSERT_EQ(0, list.size());
}

//remove hrscan
/* TEST_F(HashTest, Test_hash_hrscan) {
    key = GetRandomKey_();
    s = client->hrscan(key, "", "", 2, &list);
    ASSERT_TRUE(s.ok() && list.size() <= 4);
    list.clear();
    client->multi_del("key");
    client->hset("key", "00000000f1","v1");
    client->hset("key", "00000000f2","v2");
    s = client->hrscan("key", "00000000f3", "00000000f1", 2, &list);
    ASSERT_TRUE(s.ok() && list.size() == 4);
    ASSERT_EQ("00000000f2", list[0]);
    ASSERT_EQ("v2", list[1]);
    ASSERT_EQ("00000000f1", list[2]);
    ASSERT_EQ("v1", list[3]);
    list.clear();
    s = client->hrscan("key", "00000000f1", "00000000f3", 2, &list);
    ASSERT_EQ(0, list.size());
    s = client->multi_del("key");
    s = client->hrscan("key", "00000000f3", "00000000f1", 2, &list);
    ASSERT_EQ(0, list.size());
} */

TEST_F(HashTest, Test_hash_hmset_hmget_hdel) {
//Redis hmset/hmget/hdel
    string key, field1, field2, field3, val1, val2, val3;

    key = GetRandomKey_();
    field1 = GetRandomField_();
    field2 = field1+'2';
    field3 = field1+'3';
    val1 = GetRandomVal_();
    val2 = val1+'2';
    val3 = val1+'3';
    kvs.clear();
    keys.clear();
    list.clear();
    kvs.insert(std::make_pair(field1, val1));
    kvs.insert(std::make_pair(field2, val2));
    keys.push_back(field1);
    keys.push_back(field2);
    keys.push_back(field3);

    //all keys not exist
    s = client->multi_hdel(key, keys, &ret);
    ASSERT_TRUE(s.ok());
    ASSERT_EQ(0, ret);
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(0, list.size());
    s = client->multi_hset(key, kvs);
    ASSERT_TRUE(s.ok());
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(4, list.size());
    ASSERT_EQ(field1, list[0]);
    ASSERT_EQ(val1, list[1]);
    ASSERT_EQ(field2, list[2]);
    ASSERT_EQ(val2, list[3]);
    kvs.insert(std::make_pair(field3, val3));

    //one key not exist, two keys exist
    s = client->multi_hset(key, kvs);
    ASSERT_TRUE(s.ok());
    list.clear();
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(6, list.size());
    kvs.clear();
    val1 = val1+'1';
    val2 = val2+'2';
    val3 = val3+'3';
    kvs.insert(std::make_pair(field1, val1));
    kvs.insert(std::make_pair(field2, val2));
    kvs.insert(std::make_pair(field3, val3));

    //all keys exist, update their vals
    s = client->multi_hset(key, kvs);
    ASSERT_TRUE(s.ok());
    list.clear();
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(6, list.size());
    ASSERT_EQ(field1, list[0]);
    ASSERT_EQ(val1, list[1]);
    ASSERT_EQ(field2, list[2]);
    ASSERT_EQ(val2, list[3]);
    ASSERT_EQ(field3, list[4]);
    ASSERT_EQ(val3, list[5]);
    s = client->multi_hdel(key, keys, &ret);
    ASSERT_TRUE(s.ok());
    ASSERT_EQ(3, ret);
    list.clear();
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(0, list.size());
    kvs.clear();
    list.clear();
    keys.clear();

    int fieldNum = 10;
    for(int n = 0; n < fieldNum; n++)
    {
        kvs.insert(std::make_pair(field1 + itoa(n), val1 + itoa(n)));
        keys.push_back(field1 + itoa(n));
    }
    s = client->multi_hset(key, kvs);
    ASSERT_TRUE(s.ok());
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(fieldNum*2, list.size());

    for(int n = 0; n < fieldNum; n++)
    {
        ASSERT_EQ(field1 + itoa(n), list[n*2]);
        ASSERT_EQ(val1 + itoa(n), list[n*2+1]);
    }

    s = client->multi_hdel(key, keys, &ret);
    ASSERT_TRUE(s.ok());
    ASSERT_EQ(fieldNum, ret);
    list.clear();
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(0, list.size());

    client->sadd(key, "val");
    s = client->multi_hdel(key, keys, &ret);
    ASSERT_TRUE(s.error());
    s = client->multi_hget(key, keys, &list);
    ASSERT_TRUE(s.error());
    s = client->multi_hset(key, kvs);
    ASSERT_TRUE(s.error());
    client->multi_del(key);
}

TEST_F(HashTest, Test_hash_samefields_hmset_hmget_hdel) {
//simulate process same fields at one time.
    key = "hkey";
    field = "hfield_";
    val = "hval_";
    keysNum = 1;
    kvs2.clear();
    keys.clear();

    for(int m = 0;m < 2;m++){
        for(int n = 0;n < keysNum;n++) {
            keys.push_back(field+itoa(n));
            kvs2.push_back(field+itoa(n));
            kvs2.push_back(val+itoa(n));
        }
    }

    //all keys not exist
    client->multi_del(key);
    s = client->multi_hdel(key, keys, &ret);
    ASSERT_TRUE(s.ok());
    ASSERT_EQ(0, ret);
    list.clear();
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(0, list.size());
    //same field same value hmset
    s = client->multi_hset(key, kvs2);
    ASSERT_TRUE(s.ok());
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(keys.size()*2, list.size());

    for(int n = 0;n < 2*keysNum;n++) {
        ASSERT_EQ(field+itoa(n%keysNum), list[2*n]);
        ASSERT_EQ(val+itoa(n%keysNum), list[2*n+1]);
    }

    //same key diff value mset
    for(int n = 0;n < keysNum;n++) {
        keys.push_back(field+itoa(n));
        kvs2.push_back(field+itoa(n));
        kvs2.push_back(val+itoa(n*2));
    }

    s = client->multi_hset(key, kvs2);
    ASSERT_TRUE(s.ok());
    list.clear();
    s = client->multi_hget(key, keys, &list);
    ASSERT_EQ(keys.size()*2, list.size());

    for(int n = 0;n < 3*keysNum;n++) {
        ASSERT_EQ(field+itoa(n%keysNum), list[2*n]);
        ASSERT_EQ(val+itoa(n%keysNum*2), list[2*n+1]);
    }

    client->multi_hdel(key, keys, &ret);
    ASSERT_EQ(keysNum, ret);
}
