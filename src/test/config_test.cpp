/* config_test.cpp */

#include <fstream>
#include <iostream>
#include "data.h"

int main(int argc, char** argv)
{
    Data item;

    item["one"] = "two";
    item["three"]["four"] = "a";
    item["three"]["dbl"] = 1.234124;
    item["list"].append(42).append("2423");

    std::cout << item << "\n";

    std::vector<std::string> cases = {
        "\"test\"",
        "{:a \"b\"}",
        "[1 2 3 4]",
        "{:one {:two [1 2 3 4]} :a \"bcds\" :o 11 :u 123.123124}"
    };

    for (auto c: cases) {
        Data td;
        td.parse(c);
        std::cout << "test case: " << c << "\n" << td << "\n";
    }

    std::ifstream fs("./simulate.conf");
    Data config;
    config.parse(fs);
    std::cout << "config data: " << config << "\n";

    std::cout << "acceleration = " << config["machine"]["rapid_acceleration"] << "\n";
    std::cout << "x pitch = " << config["machine"]["x"]["pitch"] << "\n";

    return 0;
}


