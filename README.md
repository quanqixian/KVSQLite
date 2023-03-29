# 1.Introduction


# 2.Interface

KVSQLite documentation is generated using  [Doxygen](http://www.doxygen.org/).

# 3.Examples

Check out more examples [here](./example/README.md).


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


