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

    public:
        int ndim;
        
        Vertex operator-(Vertex a){
            return element_wise_operation(p, a, [](double i, double j){return i - j;});
        }

        Vertex operator+(Vertex a){
            return element_wise_operation(p, a, [](double i, double j){return i + j;});
        }

        Vertex(vector<double> a){
            p = a;
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
                        EL(data, j, i) = columns[i].at(j);
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
            x.m = n_rows;
            x.n = b.data.n;
            
            linearLeastSquares(data, b.data, &x);
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
};

class tetrahedron{ // a generalized tetrahedron in n-space
    vector<Vertex> vertices;
    int ndim;

    public:
        tetrahedron(vector<Vertex> p){
            vertices = p;
            ndim = p.size();
        }

        bool contains(Vertex p){

            int n_vertices = vertices.size();
            vector<Vertex> matrix_columns;
            bool does_contain = true;
            
            for (int i=0; i<n_vertices-1; i++){
                
                Vertex p0 = vertices[i];

                for (int j=0; j<n_vertices; j++){
                        if (i!=j)
                            matrix_columns.push_back(vertices[j] - p0);
                }
                // Solve matrix_columns * x = p - p0 for x
                Matrix x = Matrix(p - p0) / Matrix(matrix_columns);
                // return false if any(x) > 1
                does_contain = x.any([](double v){return v > 1 ? false: true;});
                
                if (!does_contain) // no need for further checking 
                    break;
            }
        
            return does_contain;
        }
};


int main(){

    const int ndim = 2;
    Vertex v1(ndim, 3.4, 8.9);
    Vertex v2(ndim, 1.2, 5.6);

    Vertex v3 = v1 + v2;

    v1.print();
    v2.print();
    v3.print();
}
