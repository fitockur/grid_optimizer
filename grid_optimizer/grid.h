#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "KDTree/alglibmisc.h"


struct node
{
	double p;  //давление
	double u, v, w; //декартовы компоненты скорости 
	double t; //температура
	double k; //кинетическая энергия турбулентности
	double omega; //диссипация кинетической энергии турбулентности
	double ro; // плотность
	double ht; // полная энтальпия газа
	//double x, y, z; //декартовы координаты центров масс ячеек сетки
};


class Grid
{
public:
	Grid(int IMAX, int JMAX, int KMAX);
	void read_file(std::string filename);
	alglib::real_2d_array get_X();
	const int size();
	//node get_node(int i, int j, int k);
private:
	int IMAX;
	int JMAX;
	int KMAX;
	alglib::real_2d_array X; // coords
	std::vector<node> Y; // values
};

