#include <iostream>
#include <cstdio>

using namespace std;

class Complex {
  public:
    double real;
    double imag;
  public:
    Complex (double a, double b) : real(a), imag(b) {}
    Complex (double a) : real(a), imag(0) {}
    Complex () {}
    void conjugate();
    friend double sqr_abs(const Complex &a);
    const Complex operator += (const Complex &a);
    const Complex operator -= (const Complex &a);
    const Complex operator *= (const Complex &a);
    const Complex operator /= (const double a);
    const Complex operator /= (const Complex &a);
    friend const Complex operator - (const Complex &i);
    friend const Complex operator + (const Complex &a, const Complex &b);
    friend const Complex operator - (const Complex &a, const Complex &b);
    friend const Complex operator * (const Complex &a, const Complex &b);
    friend const Complex operator / (const Complex &a, const double b);
    friend const Complex operator / (const Complex &a, const Complex &b);
    friend bool operator == (const Complex &a, const Complex &b);
    friend ostream & operator << (ostream &out, const Complex &a);
    friend istream & operator >> (istream &in, Complex &a);
};

// unary -
const Complex operator - (const Complex &i) {
    return Complex (-i.real, -i.imag);
}

// binary +
const Complex operator + (const Complex &a, const Complex &b) {
    return Complex (a.real + b.real, a.imag + b.imag);
}

//binary +=
const Complex Complex :: operator += (const Complex &a) {
    real += a.real;
    imag += a.imag;
    return *this;
}

//binary -
const Complex operator - (const Complex &a, const Complex &b) {
    return Complex (a.real - b.real, a.imag - b.imag);
}

//binary -=
const Complex Complex :: operator -= (const Complex &a) {
    real -= a.real;
    imag -= a.imag;
    return *this;
}

//binary *
const Complex operator * (const Complex &a, const Complex &b) {
    return Complex (a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real);
}

//binary *=
const Complex Complex :: operator *= (const Complex &a) {
    real = real * a.real - imag * a.imag;
    imag = real * a.imag + imag * a.real;
    return *this;
}

//binary /
const Complex operator / (const Complex &a, const double b) {
    return Complex (a.real / b, a.imag / b);
}

//binary /=
const Complex Complex :: operator /= (const double a) {
    real /= a;
    imag /= a;
    return *this;
}

//one more binary /
const Complex operator / (const Complex &a, const Complex &b) {
    double sqr = sqr_abs(b);
    return Complex ((a.real * b.real + a.imag * b.imag) / sqr, (a.imag * b.real - b.imag * a.real) / sqr);
}

//one more binary /=
const Complex Complex :: operator /= (const Complex &a) {
    double tmp_real = real;
    double sqr = sqr_abs(a);
    real = (real * a.real + imag * a.imag) / sqr;
    imag = (imag * a.real - a.imag * tmp_real) / sqr;
    return *this;
}

//binary ==
bool operator == (const Complex &a, const Complex &b) {
    return a.real == b.real && a.imag == b.imag;
}

double sqr_abs (const Complex &a) {
    double real = a.real;
    double imag = a.imag;
    return real * real + imag * imag;
}

void Complex :: conjugate () {
    imag = -imag;
}

ostream &operator << (ostream &out, const Complex &a) {
    out << "( " << a.real << ", " << a.imag << " )\n";
    return out;
}

istream &operator >> (istream &in, Complex &a) {
    in >> a.real >> a.imag;
    return in;
}

int main () {
    Complex a(2, 1), b (2, 7), c;
    b.conjugate();
    (a = 5).conjugate();
    cout << a << b;
    return 0;
}
