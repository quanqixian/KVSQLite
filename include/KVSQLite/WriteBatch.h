#ifndef _KVSQLITE_WRITE_BATCH_H_
#define _KVSQLITE_WRITE_BATCH_H_

#include <list>

namespace KVSQLite
{

template<typename K, typename V>
class KVSQLITE_EXPORT WriteBatch
{
public:
    enum NodeType
    {
        PUT,
        DEL
    };
    struct Node
    {
        NodeType type;
        K key;
        V value;
    };
public:
    void put(const K & key, const V & value)
    {
        m_list.push_back({NodeType::PUT, key, value});
    }
    void del(const K & key)
    {
        m_list.push_back({NodeType::DEL, key});
    }
    void clear()
    {
        m_list.clear();
    }
    std::list<Node> getList()
    {
        return m_list;
    }
private:
    std::list<Node> m_list;
};

}/* end of namespace KVSQLite */
#endif
