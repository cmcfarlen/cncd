/* gcode_parser.cpp */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <functional>

#include "gcode_parser.h"

using namespace std;

struct ModalState
{
    enum DistanceMode {
        Absolute,
        Relative
    };

    double x;
    double y;
    double z;
    double fvel;
    double rvel;

    DistanceMode distance_mode;
    int          motion_code;


    void defaults() {
        x = 0;
        y = 0;
        z = 0;
        fvel = 50;
        rvel = 100;
        distance_mode = Relative;
        motion_code = 0;
    }
};

GcodeParser::GcodeParser()
{}


void GcodeParser::parse(const std::string& path)
{
    ifstream fs(path.c_str());

    parse(fs);
}

static std::istream& parse_args(std::istream& s, std::map<char, double>& args)
{
    std::string line;

    std::getline(s, line);

    std::istringstream ss(line);
    args.clear();

    char a;
    double v;
    while (ss >> a && a != '\n') {
        if (ss >> v) {
            args[a] = v;
        }
    }

    return s;
}

inline std::ostream& operator<<(std::ostream& os, ModalState& s)
{
    os << "x:"<<s.x<<",y:"<<s.y<<",z:"<<s.z<<",f"<<s.fvel<<"P:"<<s.distance_mode<<"M:"<<s.motion_code;
    return os;
}

template <typename A, typename B>
inline std::ostream& operator<<(std::ostream& os, std::pair<A,B>& p)
{
    os << p.first << ":" << p.second;
    return os;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, std::vector<T>& args)
{
    bool f = false;
    os << "[";
    for (auto& v: args) {
        if (f) {
            os << ",";
        } else {
            f = true;
        }
        os << v;
    }
    os << "]";

    return os;
}

template <typename A, typename B>
inline std::ostream& operator<<(std::ostream& os, std::map<A, B>& args)
{
    bool f = false;
    os << "{";
    for (auto& v: args) {
        if (f) {
            os << ",";
        } else {
            f = true;
        }
        os << v.first << ":" << v.second;
    }
    os << "}";

    return os;
}

typedef std::map<char, double> argmap;
typedef void (*code_function_t)(ModalState& state, argmap& args);
typedef std::function<code_function_t> code_function;
typedef std::map<int, code_function_t> code_dispatch;
typedef std::map<char, code_dispatch> dispatch_map;

double arg(const argmap& m, char v, double def)
{
    auto i = m.find(v);
    if (i == m.end()) {
        return def;
    }
    return (*i).second;
}

void g0(ModalState& s, argmap& args)
{
    std::cout << "dispatch g0!\n";
    double x = s.x;
    double y = s.y;
    double z = s.z;
    if (s.distance_mode = ModalState::Absolute) {
        x = arg(args, 'X', x);
        y = arg(args, 'Y', y);
        z = arg(args, 'Z', z);
    } else {
        x += arg(args, 'X', x);
        y += arg(args, 'Y', y);
        z += arg(args, 'Z', z);
    }
    s.x = x;
    s.y = y;
    s.z = z;
}

code_dispatch gdispatch = {
    //{0, [] (ModalState& s, argmap& a) -> bool { std::cout << "dispatch g0!\n"; return true; }}
    //{0, g0}
    {0, g0},
    {1, g0},
    {90, [] (ModalState& s, argmap&) { s.distance_mode = ModalState::Absolute; }}
    };

dispatch_map dispatch = {
    { 'G', gdispatch },
};

void GcodeParser::parse(std::istream& s)
{
    cout << "parsing\n";
    ModalState state;

    state.defaults();

    char g;
    double v;
    int c;
    std::string block;
    std::string expr;
    std::string comment;
    std::map<char,std::vector<int> > codes;
    std::map<char,double> args;
    while (getline(s, block)) {
        codes.clear();
        args.clear();
        comment.clear();

        istringstream ss(block);
        while (ss >> g) {
            switch (g) {
                case '(':
                    std::getline(ss, comment, ')');
                    break;
                case 'G':
                case 'M':
                case 'T':
                    ss >> c;
                    codes[g].push_back(c);
                    break;
                case 'F':
                case 'X':
                case 'Y':
                case 'Z':
                case 'I':
                case 'J':
                    ss >> v;
                    args[g] = v;
            }
        }

        for (auto& p: codes) {
            for (auto code: p.second) {
                code_function_t f = gdispatch[code];
                if (f) {
                    f(state, args);
                }
            }
        }

        std::cout << "BLOCK: " << block << " CODES: " << codes << " ARGS: " << args;
        if (!comment.empty()) {
            std::cout << " COMMENT: " << comment;
        }
        std::cout << "\n\t" << state << "\n";
    }

    /*
    int code;
    std::string tmp;
    std::vector<std::pair<char, double> > args;
    while (s >> g) {
        switch (g) {
        case '(': // comment
            std::getline(s, tmp, ')');
            cout << "COMMENT: " << tmp << "\n";
            break;
        case 'G':
            if (s >> code) {
                parse_args(s, args);
                cout << "G - " << code << " args: " << args << "\n";
            } else {
                cout << "G without a code!\n";
            }
            break;
        default:
            std::getline(s, tmp);
            cout << "UNHANDLED: " << g << " - " << tmp << "\n";
            break;
        }
    }
    */

}
