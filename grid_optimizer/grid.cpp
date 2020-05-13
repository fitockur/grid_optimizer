#include "grid.h"

Grid::Grid() {}

void Grid::read_file(int IMAX, int JMAX, int KMAX, std::string filename, int t) {
	std::ifstream data_file;
	std::string path = "../data/" + filename;
	node tmp;
	double buf;
	int counter = 0;
	double x, y, z;

	this->IMAX = IMAX;
	this->IMAX = JMAX;
	this->KMAX = KMAX;
	if (t == 0) {
		this->X.setlength(IMAX * JMAX * KMAX, 3);
		this->Y.reserve(IMAX * JMAX * KMAX);
	}
	else if (t == 1) {
		this->X.setlength(IMAX * JMAX * KMAX, 3);
	}
	else
		this->Y.reserve(IMAX * JMAX * KMAX);

	data_file.open(path, std::ios::in | std::ios::binary);
	if (data_file.is_open())
	{
		for (int i = 0; i < IMAX; i++)  //считывание из файла
			for (int k = 0; k < KMAX; k++) {
				for (int j = 0; j < JMAX; j++) {
					data_file.read((char*)&tmp, sizeof(tmp));
					data_file.read((char*)&x, sizeof(x));
					data_file.read((char*)&y, sizeof(y));
					data_file.read((char*)&z, sizeof(z));

					if (t == 0) {
						this->X[counter][0] = x;
						this->X[counter][1] = y;
						this->X[counter][2] = z;
						this->Y.push_back(tmp);
					}
					else if (t == 1) {
						this->X[counter][0] = x;
						this->X[counter][1] = y;
						this->X[counter][2] = z;
					}
					else
						this->Y.push_back(tmp);
					
					counter++;
				}
				// ненужные нам пока qw и tauw, с ними отдельно,
				// когда все сойдется, чтоб зря с собой не таскать
				data_file.read((char*)&buf, sizeof(double));
				data_file.read((char*)&buf, sizeof(double));
			}
		// close the opened file.
		data_file.close();
	}
	else
		std::cout << "ERROR! File '" << filename << "' not found." << std::endl;
}

alglib::real_2d_array Grid::get_X() {
	return this->X;
}
