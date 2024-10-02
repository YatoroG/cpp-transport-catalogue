#pragma once
#include "json.h"
#include "json_builder.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include <sstream>
#include <unordered_map>
#include "transport_router.h"

namespace json_reader {

using namespace json;
using namespace transport_catalogue;

class JSON_Reader
{
public:
	void ReadRequests(std::istream& input, TransportCatalogue& catalogue, RequestHandler& handler, MapRenderer& map_renderer, TransportRouter& router);
	void ReadPropRouterRequests(Node& root_node, TransportRouter& router);
	void ReadBaseRequests(Node& root_node, TransportCatalogue& catalogue);
	void ReadStopRequests(const Array& node_array, TransportCatalogue& catalogue);
	void ReadStopDistanceRequests(const Array& node_array, TransportCatalogue& catalogue);
	void ReadBusRequests(const Array& node_array, TransportCatalogue& catalogue);
	void ReadStatRequests(Node& root_node, RequestHandler& handler, MapRenderer& map_renderer, TransportRouter& router);
	void ReadPropMapRequests(Node& root_node, MapRenderer& map_renderer);
	Node PrintBusStatRequestsResult(int request_id, std::optional<statistics::BusInfo> bus_info);
	Node PrintStopStatRequestsResult(int request_id, std::optional<statistics::StopInfo> stop_info);
	Node PrintMapStatRequestsResult(int request_id, std::ostringstream& output);
	Node PrintRouteStatRequestsResult(int request_id, std::optional<RouteAndEdgesInfo> route_info);
	svg::Color GetColorFromNode(json::Node node) const;

};

}