#include <filesystem>
#include <fstream>
#include <ios>

namespace fs = std::filesystem;

fs::path::string_type make_path(const fs::path::string_type& str) {
    return fs::path(str).native();
}

int main(int argc, char** argv) {
    std::fstream ofs();
    
    return 0;
}
