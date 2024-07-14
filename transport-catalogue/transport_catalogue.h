#pragma once
#include "geo.h"
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>

namespace transport_catalog {

struct Stop {
    std::string name;
    geo::Coordinates coordinates = { 0.0, 0.0 };
};

struct Bus {
    std::string bus_num;
    std::vector<Stop*> stops;
};

struct DistanceHasher {
    size_t operator()(const std::pair<Stop*, Stop*> stop_pair) const {
        std::hash<std::string> hasher;
        return hasher(stop_pair.first->name) + hasher(stop_pair.second->name) * 31;
    }
};

class TransportCatalogue {
public:
    TransportCatalogue() = default;
    void AddStop(std::string name, double latitude, double longitude);
    void AddBus(std::string bus_num, std::vector<Stop*> stops);
    Stop* GetStop(std::string_view stop_name) const;
    Bus* GetBus(std::string_view bus_num) const;
    size_t GetUniqueStops(const Bus& bus) const;
    size_t GetStops(const Bus& bus) const;
    double CountRouteDistance(const Bus& bus) const;
    std::set<std::string_view> GetBusesForStop(Stop* stop) const;

private:
    std::unordered_map<Stop*, std::set<std::string_view>> stop_buses_;
    std::unordered_map<std::pair<Stop*, Stop*>, double, DistanceHasher> distance_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
};

}