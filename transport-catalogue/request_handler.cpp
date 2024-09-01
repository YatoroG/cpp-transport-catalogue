#include "request_handler.h"

std::optional<statistics::BusInfo> RequestHandler::GetBusInfo(std::string_view bus_num) const {
	Bus* founded_bus = db_.GetBus(bus_num);
	if (!founded_bus) {
		return std::nullopt;
	}
	int road_distance = db_.CountRouteDistance(*founded_bus);
	statistics::BusInfo bus_info{ founded_bus->bus_num, db_.GetStops(*founded_bus),
		db_.GetUniqueStops(*founded_bus), road_distance, db_.CountRouteCurvature(*founded_bus, road_distance) };
	return bus_info;
}

std::optional<statistics::StopInfo> RequestHandler::GetStopInfo(std::string_view stop_name) const {
	Stop* founded_stop = db_.GetStop(stop_name);

	if (!founded_stop) {
		return std::nullopt;
	}

	statistics::StopInfo stop_info{ founded_stop->name, {} };

	const std::set<std::string_view>* stop_buses = db_.GetStopBuses(founded_stop);
	if (stop_buses) {
		stop_info.buses = *stop_buses;
	}
	
	return stop_info;
}

const std::set<Bus> RequestHandler::GetAllBuses() const {
	std::set<Bus> buses;
	for (const auto& bus : db_.GetBuses()) {
		if (bus.stops.size() != 0) {
			buses.insert(bus);
		}
	}
	return buses;
}

const std::set<Stop> RequestHandler::GetAllStops() const {
	std::set<Stop> stops_with_buses;
	for (const auto& stop : db_.GetStops()) {
		if (GetStopInfo(stop.name).value().buses.size() != 0) {
			stops_with_buses.insert(stop);
		}
	}
	return stops_with_buses;
}
