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
        vector<double> p;

        Vertex element_wise_operation(Vertex a, Vertex b, double (op)(double, double)){
            vector<double> result;
            vector<double> :: iterator i, j;

            for ( i=a.p.begin(), j=b.p.begin(); i<a.p.end(), j<b.p.end(); i++, j++){
                result.push_back(op(*i, *j));
            }

            return Vertex(result);
        }
        
        Vertex element_wise_operation(Vertex a, double b, double (op)(double, double)){
            vector<double> result;
            vector<double> :: iterator i, j;

            for (j=a.p.begin(); j<a.p.end(); j++){
                result.push_back(op(b, *j));
            }

            return Vertex(result);
        }

    public:
        int ndim;

        Vertex operator-(Vertex a){
            return element_wise_operation(p, a, [](double i, double j){return i - j;});
        }

        Vertex operator+(Vertex a){
            return element_wise_operation(p, a, [](double i, double j){return i + j;});
        }
        
        Vertex operator-(double a){
            return element_wise_operation(p, a, [](double i, double j){return i - j;});
        }

        Vertex operator+(double a){
            return element_wise_operation(p, a, [](double i, double j){return i + j;});
        }
        
        Vertex operator/(double a){
            return element_wise_operation(p, a, [](double i, double j){return i / j;});
        }

        Vertex operator*(double a){
            return element_wise_operation(p, a, [](double i, double j){return i * j;});
        }

        Vertex(vector<double> a){
            p = a;
            ndim = p.size();
        }

        double at(int i){
            return p.at(i);
        }

        Vertex(int ndim_, ...){   // so we can initialize a vertex like Vertex p(ndim, x, y, z);

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

        void print(){
            vector<double> :: iterator i;
            for (i=p.begin(); i<p.end(); i++){
                cout << *i << " ,";
            }
            cout << endl;
        }

};


class Matrix{

    private:
        matrix data;

        int allocate_memory(){
            NEW_MAT(data);
            return EXIT_SUCCESS;
        }

    public:

        int n_columns;
        int n_rows;
        bool initialized = false;

        Matrix(vector<Vertex> columns){

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

        Matrix(Vertex column){

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

        Matrix(matrix a){

            data.m = a.m;
            data.n = a.n;
            if (allocate_memory() == EXIT_SUCCESS){
                memcpy(&data, &a, sizeof data);
                n_columns = a.n;
                n_rows = a.m;
                initialized = true;
            }
        }

        ~Matrix(){
            free(data.data);
        }

        Matrix operator/(Matrix b){

            matrix x;
            cout << "data = " << endl;
            printMat(data);
            cout << "b.data = " << endl;
            printMat(b.data);
            linearLeastSquares(b.data, data, &x);
            cout << "x = " << endl;
            printMat(x);
            return Matrix(x);
        }

        bool any(bool (op)(double)){
            for (int i=0; i<n_columns; i++){
                for (int j=0; j<n_rows; j++){
                    double value = EL(data, j, i);
                    if (op(value))
                        return true;
                }
            }
            return false;
        }

        bool all(bool (op)(double)){
            for (int i=0; i<n_columns; i++){
                for (int j=0; j<n_rows; j++){
                    double value = EL(data, j, i);
                    if (!op(value))
                        return false;
                }
            }
            return true;
        }

        void print(){
            if (initialized)
                printMat(data);
            else
                cout << "not initialized" << endl;
        }
};

class Tetrahedron{ // a generalized tetrahedron in n-space
    vector<Vertex> vertices;
    int ndim;

    public:
        Tetrahedron(vector<Vertex> p){
            vertices = p;
            ndim = p.size();
        }
        
        Tetrahedron(int ndim_, ...){   // so we can initialize a like so Tetrahedron p(ndim, v1, v2, v3);

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

        bool contains(Vertex p){

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
                does_not_contain = x.any([](double v){return v >= 1 || v < 0 ? true: false;});

                if (does_not_contain) // no need for further checking
                    break;
            }

            return !does_not_contain;
        }
};


int main(){

    const int ndim = 3;
    Vertex v1(ndim, 3.4, 8.9, 5.4);
    Vertex v2(ndim, 1.2, 5.6, 1.2);
    Vertex v3(ndim, 4.5, 3.9, 9.1);
    Vertex v4(ndim, 2.5, 6.4, 3.8);
    Tetrahedron tetra(ndim+1, v1, v2, v3, v4);
    
    Vertex v5 = v1 + (v2 - v1) * 0.2 + (v3 - v1) * 0.3 + (v4 - v1) * 0.1;

    cout << tetra.contains(v5) << endl;

}
