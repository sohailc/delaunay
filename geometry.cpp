#include <cstdarg>
#include <iostream>
#include <vector>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <cmath>
#include <float.h>
#include <assert.h>

#include "linearLeastSquares.h"
#include "geometry.hpp"

using namespace std;

Vertex Vertex::element_wise_operation(Vertex a, Vertex b, double (op)(double, double)){
	vector<double> result;
	vector<double> :: iterator i, j;
	for ( i=a.p.begin(), j=b.p.begin(); i<a.p.end(), j<b.p.end(); i++, j++){
		result.push_back(op(*i, *j));
	}

   return Vertex(result);
}

Vertex Vertex::element_wise_operation(Vertex a, double b, double (op)(double, double)){
	vector<double> result;
	vector<double> :: iterator i, j;
	
	for (j=a.p.begin(); j<a.p.end(); j++){
		result.push_back(op(b, *j));
	}
	return Vertex(result);
}


Vertex Vertex::operator-(Vertex a){
	return element_wise_operation(p, a, [](double i, double j){return i - j;});
}

Vertex Vertex::operator+(Vertex a){
	return element_wise_operation(p, a, [](double i, double j){return i + j;});
}
        
Vertex Vertex::operator-(double a){
	return element_wise_operation(p, a, [](double i, double j){return i - j;});
}

Vertex Vertex::operator+(double a){
	return element_wise_operation(p, a, [](double i, double j){return i + j;});
}
        
Vertex Vertex::operator/(double a){
    return element_wise_operation(p, a, [](double i, double j){return j / i;});
}

Vertex Vertex::operator*(double a){
	return element_wise_operation(p, a, [](double i, double j){return i * j;});
}

Vertex::Vertex(vector<double> a){
	p = a;
	ndim = p.size();
}

double Vertex::at(int i){
	return p.at(i);
}

Vertex::Vertex(int ndim_, ...){
	double ai;
	int i = 0;
	va_list arguments;
	va_start ( arguments, ndim_ );
	for (i = 0; i<ndim_; i++ ){
		ai = va_arg ( arguments, double );
		p.push_back(ai);
	}
	va_end ( arguments );
	ndim = ndim_;
}

double Vertex :: norm(){
    // distance to the origin
    double d_square = 0;
    for (auto i=this->p.begin(); i<this->p.end(); ++i){
        d_square += (*i) * (*i);
    }
    return sqrt(d_square);
}

vector<double> Vertex :: coordinates(){
    return this->p;
}

void Vertex :: set_index(int index){
    this->index = index;
}

int Vertex :: get_index(){
    return this->index;
}

void Vertex::print(){
	vector<double> :: iterator i;
	for (i=p.begin(); i<p.end(); i++){
		cout << *i << " ,";
	}
	cout << endl;
}


int Matrix::allocate_memory(){
	NEW_MAT(data);
	return EXIT_SUCCESS;
}

Matrix::Matrix(vector<Vertex> columns){
	n_columns = columns.size();
	n_rows = columns[0].ndim;
	data.m = n_rows;
	data.n = n_columns;
	if (allocate_memory() == EXIT_SUCCESS){
		for (int i=0; i<n_columns; i++){
			for(int j=0; j<n_rows; j++){
				double value = columns[i].at(j);
				EL(data, j, i) = value;
			}
		}
		initialized = true;
	}
}

Matrix::Matrix(vector<vector<double> > columns){
    n_columns = columns.size();
    n_rows = columns[0].size();
    data.m = n_rows;
    data.n = n_columns;
    if (allocate_memory() == EXIT_SUCCESS){
        for (int i=0; i<n_columns; i++){
            for(int j=0; j<n_rows; j++){
                double value = columns[i][j];
                EL(data, j, i) = value;
            }
        }
        initialized = true;
    }
}

Matrix::Matrix(Vertex column){
	n_columns = 1;
	n_rows = column.ndim;
	data.m = n_rows;
	data.n = n_columns;
	if (allocate_memory() == EXIT_SUCCESS){
		for(int j=0; j<n_rows; j++){
			EL(data, j, 0) = column.at(j);
		}
		initialized = true;
	}
}

Matrix::Matrix(vector<double> column){
    n_columns = 1;
    n_rows = column.size();
    data.m = n_rows;
    data.n = n_columns;
    if (allocate_memory() == EXIT_SUCCESS){
        for(int j=0; j<n_rows; j++){
            EL(data, j, 0) = column[j];
        }
        initialized = true;
    }
}

Matrix::Matrix(matrix a){
	data.m = a.m;
	data.n = a.n;
	if (allocate_memory() == EXIT_SUCCESS){
		memcpy(&data, &a, sizeof data);
		n_columns = a.n;
		n_rows = a.m;
		initialized = true;
		}
}

Matrix::~Matrix(){
	free(data.data);
}


Matrix Matrix::operator/(Matrix b){
   matrix x;
   linearLeastSquares(b.data, data, &x);
   return Matrix(x);
}

double Matrix :: at(int i, int j){ // i is the row index, j is the column index
    return EL(this->data, i, j);
}

vector<vector<double> > Matrix :: get_data(){
    vector<vector<double> > rdata;
    for (int i=0; i<n_columns; i++){

        vector<double> column_data;

        for (int j=0; j<n_rows; j++){
            column_data.push_back(EL(data, j, i));
        }
        rdata.push_back(column_data);
    }
    return rdata;
}

int Matrix::any(int (op)(double)){
	for (int i=0; i<n_columns; i++){
		for (int j=0; j<n_rows; j++){
			double value = EL(data, j, i);
			if (op(value))
                return 1;
		}
	}
    return 0;
}

void Matrix::print(){
    if (initialized)
        printMat(data);
    else
        cout << "not initialized" << endl;
}

Tetrahedron::Tetrahedron(vector<Vertex> p){
	vertices = p;
	ndim = p.size();
}
        
Tetrahedron::Tetrahedron(int ndim_, ...){
	int i = 0;
	va_list arguments;
	va_start ( arguments, ndim_ );
	for (i = 0; i<ndim_; i++ ){
		Vertex ai = va_arg ( arguments, Vertex );
		vertices.push_back(ai);
	}
	va_end ( arguments );
	ndim = ndim_;
}

double truncate(double value, int precision){
	return trunc(value * pow(10, precision)) / pow(10, precision);
}

bool Tetrahedron::contains(Vertex p){
	int n_vertices = vertices.size();
	bool does_not_contain = false;
	
	for (int i=0; i<n_vertices-1; i++){
		Vertex p0 = vertices[i];
		vector<Vertex> matrix_columns;
		for (int j=0; j<n_vertices; j++){
			if (i!=j)
				matrix_columns.push_back(vertices[j] - p0);
		}
		// Solve matrix_columns * x = p - p0 for x
		Matrix x = Matrix(p - p0) / Matrix(matrix_columns);
		// return false if any(x) >= 1 or any(x) < 0
        does_not_contain = x.any([](double v){double tv = truncate(v, 6); return tv > 1 || tv < 0 ? 1: 0;});

		if (does_not_contain) // no need for further checking
            return false;
	}
    return true;
}

pair<Vertex, Vertex> find_bounding_box(list<Vertex> vertices){

    vector<double> max; // max value along each dimension
    vector<double> min; // min value along each dimension

    for (auto v=vertices.begin(); v!=vertices.end(); ++v){

        if (v == vertices.begin()){
            int ndim = v->ndim;
            for (int dim=0; dim<ndim; dim++){
                max.push_back(-DBL_MAX);
                min.push_back(DBL_MAX);
            }
        }
        vector<double> coordinates = v->coordinates();

        int dim = 0;
        for (auto c=coordinates.begin(); c<coordinates.end(); ++c, dim++){
            if (*c > max[dim]){
                max[dim] = *c;
            }
            if (*c < min[dim]){
                min[dim] = *c;
            }
        }
    }

    return pair<Vertex, Vertex> {Vertex(max), Vertex(min)};

}

pair<double, Vertex> fit_sphere(vector<Vertex> points){
    /* given a set of N points in N-1 dimensions, find the hypersphere
     * through these points. We derive inspiration from the solution
     * presented in
     * http://www.dtcenter.org/met/users/docs/write_ups/circle_fit.pdf
     * For example, if we want to fit a 3D sphere through 4 points, we
     * need to solve the following equation
     *
     * |Suu  Suv  Suw|| uc |     1 |Suuu + Svvu + Swwu|
     * |Suv  Svv  Svw|| vc | =  ---|Suuv + Svvv + Swwv|
     * |Suw  Svw  Sww|| wc |     2 |Suuw + Svvw + Swww|
     *
     * For (uc, uv, wc) = center of the sphere in mean subtracted coordinates
     * u = x - mean(x), v = y - mean(y), w = z - mean(z)
     *
     * The radius of the sphere then is
     * R ** 2 = uc ** 2 + vc ** 2 + wc **2 + (Suu + Svv + Sww) / N
     *
     * Here:
     *
     * Suu = sum(ui**2, i=0..N), Suvv = sum(ui*vi**2, i=0..N), etc
     *
     * The radius and the center of the hypersphere are returned as a pair
     */

    Vertex sum = points.front();
    unsigned ndim = sum.ndim;

    for (auto p=points.begin(); p!=points.end(); ++p){

        if (p==points.begin()){
            continue;
        }else{
            sum = sum + *p;
        }
    }

    int npoints = points.size();
    Vertex mean = sum / npoints;

    vector<vector<double> > u; // mean subtracted coordinates in shape
    // [n_points, n_dim]
    vector<double> delta;

    for (auto p=points.begin(); p!=points.end(); ++p){
        delta = (*p - mean).coordinates();
        u.push_back(delta);
    }

    vector<vector<double> > lhs;
    vector<double> rhs;

    for (unsigned column=0; column<ndim; column++){

        vector<double> lhs_column;

        for (unsigned row=0; row<ndim; row++){
            double sxy = 0;  // for example Suu or Svw in the above example
            double sxxy = 0; // for example Suuw in the above sample

            for(auto ui=u.begin(); ui<u.end(); ui++){ // loop over n_points
                sxy += (*ui)[column] * (*ui)[row];
                sxxy += (*ui)[column] * (*ui)[column] * (*ui)[row];
            }
            lhs_column.push_back(sxy);

            if (column == 0)
                rhs.push_back(sxxy/2);
            else
                rhs[row] += sxxy/2;
        }

        lhs.push_back(lhs_column);
    }

    // Solve lhs * center = rhs
    vector<double> mean_subtracted_center = (Matrix(rhs) / Matrix(lhs)).get_data()[0];
    Vertex center = Vertex(mean_subtracted_center) + mean;

    double r_square = 0;
    for (unsigned dim=0; dim<ndim; dim++){
        r_square += pow(mean_subtracted_center[dim], 2) + lhs[dim][dim] / npoints;
    }
    double r = sqrt(r_square);

    return pair<double, Vertex> {r, center};
}


#ifdef TEST_GEOMETRY
int main(){

    const int ndim = 2;
    Vertex v1(ndim, 1.0, 1.0);
    Vertex v2(ndim, 0.0, 0.0);
    Vertex v3(ndim, 0.0, 3.0);
    Vertex v4(ndim, 0.5, 0.5);
    Tetrahedron tetra(ndim+1, v1, v2, v3);

    cout << tetra.contains(v4) << endl;

    return 0;
}
#endif

#ifdef TEST_CIRCLE_FIT

void test_2d(){

    vector<Vertex> points = {
        Vertex(2, 4.0, 1.0),
        Vertex(2, 6.0, 9.0),
        Vertex(2, 9.0, 4.0)
    };

    pair<double, Vertex> r_center = fit_sphere(points);
    double r = r_center.first;
    Vertex center = r_center.second;

    for (auto p=points.begin(); p!=points.end(); ++p){
        assert (abs(r - (center-*p).norm()) < 1E-6);
    }
}

void test_3d(){

    vector<Vertex> points = {
        Vertex(3, 4.0, 1.0, 5.0),
        Vertex(3, 6.0, 9.0, 2.0),
        Vertex(3, 9.0, 4.0, -3.0),
        Vertex(3, -4.0, 2.0, 6.0)
    };

    pair<double, Vertex> r_center = fit_sphere(points);
    double r = r_center.first;
    Vertex center = r_center.second;

    for (auto p=points.begin(); p!=points.end(); ++p){
        assert (abs(r - (center-*p).norm()) < 1E-6);
    }
}

int main(){

    test_2d();
    test_3d();

    return 0;
}
#endif

