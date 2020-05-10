#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>


struct node
{
	double p;  //��������
	double u, v, w; //��������� ���������� �������� 
	double t; //�����������
	double k; //������������ ������� ��������������
	double omega; //���������� ������������ ������� ��������������
	double ro; // ���������
	double ht; // ������ ��������� ����
	double x, y, z; //��������� ���������� ������� ���� ����� �����
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

