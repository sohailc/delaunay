#include <cstdarg>
#include <iostream>
#include <vector>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <cmath>
#include <float.h>

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
	return element_wise_operation(p, a, [](double i, double j){return i / j;});
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


vector<vector<double> > find_bounding_box(list<Vertex> vertices){
    /*    The outer vector is length two: {max, min}
     *    The inner vector is length ndim; containing the maxima and the
     *    minima along each of the dimensions.
     */

    vector<double> max; // max value along each dimension
    vector<double> min; // min value along each dimension
    vector<vector<double> > aabb;

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
    aabb.push_back(max);
    aabb.push_back(min);
    return aabb;
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

}
#endif

