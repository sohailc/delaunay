#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cstdarg>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>

using namespace std;


class Vertex{

    private:
        vector<double> p;
        Vertex element_wise_operation(Vertex a, Vertex b, double (op)(double, double));
        Vertex element_wise_operation(Vertex a, double b, double (op)(double, double));

    public:
        int ndim;
			
			// The constructor         
        Vertex(int ndim_, ...);  // so we can initialize a vertex like Vertex p(ndim, x, y, z);

        Vertex operator-(Vertex a);
        Vertex operator+(Vertex a);
        Vertex operator-(double a);
        Vertex operator+(double a);
        Vertex operator/(double a);
        Vertex operator*(double a);
        Vertex(vector<double> a);
        double at(int i);
        
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
        Matrix(vector<Vertex> columns);
        Matrix(Vertex column);
        Matrix(matrix a);
        ~Matrix();
        Matrix operator/(Matrix b);
        bool any(bool (op)(double));
        bool all(bool (op)(double));
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

#endif
