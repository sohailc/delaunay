#ifndef FACE_TREE
#define FACE_TREE

#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <algorithm>

#include "geometry.hpp"

class Face{

private:
    vector<unsigned> index;
    list<Vertex*> vertices;
    list<Face> children;

public:

    Face(list<Vertex*>); // contructor

    void set_index(vector<unsigned>);
    bool contains_vertex(Vertex); // determine if a vertex falls within the bounds
    // of the face. A vertex on the face edge and in its interior will return true
    bool in_circumcircle(Vertex);  // determine if a vertex falls within the circum
    // circle of the face
    Face *next(Vertex);
    list<Face*> make_children(Vertex *); // obsolete
    void register_children(list<Face> children);
    bool is_leaf();
    void print();
    vector<vector<double> > coordinates();
    vector<unsigned> vertex_indices(); // the indices of the vertices spanning the
    // face
    bool contains_root_vertex();
};

class Edge{

public:
    vector<unsigned> index;
    vector<Face*> member_faces; // The faces which contain this edge
    bool operator==(const Edge lhs){
        return equal(this->index.begin(), this->index.end(), lhs.index.begin());
    }
};


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
    map<vector<unsigned>, Edge*> edge_map;
    Edge *get_edge(vector<unsigned>); // given indices of N vertices in N-1 space, find
    // the pointer to the corresponding edge struct.
    void add_edge_to_list(Edge, vector<unsigned>); // make sure we can find the edge
    // with the get_edge method

    void link_face_to_edges(Face * face);

    Face * root;

    void add_root_vertices(list<Vertex>&);
    Face * find_containing_face(Vertex); // find the face which contains this vertex
    vector<Face*> find_all_circumcircles(Vertex); // find all faces whos circum circle
    // contains this vertex
    list<Face*> find_neighbouring_faces(Face*); // find all faces which neighbour
    // the face given
    list<Face*> find_neighbouring_faces(Face*, list<Face*>); // find all faces which neighbour
    // the face given. This overloaded function allows the inclusion of a blacklist.
    void merge_edge_lists(vector<Edge>& edge_list, Face face);

    void add_vertex(Vertex);
    void add_vertex_new(Vertex);

public:

    VertexTree(list<Vertex>);
    ~VertexTree();
    list<Face*> get_leaves();
    list<Face*> get_nodes();
    vector<vector<vector<double> > > get_edges();
};

#endif // FACE_TREE

