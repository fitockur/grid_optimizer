#include "grid.h"


int main() {
	int imax = 81;
	int jmax = 102;
	int kmax = 82;
	std::string filename = "rough_grid.dat";
	Grid rough_grid(imax, jmax, kmax);
	rough_grid.read_file(filename);

	/*imax = 322;
	jmax = 204;
	kmax = 401;
	filename = "detailed_grid.dat";
	Grid detailed_grid(imax, jmax, kmax);
	detailed_grid.read_file(filename);*/

	alglib::kdtree kdt;
	alglib::kdtreebuild(rough_grid.get_X(), 3, 0, 2, kdt);

//	node node_0 = rough_grid.get_node(0, 0, 0);
//	std::cout << node_0.p << " " << node_0.x << std::endl;
	return 0;
}