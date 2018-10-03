#include <iostream>

template <typename T>
class smart_pointer {
    T *my_obj;
public:
    smart_pointer(T *obj) : my_obj(obj) {};
    ~smart_pointer() { delete my_obj; }
    T * operator -> () { return my_obj; }
    T & operator * () { return *my_obj; }
};

class my_class {
public:
    my_class() { 
	cout << "constructor\n";
    }
    ~my_class() { 
	cout << "destructor\n";
	throw 1;
    }
    void my_func() { cout << "got it\n"; }
};

void f() {
    smart_pointer<my_class> ptr(new my_class);
    try {
	throw 1;
    } catch(int e) {}
    cout << "code after thrown\n";
}

void f1() {
    try {
	f();
    } catch(int e) { cout << "trouble\n"; }
    cout << "code after thrown\n";
}

int main() {
    try {
	f1();
    } catch(int e) { cout << "exception catched\n"; }
    return 0;
}
