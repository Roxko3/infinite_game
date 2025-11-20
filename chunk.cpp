#include "chunk.h"

void Chunk::set(int x, int y, int type) {
    _tiles.set(index(x, y), type);
}

int Chunk::get(int x, int y) {
    return _tiles.get(index(x, y));
}

int Chunk::index(int x, int y) const {
    return y * sqrt(_tiles.size()) + x;
}

void Chunk::generate() {
    for (int y = 0; y < sqrt(_tiles.size()); y++) {
        for (int x = 0; x < sqrt(_tiles.size()); x++) {
            int tile = (x + y) % 2;
            set(x, y, tile);
        }
    }
}
Chunk::Chunk(int x, int y, int chunk_size) {
    _x = x;
    _y = y;

    _tiles.resize(chunk_size * chunk_size);
    generate();
}
