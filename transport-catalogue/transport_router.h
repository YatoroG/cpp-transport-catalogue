#pragma once
#include "domain.h"
#include "graph.h"
#include "request_handler.h"
#include "router.h"
#include <memory>
#include <variant>

const int H_TO_MIN = 60;
const int KM_TO_M = 1000;

struct RouterProps {
	int bus_wait_time = 1;
	double bus_velocity = 1.0;
};

struct WaitEdge {
	std::string stop_name;
	double wait_time;
};

struct BusEdge {
	std::string bus_name;
	int span_count;
	double ride_time;
};

struct RouteAndEdgesInfo {
	double time;
	std::vector<std::variant<BusEdge, WaitEdge>> edges;
};

class TransportRouter {
public:
	TransportRouter() = default;

	TransportRouter(TransportCatalogue& catalogue)
		: catalogue_(catalogue) {}

	TransportRouter& SetBusWaitTime(int time);
	TransportRouter& SetBusVelocity(double velocity);

	std::optional<RouteAndEdgesInfo> GetRoute(std::string_view from, std::string_view to);


private:	
	const TransportCatalogue& catalogue_;
	RouterProps properties_;
	std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_;
	std::unique_ptr<graph::Router<double>> router_;
	std::unordered_map<const domain::Stop*, graph::Edge<double>> stops_edges_;
	std::unordered_map<graph::EdgeId, WaitEdge> wait_edges_;
	std::unordered_map<graph::EdgeId, BusEdge> bus_edges_;
	
	void MakeGraph();
	std::variant<BusEdge, WaitEdge> GetEdgeInfo(graph::EdgeId edge_id);

	template <typename Iter>
	void AddBusToGraph(Iter begin, Iter end, std::string bus_name) {
		for (auto f_iter = begin; f_iter != end; ++f_iter) {
			int distance = 0;
			int span_count = 0;

			for (auto s_iter = std::next(f_iter); s_iter != end; ++s_iter) {
				span_count++;
				distance += catalogue_.CountDistanceBetweenStops(*std::prev(s_iter), *s_iter);
				graph::Edge<double> edge = { stops_edges_.at(*f_iter).to,
												stops_edges_.at(*s_iter).from,
												(distance * 1.0) / (properties_.bus_velocity * KM_TO_M / H_TO_MIN) };
				graph::EdgeId edge_id = graph_->AddEdge(edge);
				bus_edges_[edge_id] = BusEdge{ bus_name, span_count, graph_->GetEdge(edge_id).weight * 1.0 };
			}
		}
	}
};