#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

namespace analyser::file {

struct File {
    static inline const std::string command_prefix =
#ifdef _WIN32
        // Outside the container there is no /root/.config: on Windows the
        // tree-sitter CLI looks for config.json in %APPDATA%\tree-sitter itself.
        "tree-sitter parse ";
#else
        "tree-sitter parse --config-path /root/.config/tree-sitter/config.json ";
#endif
    File(const std::string &filename);
    std::string name;
    std::string ast;
    std::vector<std::string> source_lines;

private:
    std::vector<std::string> ReadSourceFile(std::ifstream &file);
    std::string GetAst(const std::string &filename);
};

}  // namespace analyser::file
