#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "alglibmisc.h"

using namespace alglib;


int main(int argc, char **argv)
{
	real_2d_array a = "[[0,0],[0,1],[1,0],[1,1]]";
	ae_int_t nx = 2;
	ae_int_t ny = 0;
	ae_int_t normtype = 2;
	kdtree kdt;
	real_1d_array x;
	real_2d_array r = "[[]]";
	ae_int_t k;
	kdtreebuild(a, nx, ny, normtype, kdt);
	x = "[-1,0]";
	//ae_int_t k = 4;
	k = kdtreequeryknn(kdt, x, 1);
	printf("%d\n", int(k)); // EXPECTED: 1
	kdtreequeryresultsx(kdt, r);
	printf("%s\n", r.tostring(1).c_str()); // EXPECTED: [[0,0]]
	real_2d_array r1 = "[[]]";
	real_1d_array BoxMin = "[-0.5,-0.5]",
		BoxMax = "[1.5,1.5]";
	kdtreequerybox(kdt, BoxMin, BoxMax);
	kdtreequeryresultsx(kdt, r1);
	printf("%s\n", r1.tostring(1).c_str()); // EXPECTED: [[0,0]]
	return 0;
}