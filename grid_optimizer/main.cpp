#include "grid.h"


int main() {
	int imax = 81;
	int jmax = 102;
	int kmax = 82;
	std::string filename = "rough_grid.dat";

	Grid rough_grid(imax, jmax, kmax);
	rough_grid.read_file(filename);

	node node_0 = rough_grid.get_node(0, 0, 0);
	std::cout << node_0.p << " " << node_0.x << std::endl;
	return 0;
}