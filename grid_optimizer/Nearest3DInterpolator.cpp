#include "Nearest3DInterpolator.h"


Nearest3DInterpolator::Nearest3DInterpolator(int rough_i, int rough_j, int rough_k, std::string rough_source,
	int detailed_i, int detailed_j, int detailed_k, std::string detailed_source) {
	// read rough grid
	this->rough_grid.read_file(rough_i, rough_j, rough_k, rough_source);
	// read detailed grid
	this->detailed_grid.read_file(detailed_i, detailed_j, detailed_k, detailed_source);
	// make tags for kdtree
	std::pair<alglib::real_2d_array, alglib::integer_1d_array> X_tags(this->rough_grid.get_X_tags());
	// make kdtree
	alglib::kdtreebuildtagged(X_tags.first, X_tags.second, 3, 0, 2, this->rough_grid_tree);
}

void Nearest3DInterpolator::project() {
	index ijk = this->detailed_grid.get_ijk();
	alglib::integer_1d_array tag;
	index ijk_c;
	index ijk_v;

	// iterate over detailed points
	for (size_t i = 0; i < ijk[0] - 1; i++)	{
		for (size_t k = 0; k < ijk[2] - 1; k++) {
			for (size_t j = 1; j < ijk[1] - 1; j++)	{
				// find nearest node and get it's tag
				alglib::kdtreequeryknn(this->rough_grid_tree, this->detailed_grid.get_xyz(i, k, j), 1);
				alglib::kdtreequeryresultstags(this->rough_grid_tree, tag);
				ijk_v = this->rough_grid.get_ijk(tag[0]);
				// std::cout << tag.tostring() << std::endl;
				// std::cout << ijk_c[0] << ' ' << ijk_c[1] << ' ' << ijk_c[2] << std::endl;
				ijk_c = {i, j, k};
				this->locate(ijk_c, ijk_v);
			}
		}
	}
}

bool Nearest3DInterpolator::hex_exists(const index & ijk_vc) {
	index ijk_max = this->rough_grid.get_ijk();
	for (size_t i = 0; i < ijk_vc.size(); i++)
		if ((ijk_vc[i] < 0) || (ijk_vc[i] >= ijk_max[i]))
			return false;
	return true;
}

bool Nearest3DInterpolator::spatial_convex_inclusion(const index & ijk_c, const index & ijk_vc) {
	return false;
}

void Nearest3DInterpolator::locate(const index & ijk, const index & ijk_v) {
	index ijk_vc;
	std::vector<index> vertices;
	bool finish = false;

	for (int di = 0; di > -2 && !finish; di--) {
		for (int dk = 0; dk > -2 && !finish; dk--) {
			for (int dj = 0; dj > -2 && !finish; dj--) {
				ijk_vc[0] = ijk_v[0] + di;
				ijk_vc[1] = ijk_v[1] + dj;
				ijk_vc[2] = ijk_v[2] + dk;
				this->locate_cnt++;
				if (this->hex_exists(ijk_vc) && this->spatial_convex_inclusion(ijk, ijk_vc)) {
					vertices = this->get_vertices(ijk_vc);
					this->interpolate(ijk, vertices);
					this->interpolate_cnt++;
					finish = true;
				}
			}
		}
	}
}
