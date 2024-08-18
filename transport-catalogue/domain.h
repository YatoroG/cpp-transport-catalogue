#pragma once
#include "geo.h"
#include <string>
#include <vector>

namespace domain
{
    struct Stop {
        std::string name;
        geo::Coordinates coordinates = { 0.0, 0.0 };

        bool operator<(const Stop& rhs) const noexcept {
            return std::lexicographical_compare(this->name.begin(), this->name.end(), rhs.name.begin(), rhs.name.end());
        }
    };

    struct Bus {
        std::string bus_num;
        std::vector<Stop*> stops;
        bool is_roundtrip = false;

        bool operator<(const Bus& rhs) const noexcept {
            return std::lexicographical_compare(this->bus_num.begin(), this->bus_num.end(), rhs.bus_num.begin(), rhs.bus_num.end());
        }
    };

};

