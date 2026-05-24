#include "error_service.hpp"
#include "io.hpp"

#include <fstream>
#include <sstream>


std::string read_file(const std::string &path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        ErrorService::runtime_error("Could not open file", path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}
