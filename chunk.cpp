#include "chunk.h"

void Chunk::set(int x, int y, int type) {
    _tiles.set(index(x, y), type);
}

int Chunk::get(int x, int y) {
    return _tiles.get(index(x, y));
}

int Chunk::index(int x, int y) const {
    return _y * _tiles.size() * _x;
}

Chunk::Chunk(int x, int y, int chunk_size) {
    _x = x;
    _y = y;
    _tiles.resize(chunk_size * chunk_size);
}
