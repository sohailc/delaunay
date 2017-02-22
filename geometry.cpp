#include <cstdarg>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#include "linearLeastSquares.h"

using namespace std;

matrix vertices_to_column_matrix(vector<Vertex> columns, matrix *a){
 
    // We assume that memory for the matrix has been allocated 
    int n_columns = columns.size();
    for (i_col=0; i_col<n_columns; i_col++){
     
        column = columns[i_col];
        n_row = column.p.size();
        for (i_row=0; i_row<n_row; i_row++){
            double element = column.p[i_row];
            EL((*a), i_row, i_col) = element;
        }
        
    }
}

class Matrix{
  
    private:
        matrix data;
        int n_columns;
        int n_rows;
    
    public:
        Matrix(vector<Vertex> columns){
            
            n_columns = columns.size();
            n_rows = columns[0].p.size();
            data.m = n_rows;
            a.n = n_columns;
            NEW_MAT(a);
            
            for (int i=0; i<n_columns; i++){
                for(int j=0; j<n_rows; j++){
                    EL(data, j, i) = columns[i].p[j];
                }
            }
        }
        
        Matrix(Vertex column){
            
            n_columns = 1;
            n_rows = column.p.size();
            data.m = n_rows;
            a.n = n_columns;
            NEW_MAT(a);
            
            for (int i=0; i<n_columns; i++){
                for(int j=0; j<n_rows; j++){
                    EL(data, j, i) = columns[i].p[j];
                }
            }
        }
        
        Matrix(matrix a){
         
            data.m = a.m;
            data.n = a.n;
            NEW_MAT(data);
            
            memcpy(&data, &a, sizeof data);
            n_columns = a.n;
            n_rows = a.m;
            
        }
        
        ~Matrix(){
            free(data.data);
        }
        
        Matrix operator/(Matrix b){
            
            matrix x; 
            Matrix result;
            
            x.m = n_rows
            x.n = b.data.n;
            
            linearLeastSquares(data, b, &x);
            return Matrix(x);
        }
};

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
        Vertex operator-(Vertex a){
            return element_wise_operation(p, a, [](double i, double j){return i - j;});
        }

        Vertex operator+(Vertex a){
            return element_wise_operation(p, a, [](double i, double j){return i + j;});
        }

        Vertex(vector<double> a){
            p = a;
        }

        Vertex(int ndim, ...){   // so we can initialize a vertex like Vertex p(ndim, x, y, z); 

            double ai;
            int i = 0;

            va_list arguments;
            va_start ( arguments, ndim );
            for (i = 0; i<ndim; i++ ){
                ai = va_arg ( arguments, double );
                p.push_back(ai);
            }
            va_end ( arguments );
        }

        void print(){
            vector<double> :: iterator i;
            for (i=p.begin(); i<p.end(); i++){
                cout << *i << " ,";
            }
            cout << endl;
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
            matrix a, b, x;
            vector<Vertex> matrix_columns;
            
            a.m = n_vertices;
            a.n = n_vertices;
            NEW_MAT(a);
            
            b.m = ndim;
            b.n = 1;
            NEW_MAT(b);
            
            x.m = ndim;
            x.n = 1;
            NEW_MAT(x);
            
            does_contain = True
            
            for (int i=0; i<n_vertices-1; i++){
                
                Vertex p0 = vertices[i];

                for (int j=0; j<n_vertices; j++){
                        if (i!=j)
                            matrix_columns.push_back(vertices[j] - p0);
                }
                // Solve matrix_columns * x = p - p0 for x
                // return false if any(x) > 1
                // Matrix x = Matrix(p - p0) / Matrix(matrix_columns);
                vertices_to_column_matrix(matrix_columns, a);
                vertices_to_column_matrix(p - p0, b);
                linearLeastSquares(matrix_columns, b, &x);
                
                for (j=0; j<ndim; j++){
                    if (EL(x, j, 1) > 1)
                        does_contain = false;
                        break;
                }
                if (!does_contain)
                    break;
            }

            free(a.data);
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
