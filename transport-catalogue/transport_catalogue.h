#pragma once
#include "domain.h"
#include "geo.h"
#include <deque>
#include <optional>
#include <set>
#include <unordered_map>

namespace statistics {
    struct BusInfo {
        std::string_view bus_num;
        int stops_num;
        int unique_stops_num;
        int route_length;
        double curvature;
    };

    struct StopInfo {
        std::string_view stop_name;
        std::set<std::string_view> buses;
    };
}

namespace  transport_catalogue
{
    using namespace domain;

    struct DistanceHasher {
        size_t operator()(const std::pair< Stop*, Stop*> stop_pair) const {
            std::hash<std::string> hasher;
            return hasher(stop_pair.first->name) + hasher(stop_pair.second->name) * 31;
        }
    };

    class TransportCatalogue {
    public:
        TransportCatalogue() = default;
        void AddStop(const std::string& name, double latitude, double longitude);
        void AddBus(const std::string& bus_num, const std::vector<Stop*>& stops, bool is_round);
        void AddDistanceBetweenStops(Stop* stop, Stop* other_stop, int distance);
        Stop* GetStop(std::string_view stop_name) const;
        Bus* GetBus(std::string_view bus_num) const;
        const std::set<std::string_view>* GetStopBuses(Stop* stop) const;
        const std::deque<Bus>& GetBuses() const;
        const std::deque<Stop>& GetStops() const;
        std::deque<const Stop*> GetStopsPointers() const;

        int GetUniqueStops(const Bus& bus) const;
        int GetStops(const Bus& bus) const;
        int CountDistanceBetweenStops( Stop* from,  Stop* to) const;
        int CountRouteDistance(const Bus& bus) const;
        double CountRouteCurvature(const Bus& bus, int real_distance) const;

    private:
        std::unordered_map<Stop*, std::set<std::string_view>> stop_buses_;
        std::unordered_map<std::pair<Stop*, Stop*>, double, DistanceHasher> geographical_distance_;
        std::unordered_map<std::pair<Stop*, Stop*>, int, DistanceHasher> road_distance_;
        std::unordered_map<std::string_view, Bus*> busname_to_bus_;
        std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
        std::deque<Bus> buses_;
        std::deque<Stop> stops_;


    };
};

