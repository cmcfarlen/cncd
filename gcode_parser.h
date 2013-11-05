/* gcode_parser.h */

#ifndef _GCODED_GCODE_PARSER_H_
#define _GCODED_GCODE_PARSER_H_

#include <string>
#include <istream>

class GcodeParser
{
public:
    GcodeParser();

    void parse(const std::string& path);
    void parse(std::istream& is);
};

#endif
