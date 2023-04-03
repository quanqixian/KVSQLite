# 1.Introduction

KVSQLite implements the interface of using sqlite in key-value mode. The interface of KVSQLite is modeled after the interface of leveldb.


# 2.Documentation

KVSQLite documentation is generated using  [Doxygen](http://www.doxygen.org/).

# 3.Examples

Here is a simple example of using EVHttpServer:

```c++
KVSQLite::DB<int, int> * pDB = nullptr;
KVSQLite::Options opt;
int val = 0;

KVSQLite::Status status = KVSQLite::DB<int, int>::open(opt, "KVSQLite.db", &pDB);
status = pDB->put(KVSQLite::WriteOptions(), 1, 100);
status = pDB->get(1, val);
status = pDB->del(KVSQLite::WriteOptions(), 1);

KVSQLite::WriteBatch<int, int> batch;
batch.put(1, 100);
batch.del(1);
status = pDB->write(KVSQLite::WriteOptions(), &batch);

delete pDB;
```

See more examples [here](./example/README.md).

# 4.Build


1. Clone the repository

```shell
git clone https://gitee.com/QWorkShop/KVSQLite.git
```

2. Generate the necessary build files

   In this step, the third-party library will be cloned.

```cmake
cd KVSQLite
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=/path/to/install -DCMAKE_BUILD_TYPE=Release
```

3. Compile the source code. In this step, third-party libraries, KVSQLite library, samples, tests will be compiled.

```cmake
cmake --build build
```

4. Install to system

```cmake
cmake --install build
```

Now you can use the KVSQLite library, include the header file in the code, link the KVSQLite library when compiling.


