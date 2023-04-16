#ifndef KVSQLITE_EXPORT_H_
#define KVSQLITE_EXPORT_H_

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
    #define KVSQLITE_EXPORT
#endif

#endif
