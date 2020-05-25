#pragma once

#include "KDTree/alglibmisc.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>


typedef std::vector<int> index;


struct node {
	double p;  //давление
	double u, v, w; //декартовы компоненты скорости 
	double t; //температура
	double k; //кинетическая энергия турбулентности
	double omega; //диссипация кинетической энергии турбулентности
	double ro; // плотность
	double ht; // полная энтальпия газа
	double x, y, z; //декартовы координаты центров масс ячеек сетки
};


class Grid {
public:
	Grid();
	void read_file(const int IMAX, const int JMAX, const int KMAX, const std::string filename);
	std::pair<alglib::real_2d_array, alglib::integer_1d_array> get_X_tags();
	index get_ijk();
	index get_ijk(const int & n);
	node get_node(const int & i, const int & j, const int & k);
	void set_node(const int & i, const int & j, const int & k, node p);
	alglib::real_1d_array get_xyz(const int & i, const int & j, const int & k);
private:
	int IMAX;
	int JMAX;
	int KMAX;
	std::vector<std::vector<std::vector<node>>> values;
};

