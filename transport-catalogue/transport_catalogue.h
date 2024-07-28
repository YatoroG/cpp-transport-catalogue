#pragma once
#include "geo.h"
#include <deque>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace statistics {
    struct BusInfo {
        std::string_view bus_num;
        size_t stops_num;
        size_t unique_stops_num;
        int route_length;
        double curvature;
    };

    struct StopInfo {
        std::string_view stop_name;
        std::set<std::string_view> buses;
    };
}

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
    void AddStop(const std::string& name, double latitude, double longitude);
    void AddBus(const std::string& bus_num, const std::vector<Stop*>& stops);
    void AddDistanceBetweenStops(Stop* stop, Stop* other_stop, int distance);
    Stop* GetStop(std::string_view stop_name) const;
    Bus* GetBus(std::string_view bus_num) const;
    std::optional<statistics::BusInfo> GetBusInfo(std::string_view bus_num) const;
    std::optional<statistics::StopInfo> GetStopInfo(std::string_view stop_name) const;

private:
    std::unordered_map<Stop*, std::set<std::string_view>> stop_buses_;
    std::unordered_map<std::pair<Stop*, Stop*>, double, DistanceHasher> geographical_distance_;
    std::unordered_map<std::pair<Stop*, Stop*>, int, DistanceHasher> road_distance_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;

    size_t GetUniqueStops(const Bus& bus) const;
    size_t GetStops(const Bus& bus) const;
    int CountRouteDistance(const Bus& bus) const;
    double CountRouteCurvature(const Bus& bus, int real_distance) const;
};

}