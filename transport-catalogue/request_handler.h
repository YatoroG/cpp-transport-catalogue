#pragma once
#include "transport_catalogue.h"

using namespace transport_catalogue;

class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& db)
    : db_(db) {
    };

    std::optional<statistics::BusInfo> GetBusInfo(std::string_view bus_num) const;
    std::optional<statistics::StopInfo> GetStopInfo(std::string_view stop_name) const;
    const std::set<Bus> GetAllBuses() const;
    const std::set<Stop> GetAllStops() const;

private:
    const TransportCatalogue& db_;
};