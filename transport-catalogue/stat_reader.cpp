#include "stat_reader.h"
#include <iomanip>
#include <iostream>
#include <algorithm>

namespace statistics {

void ParseAndPrintStat(const transport_catalog::TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    auto pos = request.find_first_not_of(' ');
    if (request.substr(pos, request.find_first_of(' ', pos)) == "Bus") {
        request.remove_prefix(pos + 3);
        pos = request.find_first_not_of(' ');
        std::string_view bus_num = request.substr(pos, pos + request.find_last_not_of(' '));

        transport_catalog::Bus* founded_bus = transport_catalogue.GetBus(bus_num);

        if (founded_bus == nullptr) {
            output << "Bus " << bus_num << ": not found" << std::endl;
            return;
        }

        output << "Bus " << founded_bus->bus_num << ": "
            << transport_catalogue.GetStops(*founded_bus) << " stops on route, "
            << transport_catalogue.GetUniqueStops(*founded_bus) << " unique stops, "
            << transport_catalogue.CountRouteDistance(*founded_bus) << std::setprecision(6) << " route length" << std::endl;
    }
    else if (request.substr(pos, request.find_first_of(' ', pos)) == "Stop") {
        request.remove_prefix(pos + 4);
        pos = request.find_first_not_of(' ');
        std::string_view stop_name = request.substr(pos, pos + request.find_last_not_of(' '));

        transport_catalog::Stop* founded_stop = transport_catalogue.GetStop(stop_name);

        if (founded_stop == nullptr) {
            output << "Stop " << stop_name << ": not found" << std::endl;
            return;
        }

        std::set<std::string_view> buses = transport_catalogue.GetBusesForStop(founded_stop);

        if (buses.empty()) {
            output << "Stop " << stop_name << ": no buses" << std::endl;
            return;
        }

        output << "Stop " << founded_stop->name << ": buses ";
        for (const auto& bus : buses) {
            output << bus;
            if (bus != *buses.rbegin()) {
                output << " ";
            }
        }
        output << std::endl;
    }
}

}