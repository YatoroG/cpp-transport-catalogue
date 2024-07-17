#include <iostream>
#include <fstream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    transport_catalog::TransportCatalogue catalogue;

    {
        input_reader::InputReader reader;
        reader.ReadInput(cin);
        reader.ApplyCommands(catalogue);
    }

    statistics::ReadInput(cin, catalogue, cout);
}