#include <vector>
#include <algorithm>
#include <memory>

constexpr char null_byte = '\0';

// fix appropriate dummy values for word-checking (i.e. check if child at nullbyte is set or not)
// => necessary?
// debug
// implement VariableSize-SIMD version

template <typename Derived>
class BaseRep
{
protected:
    virtual std::pair<bool, Derived *> try_find(char c) = 0;
    virtual std::pair<bool, Derived *> try_insert_node(char c) = 0;
    virtual bool try_delete_node(char c) = 0;
    
    bool is_word()
    {
        return try_find(null_byte).first;
    }

    void set_word()
    {
        try_insert_node(null_byte);
    }

    void reset_word()
    {
        try_delete_node(null_byte);
    }

    BaseRep() {}

public:
    bool contains(char *c)
    {
        const auto [found, p] = try_find(*c);
        if (!found)
            return false;
        if (*c == null_byte)
            return p->is_word();
        return p->contains(++c);
    }

    bool delete_word(char *c)
    {
        auto [found, p] = try_find(*c);
        if (!found)
            return false;
        if (*c == null_byte)
        {
            if (p->is_word())
            {
                p->reset_word();
                return true;
            }
            return false;
        }
        return p->delete_word(++c);
    }

    bool insert_word(char *c)
    {
        auto [inserted, p] = try_insert_node(*c);
        if (*c == null_byte)
        {
            return inserted;
        }
        return p->insert_word(++c);
    }
};

class FixedSize : public BaseRep<FixedSize>
{
    friend class BaseRep<FixedSize>;

protected:
    std::vector<FixedSize *> vec;
    std::pair<bool, FixedSize *> try_find(char c) override
    {
        return {vec[c] != nullptr, vec[c]};
    }

    std::pair<bool, FixedSize *> try_insert_node(char c) override
    {
        if (vec[c] == nullptr)
        {
            vec[c] = new FixedSize();
            return {true, vec[c]};
        }
        return {false, vec[c]};
    }

    bool try_delete_node(char c) override
    {
        if (vec[c] != nullptr)
        {
            vec[c]->~FixedSize();
            vec[c] = nullptr;
            return true;
        }
        return false;
    }

public:
    FixedSize() : vec(UINT8_MAX, nullptr) { vec.shrink_to_fit(); }
    ~FixedSize()
    {
        for (auto &p : vec)
        {
            if (p != nullptr)
            {
                p->~FixedSize();
            }
        };
    }
};

class VariableSize : public BaseRep<VariableSize>
{
    friend class BaseRep<VariableSize>;

private:
    std::vector<std::pair<char, VariableSize *>> vec;

protected:
    std::pair<bool, VariableSize *> try_find(char c) override
    {
        for (auto &p : vec)
        {
            if (p.first == c)
            {
                return {true, p.second};
            }
        }
        return {false, nullptr};
    }

    std::pair<bool, VariableSize *> try_insert_node(char c) override
    {
        for (auto &p : vec)
        {
            if (p.first == c)
            {
                return {false, p.second};
            }
        }
        vec.emplace_back(c, new VariableSize());
        return {true, vec.back().second};
    }

    bool try_delete_node(char c)
    {
        for (auto &p : vec)
        {
            if (p.first == c)
            {
                std::swap(p, vec.back());
                vec.back().second->~VariableSize();
                vec.pop_back();
                return true;
            }
        }
        return false;
    }

public:
    VariableSize() : vec() {}
    ~VariableSize()
    {
        for (auto &p : vec)
        {
            if (p.second != nullptr)
            {
                p.second->~VariableSize();
            }
        };
    }
};

class HashMap : public BaseRep<HashMap>
{
    friend class BaseRep<HashMap>;

private:
    std::unordered_map<char, HashMap *> map;

protected:
    std::pair<bool, HashMap *> try_find(char c) override
    {
        auto it = map.find(c);
        return {it != map.end(), it->second};
    }

    std::pair<bool, HashMap *> try_insert_node(char c) override
    {
        const auto [it, inserted] = map.emplace(c, new HashMap());
        return {inserted, it->second};
    }

    bool try_delete_node(char c) override
    {
        auto it = map.find(c);
        if (it != map.end())
        {
            it->second->~HashMap();
            map.erase(it);
            return true;
        }
        return false;
    }

public:
    HashMap() : map() {}
    ~HashMap()
    {
        for (auto &p : map)
        {
            if (p.second != nullptr)
            {
                p.second->~HashMap();
            }
        };
    }
};

template <class Node>
class Trie
{
private:
    std::unique_ptr<Node> root = std::make_unique<Node>();

public:
    Trie() {}

    bool contains(char *c) const
    {
        return root->contains(c);
    }

    bool delete_word(char *c)
    {
        return root->delete_word(c);
    }

    bool insert(char *c)
    {
        return root->insert_word(c);
    }
};