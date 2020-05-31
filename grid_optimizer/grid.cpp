#include "grid.h"

Grid::Grid() {
	this->IMAX = 0;
	this->IMAX = 0;
	this->KMAX = 0;
}

void Grid::read_file(const int IMAX, const int JMAX, const int KMAX, const std::string filename) {
	std::ifstream data_file;
	std::string path = "../data/" + filename;

	this->IMAX = IMAX;
	this->JMAX = JMAX;
	this->KMAX = KMAX;
	this->source = filename;

	this->values.resize(IMAX);
	this->values_tan.resize(IMAX);
	for (int i = 0; i < IMAX; i++) {
		this->values[i].resize(KMAX);
		this->values_tan[i].resize(KMAX);
		for (int k = 0; k < KMAX; k++)
			this->values[i][k].resize(JMAX);
	}

	data_file.open(path, std::ios::in | std::ios::binary);
	if (data_file.is_open()) {
		for (int i = 0; i < IMAX; i++)  //считывание из файла
			for (int k = 0; k < KMAX; k++) {

				for (int j = 0; j < JMAX; j++)
					data_file.read((char*)&this->values[i][k][j], sizeof(std::array<double, 12>));

				// qw и tauw,
				data_file.read((char*)&this->values_tan[i][k], sizeof(std::array<double, 2>));
			}
		// close the opened file.
		data_file.close();
	}
	else
		std::cout << "ERROR! File '" << filename << "' not found." << std::endl;
}

std::pair<alglib::real_2d_array, alglib::integer_1d_array> Grid::get_X_tags(int b) {
	alglib::real_2d_array X;
	alglib::integer_1d_array tags;
	int counter = 0, jmax = (1 + (this->JMAX - 1) * b);
		X.setlength(this->IMAX * jmax * this->KMAX, 3);
		tags.setlength(this->IMAX * jmax * this->KMAX);

	for (int i = 0; i < this->IMAX; i++)
		for (int k = 0; k < this->KMAX; k++)
			for (int j = 0; j < jmax; j++) {
				X[counter][0] = this->values[i][k][j][9];
				X[counter][1] = this->values[i][k][j][10];
				X[counter][2] = this->values[i][k][j][11];
				tags[counter] = counter;
				counter++;
			}
	return std::make_pair(X, tags);
}

index Grid::get_ijk() {
	index ijk = { this->IMAX, this->JMAX, this->KMAX };
	return ijk;
}

index Grid::get_ijk(const int & n, int b) {
	int jmax = (1 + (this->JMAX - 1) * b);
	int n_ = n % (this->KMAX * jmax);

	index ijk = { n / (this->KMAX * jmax), n_ % jmax, n_ / jmax };
	return ijk;
}

std::array<double, 12> Grid::get_node(const int & i, const int & j, const int & k) {
	return this->values[i][j][k];
}

std::array<double, 2> Grid::get_node_tau(const int & i, const int & k) {
	return this->values_tan[i][k];
}

void Grid::set_node(const int & i, const int & j, const int & k, std::array<double, 12> p) {
	this->values[i][j][k] = p;
}

void Grid::set_node_tau(const int & i, const int & k, std::array<double, 2> p) {
	this->values_tan[i][k] = p;
}

alglib::real_1d_array Grid::get_xyz(const int & i, const int & j, const int & k) {
	alglib::real_1d_array t;
	t.setlength(3);

	t[0] = this->values[i][j][k][9];
	t[1] = this->values[i][j][k][10];
	t[2] = this->values[i][j][k][11];

	return t;
}

void Grid::write_file() {
	std::ofstream data_file;
	std::string path = "../data/" + this->source.substr(0, this->source.find(".")) + "_res.dat";

	data_file.open(path, std::ios::out | std::ios::binary);
	if (data_file.is_open()) {
		for (int i = 0; i < this->IMAX; i++)
			for (int k = 0; k < this->KMAX; k++) {
				for (int j = 0; j < this->JMAX; j++)
					data_file.write((char*)&this->values[i][k][j], sizeof(std::array<double, 12>));
				data_file.write((char*)&this->values_tan[i][k], sizeof(std::array<double, 2>));
			}
		// close the opened file.
		data_file.close();
	}
	else
		std::cout << "ERROR! File '" << this->source << "' is not created." << std::endl;
}
