#ifndef INC_HPP_MAP
#define INC_HPP_MAP

#include <string>

struct Map {
  long map_width, map_height;
  int * data;

  const int operator()(int x, int y) const;

  Map(std::string filepath);
  ~Map();
};


#endif