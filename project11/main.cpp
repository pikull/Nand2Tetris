#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.jack or directory>" << std::endl;
        return 1;
    }

    std::string pathStr = argv[1];
    std::vector<std::string> files;
    fs::path path(pathStr);

    if (fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".jack") {
                files.push_back(entry.path().string());
            }
        }
    } else if (path.extension() == ".jack") {
        files.push_back(path.string());
    } else {
        std::cerr << "Error: Input must be a .jack file or a directory containing .jack files." << std::endl;
        return 1;
    }

    for (const auto& inFile : files) {
        std::cout << "Compiling: " << inFile << std::endl;
        
        std::string outFile = inFile.substr(0, inFile.find_last_of('.')) + ".vm";

        JackTokenizer tokenizer(inFile);
        CompilationEngine engine(&tokenizer, outFile);

        if (tokenizer.hasMoreTokens()) {
            engine.compileClass();
        }
    }

    return 0;
}
