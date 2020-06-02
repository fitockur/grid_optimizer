#include "Nearest3DInterpolator.h"


int main() {
	int rough_imax = 135;
	int rough_jmax = 51;
	int rough_kmax = 92;
	std::string rough_path = "A:\\Учеба\\ВКР\\grid_optimizer\\data\\rough_grid_drotik.dat";

	int detailed_imax = 135;
	int detailed_jmax = 101;
	int detailed_kmax = 182;
	std::string detailed_path = "A:\\Учеба\\ВКР\\grid_optimizer\\data\\detailed_grid_drotik.dat";

	Nearest3DInterpolator opt(rough_imax, rough_jmax, rough_kmax, rough_path,
		detailed_imax, detailed_jmax, detailed_kmax, detailed_path);
	opt.project();
	return 0;
}