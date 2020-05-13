#include "Nearest3DInterpolator.h"

Nearest3DInterpolator::Nearest3DInterpolator(int rough_i, int rough_j, int rough_k, std::string rough_source,
	int detailed_i, int detailed_j, int detailed_k, std::string detailed_source) {
	// read rough grid
	this->rough_grid.read_file(rough_i, rough_j, rough_k, rough_source);
	// read detailed grid firstly (only coords)
	this->detailed_grid.read_file(detailed_i, detailed_j, detailed_k, detailed_source, 1);
	// make tags for kdtree
	alglib::integer_1d_array idx;
	idx.setlength(detailed_i * detailed_j * detailed_k);
	for (size_t i = 0; i < detailed_i * detailed_j * detailed_k; i++)
		idx[i] = i;
	// make kdtree
	alglib::kdtreebuildtagged(detailed_grid.get_X(), idx, 3, 0, 2, this->detailed_grid_tree);
	// read detailed grid secondly (only values)
	this->detailed_grid.read_file(detailed_i, detailed_j, detailed_k, detailed_source, 2);
}
