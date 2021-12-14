#ifndef PROJECT_1_GRID_HPP
#define PROJECT_1_GRID_HPP

#include <vector>
#include "curve.hpp"
#include "../util/utilities.hpp"

class Grid {

private:
    double grid_interval;
    double noise;

    //void initialize_noise(uint32_t dimensions);
    void map_to_grid(Curve &curve);
    void snap(vector<double> *_vector, double interval);
    //void remove_consecutive_duplicates(Curve &curve);
    //bool are_equal_consecutive_vectors(const std::vector<Point *> *curve_data, uint32_t index) const;
    //void add_noise(Curve &curve);

public:
    explicit Grid(double grid_interval, uint32_t dimensions);
    void fit(Curve &curve);
};


#endif //PROJECT_1_GRID_HPP
