#include "Nearest3DInterpolator.h"


Nearest3DInterpolator::Nearest3DInterpolator(int rough_i, int rough_j, int rough_k, std::string rough_source,
	int detailed_i, int detailed_j, int detailed_k, std::string detailed_source) {
	// read rough grid
	this->rough_grid.read_file(rough_i, rough_j, rough_k, rough_source);
	// read detailed grid
	this->detailed_grid.read_file(detailed_i, detailed_j, detailed_k, detailed_source);
	// make tags for kdtree
	std::pair<alglib::real_2d_array, alglib::integer_1d_array> X_tags(this->rough_grid.get_X_tags(1));
	// make kdtree
	alglib::kdtreebuildtagged(X_tags.first, X_tags.second, 3, 0, 2, this->rough_grid_tree);
	// make tags for kdtree_b
	std::pair<alglib::real_2d_array, alglib::integer_1d_array> X_tags_b(this->rough_grid.get_X_tags(0));
	// make kdtree_b
	alglib::kdtreebuildtagged(X_tags_b.first, X_tags_b.second, 3, 0, 2, this->rough_grid_tree_b);
}

void Nearest3DInterpolator::project() {
	index ijk = this->detailed_grid.get_ijk();
	alglib::integer_1d_array tag, tag_b;
	index ijk_c;
	index ijk_v;
	progresscpp::ProgressBar progressBar(ijk[0], 50);
	// iterate over detailed points
	for (int i = 0; i < ijk[0]; i++) {
		for (int k = 0; k < ijk[2]; k++) {
			// body
			alglib::kdtreequeryknn(this->rough_grid_tree_b, this->detailed_grid.get_xyz(i, k, 0), 1);
			alglib::kdtreequeryresultstags(this->rough_grid_tree_b, tag_b);
			// get nearest node's ijk
			ijk_v = this->rough_grid.get_ijk(tag_b[0], 0);
			ijk_c = { i, 0, k };
			// location and interpolation
			this->locate_b(ijk_c, ijk_v);
			for (int j = 1; j < ijk[1]; j++) {
				// find nearest node and get it's tag
				alglib::kdtreequeryknn(this->rough_grid_tree, this->detailed_grid.get_xyz(i, k, j), 1); 
				alglib::kdtreequeryresultstags(this->rough_grid_tree, tag);
				// get nearest node's ijk
				ijk_v = this->rough_grid.get_ijk(tag[0], 1);
				ijk_c = {i, j, k};
				// location and interpolation
				this->locate(ijk_c, ijk_v);
			}
		}
		++progressBar;
		progressBar.display(i, this->locate_cnt, this->interpolate_cnt);
	}
	progressBar.done();
	this->detailed_grid.write_file();
}

bool Nearest3DInterpolator::hex_exists(const index & ijk_vc) {
	// check if indices belong to the domain
	index ijk_max = this->rough_grid.get_ijk();
	for (int i = 0; i < ijk_vc.size(); i++)
		if ((ijk_vc[i] < 0) || (ijk_vc[i] + 1 >= ijk_max[i]))
			return false;
	return true;
}

bool Nearest3DInterpolator::plane_exists(const std::vector<index> & ijk_vc) {
	// check if indices belong to the domain
	index ijk_max = this->rough_grid.get_ijk();
	for (size_t i = 1; i < 3; i++)
		for (size_t j = 0; j < 3; j += 2)
			if ((ijk_vc[i][j] < 0) || (ijk_vc[i][j] >= ijk_max[j]))
				return false;
	return true;
}

bool Nearest3DInterpolator::spatial_convex_inclusion(const index & ijk_c, const index & ijk_vc) {
	// check if point of detailed grid belong to the current hexahedron
	std::vector<index> steps = {
		{1, 0, 0, 1},
		{0, 1, 1, 0},
	};
	index df_tmp, da, db;
	alglib::real_1d_array op, ap, bp, point;
	point = this->detailed_grid.get_xyz(ijk_c[0], ijk_c[2], ijk_c[1]);
	//point = this->get_test_point(ijk_vc);
	// iterate over main points (boxmin and boxmax)
	for (const auto &step : steps) {
		df_tmp = { step[0], step[1], step[2] };
		// iterate over main points' planes (every main point has 3 planes)
		for (int _ = 0; _ < 3; _++) {
			// get coords of main point (O)
			if (step[3])
				op = this->rough_grid.get_xyz(ijk_vc[0], ijk_vc[2], ijk_vc[1]);
			else
				op = this->rough_grid.get_xyz(ijk_vc[0] + 1, ijk_vc[2] + 1, ijk_vc[1] + 1);
			// get coords of A point
			ap = this->rough_grid.get_xyz(ijk_vc[0] + df_tmp[0],
				ijk_vc[2] + df_tmp[2], ijk_vc[1] + df_tmp[1]);
			this->shift(df_tmp, step[3]);
			// get coords of B point
			bp = this->rough_grid.get_xyz(ijk_vc[0] + df_tmp[0],
				ijk_vc[2] + df_tmp[2], ijk_vc[1] + df_tmp[1]);
			for (size_t i = 0; i < 3; i++) {
				ap[i] -= op[i];
				bp[i] -= op[i];
				op[i] = point[i] - op[i];
			}
			// check location relative to the current plane
			//std::cout << "O: " << op.tostring(6) << std::endl;
			//std::cout << "A: " << ap.tostring(6) << std::endl;
			//std::cout << "B: " << bp.tostring(6) << std::endl;
			//std::cout << "P: " << point.tostring(6) << std::endl;
			if (this->plane_position(ap, bp, op) >= 0)
				return false;
		}
	}
	return true;
}

bool Nearest3DInterpolator::plane_inclusion(const index & ijk_c, const std::vector<index> & ijk_vc, alglib::real_1d_array & xyz_pr) {
	alglib::real_1d_array p[3], point, N, v[3], v11, OP;
	double t;
	point = this->detailed_grid.get_xyz(ijk_c[0], ijk_c[2], ijk_c[1]);
	for (size_t i = 0; i < 3; i++) {
		p[i] = this->rough_grid.get_xyz(ijk_vc[i][0], ijk_vc[i][2], ijk_vc[i][1]);
		v[i].setlength(3);
	}
	// vectors
	v11.setlength(3);
	for (size_t i = 0; i < 3; i++)	{
		v[0][i] = p[1][i] - p[0][i];
		v[1][i] = p[2][i] - p[1][i];
		v[2][i] = p[0][i] - p[2][i];
		v11[i] = -v[2][i];
	}
	// normal
	N.setlength(3);
	this->cross_product(v[0], v11, N);

	// for
	OP.setlength(3);
	for (size_t i = 0; i < 3; i++)	{
		for (size_t j = 0; j < 3; j++)
			OP[j] = point[j] - p[i][j];
		if (this->plane_position(v[i], N, OP) >= 0)
			return false;
	}

	t = (this->dot_product(N, p[0]) - this->dot_product(N, point)) / this->dot_product(N, N);
	xyz_pr[0] = point[0] + N[0] * t;
	xyz_pr[1] = point[1] + N[1] * t;
	xyz_pr[2] = point[2] + N[2] * t;

	return true;
}

void Nearest3DInterpolator::locate(const index & ijk, const index & ijk_v) {
	index ijk_vc;
	ijk_vc.resize(3);
	std::vector<index> vertices;
	bool finish = false;
	// iterate over 8 hexahedrons
	for (int di = 0; di > -2 && !finish; di--) {
		for (int dk = 0; dk > -2 && !finish; dk--) {
			for (int dj = 0; dj > -2 && !finish; dj--) {
				ijk_vc[0] = ijk_v[0] + di;
				ijk_vc[1] = ijk_v[1] + dj;
				ijk_vc[2] = ijk_v[2] + dk;
				this->locate_cnt++;
				// check existing and spatial convex inclusion of current hexahedron
				if (this->hex_exists(ijk_vc) && this->spatial_convex_inclusion(ijk, ijk_vc)) {
					// get vertices of current hexahedron
					vertices = this->get_vertices(ijk_vc);
					// interpolate values in point of detailed grid
					this->interpolate(ijk, vertices);
					this->interpolate_cnt++;
					// stop if at least once the point is interpolated
					finish = true;
				}
			}
		}
	}
}

void Nearest3DInterpolator::locate_b(const index & ijk, const index & ijk_v) {
	std::vector< std::vector<index>> ijk_vc = {
		{
			{ijk_v[0], ijk_v[1], ijk_v[2]},
			{ijk_v[0] - 1, ijk_v[1], ijk_v[2]},
			{ijk_v[0], ijk_v[1], ijk_v[2] + 1},
		},
		{
			{ijk_v[0], ijk_v[1], ijk_v[2]},
			{ijk_v[0], ijk_v[1], ijk_v[2] + 1},
			{ijk_v[0] + 1, ijk_v[1], ijk_v[2]},
		},
		{
			{ijk_v[0], ijk_v[1], ijk_v[2]},
			{ijk_v[0] + 1, ijk_v[1], ijk_v[2]},
			{ijk_v[0], ijk_v[1], ijk_v[2] - 1},
		},
		{
			{ijk_v[0], ijk_v[1], ijk_v[2]},
			{ijk_v[0], ijk_v[1], ijk_v[2] - 1},
			{ijk_v[0] - 1, ijk_v[1], ijk_v[2]},
		},
	};
	alglib::real_1d_array xyz_pr;
	xyz_pr.setlength(3);
	for (const auto &ijk_vc_i : ijk_vc) {
		this->locate_cnt++;
		// check existing and spatial convex inclusion of current hexahedron
		if (this->plane_exists(ijk_vc_i) && this->plane_inclusion(ijk, ijk_vc_i, xyz_pr)) {
			// interpolate values in point of detailed grid
			this->interpolate_b(ijk, ijk_vc_i, xyz_pr);
			this->interpolate_cnt++;
			// stop if at least once the point is interpolated
			break;
		}
	}
}

std::vector<index> Nearest3DInterpolator::get_vertices(const index & ijk_vc) {
	// get 8 vertices of current hexahedron
	std::vector<index> vertices;
	index vertice;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++) {
				vertice = { ijk_vc[0] + i, ijk_vc[1] + j, ijk_vc[2] + k };
				vertices.push_back(vertice);
			}
	return vertices;
}

void Nearest3DInterpolator::interpolate(const index & ijk, const std::vector<index>& vertices) {
	double denominator = 0;
	std::array<double, 9> numerator;
	std::array<double, 12> tmp,
		point = this->detailed_grid.get_node(ijk[0], ijk[2], ijk[1]);
	double d, w;
	for (const auto &vertice : vertices) {
		tmp = this->rough_grid.get_node(vertice[0], vertice[2], vertice[1]);
		d = sqrt(pow((tmp[9] - point[9]), 2) + pow((tmp[10] - point[10]), 2) + pow((tmp[11] - point[11]), 2));
		if (d > 0) {
			w = 1.0 / (d * d);
			denominator += w;
			for (size_t i = 0; i < 9; i++)
				numerator[i] += tmp[i] * w;
		}
		else {
			for (size_t i = 0; i < 9; i++)
				numerator[i] = tmp[i];
			this->detailed_grid.set_node(ijk[0], ijk[2], ijk[1], point);
			return;
		}
	}
	for (size_t i = 0; i < 9; i++)
		point[i] = numerator[i] / denominator;
	this->detailed_grid.set_node(ijk[0], ijk[2], ijk[1], point);
}

void Nearest3DInterpolator::interpolate_b(const index & ijk, const std::vector<index>& vertices, const alglib::real_1d_array & xyz_pr) {
	double denominator = 0;
	std::array<double, 11> numerator;
	std::array<double, 12> tmp,
		point = this->detailed_grid.get_node(ijk[0], ijk[2], ijk[1]);
	std::array<double, 2> tmp_tau,
		point_tau = this->detailed_grid.get_node_tau(ijk[0], ijk[2]);
	double d, w;
	for (const auto &vertice : vertices) {
		tmp = this->rough_grid.get_node(vertice[0], vertice[2], vertice[1]);
		tmp_tau = this->rough_grid.get_node_tau(vertice[0], vertice[2]);
		d = sqrt(pow((tmp[9] - xyz_pr[0]), 2) + pow((tmp[10] - xyz_pr[1]), 2) + pow((tmp[11] - xyz_pr[2]), 2));
		if (d > 0) {
			w = 1.0 / (d * d);
			denominator += w;
			for (size_t i = 0; i < 9; i++)
				numerator[i] += tmp[i] * w;
			numerator[9] += tmp_tau[0] * w;
			numerator[10] += tmp_tau[1] * w;
		}
		else {
			for (size_t i = 0; i < 9; i++)
				point[i] = tmp[i];
			point_tau[0] = tmp_tau[0];
			point_tau[1] = tmp_tau[1];
			this->detailed_grid.set_node(ijk[0], ijk[2], ijk[1], point);
			this->detailed_grid.set_node_tau(ijk[0], ijk[2], point_tau);
			return;
		}
	}
	for (size_t i = 0; i < 9; i++)
		point[i] = numerator[i] / denominator;
	point_tau[0] = numerator[9] / denominator;
	point_tau[1] = numerator[10] / denominator;
	this->detailed_grid.set_node(ijk[0], ijk[2], ijk[1], point);
	this->detailed_grid.set_node_tau(ijk[0], ijk[2], point_tau);
}

void Nearest3DInterpolator::shift(index & ijk, const bool & left) {
	// cyclic shift
	int tmp;
	// to left
	if (left) {
		tmp = ijk[0];
		ijk[0] = ijk[1];
		ijk[1] = ijk[2];
		ijk[2] = tmp;
	}
	// to right
	else {
		tmp = ijk[2];
		ijk[2] = ijk[1];
		ijk[1] = ijk[0];
		ijk[0] = tmp;
	}
}

double Nearest3DInterpolator::plane_position(const alglib::real_1d_array & OA, 
	const alglib::real_1d_array & OB, const alglib::real_1d_array & OP) {
	// calculate normal to plane
	alglib::real_1d_array N;
	N.setlength(3);
	this->cross_product(OA, OB, N);

	// and then dot product with OP
	return  this->dot_product(N, OP);
}

alglib::real_1d_array Nearest3DInterpolator::get_test_point(const index & ijk_vc) {
	alglib::real_1d_array point = "[0.0, 0.0, 0.0]", tmp;
	for (int i = 0; i < 2; i++)
		for (int k = 0; k < 2; k++)
			for (int j = 0; j < 2; j++) {
				tmp = this->rough_grid.get_xyz(ijk_vc[0] + i, ijk_vc[2] + k, ijk_vc[1] + j);
				for (int m = 0; m < 3; m++)
					point[m] += tmp[m];
			}
	for (int m = 0; m < 3; m++)
		point[m] /= 8;
	return point;
}

void Nearest3DInterpolator::cross_product(const alglib::real_1d_array & A, const alglib::real_1d_array & B, alglib::real_1d_array & C) {
	C[0] = A[1] * B[2] - A[2] * B[1];
	C[1] = A[2] * B[0] - A[0] * B[2];
	C[2] = A[0] * B[1] - A[1] * B[0];
}

double Nearest3DInterpolator::dot_product(const alglib::real_1d_array & A, const alglib::real_1d_array & B) {
	return A[0] * B[0] + A[1] * B[1] + A[2] * B[2];
}
