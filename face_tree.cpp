#include <iostream>
#include <vector>

using namespace std;

class FaceTree{
	
	typedef struct vertex{
		vector<double> p;
	}vertex;
	
	vector<vertex> vertices;
	
	class Node{
		vector<int> vertex_indices;
		vector<Node*> children;
		
		Node(){
			for(vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
				it = NULL;
			}
		}
		
		bool contains_vertex(vertex p){
			// geometric function to see if vertex p is in the node
		}
		
		bool is_leaf(){
			for(vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
				if (it != NULL)
					return false
			return true;
		}
		
		Node *next(vertex p){
			for(vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
				if (it != NULL){
					if (it->contains_vertex(p))
						return p;
				}
			return NULL;
		}
		
		void add_child(vertex p){
			vertices.push_back(p);
			int new_vertex_idx = vertices.size();
			int n_vertices = vertex_indices.size();
			
			for (int i=0; i<n_vertices; i++){
				for (int j=i+1; j<n_vertices; j++){
					Node new_child_node;
					new_child_node.vertex_indices.push_back(vertex_indices[i]);
					new_child_node.vertex_indices.push_back(vertex_indices[j]);
					new_child_node.vertex_indices.push_back(new_vertex_idx);
					children.push_back(new_child_node)
				}
			}
		}
		
	}
	
	vector<Node> nodes;
}
