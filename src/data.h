/* configuration.h */

#ifndef _CNCD_DATA_H_
#define _CNCD_DATA_H_

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

class Data {
public:
   enum Type {
      Empty,
      Double,
      Long,
      String,
      Map,
      Vector
   };

   Data() : itemType(Empty) {}
   Data(double d) : itemType(Double), dValue(d) {}
   Data(long l) : itemType(Long), lValue(l) {}
   Data(int l) : itemType(Long), lValue(l) {}
   Data(const std::string& s) : itemType(String), sValue(s) {}
   Data(const char* s) : itemType(String), sValue(s) {}
   Data(const Data& i);

   ~Data();

   std::map<std::string, Data>& asMap();

   std::vector<Data>& asVector();

   std::string& asString();

   double& asDouble();

   long& asLong();

   Data& append(const Data& item)
   {
       asVector().push_back(item);
       return *this;
   }

   Data& operator[](const std::string& key)
   {
       return asMap()[key];
   }

   Data& operator=(const Data& i)
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

   void print_to(std::ostream& os) const;
   void parse(std::istream& is);
   void parse(const std::string& is);

   Type itemType;
   union {
       double dValue;
       long   lValue;
       std::string sValue;
       std::map<std::string, Data> mValue;
       std::vector<Data> vValue;
   };
};

inline std::ostream& operator<<(std::ostream& os, const Data& i)
{
    i.print_to(os);
    return os;
}

inline std::istream& operator>>(std::istream& is, Data& d)
{
    d.parse(is);
    return is;
}

#endif
