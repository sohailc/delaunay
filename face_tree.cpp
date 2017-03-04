#include <iostream>
#include <vector>

#include "geometry.hpp"

using namespace std;

class FaceTree{

	class Node{
		vector<int> idx_node_vertices;
		vector<Node*> children;

		Node(){
			for(vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
				it = NULL;
			}
		}

		bool contains_vertex(vertex p){
			vector<Vertex> t;
			vector<int>::iterator i;
			for (i=idx_node_vertices.begin(); i<idx_node_vertices.end(); ++i){
				t.push_back(vertices[i]));
			}
			return Tetrahedron(t).contains(p);
		}

		bool is_leaf(){
			for(vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
				if (it != NULL)
					return false
			return true;
		}

		Node *next(Vertex p){
			for(vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
				if (it != NULL){
					if (it->contains_vertex(p))
						return p;
				}
			}
			return NULL;
		}

		void add_children(Vertex p){
			vertices.push_back(p);
			vector<int>::iterator i, j;
			
			int new_vertex_index = vertices.size();
			
			for (i=idx_node_vertices.begin(); i<idx_node_vertices.end(); ++i){

				vector<int> idx_new_node_vertices;
				idx_new_node_vertices.push_back(new_vertex_index);
				
				Node child;
				int n_nodes = nodes.size();
				
				for (j=idx_node_vertices.begin(); j<idx_node_vertices.end(); ++j){
					if (i==j){
						continue;
					}
					idx_new_node_vertices.push_back(j);	
				}
				
				child.idx_node_vertices = idx_new_node_vertices;
				nodes.push_back(child);
				children.push_back(&nodes[n_nodes]);
			}
		}

	}

	vector<Node> nodes;
	vector<Vertex> vertices;
	
	Node *find(Vertex p){
		Node *n = &nodes[0];
			
			while (!n->is_leaf()){
				n = n->next(p);
			}
		
			return n;
	}
	
	void add_vertex(Vertex p){
		Node *containing_node = find(p);
		containing_node->add_children(p);
	}
}
