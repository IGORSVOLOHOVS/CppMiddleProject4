#include "file.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#ifdef _WIN32
#include <cstdio>  // _popen/_pclose live here, MSVC has neither <unistd.h> nor <sys/wait.h>
#else
#include <sys/wait.h>  // WIFEXITED/WEXITSTATUS
#endif

namespace analyser::file {

namespace {

// popen/pclose are POSIX; MSVC ships them under underscored names and reports
// the child's exit code directly instead of an encoded wait status.
FILE *OpenReadPipe(const char *command) {
#ifdef _WIN32
    return _popen(command, "r");
#else
    return popen(command, "r");
#endif
}

// Returns the exit code of the finished command, throws if it did not exit normally.
int CloseReadPipe(FILE *pipe) {
#ifdef _WIN32
    const int exit_status = _pclose(pipe);
    if (exit_status == -1) {
        throw std::runtime_error("Command terminated abnormally");
    }
    return exit_status;
#else
    const int status = pclose(pipe);
    if (!WIFEXITED(status)) {
        throw std::runtime_error("Command terminated abnormally");
    }
    return WEXITSTATUS(status);
#endif
}

}  // namespace

namespace rv = std::ranges::views;
namespace rs = std::ranges;

File::File(const std::string &filename) : name{filename} {
    std::ifstream file(name);

    if (!file.is_open()) {
        throw std::invalid_argument("Can't open file " + filename);
    }
    ast = GetAst(filename);
    source_lines = ReadSourceFile(file);
}

std::vector<std::string> File::ReadSourceFile(std::ifstream &file) {
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::string File::GetAst(const std::string &filename) try {
    std::string full_cmd = File::command_prefix + filename + " 2>&1";
    std::string result;
    std::array<char, 256> buffer;

    using PipePtr = std::unique_ptr<FILE, decltype([](FILE *pipe) {
                                        if (!pipe)
                                            return;

                                        int exit_status = CloseReadPipe(pipe);
                                        if (exit_status != 0) {
                                            throw std::runtime_error("Command failed with exit code " +
                                                                     std::to_string(exit_status));
                                        }
                                    })>;

    FILE *raw_pipe = OpenReadPipe(full_cmd.c_str());
    if (!raw_pipe) {
        throw std::runtime_error("Failed to execute command: " + std::string(std::strerror(errno)));
    }
    PipePtr pipe(raw_pipe);

    while (fgets(buffer.data(), buffer.size(), pipe.get())) {
        result += buffer.data();
    }

    return result;
} catch (const std::exception &e) {
    throw std::runtime_error("Error while getting ast from " + filename);
}

}  // namespace analyser::file
