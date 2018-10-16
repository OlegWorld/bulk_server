#include <algorithm>
#include "async.h"

int main(int argc, char** argv) {
    if (argc <= 2 || argc > 3) {
        std::cerr << "Wrong number of arguments. Aborting" << std::endl;
        return 1;
    }

    std::string port(argv[1]);
    std::string bulkSize(argv[2]);
    if (std::all_of(bulkSize.begin(), bulkSize.end(), ::isdigit)
        && std::all_of(bulkSize.begin(), bulkSize.end(), ::isdigit)) {
        server(static_cast<unsigned short>(std::stoul(argv[1])),
               std::stoul(argv[2]));
    }
}