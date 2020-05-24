﻿#include "grid.h"

Grid::Grid() {
	this->IMAX = 0;
	this->IMAX = 0;
	this->KMAX = 0;
}

void Grid::read_file(const size_t IMAX, const size_t JMAX, const size_t KMAX, const std::string filename) {
	std::ifstream data_file;
	std::string path = "../data/" + filename;
	double buf;

	this->IMAX = IMAX;
	this->JMAX = JMAX;
	this->KMAX = KMAX;

	this->values.resize(IMAX);
	for (size_t i = 0; i < IMAX; i++) {
		this->values[i].resize(KMAX);
		for (size_t k = 0; k < KMAX; k++)
			this->values[i][k].resize(JMAX);
	}

	data_file.open(path, std::ios::in | std::ios::binary);
	if (data_file.is_open()) {
		for (size_t i = 0; i < IMAX; i++)  //считывание из файла
			for (size_t k = 0; k < KMAX; k++) {

				for (size_t j = 0; j < JMAX; j++)
					data_file.read((char*)&this->values[i][k][j], sizeof(node));

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

std::pair<alglib::real_2d_array, alglib::integer_1d_array> Grid::get_X_tags() {
	alglib::real_2d_array X;
	alglib::integer_1d_array tags;
	int counter = 0;
	X.setlength(this->IMAX * this->JMAX * this->KMAX, 3);
	tags.setlength(this->IMAX * this->JMAX * this->KMAX);

	for (size_t i = 0; i < this->IMAX; i++)
		for (size_t k = 0; k < this->KMAX; k++)
			for (size_t j = 0; j < this->JMAX; j++) {
				X[counter][0] = this->values[i][k][j].x;
				X[counter][1] = this->values[i][k][j].y;
				X[counter][2] = this->values[i][k][j].z;
				tags[counter] = counter;
				counter++;
			}

	return std::make_pair(X, tags);
}

index Grid::get_ijk() {
	index ijk = { this->IMAX, this->JMAX, this->KMAX };
	return ijk;
}

index Grid::get_ijk(const size_t & n) {
	int n_ = n % (this->KMAX * this->JMAX);

	index ijk = { n / (this->KMAX * this->JMAX), n_ % this->JMAX, n_ / this->JMAX };
	return ijk;
}

node Grid::get_node(const size_t & i, const size_t & j, const size_t & k) {
	return this->values[i][j][k];
}

void Grid::set_node(const size_t & i, const size_t & j, const size_t & k, node p) {
	this->values[i][k][j] = p;
}

alglib::real_1d_array Grid::get_xyz(const size_t & i, const size_t & j, const size_t & k) {
	alglib::real_1d_array t;
	t.setlength(3);

	t[0] = this->values[i][j][k].x;
	t[1] = this->values[i][j][k].y;
	t[2] = this->values[i][j][k].z;

	return t;
}
