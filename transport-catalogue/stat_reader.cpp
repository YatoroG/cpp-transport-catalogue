#include "stat_reader.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace statistics {

Request ParseRequest(std::string_view request_text) {
    Request request;
    auto pos = request_text.find_first_not_of(' ');
    request.keyword = request_text.substr(pos, request_text.find_first_of(' ', pos));
    request_text.remove_prefix(pos + request.keyword.size());
    pos = request_text.find_first_not_of(' ');
    request.id = request_text.substr(pos, pos + request_text.find_last_not_of(' '));
    return request;
}

void ParseAndPrintStat(const transport_catalog::TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    Request parsed_request = ParseRequest(request);
    if (parsed_request.keyword == "Bus") {
        std::optional<BusInfo> bus_info = transport_catalogue.GetBusInfo(parsed_request.id);
        PrintBusInfo(bus_info, parsed_request.id, output);
    }
    else if (parsed_request.keyword == "Stop") {
        std::optional<StopInfo> stop_info = transport_catalogue.GetStopInfo(parsed_request.id);
        PrintStopInfo(stop_info, parsed_request.id, output);
    }
}

void PrintBusInfo(std::optional<BusInfo> bus_info, std::string_view bus_num, std::ostream& output) {
    output << "Bus ";
    if (!bus_info) {
        output << bus_num << ": not found" << std::endl;
        return;
    }

    output << bus_info.value().bus_num << ": " 
        << bus_info.value().stops_num << " stops on route, "
        << bus_info.value().unique_stops_num << " unique stops, "
        << bus_info.value().route_length << std::setprecision(6) << " route length" << std::endl;
}

void PrintStopInfo(std::optional<StopInfo> stop_info, std::string_view stop_name, std::ostream& output) {
    output << "Stop ";
    if (!stop_info) {
        output << stop_name << ": not found" << std::endl;
        return;
    }

    if (stop_info.value().buses.empty()) {
        output << stop_info.value().stop_name << ": no buses" << std::endl;
        return;
    }
    
    output << stop_info.value().stop_name << ": buses ";
    for (const auto& bus : stop_info.value().buses) {
        output << bus;
        if (bus != *stop_info.value().buses.rbegin()) {
            output << " ";
        }
    }
    output << std::endl;
}

void ReadInput(std::istream& input, const transport_catalog::TransportCatalogue& transport_catalogue, std::ostream& output) {
    int stat_request_count;
    input >> stat_request_count >> std::ws;
    for (int i = 0; i < stat_request_count; ++i) {
        std::string line;
        std::getline(input, line);
        ParseAndPrintStat(transport_catalogue, line, output);
    }
}

}