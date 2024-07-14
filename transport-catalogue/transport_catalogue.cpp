#include "transport_catalogue.h"

namespace transport_catalog {

void TransportCatalogue::AddStop(std::string name, double latitude, double longitude) {
	Stop new_stop = Stop{ name, {latitude, longitude } };
	stops_.push_back(std::move(new_stop));
	stopname_to_stop_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddBus(std::string bus_num, std::vector<Stop*> stops) {
	Bus new_bus = Bus{ bus_num, stops };
	Bus& added_bus = buses_.emplace_back(std::move(new_bus));

	for (const auto& stop : stops) {
		stop_buses_[stop].insert(added_bus.bus_num);
	}

	busname_to_bus_[added_bus.bus_num] = &added_bus;

	for (size_t i = 0; i != added_bus.stops.size() - 1; ++i) {
		auto stops_pair = std::make_pair(added_bus.stops[i], added_bus.stops[i + 1]);
		distance_[stops_pair] = ComputeDistance(stops_pair.first->coordinates, stops_pair.second->coordinates);
	}
}

Stop* TransportCatalogue::GetStop(std::string_view stop_name) const {
	if (stopname_to_stop_.count(stop_name) == 0) {
		return nullptr;
	}
	return stopname_to_stop_.at(stop_name);
}

Bus* TransportCatalogue::GetBus(std::string_view bus_num) const {
	if (busname_to_bus_.count(bus_num) == 0) {
		return nullptr;
	}
	return busname_to_bus_.at(bus_num);
}

size_t TransportCatalogue::GetUniqueStops(const Bus& bus) const {
	return std::unordered_set<Stop*>(bus.stops.begin(), bus.stops.end()).size();
}

size_t TransportCatalogue::GetStops(const Bus& bus) const {
	return bus.stops.size();
}

double TransportCatalogue::CountRouteDistance(const Bus& bus) const {
	double route_distance = 0.0;
	for (size_t i = 0; i != bus.stops.size() - 1; ++i) {
		route_distance += distance_.at({bus.stops[i], bus.stops[i + 1]});
	}
	return route_distance;
}

std::set<std::string_view> TransportCatalogue::GetBusesForStop(Stop* stop) const {
	if (stop_buses_.count(stop) == 0) {
		return {};
	}

	return stop_buses_.at(stop);
}

}