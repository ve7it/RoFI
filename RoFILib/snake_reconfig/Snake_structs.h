#pragma once

#include <Configuration.h>
#include <IO.h>

inline std::tuple<int, int, int> tuple_center(const Matrix &m)
{
    auto v = center(m);
    return std::make_tuple(v(0), v(1), v(2));
}


class SpaceGrid {
public:
    using Cell = int;
    const Cell _EMPTY = -1;
    const Cell _COUNTED = -2;

    SpaceGrid(const Configuration& init)
    : _configuration(init)
    , _module_count(init.getIDs().size())
    , _side_size(4*_module_count-1)
    , _grid_size(_side_size * _side_size * _side_size)
    , _grid(_grid_size, _EMPTY) {
        _fillGrid();
    }

    Cell getCell(int x, int y, int z) const {
        return _grid[_getIndex(x, y, z)];
    }

    void setCell(int x, int y, int z, Cell cell) {
        _grid[_getIndex(x, y, z)] = cell;
    }

    bool isInGrid(int x, int y, int z) const {
    /*
        Grid accepts [-2*mc+1, 2*mc-1]
    */ 
        return _inGrid(x) && _inGrid(y) && _inGrid(z);
    }

    int getFreeness() {
        const auto& matrices = _configuration.getMatrices();
        static const std::vector<int> diff = {-1, 1};
        int freeness = 0;
        for (const auto& [id, matrix] : matrices) {
            for (const auto& side : matrix) {
                int x,y,z;
                std::tie(x,y,z) = tuple_center(side);
                for (int d : diff) {
                    freeness += _check_pos(x+d,y,z);
                    freeness += _check_pos(x,y+d,z);
                    freeness += _check_pos(x,y,z+d);
                }
            }
        }
        return freeness;
    }

    int getDist() {
        return _module_count * 8 + 2 - getFreeness();
    }

    void printGrid() const {
        int free = 0;
        int full = 0;
        for (const auto& g : _grid) {
            if (g == _EMPTY) {
                ++free;
            } else {
                ++full;
            }

        }
        std::cout << "Free: " << free <<"\tFull: " << full << std::endl;
    }

    void resetGrid() {
        _fillGrid();
    }

private:
    const Configuration& _configuration;
    const int _module_count;
    const unsigned int _side_size;
    const unsigned int _grid_size;
    std::vector<Cell> _grid;

    unsigned int _getIndex(int x, int y, int z) const {
        if(!isInGrid(x, y, z)) {
            throw std::out_of_range("Coordinates out of range");
        }

        x += 2 * _module_count - 1;
        y += 2 * _module_count - 1;
        z += 2 * _module_count - 1;
        return x * _side_size * _side_size + y * _side_size + z;
    }

    std::tuple<int,int,int> _getCoordinates(unsigned int i) const {
        if(i >= _grid_size) {
            throw std::out_of_range("Index out of range");
        }

        int z = i % _side_size;
        i = (i - z) / _side_size;
        int y = i % _side_size;
        i = (i - y) / _side_size; 
        int x = i;

        x -= 2 * _module_count - 1;
        y -= 2 * _module_count - 1;
        z -= 2 * _module_count - 1;
        return std::make_tuple(x,y,z);
    }

    bool _inGrid(int c) const {
        return 2 * _module_count > c && 2 * _module_count + c > 0;
    }

    void _fillGrid() {
        const auto& matrices = _configuration.getMatrices();
        _grid = std::vector<Cell>(_grid_size, _EMPTY);
        for (const auto& [id, matrix] : matrices) {
            for (const auto& side : matrix) {
                int x,y,z;
                std::tie(x,y,z) = tuple_center(side);
                setCell(x, y, z, id);
            }
        }
    }

    int _check_pos(int x, int y, int z) {
        if (isInGrid(x, y, z) && getCell(x, y, z) == _EMPTY) {
            setCell(x, y, z, _COUNTED);
            return 1;
        }
        return 0;
    }
};