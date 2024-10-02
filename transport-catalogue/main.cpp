#include <iostream>
#include <fstream>
#include <string>

#include "map_renderer.h"
#include "json_reader.h"
#include "request_handler.h"

using namespace std;

int main() {
    //TransportCatalogue catalogue;
    //RequestHandler req_handler(catalogue);
    //json_reader::JSON_Reader reader;    
    //MapRenderer map_renderer(req_handler);    
    //TransportRouter router(catalogue, req_handler);
    //ifstream in("s12_final_opentest_1.json");
    //reader.ReadRequests(in, catalogue, req_handler, map_renderer, router);
    //in.close();
    TransportCatalogue catalogue;
    RequestHandler req_handler(catalogue);
    json_reader::JSON_Reader reader;
    MapRenderer map_renderer(req_handler);
    TransportRouter router(catalogue);
    reader.ReadRequests(cin, catalogue, req_handler, map_renderer, router);
}