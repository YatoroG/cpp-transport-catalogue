#include "transport_router.h"

TransportRouter& TransportRouter::SetBusWaitTime(int time) {
	properties_.bus_wait_time = std::move(time);
	return *this;
}

TransportRouter& TransportRouter::SetBusVelocity(double velocity) {
	properties_.bus_velocity = std::move(velocity);
	return *this;
}

void TransportRouter::MakeGraph() {
	if (graph_) {
		return;
	}
	size_t i = 0;
	const auto stops = catalogue_.GetStopsPointers();
	graph_ = std::make_unique<graph::DirectedWeightedGraph<double>>(stops.size() * 2);
	for (const auto& stop : stops) {
		graph::Edge<double> edge = {i++, i++, properties_.bus_wait_time * 1.0};
		graph::EdgeId edge_id = graph_->AddEdge(edge);
		stops_edges_[stop] = edge;
		wait_edges_[edge_id] = WaitEdge{ stop->name, properties_.bus_wait_time * 1.0};
	}
	const auto buses = catalogue_.GetBuses();
	for (const auto& bus : buses) {
		AddBusToGraph(bus.stops.begin(), bus.stops.end(), bus.bus_num);
		if (!bus.is_roundtrip) {
			AddBusToGraph(bus.stops.rbegin(), bus.stops.rend(), bus.bus_num);
		}
	}

	router_ = std::make_unique<graph::Router<double>>(*graph_);
}

std::optional<RouteAndEdgesInfo> TransportRouter::GetRoute(std::string_view from, std::string_view to) {
	MakeGraph();
	std::vector<std::variant<BusEdge, WaitEdge>> edges;
	std::optional<graph::Router<double>::RouteInfo> route = router_->BuildRoute(stops_edges_.at(catalogue_.GetStop(from)).from, stops_edges_.at(catalogue_.GetStop(to)).from);

	if (!route) {
		return std::nullopt;
	}

	for (const auto& item : route.value().edges) {
		const auto& item_info = GetEdgeInfo(item);
		edges.push_back(item_info);
	}

	return RouteAndEdgesInfo{ route.value().weight, edges };
}

std::variant<BusEdge, WaitEdge> TransportRouter::GetEdgeInfo(graph::EdgeId edge_id) {
	if (wait_edges_.count(edge_id) != 0) {
		return wait_edges_.at(edge_id);
	}
	return bus_edges_.at(edge_id);
}
