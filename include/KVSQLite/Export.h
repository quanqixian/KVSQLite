#ifndef KVSQLITE_EXPORT_H_
#define KVSQLITE_EXPORT_H_

#ifdef SQLITE_SHARED_LIBRARY
    #ifdef _WIN32
        /*
         * The KVSQLITE_EXPORT_SYMBOL macro is defined in the Cmake file that
         * compiles the KVSQLite library 
         */
        #ifdef KVSQLITE_EXPORT_SYMBOL 
            #define KVSQLITE_EXPORT    __declspec( dllexport )
        #else
            #define KVSQLITE_EXPORT    __declspec( dllimport )
        #endif
    #else
        #ifdef KVSQLITE_EXPORT_SYMBOL 
            #define KVSQLITE_EXPORT    __attribute__((visibility("default")))
        #else 
            #define KVSQLITE_EXPORT
        #endif
    #endif
#else
    #define KVSQLITE_EXPORT
#endif

#endif /* end of KVSQLITE_EXPORT_H_ */
