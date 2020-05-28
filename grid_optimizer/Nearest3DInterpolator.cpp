#include "Nearest3DInterpolator.h"


Nearest3DInterpolator::Nearest3DInterpolator(int rough_i, int rough_j, int rough_k, std::string rough_source,
	int detailed_i, int detailed_j, int detailed_k, std::string detailed_source) {
	// read rough grid
	this->rough_grid.read_file(rough_i, rough_j, rough_k, rough_source);
	// read detailed grid
	this->detailed_grid.read_file(detailed_i, detailed_j, detailed_k, detailed_source);
	// make tags for kdtree
	std::pair<alglib::real_2d_array, alglib::integer_1d_array> X_tags(this->rough_grid.get_X_tags());
	// make kdtree
	alglib::kdtreebuildtagged(X_tags.first, X_tags.second, 3, 0, 2, this->rough_grid_tree);
}

void Nearest3DInterpolator::project() {
	index ijk = this->detailed_grid.get_ijk();
	alglib::integer_1d_array tag;
	index ijk_c;
	index ijk_v;
	progresscpp::ProgressBar progressBar(ijk[0], 70);
	//auto start_0 = std::chrono::high_resolution_clock::now();
	// iterate over detailed points
	for (int i = 0; i < ijk[0]; i++) {
		//auto start_i = std::chrono::high_resolution_clock::now();
		for (int k = 0; k < ijk[2]; k++) {
			for (int j = 1; j < ijk[1]; j++) {
				// find nearest node and get it's tag
				alglib::kdtreequeryknn(this->rough_grid_tree, this->detailed_grid.get_xyz(i, k, j), 1);
				alglib::kdtreequeryresultstags(this->rough_grid_tree, tag);
				// get nearest node's ijk
				ijk_v = this->rough_grid.get_ijk(tag[0]);
				//std::cout << tag.tostring() << std::endl;
				ijk_c = {i, j, k};
				//std::cout << ijk_c[0] << ' ' << ijk_c[1] << ' ' << ijk_c[2] << std::endl;
				// location and interpolation
				this->locate(ijk_c, ijk_v);
			}
			//std::cout << "\t\tk = " << k << " n_location = " << this->locate_cnt <<
				//" n_interpolation = " << this->interpolate_cnt << std::endl;
		}
		//auto stop_i = std::chrono::high_resolution_clock::now();
		//auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop_i - start_i);
		//system("cls");
		//std::cout << "I = " << i << "\n\tn_location = " << this->locate_cnt <<
			//"\n\tn_interpolation = " << this->interpolate_cnt << std::endl <<
			//"\nTime taken by current iteration: " << duration.count() << " seconds" << std::endl <<
			//"\nRemaining runtime of approximately: " << duration.count() * (ijk[0] - i) <<
			//" seconds" << std::endl;
		++progressBar;
		progressBar.display();
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
			// check location relative to the current plane
			//std::cout << "O: " << op.tostring(6) << std::endl;
			//std::cout << "A: " << ap.tostring(6) << std::endl;
			//std::cout << "B: " << bp.tostring(6) << std::endl;
			//std::cout << "P: " << point.tostring(6) << std::endl;
			if (this->plane_position(op, bp, ap, point) >= 0)
				return false;
		}
	}
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
	double numerator_p = 0,
		denominator = 0,
		numerator_u = 0,
		numerator_v = 0,
		numerator_w = 0,
		numerator_t = 0,
		numerator_omega = 0,
		numerator_ro = 0,
		numerator_ht = 0;
	node tmp,
		point = this->detailed_grid.get_node(ijk[0], ijk[2], ijk[1]);
	double d, w;
	for (const auto &vertice : vertices) {
		tmp = this->rough_grid.get_node(vertice[0], vertice[2], vertice[1]);
		d = sqrt(pow((tmp.x - point.x), 2) + pow((tmp.y - point.y), 2) + pow((tmp.z - point.z), 2));
		if (d > 0) {
			w = 1.0 / (d * d);
			numerator_p += tmp.p * w;
			numerator_u += tmp.u * w;
			numerator_v += tmp.v * w;
			numerator_w += tmp.w * w;
			numerator_t += tmp.t * w;
			numerator_omega += tmp.omega * w;
			numerator_ro += tmp.ro * w;
			numerator_ht += tmp.ht * w;
			denominator += w;
		}
		else {
			point.p = tmp.p;
			point.u = tmp.u;
			point.v = tmp.v;
			point.w = tmp.w;
			point.t = tmp.t;
			point.omega = tmp.omega;
			point.ro = tmp.ro;
			point.ht = tmp.ht;
			this->detailed_grid.set_node(ijk[0], ijk[2], ijk[1], point);
			return;
		}
	}
	point.p = numerator_p / denominator;
	point.u = numerator_u / denominator;
	point.v = numerator_v / denominator;
	point.w = numerator_w / denominator;
	point.t = numerator_t / denominator;
	point.omega = numerator_omega / denominator;
	point.ro = numerator_ro / denominator;
	point.ht = numerator_ht / denominator;
	this->detailed_grid.set_node(ijk[0], ijk[2], ijk[1], point);
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

double Nearest3DInterpolator::plane_position(const alglib::real_1d_array & o, 
	const alglib::real_1d_array & a, const alglib::real_1d_array & b, 
	const alglib::real_1d_array & point) {
	// calculate normal to plane
	// and then dot product with OP
	return ((a[1] - o[1]) * (b[2] - o[2]) - (a[2] - o[2]) * (b[1] - o[1])) * (point[0] - o[0]) +
		((a[2] - o[2]) * (b[0] - o[0]) - (a[0] - o[0]) * (b[2] - o[2])) * (point[1] - o[1]) +
		((a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0])) * (point[2] - o[2]);
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
