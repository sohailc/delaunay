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
#include "utils.h"

using namespace std;

Face :: Face(vector<Vertex*> vertices){
    this->vertices = vertices;
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

    return (center-p).norm() - r < 1E-10;
}

Face * Face :: next(Vertex p){
    bool found = false;
    list<Face*>:: iterator child;
    for (child=children.begin(); child!=children.end(); ++child){
        if ((*child)->contains_vertex(p)){
            found = true;
            break;
        }
    }
    if (found){
        return *child;
    }else{
        throw runtime_error("Vertex not found at current node");
    }
}

void Face :: register_children(list<Face*> children){
    this->children = children;
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

    pair<Vertex, Vertex> aabb = find_bounding_box(vertices);

    Vertex max = aabb.first;
    Vertex min = aabb.second;
    int ndim = max.ndim;

    Vertex mid = (max+min) / 2 ;
    max = (max - mid) * 100 + mid;
    min = (min - mid) * 100 + mid;
    double diagonal = (max-min).norm();

    vertices.push_front(min);

    for (int dim=0; dim<ndim; dim++){

        vector<double> r = fill<vector<double> >(0, ndim, 0.0);

        r[dim] = 2 * diagonal;
        vertices.push_front(min + Vertex(r));
    }
}


VertexTree :: VertexTree(list<Vertex> vertices){

    add_root_vertices(vertices);
    this->n_vertices = vertices.size();

    vector<Vertex*> root_vertices;

    int count = 0;

    for (auto v = vertices.begin(); v!=vertices.end(); ++v, count++){
        v->set_index(count);
        int ndim = v->ndim;

        if (count <= ndim){

            this->vertices.push_back(*v);
            this->vertex_map[v->get_index()] = &this->vertices.back();
            root_vertices.push_back(&this->vertices.back());

            if (count == ndim){

                Face root(root_vertices);
                this->faces.push_back(root);
                link_face_to_edges(&this->faces.back());
            }

            continue;
        }

        add_vertex(*v);
    }
}


Edge * VertexTree :: get_edge(vector<unsigned> key){
    // given vertex indices, get the pointer to the edge structure
    sort(key.begin(), key.end());
    return this->edge_map[key];
}

void VertexTree :: add_edge_to_list(Edge edge, vector<unsigned> key){
    // make sure that the edge can be found given a key
    sort(key.begin(), key.end());
    this->edges.push_back(edge);
    this->edge_map[key] = &this->edges.back();
}

vector<vector<unsigned> > get_edge_keys(Face face){

    vector<unsigned> vertex_indices = face.vertex_indices();
    vector<vector<unsigned> > keys;

    for(auto idx1=vertex_indices.begin(); idx1<vertex_indices.end(); ++idx1){

        vector<unsigned> key;

        for(auto idx2=vertex_indices.begin(); idx2<vertex_indices.end(); ++idx2){
            if (idx1!=idx2){
                key.push_back(*idx2);
            }
        }
        keys.push_back(key);
    }
    return keys;
}


void VertexTree :: link_face_to_edges(Face * face){

    vector<vector<unsigned> > edge_keys = get_edge_keys(*face);

    for(auto key=edge_keys.begin(); key<edge_keys.end(); ++key){

        Edge *edge = this->get_edge(*key);

        if (edge == NULL){
            Edge new_edge;
            new_edge.member_faces.push_back(face);
            new_edge.index = *key;
            add_edge_to_list(new_edge, *key);
        }else{
            edge->member_faces.push_back(face);
        }
    }
}

Face * VertexTree :: find_containing_face(Vertex p){
    Face *n = &this->faces.front();

    while (!n->is_leaf()){
        n = n->next(p);
    }
    return n;
}


void VertexTree :: add_vertex(Vertex vertex){

    unsigned new_vertex_index = vertex.get_index();
    this->vertices.push_back(vertex);
    this->vertex_map[new_vertex_index] = &this->vertices.back();

    vector<Face*> faces = find_all_circumcircles(vertex);
    vector<Edge> edge_list;

    for (auto face=faces.begin(); face<faces.end(); ++face){
        merge_edge_lists(edge_list, **face);
    }

    list<Face*> new_faces;
    for (auto edge=edge_list.begin(); edge<edge_list.end(); ++edge){ // every edge will make a new face

        vector<Vertex*> new_face_vertices = {this->vertex_map[new_vertex_index]};

        for(auto vertex_index=edge->index.begin(); vertex_index<edge->index.end(); ++vertex_index){
            new_face_vertices.push_back(this->vertex_map[*vertex_index]);
        }

        Face new_face(new_face_vertices);
        this->faces.push_back(new_face);
        link_face_to_edges(&this->faces.back());
        new_faces.push_back(&this->faces.back());
    }

    for (auto face=faces.begin(); face<faces.end(); ++face){
        (*face)->register_children(new_faces);
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

            //list<Face*> blacklist = candidates;
            //blacklist.push_back(n);

            //list<Face*> new_candidates = find_neighbouring_faces(*candidate, blacklist);
            // find the neighbours of this candidate
            // but exclude the candidates already present in the list
            //candidates.merge(new_candidates);
        }
    }

    return faces;
}

list<Face*> VertexTree :: find_neighbouring_faces(Face* face){
    list<Face*> blacklist; // an empty blacklist
    return find_neighbouring_faces(face, blacklist);
}

list<Face*> VertexTree :: find_neighbouring_faces(Face *face, list<Face*> blacklist){
    /*   Find all neighbours of a face, except if its index occures in the blacklist.
     */

    list<Face*> neighbours;

    Edge *edge;

    blacklist.push_back(face); // the face determining the neighbours is always in the black list. We do not want
    // this function to return itself as a neighbour, or else we will get stuck in an infinite loop

    vector<vector<unsigned> > edge_keys = get_edge_keys(*face);

    for (auto edge_key=edge_keys.begin(); edge_key<edge_keys.end(); ++edge_key){

        edge = get_edge(*edge_key);

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

void VertexTree :: merge_edge_lists(vector<Edge>& edge_list, Face face){

    // Calculate the edges of the face and find the one which is in the edge list. Delete that edge
    // from the edge list and insert the other edges from face into the list of edges.

   vector<vector<unsigned> > face_edge_keys = get_edge_keys(face);
   vector<vector<unsigned> > :: iterator face_edge_key;

   for (face_edge_key=face_edge_keys.begin();  face_edge_key<face_edge_keys.end();  ++face_edge_key){

       Edge *new_edge = get_edge(*face_edge_key);
       vector<Edge> :: const_iterator i = find(edge_list.begin(), edge_list.end(), *new_edge);

       if (i != edge_list.end()){
           edge_list.erase(i);
       }else{
           edge_list.push_back(*new_edge);
       }
   }
}


vector<vector<vector<double> > > VertexTree :: get_edges(){
    /*   Output shape is [n_edges, 2, ndim]
     *   TODO: MAKE THIS FUNCTION ND AGNOSTIC
     *
     */

    vector<unsigned> edge_index;
    Vertex *p1, *p2;
    vector<vector<vector<double> > > out;

    for (auto edge=this->edges.begin(); edge!=this->edges.end(); ++edge){

        if(edge->contains_root_vertices())
            continue;

        bool atleast_one_leaf = false;

        for(auto face=edge->member_faces.begin(); face<edge->member_faces.end(); ++face){
            if ((*face)->is_leaf())
            {
                atleast_one_leaf = true;
                break;
            }
        }
        if (!atleast_one_leaf)
            continue;

        edge_index = edge->index;

        p1 = this->vertex_map[edge_index[0]];
        p2 = this->vertex_map[edge_index[1]];

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
