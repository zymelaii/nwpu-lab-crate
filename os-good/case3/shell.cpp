#include <stdlib.h>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

std::string parse(const std::string &input) {
    const auto start = input.find_first_not_of(" \t");
    const auto size  = input.find_last_not_of(" \t\n") - start + 1;
    return start != input.npos ? input.substr(start, size) : "";
}

void execute(const std::string &cmdline) {
    if (cmdline.empty()) {
        //! ignore
    } else if (cmdline == "pwd") {
        std::cout << fs::current_path().string() << std::endl;
    } else if (cmdline.find("echo ") == 0) {
        std::cout << cmdline.substr(5) << std::endl;
    } else if (cmdline.find("cd ") == 0) {
        auto path = fs::current_path().append(cmdline.substr(3));
        if (fs::exists(path)) {
            fs::current_path(path);
        } else {
            std::cout << "error: path `" << path.string() << "` not exists"
                      << std::endl;
        }
    } else if (cmdline == "ls") {
        for (auto entry : fs::directory_iterator(fs::current_path())) {
            if (entry.is_directory()) {
                std::cout << "D " << entry.path().stem().string() << std::endl;
            } else if (entry.is_regular_file()) {
                std::cout << "F " << entry.path().stem().string() << std::endl;
            }
        }
    } else if (cmdline == "exit") {
        exit(0);
    } else {
        std::cout << "error: unknown action <execute '" << cmdline << "'>"
                  << std::endl;
    }
}

void run(const std::string &input) {
    auto cmdline = parse(input);
    execute(cmdline);
}

int main(int argc, char *argv[]) {
    std::string buffer{};
    while (true) {
        std::cout << fs::current_path().string() << "$ ";
        std::getline(std::cin, buffer);
        if (std::cin.fail() || std::cin.eof()) { break; }
        run(buffer);
    }
    return 0;
}
