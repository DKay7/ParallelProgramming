#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unistd.h>
#include "debug.h"

class ArgParser final {
public:
    ArgParser(int argc, char **argv, std::string optstring)
        : argc(argc), argv(argv), optstring(optstring) 
    
    {

        parse_args();

        DEBUG(
            for (auto &[key, value] : parsed_values) {
                std::cout << key << "[" << value << "]\n";
            }
        )

    }

    template<typename Type>
    Type get_arg(char arg_name) {
        Type result;

        std::stringstream ss(parsed_values.at(arg_name));

        ss >> result;
        return result;
    }

private:
    int argc;
    char **argv;
    std::string optstring;
    std::unordered_map<char, std::string> parsed_values;

    void parse_args() {
        char result = 0;
        while ( (result = getopt(argc, argv, optstring.c_str())) != -1 ) {
            switch (result) {
                case '?':
                    std::cerr << "Error while parsong arguments";
                
                default:
                    parsed_values[result] = optarg;
            }
        }
    }
};
