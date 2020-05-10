#include "grid.h"

Grid::Grid(int IMAX, int JMAX, int KMAX) 
	: IMAX(IMAX), JMAX(JMAX), KMAX(KMAX) {}

void Grid::read_file(std::string filename) {
	std::ifstream data_file;
	node tmp;
	double trash;
	std::string path = "../data/" + filename;
	
	this->nodes.resize(this->IMAX);
	data_file.open(path, std::ios::in | std::ios::binary);
	for (int i = 0; i < this->IMAX; i++)
	{
		this->nodes[i].resize(this->KMAX);
		for (int k = 0; k < this->KMAX; k++)
			this->nodes[i][k].resize(this->JMAX);
	}

	if (data_file.is_open())
	{
		for (int i = 0; i < IMAX; i++)  //считывание из файла
			for (int k = 0; k < KMAX; k++) {
				for (int j = 0; j < JMAX; j++) {
					data_file.read((char*)&tmp.p, sizeof(double));
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
					data_file.read((char*)&tmp.z, sizeof(double));

					this->nodes[i][k][j] = tmp;
				}
				// ненужные нам пока qw и tauw, с ними отдельно,
				// когда все сойдется, чтоб зря с собой не таскать
				data_file.read((char*)&trash, sizeof(double));
				data_file.read((char*)&trash, sizeof(double));
			}
		// close the opened file.
		data_file.close();
	}
	else
		std::cout << "ERROR! File '" << filename << "' not found." << std::endl;
}

node Grid::get_node(int i, int j, int k) {
	return	this->nodes[i][k][j];
}
