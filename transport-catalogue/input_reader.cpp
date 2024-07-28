#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>

namespace input_reader {

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return { nan, nan };
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return { lat, lng };
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return { std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1)) };
}

std::pair<std::string_view, int> ParseDistance(std::string_view str) {
    int pos = str.find_first_not_of(' ');
    int m_pos = str.find_first_of('m');
    int distance = std::stoi(std::string(str.substr(pos, m_pos)));
    int to_pos = str.find("to");
    int stop_pos = str.find_first_not_of(' ', to_pos + 2);
    std::string_view stop_name = str.substr(stop_pos);
    return { stop_name, distance };
}

std::vector<std::pair<std::string_view, int>> ParseStopsDistance(std::string_view str) {
    std::vector<std::string_view> distances = Split(str, ',');
    std::vector<std::pair<std::string_view, int>> distances_to_stops;
    for (auto distance : distances) {
        distances_to_stops.push_back(ParseDistance(distance));
    }
    return distances_to_stops;
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] transport_catalog::TransportCatalogue& catalogue) const {
    for (const auto& command_line : commands_) {
        if (command_line.command == "Stop") {
            geo::Coordinates stop_coord = ParseCoordinates(command_line.description);
            catalogue.AddStop(command_line.id, stop_coord.lat, stop_coord.lng);
        }
    }

    for (const auto& command_line : commands_) {
        if (command_line.command == "Stop") {
            int first_comma = command_line.description.find_first_of(',');
            int second_comma = command_line.description.find_first_of(',', first_comma + 1);
            if (second_comma != std::string::npos) {
                transport_catalog::Stop* stop = catalogue.GetStop(command_line.id);
                std::string distances = command_line.description.substr(second_comma + 1);
                std::vector<std::pair<std::string_view, int>> distance_to_stops = ParseStopsDistance(distances);
                for (const auto& distance : distance_to_stops) {
                    catalogue.AddDistanceBetweenStops(stop, catalogue.GetStop(distance.first), distance.second);
                }
            }
        }
    }

    for (const auto& command_line : commands_) {
        if (command_line.command == "Bus") {
            std::vector<std::string_view> stops_names = ParseRoute(command_line.description);
            std::vector<transport_catalog::Stop*> stops;
            for (auto& stop_name : stops_names) {
                stops.push_back(catalogue.GetStop(stop_name));
            }
            catalogue.AddBus(command_line.id, stops);
        }
    }
}

void InputReader::ReadInput(std::istream& input) {
    int base_request_count;
    input >> base_request_count >> std::ws;
    for (int i = 0; i < base_request_count; ++i) {
        std::string line;
        std::getline(input, line);
        ParseLine(line);
    }
}

}