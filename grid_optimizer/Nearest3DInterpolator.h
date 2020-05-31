#pragma once
#include <cmath>
#include "grid.h"
#include "PBar.h"
#include "KDTree/alglibmisc.h"


class Nearest3DInterpolator {
private:
	Grid rough_grid;
	Grid detailed_grid;
	alglib::kdtree rough_grid_tree;
	alglib::kdtree rough_grid_tree_b;
	bool hex_exists(const index & ijk_vc);
	bool plane_exists(const std::vector<index> & ijk_vc);
	bool spatial_convex_inclusion(const index & ijk_c, const index & ijk_vc);
	bool plane_inclusion(const index & ijk_c, const std::vector<index> & ijk_vc, alglib::real_1d_array & xyz_pr);
	void locate(const index & ijk, const index & ijk_v);
	void locate_b(const index & ijk, const index & ijk_v);
	std::vector<index> get_vertices(const index & ijk_vc);
	void interpolate(const index & ijk, const std::vector<index> & vertices);
	void interpolate_b(const index & ijk, const std::vector<index> & vertices, const alglib::real_1d_array & xyz_pr);
	void shift(index & ijk, const bool & left);
	double plane_position(const alglib::real_1d_array & OA, const alglib::real_1d_array & OB,
		const alglib::real_1d_array & OP);
	alglib::real_1d_array get_test_point(const index & ijk_vc);
	void cross_product(const alglib::real_1d_array & A, const alglib::real_1d_array & B, alglib::real_1d_array & C);
	double dot_product(const alglib::real_1d_array & A, const alglib::real_1d_array & B);
public:
	int locate_cnt;
	int interpolate_cnt;
	int locate_b_cnt;
	int interpolate_b_cnt;
	Nearest3DInterpolator(int rough_i, int rough_j, int rough_k, std::string rough_source,
		int detailed_i, int detailed_j, int detailed_k, std::string detailed_source);
	void project();
};

