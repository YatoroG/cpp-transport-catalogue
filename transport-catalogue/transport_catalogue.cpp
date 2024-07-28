#include "transport_catalogue.h"

namespace transport_catalog {

void TransportCatalogue::AddStop(const std::string& name, double latitude, double longitude) {
	Stop new_stop = Stop{ name, {latitude, longitude } };
	stops_.push_back(std::move(new_stop));
	stopname_to_stop_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddBus(const std::string& bus_num, const std::vector<Stop*>& stops) {
	Bus new_bus = Bus{ bus_num, stops };
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

size_t TransportCatalogue::GetUniqueStops(const Bus& bus) const {
	return std::unordered_set<Stop*>(bus.stops.begin(), bus.stops.end()).size();
}

size_t TransportCatalogue::GetStops(const Bus& bus) const {
	return bus.stops.size();
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

std::optional<statistics::BusInfo> TransportCatalogue::GetBusInfo(std::string_view bus_num) const {
	Bus* founded_bus = GetBus(bus_num);
	if (!founded_bus) {
		return std::nullopt;
	}
	int road_distance = CountRouteDistance(*founded_bus);
	statistics::BusInfo bus_info{ founded_bus->bus_num, GetStops(*founded_bus),
		GetUniqueStops(*founded_bus), road_distance, CountRouteCurvature(*founded_bus, road_distance)};
	return bus_info;
}

std::optional<statistics::StopInfo> TransportCatalogue::GetStopInfo(std::string_view stop_name) const {
	Stop* founded_stop = GetStop(stop_name);

	if (!founded_stop) {
		return std::nullopt;
	}

	statistics::StopInfo stop_info{ founded_stop->name, {} };

	if (stop_buses_.count(founded_stop) != 0) {
		const std::set<std::string_view>& stop_buses = stop_buses_.at(founded_stop);
		stop_info.buses = stop_buses;
	}

	return stop_info;
}

}