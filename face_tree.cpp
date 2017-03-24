/*  This module calculates the delaunay traingulation based on the
 *  Bowyer–Watson algorithm. See:
 *  https://en.wikipedia.org/wiki/Bowyer%E2%80%93Watson_algorithm
 */

#include <list>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "geometry.hpp"
#include "face_tree.h"

using namespace std;

Face :: Face(list<Vertex*> vertices){
    this->vertices = vertices;
}

void Face :: set_index(unsigned index){
    this->index = index;
}

bool Face :: contains_vertex(Vertex p){
    vector<Vertex> t;

    for (auto i=this->vertices.begin(); i!=this->vertices.end(); ++i){
        t.push_back(**i);
    }
    return Tetrahedron(t).contains(p);
}

bool Face :: in_circumcircle(Vertex p){

    vector<Vertex> t;

    for (auto i=this->vertices.begin(); i!=this->vertices.end(); ++i){
        t.push_back(**i);
    }

    pair<double, Vertex> r_center = fit_sphere(t);
    double r = r_center.first;
    Vertex center = r_center.second;

    return r - (center-p).norm() < 1E-10;
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

vector<unsigned> Face:: vertex_indices(){
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

unsigned VertexTree :: calculate_edge_index(unsigned i, unsigned j){

    if (j < i){    // flip i and j
        unsigned tmp = i;
        i = j;
        j = tmp;
    }
    return i + j * this->n_vertices;
}

unsigned VertexTree :: calculate_face_index(vector<unsigned> vertex_indices){

    sort(vertex_indices.begin(), vertex_indices.end());
    unsigned face_index = 0;
    int p = 0;

    for(auto i=vertex_indices.begin(); i<vertex_indices.end(); ++i, p++){
        face_index += *i * pow(this->n_vertices, p);
    }
    return face_index;
}

void VertexTree :: link_face_to_edges(Face * face){

    Edge *edge;
    unsigned i, j, edge_index;
    vector<unsigned> indices = face->vertex_indices();
    vector<unsigned> :: iterator idx;

    for (idx=indices.begin(); idx<indices.end(); ++idx){

        if (idx==indices.begin()){
            i = indices.back();
        }else{
            i = *(idx-1);
        }
        j = *idx;

        edge_index = calculate_edge_index(i, j);
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
            unsigned root_index = calculate_face_index(this->root->vertex_indices());
            this->root->set_index(root_index);
            this->face_map[root_index] = this->root;
            link_face_to_edges(this->root);
        }

        add_vertex(*v);
    }
}

VertexTree :: ~VertexTree(){
    delete this->root;
}

Face * VertexTree :: find_containing_face(Vertex p){
    Face *n = this->root;

    while (!n->is_leaf()){
        n = n->next(p);
    }
    return n;
}

vector<Edge> merge_edge_lists(vector<Edge> edge_list, Face face){

    // Calculate the edges of the face and find the one which is in the edge list. Delete that edge
    // from the edge list and insert the other edges from face into the list of edges.

    vertex_indices = face.vertex_indices;
    vector<Edge> face_edges;
    vector<unsigned> face_edge_indices;

    for (auto idx=vertex_indices.begin(); idx<vertex_indices.end(); ++idx){

        if (idx==vertex_indices.begin()){
            i = vertex_indices.back();
        }else{
            i = *(idx-1);
        }
        j = *idx;

        edge_index = calculate_edge_index(i, j);
        face_edge = this->edge_map[edge_index];
        face_edges.push_back(face_edge);
        face_edge_indices.push_back(edge_index);
    }

    for(auto idx=edge_list.begin(); idx<edge_list.end(); ++idx){

        if (find(face_edge_indices.begin(), face_edge_indices.end(), *idx.index) != face_edge_indices.end())
        {
            // left off here
        }
    }

}

vector<Face*> VertexTree :: find_all_circumcircles(Vertex vertex){

    vector<Face*> faces;
    Face *n = find_containing_face(vertex);

    faces.push_back(n);

    list<Face*> candidates = find_neighbouring_faces(n);

    for (auto candidate = candidates.begin(); candidate!=candidates.end(); ++candidate){

        if ((*candidate)->in_circumcircle(vertex)){

            faces.push_back(*candidate);
            list<Face*> new_candidates = find_neighbouring_faces(*candidate, candidates); // find the neighbours of this candidate
            // but exclude the candidates already present in the list (second argument)
            candidates.merge(new_candidates);
        }
    }

    return faces;
}

list<Face*> VertexTree :: find_neighbouring_faces(Face* face){
    list<Face*> blacklist; // an empty blacklist
    return find_neighbouring_faces(face, blacklist);
}

list<Face*> VertexTree :: find_neighbouring_faces(Face* face, list<Face*> blacklist){
    /*   Find all neighbours of a face, except if its index occures in the blacklist.
     */

    list<Face*> neighbours;

    unsigned i, j, edge_index;
    Edge *edge;

    blacklist.push_back(face); // the face determining the neighbours is always in the black list. We do not want
    // this function to return itself as a neighbour, or else we will get stuck in an infinite loop

    vector<unsigned> vertex_indices = face->vertex_indices();

    for (auto idx=vertex_indices.begin(); idx<vertex_indices.end(); ++idx){

        if (idx==vertex_indices.begin()){
            i = vertex_indices.back();
        }else{
            i = *(idx-1);
        }
        j = *idx;

        edge_index = calculate_edge_index(i, j);
        edge = this->edge_map[edge_index];

        // loop over the member faces of the edge.
        for (auto member_face=edge->member_faces.begin(); member_face<edge->member_faces.end(); ++member_face){

            bool in_blacklist = find(blacklist.begin(), blacklist.end(), *member_face) != blacklist.end();

            if (in_blacklist){
                continue;
            }
            if ((*member_face)->is_leaf())
            {
                neighbours.push_back(*member_face);
                break; // there should only be one qualifying face per edge.
            }
        }
    }

    return neighbours;
}

void VertexTree :: add_vertex(Vertex vertex){

    vector<Face*> faces = find_all_circumcircles(vertex);

    Face *n = find_containing_face(vertex);

    this->vertices.push_back(vertex);
    this->vertex_map[vertex.get_index()] = &this->vertices.back();

    list<Face*> children = n->make_children(&this->vertices.back());
    for (auto child=children.begin(); child!=children.end(); ++child){

        unsigned child_index = calculate_face_index((*child)->vertex_indices());
        (*child)->set_index(child_index);
        this->face_map[child_index] = *child;
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
