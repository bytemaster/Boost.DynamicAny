#include "any_ref.hpp"
#include <iostream>

void times2( double& v ) { v*=2; }
void print( const double& v ) { std::cerr<<v<<std::endl; }

int main( int argc, char** agv )
{
    int five = 5;
    int& f = five;

    boost::any_ref r(f);

    int& fiver = r;
    const int& cfiver = r;

    std::cerr<<&fiver<<" " << &five << " " << &f << std::endl;
    
    std::cerr<<fiver<<std::endl;
    std::cerr<<cfiver<<std::endl;

    five = 6;
    std::cerr<<f<<std::endl;
    std::cerr<<fiver<<std::endl;
    std::cerr<<cfiver<<std::endl;

    typedef int* intptr;


    double t = 5.5;
    r = t;

    double* d = r.ptr<double>();
    std::cerr<<"double*: "<<d<<std::endl;

    std::cerr<<d<<std::endl;
    std::cerr<<*d<<std::endl;

    times2(r);
    std::cerr<<"t: "<<t<<std::endl;
    print(r);

    const double cd = 22;
    boost::any_ref cr = cd;
    times2(cr);

    intptr i = &five;
    intptr& ir = i;

    r = i;

    
    intptr ip   = r;
    intptr& ipr = r;
    intptr* ipp = r.ptr<intptr>();
    std::cerr<<"ip: " <<ip<<"  ipr: "<<ipr<<" ipp: "<<ipp<<"\n";
    std::cerr<<"*ipp"<<*ipp<<std::endl;


    return 0;
}
