#include "Map.hpp"

const int Map::operator()(int x, int y) const {
    return data[x + map_height * y];
}

Map::Map(std::string filepath) {
    FILE * file = fopen(filepath.c_str(), "rb");

    fread(&map_width, sizeof(long), 1, file);
    fread(&map_height, sizeof(long), 1, file);

    data = new int[sizeof(int) * map_height * map_height];

    fread(data, sizeof(int), map_height * map_height, file);

    fclose(file);
}

Map::~Map() {
    delete[] data;
}