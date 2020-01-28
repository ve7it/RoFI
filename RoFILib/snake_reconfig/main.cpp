#include <exception>
#include <fstream>
#include <tuple>
#include <cxxopts.hpp>
#include <Configuration.h>
#include <IO.h>

enum Cell { EMPTY, OCCUPIED, PORT };


class SpaceGrid {
public:
    SpaceGrid(const Configuration& init)
    : _configuration(init)
    , _module_count(init.getIds().size())
    , _side_size(4*_module_count-1)
    , _grid_size(_side_size * _side_size * _side_size);
    , _grid(_grid_size, Cell::EMPTY) {
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

private:
    const Configuration& _configuration;
    const unsigned int _module_count;
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
        return c < 2 * _module_count && c > - 2 * _module_count;
    }

    void _fillGrid() {
        _configuration.computeMatrices();
        const auto& matrices = _configuration.getMatrices();
        for (const auto& [id, matrix] : matrices) {
            for (const auto& side : matrix) {
                const auto& center = center(side);
                //oof vec4 -> xyz
                setCell(, Cell::OCCUPIED);
            }
        }
    }
};


int main(int argc, char* argv[])
{
    if (argc < 2) {
        throw std::invalid_argument("No path given");
    }

    std::ifstream initInput;
    initInput.open(argv[1]);
    Configuration init;
    readConfiguration(initInput, init);


}