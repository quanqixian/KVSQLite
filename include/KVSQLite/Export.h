#ifndef KVSQLITE_EXPORT_H_
#define KVSQLITE_EXPORT_H_

#ifdef KVSQLITE_BUILD_WITH_SOURCES
    #define KVSQLITE_EXPORT
#else
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
#endif

#endif /* end of KVSQLITE_EXPORT_H_ */
