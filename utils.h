#ifndef UTILS_H
#define UTILS_H

template<typename T> T range(int begin, int end){

    T t;
    for (int i=begin; i<end; i++){
        t.push_back(i);
    }
    return t;
}

template<typename T> T fill(int begin, int end, double fill_value){

    T t;
    for (int i=begin; i<end; i++){
        t.push_back(fill_value);
    }
    return t;
}

#endif // UTILS_H
