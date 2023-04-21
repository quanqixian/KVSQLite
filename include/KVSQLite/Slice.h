/**
 * @file Slice.h
 * @brief Slice is a simple structure containing a pointer into some external
 * storage and a size.  The user of a Slice must ensure that the slice
 * is not used after the corresponding external storage has been
 * deallocated.
 *
 * Multiple threads can invoke const methods on a Slice without
 * external synchronization, but if any of the threads may call a
 * non-const method, all threads accessing the same Slice must use
 * external synchronization.
 */

#ifndef _KVSQLITE_SLICE_H_
#define _KVSQLITE_SLICE_H_

#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>

#include "KVSQLite/Export.h"

namespace KVSQLite
{

class KVSQLITE_EXPORT Slice
{
public:
    /**
     * @brief      Create an empty slice.
     */
    Slice() : m_data(""), m_size(0){}

    /**
     * @brief      Create a slice that refers to d[0,n-1].
     * @param[in]  d : data pointer
     * @param[in]  n : data size
     */
    Slice(const char* d, size_t n) : m_data(d), m_size(n) {}

    /**
     * @brief      Create a slice that refers to the contents of "s"
     * @param[in]  s : another Slice object
     */
    Slice(const std::string& s) : m_data(s.data()), m_size(s.size()) {}

    /**
     * @brief      Create a slice that refers to s[0,strlen(s)-1]
     * @param[in]  s : data pointer
     */
    Slice(const char* s) : m_data(s), m_size(strlen(s)) {}

    /**
     * @brief      Constructs a copy of Slice.
     */
    Slice(const Slice&) = default;

    /**
     * @brief      Assigns a new value to the Slice, replacing its current contents.
     */
    Slice& operator= (const Slice&) = default;

    /**
     * @brief      Return a pointer to the beginning of the referenced data
     * @return     A pointer to the data.
     */
    const char* data() const {return m_data;}

    /**
     * @brief      Return the length (in bytes) of the referenced data
     * @return     Length of data.
     */
    size_t size() const {return m_size;}

    /**
     * @brief      Return true iff the length of the referenced data is zero.
     * @return     return true if mepty.
     */
    bool empty() const {return m_size == 0;}

    /**
     * @brief      Return the ith byte in the referenced data. REQUIRES: n < size()
     * @return     return "data[i]"
     */
    char operator[](size_t n) const
    {
        assert(n < size());
        return m_data[n];
    }

    /**
     * @brief      Change this slice to refer to an empty array
     */
    void clear()
    {
        m_data = "";
        m_size = 0;
    }

    /**
     * @brief      Drop the first "n" bytes from this slice.
     */
    void removePrefix(size_t n)
    {
        assert(n <= size());
        m_data += n;
        m_size -= n;
    }

    /**
     * @brief      transform data to string
     * @return     Return a string that contains the copy of the referenced data.
     */
    std::string toString() const {return std::string(m_data, m_size);}

    /**
     * @brief      Three-way comparison.  
     * @return     Returns value:
     * <  0 iff "*this" <  "b",
     * == 0 iff "*this" == "b",
     * >  0 iff "*this" >  "b"
     */
    inline int compare(const Slice& b) const
    {
        const size_t min_len = (m_size < b.m_size) ? m_size : b.m_size;
        int r = memcmp(m_data, b.m_data, min_len);
        if (r == 0)
        {
            if (m_size < b.m_size)
            {
                r = -1;
            }
            else if(m_size > b.m_size)
            {
                r = +1;
            }
        }
        return r;
    }

    /**
     * @brief      Determine whether the current Slice starts with x
     * @return     Return true iff "x" is a prefix of "*this"
     */
    bool starts_with(const Slice& x) const
    {
        return ((m_size >= x.m_size) && (memcmp(m_data, x.m_data, x.m_size) == 0));
    }
private:
    const char* m_data;
    size_t m_size;
};

inline bool operator == (const Slice& x, const Slice& y)
{
    return ((x.size() == y.size()) &&
          (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator != (const Slice& x, const Slice& y) {return !(x == y);}

} /* end of namespace */

#endif
