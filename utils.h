#ifndef UTILS_H
#define UTILS_H

template<typename T> T range(int begin, int end){

    T t;
    for (int i=begin; i<end; i++){
        t.push_back(i);
    }
    return t;
}

#endif // UTILS_H
