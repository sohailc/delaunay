#include <cstdarg>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

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
        Vertex operator-(Vertex a){
            return element_wise_operation(p, a, [](double i, double j){return i - j;});
        }

        Vertex operator+(Vertex a){
            return element_wise_operation(p, a, [](double i, double j){return i + j;});
        }

        Vertex(vector<double> a){
            p = a;
        }

        Vertex(int argnums, ...){

            double ai;
            int i = 0;

            va_list arguments;
            va_start ( arguments, argnums );
            for (i = 0; i<argnums; i++ ){
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
            matrix a;
            vector<Vertex> matrix_columns;
            
			a.m = n_vertices;
			a.n = n_vertices;
			NEW_MAT(a);
            
            for (int i=0; i<n_vertices-1; i++){
                
                Vertex p0 = vertices[i];
				
                for (int j=0; j<n_vertices; j++){
					
					if (i==j)
						continue;
					
					matrix_columns.push_back(vertices[j] - p0);
					// Solve matrix_columns * x = p - p0 for x
					// return false if any(x) > 1
					
					
				}


            }

			free(a.data);
            return true;
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
