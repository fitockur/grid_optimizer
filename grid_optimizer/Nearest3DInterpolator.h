#pragma once
#include <cmath>
#include "grid.h"
#include "KDTree/alglibmisc.h"


class Nearest3DInterpolator {
private:
	Grid rough_grid;
	Grid detailed_grid;
	alglib::kdtree detailed_grid_tree;
	double vector_norm(std::vector<double> first, std::vector<double> last);
	double weight(double ro);
public:
	Nearest3DInterpolator(int rough_i, int rough_j, int rough_k, std::string rough_source,
		int detailed_i, int detailed_j, int detailed_k, std::string detailed_source);
	void interpolate();
};

