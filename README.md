# 1.Introduction

[![build and test](https://github.com/quanqixian/KVSQLite/actions/workflows/build-test.yml/badge.svg)](https://github.com/quanqixian/KVSQLite/actions/workflows/build-test.yml)
[![badge](https://img.shields.io/badge/license-MIT-blue)](https://github.com/quanqixian/EVHttpServer/blob/master/LICENSE)
[![badge](https://img.shields.io/badge/document-doxygen-brightgreen)](https://quanqixian.github.io/EVHttpServer/)

KVSQLite implements the interface of using sqlite in key-value mode. The interface of KVSQLite is modeled after the interface of leveldb.


# 2.Documentation

KVSQLite documentation is generated using  [Doxygen](http://www.doxygen.org/).

# 3.Examples

## Opening A Database

A KVSQLite database has a name which corresponds to a file system directory. All
of the contents of database are stored in this directory. The following example
shows how to open a database, creating it if necessary:

```c++
#include <cassert>
#include "KVSQLite/DB.h"

KVSQLite::DB<std::string, std::string> * pDB = nullptr;
KVSQLite::Options options;
options.create_if_missing = true;
KVSQLite::Status status = KVSQLite::DB<std::string, std::string>::open(options, "KVSQLite.db", &db);
assert(status.ok());
...
```

If you want to raise an error if the database already exists, add the following
line before the `KVSQLite::DB::Open` call:

```c++
options.error_if_exists = true;
```

## Status

You may have noticed the `KVSQLite::Status` type above. Values of this type are
returned by most functions in KVSQLite that may encounter an error. You can check
if such a result is ok, and also print an associated error message:

```c++
KVSQLite::Status s = ...;
if (!s.ok()) cerr << s.toString() << endl;
```

## Closing A Database

When you are done with a database, just delete the database object. Example:

```c++
... open the db as described above ...
... do something with db ...
delete db;
```

## Reads And Writes

The database provides `put`, `del`, and `get` methods to modify/query the database.
For example, the following code moves the value stored under key1 to key2.

```c++
std::string value;
KVSQLite::Status s = pDB->get(key1, value);
if (s.ok()) s = pDB->put(KVSQLite::WriteOptions(),  key2, value);
if (s.ok()) s = pDB->del(KVSQLite::WriteOptions(), key1);
```

## Atomic Updates

Note that if the process dies after the Put of key2 but before the delete of
key1, the same value may be left stored under multiple keys. Such problems can
be avoided by using the `WriteBatch` class to atomically apply a set of updates:

```c++
#include "KVSQLite/WriteBatch.h"
...
std::string value;
KVSQLite::Status s = db->get(key1, &value);
if (s.ok()) {
  KVSQLite::WriteBatch<std::string, std::string>  batch;
  batch.del(key1);
  batch.put(key2, value);
  s = db->Write(KVSQLite::WriteOptions(), &batch);
}
```

The `WriteBatch` holds a sequence of edits to be made to the database, and these
edits within the batch are applied in order. Note that we called Delete before
Put so that if key1 is identical to key2, we do not end up erroneously dropping
the value entirely.

Apart from its atomicity benefits, `WriteBatch` may also be used to speed up
bulk updates by placing lots of individual mutations into the same batch.

## Synchronous Writes

By default, each write to KVSQLite is asynchronous: it returns after pushing the
write from the process into the operating system. The transfer from operating
system memory to the underlying persistent storage happens asynchronously. The
sync flag can be turned on for a particular write to make the write operation
not return until the data being written has been pushed all the way to
persistent storage.

```c++
KVSQLite::WriteOptions write_options;
write_options.sync = true;
db->put(write_options, ...);
```

Asynchronous writes are often more than a thousand times as fast as synchronous
writes. The downside of asynchronous writes is that a crash of the machine may
cause the last few updates to be lost. Note that a crash of just the writing
process (i.e., not a reboot) will not cause any loss since even when sync is
false, an update is pushed from the process memory into the operating system
before it is considered done.

Asynchronous writes can often be used safely. For example, when loading a large
amount of data into the database you can handle lost updates by restarting the
bulk load after a crash. A hybrid scheme is also possible where every Nth write
is synchronous, and in the event of a crash, the bulk load is restarted just
after the last synchronous write finished by the previous run. (The synchronous
write can update a marker that describes where to restart on a crash.)

`WriteBatch` provides an alternative to asynchronous writes. Multiple updates
may be placed in the same WriteBatch and applied together using a synchronous
write (i.e., `write_options.sync` is set to true). The extra cost of the
synchronous write will be amortized across all of the writes in the batch.

## Concurrency

A database may only be opened by one process at a time. The KVSQLite
implementation acquires a lock from the operating system to prevent misuse.
Within a single process, the same `KVSQLite::DB` object may be safely shared by
multiple concurrent threads. I.e., different threads may write into or call get 
on the same database without any external synchronization.
(the KVSQLite implementation will automatically do the required synchronization).
However other objects (like  `WriteBatch`) may require external
synchronization. If two threads share such an object, they must protect access
to it using their own locking protocol. More details are available in the public
header files.



See more examples [here](./example/README.md).

# 4.Build


1. Clone the repository

```shell
git clone https://github.com/quanqixian/KVSQLite.git
```

2. Generate the necessary build files

   In this step, the third-party library will be cloned.

```cmake
cd KVSQLite
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=/path/to/install -DCMAKE_BUILD_TYPE=Release
```

3. Compile the source code. In this step, third-party libraries, KVSQLite library, samples, tests will be compiled.

```cmake
cmake --build build --config Release
```

4. Install to system

```cmake
cmake --install build
```

Now you can use the KVSQLite library, include the header file in the code, link the KVSQLite library when compiling.

# 5.Blessing

- May you do good and not evil.
- May you find forgiveness for yourself and forgive others.
- May you share freely, never taking more than you give.

