#pragma once

#include <vector>
#include "type_info.h"

template <typename T>
class SchemaBuilder
{
public:
    SchemaBuilder() 
    : json{"[]"}
    {}

    template <typename U>
    SchemaBuilder& add(std::string name, U T::* member)
    {
        if (json.size() == 2)
        {
            json.insert(json.size() - 1, field_to_json<U>(name));
        }
        else
        {
            json.insert(json.size() - 1, ", " + field_to_json<U>(name));
        }

        return *this;
    }

    std::string generate()
    {
        return json;
    }

private:
    std::string json;
};

template <typename T>
struct Schema
{
    static std::string get_json()
    {
        throw std::invalid_argument("Type does not have Schema");
    }
};

struct Trade
{
    std::uint64_t id;
    std::uint32_t price;
};

template <>
struct Schema<Trade>
{
    static std::string get_json()
    {
        return SchemaBuilder<Trade>()
                .add("id", &Trade::id)
                .add("price", &Trade::price)
                .generate()
        ;
    }
};
