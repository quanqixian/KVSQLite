#ifndef KVSQLITE_EXPORT_H_
#define KVSQLITE_EXPORT_H_

#ifdef _WIN32
    /*
     * The KVSQLITE_EXPORT_SYMBOL macro is defined in the Cmake file that
     * compiles the KVSQLite library 
     */
    #ifdef KVSQLITE_EXPORT_SYMBOL 
        #define Q_SQL_EXPORT    __declspec( dllexport )
    #else
        #define Q_SQL_EXPORT    __declspec( dllimport )
    #endif
#else
    #define Q_SQL_EXPORT
#endif

#endif
