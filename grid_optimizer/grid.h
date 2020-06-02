#pragma once

#include "KDTree/alglibmisc.h"
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <iostream>


typedef std::vector<int> index;


/*struct std::array<double, 12> {
	double p;  //давление
	double u, v, w; //декартовы компоненты скорости 
	double t; //температура
	double k; //кинетическая энергия турбулентности
	double omega; //диссипация кинетической энергии турбулентности
	double ro; // плотность
	double ht; // полная энтальпия газа
	double x, y, z; //декартовы координаты центров масс ячеек сетки
};*/


class Grid {
public:
	Grid();
	void read_file(const int IMAX, const int JMAX, const int KMAX, const std::string filename);
	std::pair<alglib::real_2d_array, alglib::integer_1d_array> get_X_tags(int j);
	index get_ijk();
	index get_ijk(const int & n, int b);
	std::array<double, 12> get_node(const int & i, const int & j, const int & k);
	std::array<double, 2> get_node_tau(const int & i, const int & k);
	void set_node(const int & i, const int & j, const int & k, std::array<double, 12> p);
	void set_node_tau(const int & i, const int & k, std::array<double, 2> p);
	alglib::real_1d_array get_xyz(const int & i, const int & j, const int & k);
	void write_file();
	void to_tecplot();
private:
	std::string source;
	int IMAX;
	int JMAX;
	int KMAX;
	std::vector<std::vector<std::vector<std::array<double, 12>>>> values;
	std::vector<std::vector<std::array<double, 2>>> values_tan;
};

