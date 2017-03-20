/*  Note: Although the C++ STL implements a binary search, that implementation
 *  only gives back a boolean indicating if the element you are looking for is
 *  contained in the array. We need the index of the element if it exists.
 */

#include <vector>
#include <iostream>
#include <functional>

using namespace std;

template <class A> class LinkedList{

private:
    function<unsigned(A)> key_function;
    vector<A> elements;
    bool found=true;

    unsigned find_key(unsigned key){
        unsigned lower = 0;
        unsigned upper = this->elements.size();

        if (upper == 0){
            this->found = false;
            return 0;
        }

        unsigned index = unsigned(upper/2);
        unsigned index_key = key_function(this->elements[index]);
        this->found = true;

        while (index_key!=key){

            if (index_key<key){
                lower = index;
            }else{
                upper = index;
            }

            index = unsigned((upper+lower)/2);
            index_key = key_function(this->elements[index]);

            if (upper==lower+1){
                this->found = false;
                return upper;
            }
        }
        return index;
    }


public:
    LinkedList(function<unsigned(A)> key){
        this->key_function = key;
    }

    void add(A a){
        unsigned key_value = this->key_function(a);
        const unsigned index = find_key(key_value);

        if (this->found){
            cout << "Element with that key already exists in linked list";
        }{
            typename vector<A> :: iterator p = this->elements.begin() + index;
            this->elements.insert(p, a);
        }
    }

    A * find(unsigned key_value){
        unsigned index = find_key(key_value);

        if (this->found){
            return &(this->elements[index]);
        }else{
            return NULL;
        }
    }


};


#ifdef TEST_LINKED

class TestClass{
public:
    unsigned index;
    vector<double> values;
};

int main(){

    LinkedList<TestClass> l([](TestClass t){return t.index;});

    TestClass test1, test2, test3, test4, test5, test6, test7, test8;

    test1.index = 0;
    test1.values.push_back(2.0);
    test1.values.push_back(3.0);
    test1.values.push_back(3.5);

    test2.index = 10;
    test2.values.push_back(2.5);
    test2.values.push_back(3.5);
    test2.values.push_back(3.7);

    test3.index = 22;
    test3.values.push_back(1.0);
    test3.values.push_back(7.0);
    test3.values.push_back(9.5);

    test4.index = 18;
    test4.values.push_back(12.35);
    test4.values.push_back(30.53);
    test4.values.push_back(-13.7);

    test5.index = 4;
    test5.values.push_back(-2.0);
    test5.values.push_back(-3.0);
    test5.values.push_back(-3.5);

    test6.index = 50;
    test6.values.push_back(-2.5);
    test6.values.push_back(-3.5);
    test6.values.push_back(-3.7);

    test7.index = 16;
    test7.values.push_back(-1.0);
    test7.values.push_back(-7.0);
    test7.values.push_back(-9.5);


    l.add(test1);
    l.add(test2);
    l.add(test3);
    l.add(test4);
    l.add(test5);
    l.add(test6);
    l.add(test7);

    cout << "element 16" << endl;
    TestClass *t = l.find(16);
    vector<double> values = t->values;

    for(auto i=values.begin(); i<values.end(); ++i){
        cout << *i << endl;
    }

    t->values.push_back(-100.0);

    test8.index = 29;
    test8.values.push_back(-12.35);
    test8.values.push_back(-30.53);
    test8.values.push_back(13.7);
    l.add(test8);

    cout << "element 16 again" << endl;
    TestClass *t_again = l.find(16);
    vector<double> values_again = t_again->values;

    for(auto i=values_again.begin(); i<values_again.end(); ++i){
        cout << *i << endl;
    }

    cout << "element 29" << endl;
    TestClass *t_29 = l.find(29);
    vector<double> values_29 = t_29->values;

    for(auto i=values_29.begin(); i<values_29.end(); ++i){
        cout << *i << endl;
    }
}
#endif
