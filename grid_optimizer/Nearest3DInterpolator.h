#pragma once
#include <cmath>
#include "grid.h"
#include "KDTree/alglibmisc.h"


class Nearest3DInterpolator {
private:
	Grid rough_grid;
	Grid detailed_grid;
	alglib::kdtree rough_grid_tree;
	bool hex_exists(const index & ijk_vc);
	bool spatial_convex_inclusion(const index & ijk_c, const index & ijk_vc);
	void locate(const index & ijk, const index & ijk_v);
	std::vector<index> get_vertices(const index & ijk_vc);
	void interpolate(const index & ijk, const std::vector<index> & vertices);
	void shift(index & ijk, const bool & left);
	double plane_position(const alglib::real_1d_array & o, const alglib::real_1d_array & a,
		const alglib::real_1d_array & b, const alglib::real_1d_array & point);
public:
	int locate_cnt;
	int interpolate_cnt;
	Nearest3DInterpolator(int rough_i, int rough_j, int rough_k, std::string rough_source,
		int detailed_i, int detailed_j, int detailed_k, std::string detailed_source);
	void project();
};

