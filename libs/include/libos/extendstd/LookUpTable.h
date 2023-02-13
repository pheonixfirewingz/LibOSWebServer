#pragma once
#include "Iterator.h"
#include <initializer_list>
#include <type_traits>
namespace std
{
template<typename T>
concept is_numeric_type = requires(T param) {
                          requires std::is_integral_v<T> || std::is_floating_point_v<T>;
                          requires !std::is_same_v<bool, T>;
                          requires std::is_arithmetic_v<decltype(param + 1)>;
                          requires !std::is_pointer_v<T>;
                      };

template<typename Key, typename Value>
    requires std::is_numeric_type<Key>
struct TableEntry
{
    Key key = 0;
    Value value;
    TableEntry() noexcept = default;
    constexpr TableEntry(const Key key, const Value value) noexcept
        : key(key)
        , value(value)
    {
    }

    constexpr TableEntry &operator=(const TableEntry &entry) noexcept
    {
        this->key = entry.key;
        this->value = entry.value;
        return *this;
    }

    bool operator==(const Key &key_in) const noexcept
    {
        return key == key_in;
    };
};

template<typename Key, typename Value> struct ReadOnlyLookupTable : public ForwardIterator<TableEntry<Key, Value>>
{
  private:
    TableEntry<Key, Value> *table;
    size_t size_of_table = 0;

  public:
    constexpr ReadOnlyLookupTable() noexcept = default;
    constexpr ReadOnlyLookupTable(initializer_list<TableEntry<Key, Value>> entries)
    {
        table = new TableEntry<Key, Value>[(size_of_table = entries.size())];
        size_t index = 0;
        for (auto &entry : entries)
            table[index++] = entry;
    }
    constexpr ~ReadOnlyLookupTable() noexcept
    {
        if (table != nullptr)
            delete[] table;
    }

    virtual constexpr size_t i_size() const noexcept override final
    {
        return size_of_table;
    }

    constexpr size_t size() const noexcept
    {
        return size_of_table;
    }

    virtual constexpr TableEntry<Key, Value> *begin() noexcept override final
    {
        return table;
    }

    virtual constexpr const TableEntry<Key, Value> *begin() const noexcept override final
    {
        return table;
    }

    virtual constexpr TableEntry<Key, Value> *cbegin() const noexcept override final
    {
        return table;
    }

    constexpr Value *find(const Key &key) const noexcept
    {
        for (size_t i = 0; i < size_of_table; i++)
        {
            if (table[i] == key)
                return &table[i].value;
        }
        return nullptr;
    }
};
} // namespace std