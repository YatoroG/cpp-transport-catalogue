#pragma once
#include "transport_catalogue.h"

using namespace transport_catalogue;

class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db)
    : db_(db) {
    };

    std::optional<statistics::BusInfo> GetBusInfo(std::string_view bus_num) const;
    std::optional<statistics::StopInfo> GetStopInfo(std::string_view stop_name) const;
    const std::set<Bus> GetAllBuses() const;
    const std::set<Stop> GetAllStops() const;

    // Этот метод будет нужен в следующей части итогового проекта
    //svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    //const renderer::MapRenderer& renderer_;
};