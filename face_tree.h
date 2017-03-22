#ifndef FACE_TREE
#define FACE_TREE

#include <map>
#include <list>
#include <vector>
#include <iostream>

#include "geometry.hpp"

class Face{

private:
    list<Vertex*> vertices;
    list<Face> children;

public:

    Face(list<Vertex*>);
    bool contains_vertex(Vertex);
    Face *next(Vertex);
    list<Face*> make_children(Vertex *);
    bool is_leaf();
    void print();
    vector<vector<double> > coordinates();
    vector<unsigned> indices();
    bool contains_root_vertex();
};

typedef struct Edge{
    unsigned index; // if the edge is spanned by vertices i and j, the edge
    // index is i + j * n_vertices, where n_vertices is the total number of
    // vertices in the tree
    list<Face*> member_faces; // The faces which contain this edge
} Edge;

class VertexTree{

private:
    unsigned n_vertices; // the number of input vertices, plus the number of
    // vertices making up the root (3 for 2D, 4 for 3D). We cannot relay on the
    // the size of this->vertices to give us n_vertices as during the execution of
    // the algorithm this list gets incremented progressively.

    list<Vertex> vertices;
    map<unsigned, Vertex*> vertex_map; // given a vertex index, find the pointer to the
    // corresponding vertex structure.

    list<Edge> edges;
    map<unsigned, Edge*> edge_map; // given an edge index, find the pointer to the corresponding
    // edge struct. This will quickly give us access to member faces.
    void link_face_to_edges(Face * face);

    Face * root;
    list<Face*> faces;  // TBD: Do we really need this?

    void add_root_vertices(list<Vertex>&);
    Face * find(Vertex);
    void add_vertex(Vertex);

public:

    VertexTree(list<Vertex>);
    ~VertexTree();
    list<Face*> get_leaves();
    list<Face*> get_nodes();
    vector<vector<vector<double> > > get_edges();
};

#endif // FACE_TREE

