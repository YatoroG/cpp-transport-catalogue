#pragma once
#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include <sstream>
#include <unordered_map>

namespace json_reader {

using namespace json;
using namespace transport_catalogue;

class JSON_Reader
{
public:
	void ReadRequests(std::istream& input, TransportCatalogue& catalogue, RequestHandler& handler, MapRenderer& map_renderer);
	void ReadBaseRequests(Node& root_node, TransportCatalogue& catalogue);
	void ReadStatRequests(Node& root_node, RequestHandler& handler, MapRenderer& map_renderer);
	void ReadPropMapRequests(Node& root_node, MapRenderer& map_renderer);
	Node PrintBusStatRequestsResult(int request_id, std::optional<statistics::BusInfo> bus_info);
	Node PrintStopStatRequestsResult(int request_id, std::optional<statistics::StopInfo> stop_info);
	Node PrintMapStatRequestsResult(int request_id, std::ostringstream& output);
	svg::Color GetColorFromNode(json::Node node) const;
};

}