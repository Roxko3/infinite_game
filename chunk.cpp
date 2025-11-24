#include "chunk.h"

int Chunk::CHUNK_SIZE = 3;

void Chunk::set(int x, int y, int type) {
    _tiles.set(index(x, y), type);
}

int Chunk::get(int x, int y) {
    return _tiles[index(x, y)];
}

int Chunk::index(int x, int y) const {
    return y * CHUNK_SIZE + x;
}

void Chunk::generate() {
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int tile = Math::random(0,8);
            set(x, y, tile < 8 ? 0 : 1);
        }
    }
}
Chunk::Chunk(int x, int y) {
    _x = x;
    _y = y;
    _is_ready.clear();
    _tiles.resize(CHUNK_SIZE * CHUNK_SIZE);
}
