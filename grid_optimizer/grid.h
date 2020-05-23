#pragma once

#include "KDTree/alglibmisc.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>


typedef std::vector<size_t> index;


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
	void read_file(int IMAX, int JMAX, int KMAX, std::string filename);
	std::pair<alglib::real_2d_array, alglib::integer_1d_array> get_X_tags();
	index get_ijk();
	index get_ijk(int n);
	node get_node(int i, int j, int k);
	alglib::real_1d_array get_xyz(int i, int j, int k);
private:
	size_t IMAX;
	size_t JMAX;
	size_t KMAX;
	std::vector<std::vector<std::vector<node>>> values;
};

