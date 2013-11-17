/* configuration.cpp */

#include <sstream>

#include "data.h"

Data::Data(const Data& i) : itemType(Empty)
{
   switch (i.itemType) {
       case Empty:
           break;
       case String:
           asString() = i.sValue;
           break;
       case Map:
           asMap() = i.mValue;
           break;
       case Vector:
           asVector() = i.vValue;
           break;
       case Double:
           asDouble() = i.dValue;
           break;
       case Long:
           asLong() = i.lValue;
           break;
   }
}

Data::~Data()
{
   switch (itemType) {
       case String:
           sValue.~basic_string<char>();
           break;
       case Map:
           mValue.~map<std::string,Data>();
           break;
       case Vector:
           vValue.~vector<Data>();
           break;
   }
}

std::map<std::string, Data>& Data::asMap()
{
   switch (itemType) {
   case Empty:
       break;
   case Map:
       return mValue;
   case Vector:
       vValue.~vector<Data>();
       break;
   case String:
       sValue.~basic_string<char>();
       break;
   }
   itemType = Map;
   new (&mValue) std::map<std::string, Data>();
   return mValue;
}

std::vector<Data>& Data::asVector()
{
   switch (itemType) {
   case Empty:
       break;
   case Map:
       mValue.~map<std::string,Data>();
       break;
   case Vector:
       return vValue;
   case String:
       sValue.~basic_string<char>();
       break;
   }
   itemType = Vector;
   new (&vValue) std::vector<Data>();
   return vValue;
}

std::string& Data::asString()
{
   switch (itemType) {
   case Empty:
       break;
   case Map:
       mValue.~map<std::string,Data>();
       break;
   case Vector:
       vValue.~vector<Data>();
       break;
   case String:
       return sValue;
   }
   itemType = String;
   new (&sValue) std::string();
   return sValue;
}

double& Data::asDouble()
{
   switch (itemType) {
   case String:
       {
           double tmp;
           std::istringstream ss(sValue);
           ss >> tmp;
           sValue.~basic_string<char>();
           dValue = tmp;
       }
       break;
   case Map:
       mValue.~map<std::string,Data>();
       break;
   case Vector:
       vValue.~vector<Data>();
       break;
   case Double:
       break;
   case Long:
       {
           double tmp = lValue;
           dValue = tmp;
       }
       break;
   }
   itemType = Double;
   return dValue;
}

long& Data::asLong()
{
   switch (itemType) {
   case String:
       {
           long tmp;
           std::istringstream ss(sValue);
           ss >> tmp;
           sValue.~basic_string<char>();
           dValue = tmp;
       }
       break;
   case Map:
       mValue.~map<std::string,Data>();
       break;
   case Vector:
       vValue.~vector<Data>();
       break;
   case Long:
       break;
   case Double:
       {
           long tmp = (long)dValue;
           lValue = tmp;
       }
       break;
   }
   itemType = Long;
   return lValue;
}

static void indent(std::ostream& os, int l)
{
    while (l--) {
        os << "  ";
    }
}

void Data::print_to(std::ostream& os) const
{

    bool f = true;
    switch (itemType) {
        case Empty:
            os << "nil";
            break;
        case String:
            os << '"' << sValue << '"';
            break;
        case Double:
            os << dValue;
            break;
        case Long:
            os << lValue;
            break;
        case Vector:
            os << "[";
            for (auto v: vValue) {
                if (f) {
                    f = false;
                } else {
                    os << " ";
                }

                v.print_to(os);
            }
            os << "]";
            break;
        case Map:
            os << "{";
            for (auto& v: mValue) {
                if (f) {
                    f = false;
                } else {
                    os << " ";
                }

                os << ":" << v.first << " ";
                v.second.print_to(os);
            }
            os << "}";
            break;
    }
}

static void skip_space(std::istream& is)
{
    char c;
    std::string comment;
    while (is) {
        is.get(c);
        if (std::isspace(c) ||
            c == ',') {
            continue;
        }
        if (c == ';') {
            std::getline(is, comment);
            continue;
        }
        break;
    }
    if (is)
        is.unget();
}

void Data::parse(std::istream& is)
{
    skip_space(is);
    char c;
    is.get(c);
    switch (c) {
    case '{':
        {
            std::map<std::string,Data>& m = asMap();
            std::string key;
            Data val;

            skip_space(is);

            while (is && is.peek() != '}') {
                is >> key;
                val.parse(is);

                m[key.substr(1)] = val;
            }
            if (is) {
                is.get();
            }
        }
        break;
    case '[':
        {
            skip_space(is);
            std::vector<Data>& v = asVector();
            while (is && is.peek() != ']') {
                Data d;
                d.parse(is);
                v.push_back(d);
            }
            if (is) {
                is.get();
            }
        }
        break;
    case '"':
        {
            std::string v;

            std::getline(is, v, '"');
            asString() = v;
        }
        break;
    default:
        {
            is.unget();
            long lv;

            is >> lv;
            if (is.peek() == '.') {
                double rem;
                is >> rem;
                asDouble() = lv + rem;
            } else {
                asLong() = lv;
            }
        }
        break;
    }

    skip_space(is);
}

void Data::parse(const std::string& s)
{
    std::istringstream ss(s);

    parse(ss);
}

