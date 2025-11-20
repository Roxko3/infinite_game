#ifndef CHUNK_H
#define CHUNK_H

#include "sfw.h"

//grass = 0
//wall = 1
class Chunk{

public:
    int _x;
    int _y;

    Vector<int> _tiles;

    Chunk(int x, int y, int chunk_size);

    Chunk() : _x(0), _y(0) {}

    virtual void set(int x, int y, int type);
    virtual int get(int x, int y);
    virtual int index(int x, int y) const;
    virtual void generate();
};

#endif
