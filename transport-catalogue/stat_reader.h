#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace statistics {

void ParseAndPrintStat(const transport_catalog::TransportCatalogue& transport_catalogue, std::string_view request,
    std::ostream& output);

}