#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cstdarg>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>

#include "linearLeastSquares.h"

using namespace std;


class Vertex{

    private:
        unsigned index;
        vector<double> p;
        Vertex element_wise_operation(Vertex a, Vertex b, double (op)(double, double));
        Vertex element_wise_operation(Vertex a, double b, double (op)(double, double));

    public:
        unsigned ndim;
			
        // The constructor s
        Vertex(int ndim_, ...);  // so we can initialize a vertex like Vertex p(ndim, x, y, z);
        Vertex(vector<double> a);

        Vertex operator-(Vertex a);
        Vertex operator+(Vertex a);
        Vertex operator-(double a);
        Vertex operator+(double a);
        Vertex operator/(double a);
        Vertex operator*(double a);

        double norm();

        double at(int i);
        void set_index(int);
        int get_index();
        vector<double> coordinates();
        
        void print();
};


class Matrix{

    private:
        matrix data;
        int allocate_memory();

    public:

        int n_columns;
        int n_rows;
        bool initialized = false;

        // constructors
        Matrix(vector<Vertex> columns);
        Matrix(vector<vector<double> > columns);
        Matrix(Vertex column);
        Matrix(vector<double> column);
        Matrix(matrix a);

        // destructor
        ~Matrix();

        vector<vector<double> > get_data(); // get the data column wise
        Matrix operator/(Matrix b); // solve a linear equation like Ax = b for x
        double at(int, int);
        int any(int (op)(double));
        void print();
};

class Tetrahedron{ // a generalized tetrahedron in n-space
    vector<Vertex> vertices;
    int ndim;

    public:
    		// The constructors
        Tetrahedron(vector<Vertex> p);
        Tetrahedron(int ndim_, ...);

        bool contains(Vertex p);
};

pair<Vertex, Vertex> find_bounding_box(list<Vertex> vertices);
pair<double, Vertex> fit_sphere(vector<Vertex> points);

#endif
