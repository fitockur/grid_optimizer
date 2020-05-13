#include "Nearest3DInterpolator.h"


int main() {
	int rough_imax = 81;
	int rough_jmax = 102;
	int rough_kmax = 82;
	std::string rough_filename = "rough_grid.dat";

	int detailed_imax = 322;
	int detailed_jmax = 204;
	int detailed_kmax = 401;
	std::string detailed_filename = "detailed_grid.dat";

	Nearest3DInterpolator opt(rough_imax, rough_jmax, rough_kmax, rough_filename,
		detailed_imax, detailed_jmax, detailed_kmax, detailed_filename);

//	node node_0 = rough_grid.get_node(0, 0, 0);
//	std::cout << node_0.p << " " << node_0.x << std::endl;
	return 0;
}