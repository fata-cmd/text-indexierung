#include <vector>
#include <algorithm>
#include <memory>
#include <cassert>
#include "immintrin.h"
#include "util.h"


class Fixed
{
protected:
    std::vector<Fixed *> vec;

public:
    Fixed() : vec(256, nullptr) { vec.shrink_to_fit(); }
    ~Fixed()
    {
        for (auto &p : vec)
        {
            if (p != nullptr && p != this)
            {
                p->~Fixed();
            }
        };
    }

    bool contains(const u_char *c) const
    {
        const bool found = vec[*c] != nullptr;
        if (*c == sentinel || !found)
            return found;
        return vec[*c]->contains(++c);
    }

    bool delete_word(const u_char *c)
    {
        if (vec[*c] != nullptr)
        {
            if (*c == sentinel)
            {
                vec[*c] = nullptr;
                return true;
            }
            return vec[*c]->delete_word(++c);
        }
        return false;
    }

    bool insert_word(const u_char *c)
    {
        if (vec[*c] != nullptr)
        {
            if (*c == sentinel)
                return false;
        }
        else
        {
            if (*c == sentinel)
            {
                vec[*c] = this;
                return true;
            }
            vec[*c] = new Fixed();
        }
        return vec[*c]->insert_word(++c);
    }
};

constexpr unsigned invalid_pos = UINT32_MAX;

class Variable
{

protected:
    std::vector<u_char> chars;
    std::vector<Variable *> ptrs;

    unsigned find(const u_char c) const
    {
        for (size_t i = 0; i < chars.size(); ++i)
        {
            if (chars[i] == c)
                return i;
        }
        return invalid_pos;
    }

    void insert_node(const u_char c)
    {
        chars.emplace_back(c);
        ptrs.emplace_back(c != sentinel ? new Variable() : nullptr);
    }

    void delete_node(const unsigned pos)
    {
        std::swap(chars[pos], chars.back());
        std::swap(ptrs[pos], ptrs.back());
        if (ptrs.back() != nullptr)
            ptrs.back()->~Variable();
        chars.pop_back();
        ptrs.pop_back();
    }

public:
    Variable() : chars(), ptrs() {}
    ~Variable()
    {
        for (auto &p : ptrs)
        {
            if (p != nullptr)
            {
                p->~Variable();
            }
        };
    }

    bool contains(const u_char *c) const
    {
        const auto pos = find(*c);
        const bool found = pos != invalid_pos;
        if (*c == sentinel || !found)
            return found;
        return ptrs[pos]->contains(++c);
    }

    bool delete_word(const u_char *c)
    {
        const auto pos = find(*c);
        if (*c == sentinel)
        {
            if (pos != invalid_pos)
            {
                delete_node(pos);
                return true;
            }
            return false;
        }
        if (pos == invalid_pos)
            return false;
        return ptrs[pos]->delete_word(++c);
    }

    bool insert_word(const u_char *c)
    {
        auto pos = find(*c);
        if (pos == invalid_pos)
        {
            insert_node(*c);
            if (*c == sentinel)
                return true;
            else
                pos = ptrs.size() - 1U;
        }
        else
        {
            if (*c == sentinel)
                return false;
        }
        return ptrs[pos]->insert_word(++c);
    }
};

class VariableSIMD : public Variable
{

private:
    static constexpr size_t simd_width = 32;

    size_t size() const
    {
        return chars.size();
    }

    size_t find(const u_char target) const
    {
        size_t s = size();

        __m256i target_vec = _mm256_set1_epi8(target);

        size_t i = 0;

        for (; i <= s; i += simd_width)
        {
            __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(&chars[i]));

            __m256i cmp = _mm256_cmpeq_epi8(chunk, target_vec);

            int mask = _mm256_movemask_epi8(cmp);

            if (mask)
            {
                size_t pos = i + std::__countr_zero(mask);
                return pos < s ? pos : invalid_pos;
            }
        }
        
        return invalid_pos;
    }

public:
    VariableSIMD() : Variable()
    {
    }
};

class HashMap
{

private:
    std::unordered_map<u_char, HashMap *> map;

protected:
public:
    HashMap() : map()
    {
    }
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

    bool contains(const u_char *c) const
    {
        auto it = map.find(*c);
        const bool found = it != map.end();
        if (*c == sentinel || !found)
            return found;
        return it->second->contains(++c);
    }

    bool delete_word(const u_char *c)
    {
        auto it = map.find(*c);
        const bool found = it != map.end();
        if (*c == sentinel)
        {
            if (found)
            {
                if (it->second != nullptr)
                    it->second->~HashMap();
                map.erase(it);
                return true;
            }
            return false;
        }
        if (!found)
            return false;
        return it->second->delete_word(++c);
    }

    bool insert_word(const u_char *c)
    {
        auto [it, inserted] = map.emplace(*c, *c != sentinel ? new HashMap() : nullptr);
        if (*c == sentinel)
        {
            return inserted;
        }
        return it->second->insert_word(++c);
    }
};

class HashMapGlobal
{
    using key_t = uint64_t;

private:
    static std::unordered_map<key_t, HashMapGlobal *> map;
    static key_t id_counter;
    key_t id;
    std::vector<u_char> chars;

    key_t key(const u_char c) const
    {
        return (id << 8) | c;
    }

public:
    HashMapGlobal() : id(id_counter++), chars()
    {
    }
    ~HashMapGlobal()
    {
        for (const auto c : chars)
        {
            if (c != sentinel)
            {
                auto it = map.find(key(c));
                if (it != map.end())
                {
                    it->second->~HashMapGlobal();
                }
            }
        };
    }

    bool contains(const u_char *c) const
    {
        auto it = map.find(key(*c));
        const bool found = it != map.end();
        if (*c == sentinel || !found)
            return found;
        return it->second->contains(++c);
    }

    bool delete_word(const u_char *c)
    {
        auto it = map.find(key(*c));
        const bool found = it != map.end();
        if (*c == sentinel)
        {
            if (found)
            {
                if (it->second != nullptr)
                    it->second->~HashMapGlobal();
                map.erase(it);
                std::swap(*std::find(chars.begin(), chars.end(), *c), chars.back());
                chars.pop_back();
                return true;
            }
            return false;
        }
        if (!found)
            return false;
        return it->second->delete_word(++c);
    }

    bool insert_word(const u_char *c)
    {
        auto [it, inserted] = map.emplace(key(*c), *c != sentinel ? new HashMapGlobal() : nullptr);
        if (inserted)
        {
            chars.push_back(*c);
        }
        if (*c == sentinel)
        {
            return inserted;
        }
        return it->second->insert_word(++c);
    }
};

template <class Node>
class Trie
{
private:
    std::unique_ptr<Node> root = std::make_unique<Node>();
    size_t elements = 0;

public:
    Trie() {}

    bool contains(const u_char *c) const
    {
        return root->contains(c);
    }

    bool contains(const std::string &s)
    {
        const u_char *uc = uc_str(s.c_str());
        return contains(uc);
    }

    bool delete_word(const u_char *c)
    {
        return root->delete_word(c);
    }

    bool delete_word(const std::string &s)
    {
        const u_char *uc = uc_str(s.c_str());
        return delete_word(uc);
    }

    bool insert_word(const u_char *c)
    {
        return root->insert_word(c);
    }

    bool insert_word(const std::string &s)
    {
        const u_char *uc = uc_str(s.c_str());
        return insert_word(uc);
    }

    size_t size() const
    {
        return elements;
    }
};