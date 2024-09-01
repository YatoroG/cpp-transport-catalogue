#include "json_reader.h"
namespace json_reader {

void JSON_Reader::ReadRequests(std::istream& input, TransportCatalogue& catalogue, RequestHandler& handler, MapRenderer& map_renderer) {
	const Document& doc = Load(input);
	const Node& doc_root = doc.GetRoot();

	if (!doc_root.IsDict()) {
		throw ParsingError("Error");
	}

	auto requests_map = doc_root.AsDict();
	ReadBaseRequests(requests_map.at("base_requests"), catalogue);
	ReadPropMapRequests(requests_map.at("render_settings"), map_renderer);	
	ReadStatRequests(requests_map.at("stat_requests"), handler, map_renderer);
}

void JSON_Reader::ReadBaseRequests(Node& root_node, TransportCatalogue& catalogue) {
	const auto& node_array = root_node.AsArray();
	ReadStopRequests(node_array, catalogue);
	ReadStopDistanceRequests(node_array, catalogue);
	ReadBusRequests(node_array, catalogue);
}

void JSON_Reader::ReadStopRequests(const Array& node_array, TransportCatalogue& catalogue) {
	for (const auto& node : node_array) {
		const auto& node_info = node.AsDict();
		if (node_info.at("type") == "Stop") {
			catalogue.AddStop(node_info.at("name").AsString(),
				node_info.at("latitude").AsDouble(),
				node_info.at("longitude").AsDouble());
		}
	}
}

void JSON_Reader::ReadStopDistanceRequests(const Array& node_array, TransportCatalogue& catalogue) {
	for (const auto& node : node_array) {
		const auto& node_info = node.AsDict();
		if (node_info.at("type") == "Stop") {
			for (const auto& stop_distance : node_info.at("road_distances").AsDict()) {
				Stop* stop = catalogue.GetStop(node_info.at("name").AsString());
				Stop* other_stop = catalogue.GetStop(stop_distance.first);
				catalogue.AddDistanceBetweenStops(stop, other_stop, stop_distance.second.AsInt());
			}
		}
	}
}

void JSON_Reader::ReadBusRequests(const Array& node_array, TransportCatalogue& catalogue) {
	for (const auto& node : node_array) {
		const auto& node_info = node.AsDict();
		if (node_info.at("type") == "Bus") {
			std::vector<Stop*> stops;
			for (const auto& stop_name : node_info.at("stops").AsArray()) {
				stops.emplace_back(catalogue.GetStop(stop_name.AsString()));
			}
			if (node_info.at("is_roundtrip").AsBool() == false) {
				stops.insert(stops.end(), stops.rbegin() + 1, stops.rend());
			}
			catalogue.AddBus(node_info.at("name").AsString(), stops, node_info.at("is_roundtrip").AsBool());
		}
	}
}

void JSON_Reader::ReadStatRequests(Node& root_node, RequestHandler& handler, MapRenderer& map_renderer) {
	std::ostringstream output;
	Array requests;
	const auto& node_array = root_node.AsArray();
	for (const auto& node : node_array) {
		const auto& node_info = node.AsDict();
		if (node_info.at("type") == "Bus") {
			std::optional<statistics::BusInfo> bus_info = handler.GetBusInfo(node_info.at("name").AsString());
			requests.push_back(PrintBusStatRequestsResult(node_info.at("id").AsInt(), bus_info));
		}
		else if (node_info.at("type") == "Stop") {
			std::optional<statistics::StopInfo> stop_info = handler.GetStopInfo(node_info.at("name").AsString());
			requests.push_back(PrintStopStatRequestsResult(node_info.at("id").AsInt(), stop_info));
		}
		else if (node_info.at("type") == "Map") {
			map_renderer.DrawMap(output);
			requests.push_back(PrintMapStatRequestsResult(node_info.at("id").AsInt(), output));
		}
	}
	Document doc{ requests };
	Print(doc, std::cout);
}

svg::Color JSON_Reader::GetColorFromNode(json::Node node) const {
	if (node.IsArray()) {
		if (node.AsArray().size() == 3) {
			return svg::Rgb(node.AsArray()[0].AsInt(),
				node.AsArray()[1].AsInt(),
				node.AsArray()[2].AsInt());
		}
		else if (node.AsArray().size() == 4) {
			return svg::Rgba(node.AsArray()[0].AsInt(),
				node.AsArray()[1].AsInt(),
				node.AsArray()[2].AsInt(),
				node.AsArray()[3].AsDouble());

		}
	}
	return node.AsString();
}

void JSON_Reader::ReadPropMapRequests(Node& root_node, MapRenderer& map_renderer) {
	const auto& node_map = root_node.AsDict();
	const auto& bus_offset_node = node_map.at("bus_label_offset").AsArray();
	const auto& stop_offset_node = node_map.at("stop_label_offset").AsArray();
	const auto& bus_offset_x = bus_offset_node[0].AsDouble();
	const auto& bus_offset_y = bus_offset_node[1].AsDouble();
	const auto& stop_offset_x = stop_offset_node[0].AsDouble();
	const auto& stop_offset_y = stop_offset_node[1].AsDouble();
	std::vector<svg::Color> colors;

	for (const auto& node : node_map.at("color_palette").AsArray()) {
		colors.push_back(GetColorFromNode(node));
	}

	map_renderer.SetWidth(node_map.at("width").AsDouble())
		.SetHeight(node_map.at("height").AsDouble())
		.SetPadding(node_map.at("padding").AsDouble())
		.SetBusLabelFontSize(node_map.at("bus_label_font_size").AsInt())
		.SetBusLabelOffset({ bus_offset_x, bus_offset_y })
		.SetStopLabelFontSize(node_map.at("stop_label_font_size").AsInt())
		.SetStopLabelOffset({ stop_offset_x, stop_offset_y })
		.SetStopRadius(node_map.at("stop_radius").AsDouble())
		.SetLineWidth(node_map.at("line_width").AsDouble())
		.SetUnderlayerColor(GetColorFromNode(node_map.at("underlayer_color")))
		.SetUnderlayerWidth(node_map.at("underlayer_width").AsDouble())
		.SetColorPalette(colors);
}

Node JSON_Reader::PrintBusStatRequestsResult(int request_id, std::optional<statistics::BusInfo> bus_info) {
	Node bus_node;

	if (!bus_info) {
		bus_node = Builder{}
						.StartDict()
							.Key("request_id").Value(request_id)
							.Key("error_message").Value("not found")
						.EndDict()
					.Build();
	}
	else {
		bus_node = Builder{}
						.StartDict()
							.Key("request_id").Value(request_id)
							.Key("curvature").Value(bus_info.value().curvature)
							.Key("route_length").Value(bus_info.value().route_length)
							.Key("stop_count").Value(bus_info.value().stops_num)
							.Key("unique_stop_count").Value(bus_info.value().unique_stops_num)
						.EndDict()
					.Build();
	}

	return bus_node;
}

Node JSON_Reader::PrintStopStatRequestsResult(int request_id, std::optional<statistics::StopInfo> stop_info) {
	Node stop_node;

	if (!stop_info) {
		stop_node = Builder{}
						.StartDict()
							.Key("request_id").Value(request_id)
							.Key("error_message").Value("not found")
						.EndDict()
					.Build();
	}
	else {
		Array buses_array;
		for (const auto& bus : stop_info.value().buses) {
			buses_array.push_back(std::string{ bus });
		}
		stop_node = Builder{}
						.StartDict()
							.Key("request_id").Value(request_id)
							.Key("buses").Value(buses_array)
						.EndDict()
					.Build();
	}

	return stop_node;
}

Node JSON_Reader::PrintMapStatRequestsResult(int request_id, std::ostringstream& output) {
	Node svg_str = Builder{}
						.StartDict()
							.Key("request_id").Value(request_id)
							.Key("map").Value(output.str())
						.EndDict()
					.Build();
	return svg_str;
}

}