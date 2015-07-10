// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#include "test_eq.h"

#include <castor.h>
#include <vector>
#include <includes/functional.h>

using namespace castor;
using namespace std;

namespace {
// structs A and B are inter convertible
struct A; 
struct B;

struct A {
    A() { }

    A(const B&)
    { }

    void operator=(const B&)
    { }

    bool operator==(const B&) const{
        return true;
    }
};

struct B {
    B() 
    { }

    B(const A&){}

    void operator =(const A&)
    { }

    bool operator==(const A&) const{
        return true;
    }
};

bool myStrCmp(const string&l , const string& r) {
    return l==r;
}

bool cmpAB(const A&, const B&) {
    return true;
}

inline 
bool cmpi(int i, int j) {
    return i==j;
}


int compute(int j, int k) {
    return j/k-1;
}

struct Compute {
    int operator()(int j, int k) {
        return j/k-1;
    }
};

struct Compute2 {
    int j; 
    Compute2(int j) : j(j) 
    {}
    int apply(int k) const {
        return j/k-1;
    }
};

struct Functor6 {
    typedef int result_type;

    int operator() (void)                                               { return 0; }
    int operator() (int i1)                                             { return i1; }
    int operator() (int i1, int i2)                                     { return i1+i2; }
    int operator() (int i1, int i2, int i3)                             { return i1+i2+i3; }
    int operator() (int i1, int i2, int i3, int i4)                     { return i1+i2+i3+i4; }
    int operator() (int i1, int i2, int i3, int i4, int i5)             { return i1+i2+i3+i4+i5; }
    int operator() (int i1, int i2, int i3, int i4, int i5, int i6)     { return i1+i2+i3+i4+i5+i6; }

    int zero (void)                                                 { return 0; }

    int method (void)                                               { return 0; }
    int method (int i1)                                             { return i1; }
    int method (int i1, int i2)                                     { return i1+i2; }
    int method (int i1, int i2, int i3)                             { return i1+i2+i3; }
    int method (int i1, int i2, int i3, int i4)                     { return i1+i2+i3+i4; }
    int method (int i1, int i2, int i3, int i4, int i5)             { return i1+i2+i3+i4+i5; }
    int method (int i1, int i2, int i3, int i4, int i5, int i6)     { return i1+i2+i3+i4+i5+i6; }

    int cmethod (int i1, int i2) const                              { return i1+i2; }

	int max(int i, int j) { return (i>j)?i:j; }
    double max(double i, double j) { return (i>j)?i:j; }
};

int getN(void)                                                                       { return 0; }
int getN(int /*i1*/)                                                                 { return 1; }
int getN(int /*i1*/, int /*i2*/)                                                     { return 2; }
int getN(int /*i1*/, int /*i2*/, int /*i3*/)                                         { return 3; }
int getN(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/)                             { return 4; }
int getN(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/, int /*i5*/)                 { return 5; }
int getN(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/, int /*i5*/, int /*i6*/)     { return 6; }


int getN2(void)                                                                                            { return 0; }
int getN2(int /*i1*/)                                                                                      { return 1; }
int getN2(int /*i1*/, int /*i2*/)                                                                          { return 2; }
int getN2(int /*i1*/, int /*i2*/, int /*i3*/)                                                              { return 3; }
int getN2(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/)                                                  { return 4; }
int getN2(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/, int /*i5*/)                                      { return 5; }
int getN2(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/, int /*i5*/, int /*i6*/)                          { return 6; }

double getN2(double /*i1*/)                                                                                { return -1; }
double getN2(double /*i1*/, double /*i2*/)                                                                 { return -2; }
double getN2(double /*i1*/, double /*i2*/, double /*i3*/)                                                  { return -3; }
double getN2(double /*i1*/, double /*i2*/, double /*i3*/, double /*i4*/)                                   { return -4; }
double getN2(double /*i1*/, double /*i2*/, double /*i3*/, double /*i4*/, double /*i5*/)                    { return -5; }
double getN2(double /*i1*/, double /*i2*/, double /*i3*/, double /*i4*/, double /*i5*/, double /*i6*/)     { return -6; }



} // namespace{}


//--------------------------------------------------------
// Test : EQ
//--------------------------------------------------------

void test_eq() {
	lref<string> ll , lr;
    string l, r;
    eq("l",r)();
    eq("l",r,&myStrCmp)();
    eq(l,"r")();
    eq(l,"r",&myStrCmp)();
    eq(l,r)();
    eq(l,r,&myStrCmp)();
    eq(ll,r)();
    eq(ll,r,&myStrCmp)();
    eq(ll,"r")();
    eq(ll,"r",&myStrCmp)();
    eq("l",lr)();
    eq("l",lr,&myStrCmp)();
    eq(l,lr)();
    eq(l,lr,&myStrCmp)();
    eq(ll,lr)();
    eq(ll,lr,&myStrCmp)();

    lref<A> la = A(); lref<B> lb; A a; B b;
    eq(la,lb)();
    eq(a,b)();
    eq(a,b, &cmpAB)();
    eq(la,b)();
    eq(la,b,&cmpAB)();
    eq(a,lb)();
    eq(a,lb,&cmpAB)();
    eq(la,la)();
    eq(la,la,&cmpAB)();
}

void test_eq_values() {
    relation r = eq<int>( 2, 2 );
	int i=0;
	for(; r(); ++i);
	if(i!=1)
		throw "failed test_eq_values 1";

    r = eq<int>( 2, 3 );
    int j=0;
	for(; r(); ++j);
	if(j!=0)
		throw "failed test_eq_values 2";
}

void test_eq_with_coll() {
    {
    int a[] = { 1,2,3,4 };
    vector<int> v (a+0, a+4);
    lref<vector<int> > rv;
    relation r = eq(rv,v);
    int i=0;
    for(; r(); ++i) {
        if(v!=(*rv))
            throw "failed test_eq_with_coll 1";
    }
    if(i!=1 || rv.defined())
        throw "failed test_eq_with_coll 1";
    }
    {
    int b[] = { 1,2,3,4 };
    vector<int> v2 (b+0, b+4);
    lref<vector<int> > rv2;
    if(eq(rv2,v2)()) {
        if(v2!=(*rv2))
            throw "failed test_eq_with_coll 2";
    }
    else
        throw "failed test_eq_with_coll 2";
    }
}

void test_eq_seq() {
    { // generate container with elements
    int ai[] = {1,2,3};
    lref<vector<int> > vi;
    relation r = eq_seq(vi, ai, ai+3);
    int i=0;
    for(; r(); ++i) {
        if(i>0 || (*vi).at(0)!=1 || (*vi).at(1)!=2 || (*vi).at(2)!=3 )
            throw "failed test_eq_seq 1";
    }
    if(i!=1 || vi.defined())
	    throw "failed test_eq_seq 1";
    }
    {// compare using custom comparator
    const int ai[] = {1,2,3};
    lref<vector<int> > vi = vector<int>(ai+0, ai+3);

    relation r = eq_seq(vi, ai, ai+3, &cmpi);
	if(!r())
        throw "failed test_eq_seq 2";
    }
    {
    const int bi[] = {1,2,3};
    lref<vector<int> > vj = vector<int>(bi, bi+3);
    int ci[] = {1,2,3};
    relation r2 = eq_seq(vj, ci, ci+3);
	int j=0;
	for(; r2(); ++j);
	if(j!=1 || !vj.defined())
		throw "failed test_eq_seq 3";
    }
    {
    const int di[] = {1,2,3};
    lref<vector<int> > vk = vector<int>(di, di+3);
    int ei[] = {3,2,3};
    relation r3 = eq_seq(vk, ei, ei+3);
	int k=0;
	for(; r3(); ++k);
	if(k!=0 || !vk.defined())
		throw "failed test_eq_seq 4";
    }
    { // with const_iterator
    const int di[] = {1,2,3};
    lref<vector<int> > vk = vector<int>(di, di+3);
    const vector<int> cv(di, di+3);
    relation r3 = eq_seq(vk, cv.begin(), cv.end());
	int k=0;
	for(; r3(); ++k);
	if(k!=1 || !vk.defined())
		throw "failed test_eq_seq 5";
    }
}

void test_eq_f() {
    { // unify .. should pass
    lref<int> refi;
    relation r = eq_f( refi, detail::bind<int>(plus<int>(), 6, 3) );
	int i=0;
	for(; r(); ++i) {
        if(i>0 || *refi!=6+3)
            throw "failed test_eq_f 1";
    }
	if(i!=1 || refi.defined())
		throw "failed test_eq_f 1";
    }
    { // comparison .. should pass
    lref<int> refk = 9;
    relation r2 = eq_f( refk, detail::bind<int>(plus<int>(), 6, 3) );
	int k=0;
	for(; r2(); ++k) {
        if(k>0 || *refk!=6+3)
            throw "failed test_eq_f 2";
    }
	if(k!=1 || !refk.defined())
		throw "failed test_eq_f 2";
    }
    { // comparison .. should fail
    lref<int> refj = 1;
    relation r3 = eq_f( refj, detail::bind<int>(plus<int>(), 6, 3) );
	int j=0;
	for(; r3(); ++j);
	if(j!=0 || !refj.defined())
		throw "failed test_eq_f 3";
    }
    { // regular functions
    lref<int> li, lj, lk;
    relation r = eq(lj,6) && eq(lk,2) && eq_f(li,&compute, lj, lk);
    if(!r() && *li!=2)
       throw "failed test_eq_f 4";

    if(!eq_f<int>(0,getN)())
        throw "failed test_eq_f 4";
    if(!eq_f<int>(1,getN,1)())
        throw "failed test_eq_f 4";
    if(!eq_f<int>(2,getN,1,1)())
        throw "failed test_eq_f 4";
    if(!eq_f<int>(3,getN,1,1,1)())
        throw "failed test_eq_f 4";
    if(!eq_f<int>(4,getN,1,1,1,1)())
        throw "failed test_eq_f 4";
    if(!eq_f<int>(5,getN,1,1,1,1,1)())
        throw "failed test_eq_f 4";
    if(!eq_f<int>(6,getN,1,1,1,1,1,1)())
        throw "failed test_eq_f 4";

    }
    { // regular functions - with overload ambiguity
    if(!eq_f<int>(0,getN2)())
        throw "failed test_eq_f 5";
    if(!eq_f<int,int>(1,getN2,1)())
        throw "failed test_eq_f 5";
    if(!eq_f<int,int,int>(2,getN2,1,1)())
        throw "failed test_eq_f 5";
    if(!eq_f<int,int,int,int>(3,getN2,1,1,1)())
        throw "failed test_eq_f 5";
    if(!eq_f<int,int,int,int,int>(4,getN2,1,1,1,1)())
        throw "failed test_eq_f 5";
    if(!eq_f<int,int,int,int,int,int>(5,getN2,1,1,1,1,1)())
        throw "failed test_eq_f 5";
    if(!eq_f<int,int,int,int,int,int,int>(6,getN2,1,1,1,1,1,1)())
        throw "failed test_eq_f 5";

    }
    { // function objects -  compare mode
    lref<int> li, lj, lk;
    relation r = eq(lj,6) && eq(lk,2) && eq_f(li, Compute(),lj,lk);
    if(!r() && *li!=2)
       throw "failed test_eq_f 6";
    if(!eq_f<int>(0,Functor6())())
        throw "failed test_eq_f 6";
    Functor6 f;
    if(!eq_f<int>(1,f,1)())
        throw "failed test_eq_f 6";
    if(!eq_f<int>(2,Functor6(),1,1)())
        throw "failed test_eq_f 6";
    if(!eq_f<int>(3,f,1,1,1)())
        throw "failed test_eq_f 6";
    if(!eq_f<int>(4,Functor6(),1,1,1,1)())
        throw "failed test_eq_f 6";
    const Functor6 cf=Functor6(); lref<int> i=1; lref<const int> ci=1;
    if(!eq_f<int>(5,cf,1,1,1,1,ci)())
        throw "failed test_eq_f 6";
    if(!eq_f<int>(6,Functor6(),1,1,1,i,1,1)())
        throw "failed test_eq_f 6";
    if( eq_f<int>(7,Functor6(),1,1,1,i,1,ci)())
        throw "failed test_eq_f 6";
    }
    {
    // function objects - generate mode
    lref<int> i; const lref<int> ci;
    if(!eq_f(i,Functor6())()  ||  *i!=0 )
        throw "failed test_eq_f 7";
    i.reset();
    Functor6 f;
    if(!eq_f(i,f,1)()  ||  *i!=1 )
        throw "failed test_eq_f 7";
    i.reset();
    if(!eq_f(i,Functor6(),1,1)()  ||  *i!=2 )
        throw "failed test_eq_f 7";
    i.reset();
    if(!eq_f(i,f,1,1,1)()  ||  *i!=3 )
        throw "failed test_eq_f 7";
    if(!eq_f(ci,Functor6(),1,1,1,1)()  ||  *ci!=4 )
        throw "failed test_eq_f 7";
    i.reset();
    const Functor6 cf=Functor6(); lref<int> i2=1; const lref<const int> ci2=1;
    if(!eq_f(i,cf,1,1,1,1,ci2)() || *i!=5 )
        throw "failed test_eq_f 7";
    i.reset();
    if(!eq_f(i,Functor6(),1,1,1,i2,1,1)()  ||  *i!=6 )
        throw "failed test_eq_f 7";
    i=0;
    if( eq_f(i,Functor6(),1,1,1,i2,1,ci2)() )
        throw "failed test_eq_f 7";
    }
    { // ILE
    lref<int> li, lj, lk;
    relation r = eq(lj,6) && eq(lk,2) && eq_f(li,lj/lk-1);
    if(!r() && *li!=2)
       throw "failed test_eq_f 8";
    }
}

namespace {
struct Base {
    int b;
    Base():b(0) {}
    int foo(){ return 1; }
    int foo_base(){ return 2; }
};
struct Derived : Base {
    int d;
    Derived() : d(10), Base() {}
    int foo() { return 3; }
    int foo_derived(){ return 4; }
};
}

void test_eq_mf() {
    {
    lref<int> refi;
    lref<Compute2> comp2 = Compute2(6);
    lref<int> two=2;
    relation r = eq_mf(refi, comp2, &Compute2::apply, two);
	int i=0;
	for(; r(); ++i) {
        if(i>0 || *refi!=2)
            throw "failed test_eq_mf 1";
    }
	if(i!=1 || refi.defined())
		throw "failed test_eq_mf 1";
    }
    {
    lref<int> refi;
    lref<Compute2> comp2 = Compute2(6);
    lref<int> li=2;
    relation r = eq_mf(refi, comp2, &Compute2::apply, li);
	int i=0;
	for(; r(); ++i) {
        if(i>0 || *refi!=2)
            throw "failed test_eq_mf 2";
    }
	if(i!=1 || refi.defined())
		throw "failed test_eq_mf 2";
    }
    {
    // generate mode
    lref<int> i; const lref<int> ci;
    lref<Functor6> f = Functor6();
    if(!eq_mf(i,f,&Functor6::zero)()  ||  *i!=0 )
        throw "failed test_eq_mf 3";
    if(!eq_mf(i,f,&Functor6::method)()  ||  *i!=0 )
        throw "failed test_eq_mf 3";
    i.reset();
    if(!eq_mf(i,f,&Functor6::method,1)()  ||  *i!=1 )
        throw "failed test_eq_mf 3";
    i.reset();
    if(!eq_mf(i,f,&Functor6::method,1,1)()  ||  *i!=2 )
        throw "failed test_eq_mf 3";
    i.reset();
    if(!eq_mf(i,f,&Functor6::method,1,1,1)()  ||  *i!=3 )
        throw "failed test_eq_mf 3";
    if(!eq_mf(ci,f,&Functor6::method,1,1,1,1)()  ||  *ci!=4 )
        throw "failed test_eq_mf 3";
    i.reset();
    lref<int> i2=1; const lref<const int> ci2=1;
    if(!eq_mf(i,f,&Functor6::method,1,1,1,1,ci2)() || *i!=5 )
        throw "failed test_eq_mf 3";
    i.reset();
    if(!eq_mf(i,f,&Functor6::method,1,1,1,i2,1,1)()  ||  *i!=6 )
        throw "failed test_eq_mf 3";
    i.reset();
    // const method
    if(!eq_mf<int,Functor6>(i,f,&Functor6::cmethod,1,1)() || *i!=2 )
        throw "failed test_eq_mf 3";
    // const Obj, const method
#ifndef __GNUG__
    lref<const Functor6> fc= Functor6();
    if(!eq_mf<int,const Functor6>(i,fc,&Functor6::cmethod,1,1)() || *i!=2 )
        throw "failed test_eq_mf 3";
#endif
	// const lref, const method -- turn the const lref into plain lref and then use it
    const lref<Functor6> cf= Functor6();
    f = cf;
    if(!eq_mf<int,Functor6>(i,f,&Functor6::cmethod,1,1)() || *i!=2 )
        throw "failed test_eq_mf 3";
    i=0;
    if( eq_mf(i,f,&Functor6::method,1,1,1,i2,1,ci2)() )
        throw "failed test_eq_mf 3";
    }
    { // member function of base type
        lref<int> one=1, two=2, three=3, four=4;
        lref<Derived> d = Derived();
        if(!eq_mf(one,d,&Base::foo)())
            throw "failed test_eq_mf 4";
        if(!eq_mf(three,d,&Derived::foo)())
            throw "failed test_eq_mf 4";

        if(!eq_mf(two,d,&Base::foo_base)())
            throw "failed test_eq_mf 4";
        if(!eq_mf(two,d,&Derived::foo_base)())
            throw "failed test_eq_mf 4";

        if(!eq_mf(four,d,&Derived::foo_derived)())
            throw "failed test_eq_mf 4";
    }
}

struct employee {
    string name;
    int salary;
    employee(string name, int salary) : name(name), salary(salary)
    { }
    bool operator == (const employee& rhs) const {
        return name==rhs.name && salary==rhs.salary;
    }
};

void test_eq_mem() {
    { // compare
    lref<pair<int,string> > p = pair<int,string>(1,"Roshan");
    lref<int> one=1;
    relation r = eq_mem(one, p,&pair<int,string>::first);
	int i=0;
	for(; r(); ++i) {
        if(i>0 || *one!=1)
            throw "failed test_eq_mem 1";
    }
	if(i!=1 || !one.defined())
		throw "failed test_eq_mem 1";
    }
    { // generate
    lref<pair<int,string> > p = pair<int,string>(1,"Roshan");
    lref<string> name;
    relation r = eq_mem(name, p,&pair<int,string>::second);
	int i=0;
	for(; r(); ++i) {
        if(i>0 || *name!="Roshan")
            throw "failed test_eq_mem 2";
    }
	if(i!=1 || name.defined())
		throw "failed test_eq_mem 2";
    }
    {
    list<employee> employees; employees.push_back(employee("Roshan",50)); employees.push_back(employee("Roshan",90));
    lref<int> salary;
    lref<employee> e;
    relation salaries = item(e, employees.begin(), employees.end()) && eq_mem(salary,e,&employee::salary);
    int total=0;
    while(salaries())
      total+=*salary;
    if(total!=140)
        throw "failed test_eq_mem 3";
    }
    { // with const Obj- explicit template args
    lref<const pair<int,string> > p = pair<int,string>(1,"Roshan");
    relation r = eq_mem<int,const pair<int,string> >(1, p,&pair<int,string>::first);
	int i=0;
	for(; r(); ++i);
	if(i!=1)
		throw "failed test_eq_mem 4";
    }
    {// members of base type
    lref<Derived> d = Derived();
    if(!eq_mem<int>(0,d, &Base::b)())
        throw "failed test_eq_mem 5";
    if(!eq_mem<int>(0,d, &Derived::b)())
        throw "failed test_eq_mem 5";
    if(!eq_mem<int>(10,d, &Derived::d)())
        throw "failed test_eq_mem 5";
    }
    { // on "string" literals
    lref<pair<string,string> > p = make_pair("Castor","C++");
    if(!eq_mem<string>("Castor",p, &pair<string,string>::first)())
        throw "failed test_eq_mem 6";
    if(!eq_mem("Castor",p, &pair<string,string>::first)())
        throw "failed test_eq_mem 6";
    }
}

