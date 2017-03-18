#include <list>
#include <vector>
#include <iostream>

#include "geometry.hpp"

using namespace std;

class VertexTree{

	class Node{

		private:
			list<Vertex*> node_vertices;
			list<Node> children;

		public:
		
			Node(list<Vertex*> vertices){
				this->node_vertices = vertices;
			}
			
			bool contains_vertex(Vertex p){
				vector<Vertex> t;
				
				for (auto i=node_vertices.begin(); i!=node_vertices.end(); ++i){
					t.push_back(**i);
				}
				return Tetrahedron(t).contains(p);
			}
			
			Node *next(Vertex p){
				bool found = false;
				list<Node>:: iterator child;
				for (child=children.begin(); child!=children.end(); ++child){
					if ((*child).contains_vertex(p)){
						found = true;
						break;
					}
				}
				if (found){
					return &(*child);				
				}else{
					throw runtime_error("Vertex not found at current node");
				}
			}
			
			list<Node*> make_children(Vertex *p){
			
				list<Node*> new_children;
				
				for (auto i=node_vertices.begin(); i!=node_vertices.end(); ++i){

					list<Vertex*> new_node_vertices;
					new_node_vertices.push_back(p);
				
					for (auto j=node_vertices.begin(); j!=node_vertices.end(); ++j){
						if (*i==*j){
							continue;
						}
						new_node_vertices.push_back(*j);	
					}
				
					Node child(new_node_vertices);
					this->children.push_back(child);
					new_children.push_back(&this->children.back());
				}
				
				return new_children;
			}
			
			bool is_leaf(){
				if (children.size())
					return false;
				return true;
			}
		
			void print(){

				for (auto i=node_vertices.begin(); i!=node_vertices.end(); i++){
					(*i)->print();
				}
			
			}
	};
	
	private:
		list<Vertex> vertices;
		Node* root;
		list<Node*> nodes;
	
	public:
		
		VertexTree(){

			list<Vertex*> v;
			
			Vertex v1(2, 0.0, 0.0);
			this->vertices.push_back(v1);
			v.push_back(&this->vertices.back());
			
			Vertex v2(2, 3.0, 0.0);
			this->vertices.push_back(v2);
			v.push_back(&this->vertices.back());
			
			Vertex v3(2, 0.0, 3.0);
			this->vertices.push_back(v3);
			v.push_back(&this->vertices.back());
			
			root = new Node(v);	
		}
		
		~VertexTree(){
			delete root;		
		}
		
		Node *find(Vertex p){
			Node *n = root;
			
			while (!n->is_leaf()){
				n = n->next(p);
			}
			return n;
		}
		
		void add_vertex(Vertex vertex){
			Node *n = find(vertex);
			this->vertices.push_back(vertex);
			list<Node*> children = n->make_children(&this->vertices.back());
			for (auto child=children.begin(); child!=children.end(); ++child){
				this->nodes.push_back(*child);	
			}
		}
		
		list<Node*> find_leaves(){
			list<Node*> leaves;
			for (auto node=this->nodes.begin(); node!=this->nodes.end(); ++node){
				if ((*node)->is_leaf());
					leaves.push_back(*node);
			}
			return leaves;
		}

};

#ifdef TEST_FACE
int main(){
	
	VertexTree v;
	
	Vertex v1(2, 1.0, 1.0);
	v.add_vertex(v1);
			
	Vertex v2(2, 1.0, 0.5);
	v.add_vertex(v2);
	
	auto leaves = v.find_leaves();
	int count=0;	
	
	for (auto leaf=leaves.begin(); leaf!=leaves.end(); ++leaf, count++){
		cout << "leaf " << count << endl;
		(*leaf)->print();	
	}

	return 0;
}
#endif
