#ifndef FACE_TREE
#define FACE_TREE

#include <list>
#include <vector>
#include <iostream>

#include "geometry.hpp"

class Node{

private:
    list<Vertex*> node_vertices;
    list<Node> children;

public:

    Node(list<Vertex*>);
    bool contains_vertex(Vertex);
    Node *next(Vertex);
    list<Node*> make_children(Vertex *);
    bool is_leaf();
    void print();
    vector<vector<double> > coordinates();
};

class VertexTree{

private:
    list<Vertex> vertices;
    Node* root;
    list<Node*> nodes;

public:

    VertexTree(list<Vertex*>);
    ~VertexTree();
    Node *find(Vertex);
    void add_vertex(Vertex);
    list<Node*> get_leaves();
    list<Node*> get_nodes();
};

#endif // FACE_TREE

