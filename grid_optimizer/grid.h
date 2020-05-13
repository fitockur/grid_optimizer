#pragma once

#include "KDTree/alglibmisc.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>


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
	Grid();
	void read_file(int IMAX, int JMAX, int KMAX, std::string filename, int t=0);
	alglib::real_2d_array get_X();
private:
	int IMAX;
	int JMAX;
	int KMAX;
	alglib::real_2d_array X; // coords
	std::vector<node> Y; // values
};

