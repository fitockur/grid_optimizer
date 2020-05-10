#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>


struct node
{
	double p;  //давление
	double u, v, w; //декартовы компоненты скорости 
	double t; //температура
	double k; //кинетическа€ энерги€ турбулентности
	double omega; //диссипаци€ кинетической энергии турбулентности
	double ro; // плотность
	double ht; // полна€ энтальпи€ газа
	double x, y, z; //декартовы координаты центров масс €чеек сетки
	int type;
};


class Grid
{
public:
	Grid(int IMAX, int JMAX, int KMAX);
	void read_file(std::string filename);
private:
	int IMAX;
	int JMAX;
	int KMAX;
	std::vector<std::vector<std::vector<node>>> nodes;
};

