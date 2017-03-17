#pragma once
#ifndef __ARRAY2D__
#define __ARRAY2D__

#include <array>
#include <cassert>

template<typename T, size_t size_x, size_t size_y>
class Array2d
{
public:

    const T& at(size_t x, size_t y) const
    {
        assert(x < sizeX() && "array x subscript out of range");
        assert(y < sizeY() && "array y subscript out of range");
        return _data[y * sizeX() + x];
    }

    T& at(size_t x, size_t y)
    {
        assert(x < sizeX() && "array x subscript out of range");
        assert(y < sizeY() && "array y subscript out of range");
        return _data[y * sizeX() + x];
    }

    size_t sizeX() const { return size_x; }

    size_t sizeY() const { return size_y; }

    size_t size() const { return _data.size(); }

private:

    std::array<T, size_x * size_y> _data;

};

#endif
