#ifndef FACE_TREE
#define FACE_TREE

#include <map>
#include <list>
#include <vector>
#include <iostream>

#include "geometry.hpp"

class Face{

private:
    unsigned index;
    list<Vertex*> vertices;
    list<Face> children;

public:

    Face(list<Vertex*>); // contructor

    void set_index(unsigned);
    bool contains_vertex(Vertex); // determine if a vertex falls within the bounds
    // of the face. A vertex on the face edge and in its interior will return true
    bool in_circumcircle(Vertex);  // determine if a vertex falls within the circum
    // circle of the face
    Face *next(Vertex);
    list<Face*> make_children(Vertex *);
    bool is_leaf();
    void print();
    vector<vector<double> > coordinates();
    vector<unsigned> vertex_indices(); // the indices of the vertices spanning the
    // face
    bool contains_root_vertex();
};

typedef struct Edge{
    unsigned index; // if the edge is spanned by vertices i and j, the edge
    // index is i + j * n_vertices, where n_vertices is the total number of
    // vertices in the tree
    vector<Face*> member_faces; // The faces which contain this edge
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
    unsigned calculate_edge_index(unsigned, unsigned); // given vertex indices i and j
    // find the index of the edge between these vertices. This is equal to i + j * n_vertices if i < j

    map<unsigned, Face*> face_map; // given a face index find the pointer to the correspondig
    // face struct.
    unsigned calculate_face_index(vector<unsigned>); // given vertex indices, calculate the face index
    // of the face being spanned by the vertices. In 2D the vector has length 3 since we have three
    // vertices spanning the face. In 3D this is length 4. The face index is given by
    // sum(idx_i * n_vertices**i, i=0..ndim) assuming that the indexes as ordered small to large
    void link_face_to_edges(Face * face); // Given an edge index, we want to find all faces which share
    // this edge

    Face * root;

    void add_root_vertices(list<Vertex>&);
    Face * find_containing_face(Vertex); // find the face which contains this vertex
    vector<Face*> find_all_circumcircles(Vertex); // find all faces whos circum circle
    // contains this vertex
    list<Face*> find_neighbouring_faces(Face*); // find all faces which neighbour
    // the face given
    list<Face*> find_neighbouring_faces(Face*, list<Face*>); // find all faces which neighbour
    // the face given. This overloaded function allows the inclusion of a blacklist.

    void add_vertex(Vertex);

public:

    VertexTree(list<Vertex>);
    ~VertexTree();
    list<Face*> get_leaves();
    list<Face*> get_nodes();
    vector<vector<vector<double> > > get_edges();
};

#endif // FACE_TREE

