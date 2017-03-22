#include <list>
#include <vector>
#include <iostream>
#include <cmath>

#include "geometry.hpp"
#include "face_tree.h"

using namespace std;

Face :: Face(list<Vertex*> vertices){
    this->vertices = vertices;
}

bool Face :: contains_vertex(Vertex p){
    vector<Vertex> t;

    for (auto i=vertices.begin(); i!=vertices.end(); ++i){
        t.push_back(**i);
    }
    return Tetrahedron(t).contains(p);
}

Face * Face :: next(Vertex p){
    bool found = false;
    list<Face>:: iterator child;
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

list<Face*> Face :: make_children(Vertex *p){

    list<Face*> new_children;

    for (auto i=vertices.begin(); i!=vertices.end(); ++i){

        list<Vertex*> new_node_vertices;
        new_node_vertices.push_back(p);

        for (auto j=vertices.begin(); j!=vertices.end(); ++j){
            if (*i==*j){
                continue;
            }
            new_node_vertices.push_back(*j);
        }

        Face child(new_node_vertices);
        this->children.push_back(child);
        new_children.push_back(&this->children.back());
    }

    return new_children;
}

bool Face :: is_leaf(){
    if (children.size() > 0)
        return false;
    return true;
}

bool Face :: contains_root_vertex(){
    unsigned index;
    for (auto v = this->vertices.begin(); v!=this->vertices.end(); ++v){
        unsigned ndim = (*v)->ndim;
        index = (*v)->get_index();
        if (index <= ndim)
            return true;
    }
    return false;
}

void Face :: print(){

    for (auto i=vertices.begin(); i!=vertices.end(); i++){
        (*i)->print();
    }
}

vector<vector<double> > Face :: coordinates(){
    vector<vector<double> > c;

    for(auto vertex=this->vertices.begin(); vertex!=this->vertices.end(); ++vertex){
        c.push_back((*vertex)->coordinates());
    }

    return c;
}

vector<unsigned> Face:: indices(){
    vector<unsigned> idx;

    for(auto vertex=this->vertices.begin(); vertex!=this->vertices.end(); ++vertex){
        idx.push_back((*vertex)->get_index());
    }
    return idx;
}

void VertexTree :: add_root_vertices(list<Vertex> &vertices){

    vector<vector<double> > aabb = find_bounding_box(vertices);
    double diagonal = 0;
    vector<double> max = aabb[0];
    vector<double> min = aabb[1];

    for (unsigned ndim=0; ndim<max.size(); ndim++){
        diagonal += pow(max[ndim] - min[ndim], 2);
    }
    diagonal = sqrt(diagonal);

    Vertex root_vertex0(min);
    vertices.push_front(root_vertex0);

    for (unsigned ndim=0; ndim<max.size(); ndim++){
        vector<double> r;
        for (unsigned i=0; i<max.size(); i++){
            r.push_back(0.0);
        }
        r[ndim] = 2 * diagonal;
        Vertex root_vertexN = Vertex(min) + Vertex(r);
        vertices.push_front(root_vertexN);
    }
}

void VertexTree :: link_face_to_edges(Face * face){

    Edge *edge;
    unsigned i, j, edge_index;
    vector<unsigned> indices = face->indices();
    vector<unsigned> :: iterator idx;

    for (idx=indices.begin(); idx<indices.end(); ++idx){

        if (idx==indices.begin()){
            i = indices.back();
        }else{
            i = *(idx-1);
        }
        j = *idx;

        edge_index = i + j * this->n_vertices;
        edge = this->edge_map[edge_index];

        if (edge == NULL){
            Edge new_edge;
            new_edge.index = edge_index;
            new_edge.member_faces.push_back(face);
            this->edges.push_back(new_edge);
            this->edge_map[edge_index] = &this->edges.back();
        }else{
            edge->member_faces.push_back(face);
        }
    }
}

VertexTree :: VertexTree(list<Vertex> vertices){

    add_root_vertices(vertices);
    this->n_vertices = vertices.size();

    list<Vertex*> root_vertices;

    int count = 0;
    for (auto v = vertices.begin(); v!=vertices.end(); ++v, count++){
        v->set_index(count);
        int ndim = v->ndim;

        if (count < ndim + 1){
            this->vertices.push_back(*v);
            this->vertex_map[v->get_index()] = &this->vertices.back();
            root_vertices.push_back(&this->vertices.back());
            continue;
        }else if (count == ndim + 1){
            this->root = new Face(root_vertices);
            link_face_to_edges(this->root);
        }

        add_vertex(*v);
    }
}

VertexTree :: ~VertexTree(){
    delete this->root;
}

Face * VertexTree :: find(Vertex p){
    Face *n = this->root;

    while (!n->is_leaf()){
        n = n->next(p);
    }
    return n;
}

void VertexTree :: add_vertex(Vertex vertex){
    Face *n = find(vertex);

    // TODO: We have the face (n) in which the new vertex is placed.
    // In order to make a Delaunay triangulation we need to find all
    // vertices in neighbouring faces which fall in the circumfence
    // of this face.

    this->vertices.push_back(vertex);
    this->vertex_map[vertex.get_index()] = &this->vertices.back();

    list<Face*> children = n->make_children(&this->vertices.back());
    for (auto child=children.begin(); child!=children.end(); ++child){
        this->faces.push_back(*child);
        link_face_to_edges(*child);
    }
}

vector<vector<vector<double> > > VertexTree :: get_edges(){
    /*   Output shape is [n_edges, 2, ndim]
     */

    unsigned i, j, edge_index;
    Vertex *p1, *p2;
    vector<vector<vector<double> > > out;

    for (auto edge=this->edges.begin(); edge!=this->edges.end(); ++edge){
        edge_index = edge->index;
        i = edge_index % this->n_vertices;
        j = edge_index / this->n_vertices; // integer division

        p1 = this->vertex_map[i];
        p2 = this->vertex_map[j];

        vector<double> coordinates1 = p1->coordinates();
        vector<double> coordinates2 = p2->coordinates();
        vector<vector<double> > coordinates = {coordinates1, coordinates2};
        out.push_back(coordinates);
    }
    return out;
}

#ifdef TEST_FACE
int main(){

    return 0;
}
#endif
