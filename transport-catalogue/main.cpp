#include <iostream>
#include <fstream>
#include <string>

#include "map_renderer.h"
#include "json_reader.h"
#include "request_handler.h"

using namespace std;

int main() {
    TransportCatalogue catalogue;
    RequestHandler req_handler(catalogue);
    json_reader::JSON_Reader reader;    
    MapRenderer map_renderer(req_handler);
    reader.ReadRequests(cin, catalogue, req_handler, map_renderer);
}