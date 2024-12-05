#include <vector>
#include <algorithm>
#include <memory>
#include <cassert>
#include "immintrin.h"

constexpr char null_byte = '\0';

// fix appropriate dummy values for word-checking (i.e. check if child at nullbyte is set or not)
// => necessary?
// debug
// implement VariableSize-SIMD version

template <typename Derived>
class BaseRep
{
protected:
    virtual std::pair<bool, Derived *> try_find(const char c) = 0;
    virtual std::pair<bool, Derived *> try_insert_node(const char c) = 0;
    virtual bool try_delete_node(const char c) = 0;

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
    bool contains(const char *c)
    {
        if (*c == null_byte)
            return is_word();
        const auto [found, p] = try_find(*c);
        if (!found)
            return false;
        return p->contains(++c);
    }

    bool delete_word(const char *c)
    {
        if (*c == null_byte)
        {
            if (is_word())
            {
                reset_word();
                return true;
            }
            return false;
        }
        auto [found, p] = try_find(*c);
        if (!found)
            return false;
        return p->delete_word(++c);
    }

    bool insert_word(const char *c)
    {
        auto [inserted, p] = try_insert_node(*c);
        if (*c == null_byte)
        {
            return inserted;
        }
        return p->insert_word(++c);
    }
};

class Fixed : public BaseRep<Fixed>
{
    friend class BaseRep<Fixed>;

protected:
    std::vector<Fixed *> vec;
    std::pair<bool, Fixed *> try_find(const char c) override
    {
        return {vec[c] != nullptr, vec[c]};
    }

    std::pair<bool, Fixed *> try_insert_node(const char c) override
    {
        if (vec[c] == nullptr)
        {
            vec[c] = c != null_byte ? new Fixed() : this;
            return {true, vec[c]};
        }
        return {false, vec[c]};
    }

    bool try_delete_node(char c) override
    {
        if (vec[c] != nullptr)
        {
            if (vec[c] != this)
                vec[c]->~Fixed();
            vec[c] = nullptr;
            return true;
        }
        return false;
    }

public:
    Fixed() : vec(UINT8_MAX, nullptr) { vec.shrink_to_fit(); }
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
};

class Variable : public BaseRep<Variable>
{
    friend class BaseRep<Variable>;

private:
    std::vector<std::pair<char, Variable *>> vec;

protected:
    std::pair<bool, Variable *> try_find(const char c) override
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

    std::pair<bool, Variable *> try_insert_node(const char c) override
    {
        auto [found, p] = try_find(c);
        if (found)
            return {false, p};
        vec.emplace_back(c, c != null_byte ? new Variable() : nullptr);
        return {true, vec.back().second};
    }

    bool try_delete_node(char c)
    {
        for (auto &p : vec)
        {
            if (p.first == c)
            {
                std::swap(p, vec.back());
                if (vec.back().second != nullptr)
                    vec.back().second->~Variable();
                vec.pop_back();
                return true;
            }
        }
        return false;
    }

public:
    Variable() : vec() {}
    ~Variable()
    {
        for (auto &p : vec)
        {
            if (p.second != nullptr)
            {
                p.second->~Variable();
            }
        };
    }
};

class VariableSIMD : public BaseRep<VariableSIMD>
{
    friend class BaseRep<VariableSIMD>;

private:
    class simd_vector
    {
    private:
        using T = std::pair<char, VariableSIMD *>;
        static constexpr size_t simd_width = 32;
        std::vector<char> chars;
        u_char actual_end_pos;

    public:
        std::vector<VariableSIMD *> ptrs;
        size_t size()
        {
            return chars.size();
        }

        void emplace_back(char c, VariableSIMD *p)
        {
            if (size() == actual_end_pos)
            {
                auto new_size = size() + simd_width;
                chars.resize(new_size, '0');
                chars.shrink_to_fit();
                ptrs.resize(new_size, nullptr);
                ptrs.shrink_to_fit();
            }
            chars[actual_end_pos] = c;
            ptrs[actual_end_pos++] = p;
        }

        auto back()
        {
            return ptrs[actual_end_pos - 1];
        }

        void pop_back()
        {
            --actual_end_pos;
            if (ptrs[actual_end_pos] != nullptr)
                ptrs[actual_end_pos]->~VariableSIMD();
            if (size() - actual_end_pos == simd_width)
            {
                auto new_size = size() - simd_width;
                chars.resize(new_size);
                chars.shrink_to_fit();
                ptrs.resize(new_size);
                ptrs.shrink_to_fit();
            }
        }

        auto end()
        {
            return actual_end_pos;
        }

        size_t scan_for_pos(char target)
        {
            // Assuming we're scanning from the start or a specific point in the vector
            size_t s = size();

            __m256i target_vec = _mm256_set1_epi8(target); // Broadcast the target char across the 256-bit register

            // Iterate over the vector in chunks of 32 elements (since AVX2 processes 32 bytes at a time)
            for (size_t i = 0; i + simd_width <= s; i += simd_width)
            {
                // Load 32 bytes of data from the vector into the AVX2 register
                __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(&chars[i]));

                // Compare the chunk with the target character
                __m256i cmp = _mm256_cmpeq_epi8(chunk, target_vec);

                // Create a bitmask where each bit corresponds to a comparison result (1 if equal, 0 if not)
                int mask = _mm256_movemask_epi8(cmp);

                // If there is a match, report the position(s)
                if (mask)
                {
                    size_t pos = i + std::__countr_zero(mask);
                    assert((chars[pos] == target));
                    return pos;
                }
            }
            return s;
        }

        std::pair<bool, VariableSIMD *> scan(char target)
        {
            size_t pos = scan_for_pos(target);
            if (pos != size())
            {
                return {pos < actual_end_pos, ptrs[pos]};
            }
            return {false, nullptr};
        }

        void erase(size_t pos)
        {
            auto back_pos = actual_end_pos - 1;
            std::swap(chars[pos], chars[back_pos]);
            std::swap(ptrs[pos], ptrs[back_pos]);
            pop_back();
        }

        simd_vector() : chars(), actual_end_pos(0), ptrs() {}
    };

    simd_vector vec;

protected:
    std::pair<bool, VariableSIMD *> try_find(const char c) override
    {
        return vec.scan(c);
    }

    std::pair<bool, VariableSIMD *> try_insert_node(const char c) override
    {
        auto [found, p] = try_find(c);
        if (found)
            return {false, p};
        vec.emplace_back(c, c != null_byte ? new VariableSIMD() : nullptr);
        return {true, vec.back()};
    }

    bool try_delete_node(const char c)
    {
        auto pos = vec.scan_for_pos(c);
        if (pos != vec.end())
        {
            vec.erase(pos);
            return true;
        }
        return false;
    }

public:
    VariableSIMD() : vec()
    {
    }
    ~VariableSIMD()
    {
        for (auto &p : vec.ptrs)
        {
            if (p != nullptr)
            {
                p->~VariableSIMD();
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
    std::pair<bool, HashMap *> try_find(const char c) override
    {
        auto it = map.find(c);
        return {it != map.end(), it != map.end() ? it->second : nullptr};
    }

    std::pair<bool, HashMap *> try_insert_node(const char c) override
    {
        const auto [it, inserted] = map.emplace(c, c != null_byte ? new HashMap() : nullptr);
        return {inserted, it->second};
    }

    bool try_delete_node(const char c) override
    {
        auto it = map.find(c);
        if (it != map.end())
        {
            if (it->second != nullptr)
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

    bool contains(const char *c) const
    {
        return root->contains(c);
    }

    bool delete_word(const char *c)
    {
        return root->delete_word(c);
    }

    bool insert(const char *c)
    {
        return root->insert_word(c);
    }
};