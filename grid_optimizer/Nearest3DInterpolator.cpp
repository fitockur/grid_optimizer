#include "Nearest3DInterpolator.h"

double Nearest3DInterpolator::vector_norm(std::vector<double> first, std::vector<double> last) {
	double sum = 0;
	for (size_t i = 0; i < first.size(); i++)
		sum += (first[i] - last[i]) * (first[i] - last[i]);

	return sqrt(sum);
}

double Nearest3DInterpolator::weight(double ro) {
	return ro * ro;
}

Nearest3DInterpolator::Nearest3DInterpolator(int rough_i, int rough_j, int rough_k, std::string rough_source,
	int detailed_i, int detailed_j, int detailed_k, std::string detailed_source) {
	// read rough grid
	this->rough_grid.read_file(rough_i, rough_j, rough_k, rough_source);
	// read detailed grid
	this->detailed_grid.read_file(detailed_i, detailed_j, detailed_k, detailed_source);
	// make tags for kdtree
	std::pair<alglib::real_2d_array, alglib::integer_1d_array> X_tags(this->detailed_grid.get_X_tags());
	// make kdtree
	alglib::kdtreebuildtagged(X_tags.first, X_tags.second, 3, 0, 2, this->detailed_grid_tree);
}

void Nearest3DInterpolator::interpolate() {
	std::vector<int> ijk = this->rough_grid.get_ijk();
	alglib::real_1d_array BoxMin, BoxMax;
	BoxMin.setlength(3);
	BoxMax.setlength(3);
	// iterate over curved hexagons
	for (size_t i = 0; i < ijk[0] - 1; i++)	{
		for (size_t k = 0; k < ijk[2] - 1; k++) {
			for (size_t j = 0; j < ijk[1] - 1; j++)	{
				BoxMin[0] = this->rough_grid.get_node(i, k, j).x;
				BoxMin[1] = this->rough_grid.get_node(i, k, j).y;
				BoxMin[2] = this->rough_grid.get_node(i, k, j).z;

				BoxMax[0] = this->rough_grid.get_node(i + 1, k + 1, j + 1).x;
				BoxMax[1] = this->rough_grid.get_node(i + 1, k + 1, j + 1).y;
				BoxMax[2] = this->rough_grid.get_node(i + 1, k + 1, j + 1).z;
			}

		}

	}
}
