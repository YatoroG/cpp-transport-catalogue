#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace statistics {

struct Request {
    std::string_view keyword;
    std::string_view id;
};

Request ParseRequest(std::string_view request);
void ParseAndPrintStat(const transport_catalog::TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
void PrintBusInfo(std::optional<BusInfo> bus_info, std::string_view bus_num, std::ostream& output);
void PrintStopInfo(std::optional<StopInfo> stop_info, std::string_view stop_name, std::ostream& output);
void ReadInput(std::istream& input, const transport_catalog::TransportCatalogue& transport_catalogue, std::ostream& output);
}