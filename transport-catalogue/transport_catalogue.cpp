#include "transport_catalogue.h"
#include <unordered_set>

namespace  transport_catalogue
{	
	void TransportCatalogue::AddStop(const std::string& name, double latitude, double longitude)
	{
		Stop new_stop = Stop{ name, {latitude, longitude } };
		stops_.push_back(std::move(new_stop));
		stopname_to_stop_[stops_.back().name] = &stops_.back();
	}

	void TransportCatalogue::AddBus(const std::string& bus_num, const std::vector<Stop*>& stops, bool is_round)
	{
		Bus new_bus = Bus{ bus_num, stops, is_round };
		Bus& added_bus = buses_.emplace_back(std::move(new_bus));

		for (const auto& stop : stops) {
			stop_buses_[stop].insert(added_bus.bus_num);
		}

		busname_to_bus_[added_bus.bus_num] = &added_bus;

		for (size_t i = 0; i != added_bus.stops.size() - 1; ++i) {
			auto stops_pair = std::make_pair(added_bus.stops[i], added_bus.stops[i + 1]);
			geographical_distance_[stops_pair] = ComputeDistance(stops_pair.first->coordinates, stops_pair.second->coordinates);
		}
	}	

	void TransportCatalogue::AddDistanceBetweenStops(Stop* stop, Stop* other_stop, int distance) {
		road_distance_[{stop, other_stop}] = distance;
	}

	Stop* TransportCatalogue::GetStop(std::string_view stop_name) const {
		if (auto founded_stop = stopname_to_stop_.find(stop_name); founded_stop != stopname_to_stop_.end()) {
			return founded_stop->second;
		}
		return nullptr;
	}

	Bus* TransportCatalogue::GetBus(std::string_view bus_num) const {
		if (auto founded_bus = busname_to_bus_.find(bus_num); founded_bus != busname_to_bus_.end()) {
			return founded_bus->second;
		}
		return nullptr;
	}

	const std::set<std::string_view>* TransportCatalogue::GetStopBuses(Stop* stop) const
	{
		if (stop_buses_.count(stop) != 0) {
			const std::set<std::string_view>* stop_buses = &stop_buses_.at(stop);
			return stop_buses;
		}
		return nullptr;
	}

	const std::deque<Bus>& TransportCatalogue::GetBuses() const {
		return buses_;
	}

	const std::deque<Stop>& TransportCatalogue::GetStops() const {
		return stops_;
	}

	int TransportCatalogue::GetUniqueStops(const Bus& bus) const {
		return (int)std::unordered_set<Stop*>(bus.stops.begin(), bus.stops.end()).size();
	}

	int TransportCatalogue::GetStops(const Bus& bus) const {
		return (int)bus.stops.size();
	}

	int TransportCatalogue::CountRouteDistance(const Bus& bus) const {
		int route_distance = 0;
		for (size_t i = 0; i != bus.stops.size() - 1; ++i) {
			if (road_distance_.count({ bus.stops[i], bus.stops[i + 1] }) == 0) {
				route_distance += road_distance_.at({ bus.stops[i + 1], bus.stops[i] });
			}
			else {
				route_distance += road_distance_.at({ bus.stops[i], bus.stops[i + 1] });
			}
		}

		return route_distance;
	}

	double TransportCatalogue::CountRouteCurvature(const Bus& bus, int real_distance) const {
		double curvature = 0.0;
		for (size_t i = 0; i != bus.stops.size() - 1; ++i) {
			curvature += geographical_distance_.at({ bus.stops[i], bus.stops[i + 1] });
		}
		return real_distance / curvature;
	}

}