#include "grid.h"

Grid::Grid(int IMAX, int JMAX, int KMAX) 
	: IMAX(IMAX), JMAX(JMAX), KMAX(KMAX) {
	this->X.setlength(IMAX * JMAX * KMAX, 3);
	this->Y.reserve(IMAX * JMAX * KMAX);
}

void Grid::read_file(std::string filename) {
	std::ifstream data_file;
	std::string path = "../data/" + filename;

	node tmp;
	double buf;
	int counter = 0;
	double x, y, z;

	data_file.open(path, std::ios::in | std::ios::binary);
	if (data_file.is_open())
	{
		for (int i = 0; i < this->IMAX; i++)  //считывание из файла
			for (int k = 0; k < this->KMAX; k++) {
				for (int j = 0; j < this->JMAX; j++) {
					/*data_file.read((char*)&tmp.p, sizeof(double));
					data_file.read((char*)&tmp.u, sizeof(double));
					data_file.read((char*)&tmp.v, sizeof(double));
					data_file.read((char*)&tmp.w, sizeof(double));
					data_file.read((char*)&tmp.t, sizeof(double));
					data_file.read((char*)&tmp.k, sizeof(double));
					data_file.read((char*)&tmp.omega, sizeof(double));
					data_file.read((char*)&tmp.ro, sizeof(double));
					data_file.read((char*)&tmp.ht, sizeof(double));
					data_file.read((char*)&tmp.x, sizeof(double));
					data_file.read((char*)&tmp.y, sizeof(double));
					data_file.read((char*)&tmp.z, sizeof(double));*/
					data_file.read((char*)&tmp, sizeof(tmp));
					this->Y.push_back(tmp);
					data_file.read((char*)&this->X[counter][0], sizeof(double));
					data_file.read((char*)&this->X[counter][1], sizeof(double));
					data_file.read((char*)&this->X[counter][2], sizeof(double));
					counter++;
					//this->nodes[i][k][j] = tmp;
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

const int Grid::size() {
	return this->IMAX * this->JMAX * this->KMAX;
}

/*node Grid::get_node(int i, int j, int k) {
	return	this->nodes[i][k][j];
}*/
