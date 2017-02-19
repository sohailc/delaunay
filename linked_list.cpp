/*  Usage:
 * 	
 * l = LinkedList();
 * l.add(1, 3)
 * l.add(6, 9)
 * l.add(8, 2)
 * cout << l.query(6) << endl;  // should print 9
 * 
 */

#include <iostream>
#include <vector>

using namespace std;

class LinkedList{  
	class Node{
		public:
			int value;
			int key;
			Node *left;
			Node *right;
			
			Node(){
				left = NULL;
				right = NULL;
			}
			
			bool is_leaf(){
				return ((left == NULL) && (right == NULL));
			}
			
			Node *next(int query_key){
				if (query_key < key)
					return left;
				else if (query_key > key)
					return right;
				else
					return this;
			}

			void add_child(Node *n){
				if (n->key < key)
					left = n;
				else if (n->key > key)
					right = n;
				else
					cout << "Error: cannot add already exisiting key " << n->key << endl;
			}
	};
	
	private:
		
		Node *find(int key){
			Node *n = &tree[0];
			
			while (!n->is_leaf()){
				n = n->next(key);
				if (key == n->key)
					break;
			}
		
			return n;
		}
	
	public:
		vector<Node> tree;
		bool initialized;
		
		LinkedList(){
			initialized = false;
		}
		
		void add(int key, int value){
			
			Node new_node;
			int new_idx = tree.size(); 
			new_node.key = key;
			new_node.value = value;
			tree.push_back(new_node);
			
			if (!(initialized)){
				initialized = true;
				return;
			}
			
			Node *n = find(key);
			n->add_child(&tree[new_idx]);
		}
		
		int query(int key){
			Node *n = find(key);
			if (n->key != key){
				cout << "ERROR: key " << key << " not found " << endl;
				return 0;
			}
			return n->value;
		}
};

int main(){
	
	LinkedList l;
	l.add(1, 3);
	l.add(6, 9);
	l.add(8, 2);
	l.add(10, 4);
	l.add(80, 120);
	l.add(130, -4);
	cout << l.query(10) << endl;  // should print 9
	
	return 0;
}
