// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#include <castor.h>
#include <iostream>
#include <vector>
#include <set>
#include <map>

using namespace castor;
using namespace std;

namespace {
int getNIntResult=0;
double getNDblResult=0;

int getN(void)                                               { return getNIntResult=0; }
int getN(int /*i1*/)                                             { return getNIntResult=1; }
int getN(int /*i1*/, int /*i2*/)                                     { return getNIntResult=2; }
int getN(int /*i1*/, int /*i2*/, int /*i3*/)                             { return getNIntResult=3; }
int getN(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/)                     { return getNIntResult=4; }
int getN(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/, int /*i5*/)             { return getNIntResult=5; }
int getN(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/, int /*i5*/, int /*i6*/)     { return getNIntResult=6; }

double getN(double /*i1*/)                                                            { return getNDblResult=-1; }
double getN(double /*i1*/, double /*i2*/)                                                 { return getNDblResult=-2; }
double getN(double /*i1*/, double /*i2*/, double /*i3*/)                                      { return getNDblResult=-3; }
double getN(double /*i1*/, double /*i2*/, double /*i3*/, double /*i4*/)                           { return getNDblResult=-4; }
double getN(double /*i1*/, double /*i2*/, double /*i3*/, double /*i4*/, double /*i5*/)                { return getNDblResult=-5; }
double getN(double /*i1*/, double /*i2*/, double /*i3*/, double /*i4*/, double /*i5*/, double /*i6*/)     { return getNDblResult=-6; }


int getN2Result=0;
double getN2(double /*i1*/)                                                            { return getN2Result=-1; }
double getN2(double /*i1*/, lref<double> /*i2*/)                                           { return getN2Result=-2; }
double getN2(lref<double> /*i1*/, double /*i2*/, double /*i3*/)                                { return getN2Result=-3; }
double getN2(double /*i1*/, double /*i2*/, double /*i3*/, double /*i4*/)                           { return getN2Result=-4; }
double getN2(double /*i1*/, double /*i2*/, double /*i3*/, double /*i4*/, double /*i5*/)                { return getN2Result=-5; }
double getN2(double /*i1*/, double /*i2*/, double /*i3*/, double /*i4*/, double /*i5*/, double /*i6*/)     { return getN2Result=-6; }


struct Functor6 {
    typedef int result_type;
    static int result;

    int operator() (void)                                               { return result= 0; }
    int operator() (int i1)                                             { return result= i1; }
    int operator() (int i1, int i2)                                     { return result= i1+i2; }
    int operator() (int i1, int i2, int i3)                             { return result= i1+i2+i3; }
    int operator() (int i1, int i2, int i3, int i4)                     { return result= i1+i2+i3+i4; }
    int operator() (int i1, int i2, int i3, int i4, int i5)             { return result= i1+i2+i3+i4+i5; }
    int operator() (int i1, int i2, int i3, int i4, int i5, int i6)     { return result= i1+i2+i3+i4+i5+i6; }

    int method (void)                                               { return result= 0; }
    int method (int i1)                                             { return result= i1; }
    int method (int i1, int i2)                                     { return result= i1+i2; }
    int method (int i1, int i2, int i3)                             { return result= i1+i2+i3; }
    int method (int i1, int i2, int i3, int i4)                     { return result= i1+i2+i3+i4; }
    int method (int i1, int i2, int i3, int i4, int i5)             { return result= i1+i2+i3+i4+i5; }
    int method (int i1, int i2, int i3, int i4, int i5, int i6)     { return result= i1+i2+i3+i4+i5+i6; }

	int max(int i, int j) { return (i>j)?i:j; }
    double max(double i, double j) { return (i>j)?i:j; }

	int cmax(int i, int j) const { return (i>j)?i:j; }

};

int Functor6::result=6;

}  // namespace {}


void test_eval() {
    {
    lref<int> i1=1;
    Functor6 f;
    eval(f)();
    if(f.result!=0)
        throw "failed test test_eval 1";
    eval(f,1)();
    if(f.result!=1)
        throw "failed test test_eval 1";
	eval(f,i1,2)();
	if(f.result!=3)
		throw "failed test test_eval 1";
	eval(f,1,2,3)();
	if(f.result!=6)
		throw "failed test test_eval 1";
	eval(f,1,2,3,4)();
	if(f.result!=10)
		throw "failed test test_eval 1";
	eval(f,i1,2,3,4,5)();
	if(f.result!=15)
		throw "failed test test_eval 1";
	eval(f,1,2,3,4,5,6)();
	if(f.result!=21)
		throw "failed test test_eval 1";
	}

	{//2
	lref<int> i1=1;
	eval(getN)();
	if(getNIntResult!=0)
		throw "failed test test_eval 2";
#ifdef __BCPLUSPLUS__
	eval<int,int,int>(getN,1)();
#else
	eval<int,int>(getN,1)();
#endif
	if(getNIntResult!=1)
		throw "failed test test_eval 2";


#ifdef __BCPLUSPLUS__
	eval<int,int,int,lref<int>,int>(getN,i1,2)();
#else
	eval<int,int,int>(getN,i1,2)();
#endif
	if(getNIntResult!=2)
		throw "failed test test_eval 2";

#ifdef __BCPLUSPLUS__
	eval<int,int,int,int,int,int,int>(getN,1,2,3)();
#else
	eval<int,int,int,int>(getN,1,2,3)();
#endif
	if(getNIntResult!=3)
		throw "failed test test_eval 2";

#ifdef __BCPLUSPLUS__
	eval<int,int,int,int,int,int,int,int,int>(getN,1,2,3,4)();
#else
	eval<int,int,int,int>(getN,1,2,3,4)();
#endif
	if(getNIntResult!=4)
		throw "failed test test_eval 2";

#ifdef __BCPLUSPLUS__
	eval<int,int,int,int,int,int, lref<int>,int,int,int,int>(getN,i1,2,3,4,5)();
#else
	eval<int,int,int,int,int,int>(getN,i1,2,3,4,5)();
#endif
	if(getNIntResult!=5)
		throw "failed test test_eval 2";

#ifdef __BCPLUSPLUS__
	eval<int,int,int,int,int,int,int, int,int,int,int,int,int>(getN,1,2,3,4,5,6)();
#else
	eval<int,int,int,int,int,int,int>(getN,1,2,3,4,5,6)();
#endif
	if(getNIntResult!=6)
		throw "failed test test_eval 2";
	}


	{//3
	lref<int> i1=1;
#ifdef __BCPLUSPLUS__
	eval<double,double,int>(getN,1)();
#else
	eval<double,double>(getN,1)();
#endif
	if(getNDblResult!=-1)
		throw "failed test test_eval 3";

#ifdef __BCPLUSPLUS__
	eval<double,double,double,lref<int>,int>(getN,i1,2)();
#else
	eval<double,double,double>(getN,i1,2)();
#endif
	if(getNDblResult!=-2)
		throw "failed test test_eval 3";

#ifdef __BCPLUSPLUS__
	eval<double,double,double,double,int,int,int>(getN,1,2,3)();
#else
	eval<double,double,double,double>(getN,1,2,3)();
#endif
	if(getNDblResult!=-3)
		throw "failed test test_eval 3";

#ifdef __BCPLUSPLUS__
	eval<double,double,double,double,double,int,int,int,int>(getN,1,2,3,4)();
#else
	eval<double,double,double,double,double>(getN,1,2,3,4)();
#endif
	if(getNDblResult!=-4)
		throw "failed test test_eval 3";

#ifdef __BCPLUSPLUS__
	eval<double,double,double,double,double,double,lref<int>,int,int,int,int>(getN,i1,2,3,4,5)();
#else
	eval<double,double,double,double,double,double>(getN,i1,2,3,4,5)();
#endif
	if(getNDblResult!=-5)
		throw "failed test test_eval 3";

#ifdef __BCPLUSPLUS__
	eval<double,double,double,double,double,double,double,int,int,int,int,int,int>(getN,1,2,3,4,5,6)();
#else
	eval<double,double,double,double,double,double,double>(getN,1,2,3,4,5,6)();
#endif
	if(getNDblResult!=-6)
        throw "failed test test_eval 3";
    }
	{
    lref<int> i1=1;
    eval(getN2,1)();
    if(getN2Result!=-1)
        throw "failed test test_eval 4";
    eval(getN2,i1,2)();
    if(getN2Result!=-2)
        throw "failed test test_eval 4";
    eval(getN2,1,2,3)();
    if(getN2Result!=-3)
        throw "failed test test_eval 4";
    eval(getN2,1,2,3,4)();
    if(getN2Result!=-4)
        throw "failed test test_eval 4";
    eval(getN2,i1,2,3,4,5)();
    if(getN2Result!=-5)
        throw "failed test test_eval 4";
    eval(getN2,1,2,3,4,5,6)();
    if(getN2Result!=-6)
        throw "failed test test_eval 4";
    }
}


namespace {
struct Base {
    int b;
    Base():b(0) {}
    void foo(int){ }
    void foo_base(int,int){ }
};
struct Derived : Base {
    int d;
    Derived() : d(10), Base() {}
    void foo(int) { }
    void foo_derived(int,int,int){ }
};
}

void test_eval_mf() {
	{
	lref<int> i1=1;
	lref<Functor6> f = Functor6();

#ifdef __BCPLUSPLUS__
	int(Functor6::* m0)(void) = &Functor6::method;
	eval_mf(f,m0)();
#else
	eval_mf(f,&Functor6::method)();
#endif
	if(f->result!=0)
		throw "failed test test_eval_mf_mf 1";

#ifdef __BCPLUSPLUS__
	int(Functor6::* m1)(int) = &Functor6::method;
	eval_mf(f,m1,1)();
#else
	eval_mf(f,&Functor6::method,1)();
#endif
	if(f->result!=1)
		throw "failed test test_eval_mf_mf 1";

#ifdef __BCPLUSPLUS__
	int(Functor6::* m2)(int,int) = &Functor6::method;
	eval_mf(f,m2,i1,2)();
#else
	eval_mf(f,&Functor6::method,i1,2)();
#endif
	if(f->result!=3)
		throw "failed test test_eval_mf_mf 1";

#ifdef __BCPLUSPLUS__
	int(Functor6::* m3)(int,int,int) = &Functor6::method;
	eval_mf(f,m3,1,2,3)();
#else
	eval_mf(f,&Functor6::method,1,2,3)();
#endif
	if(f->result!=6)
		throw "failed test test_eval_mf_mf 1";

#ifdef __BCPLUSPLUS__
	int(Functor6::* m4)(int,int,int,int) = &Functor6::method;
	eval_mf(f,m4,1,2,3,4)();
#else
	eval_mf(f,&Functor6::method,1,2,3,4)();
#endif
	if(f->result!=10)
		throw "failed test test_eval_mf_mf 1";

#ifdef __BCPLUSPLUS__
	int(Functor6::* m5)(int,int,int,int,int) = &Functor6::method;
	eval_mf(f,m5,i1,2,3,4,5)();
#else
	eval_mf(f,&Functor6::method,i1,2,3,4,5)();
#endif
	if(f->result!=15)
		throw "failed test test_eval_mf_mf 1";

#ifdef __BCPLUSPLUS__
	int(Functor6::* m6)(int,int,int,int,int,int) = &Functor6::method;
	eval_mf(f,m6,1,2,3,4,5,6)();
#else
	eval_mf(f,&Functor6::method,1,2,3,4,5,6)();
#endif
	if(f->result!=21)
		throw "failed test test_eval_mf 1";
	}
    {
	lref<Functor6> f = Functor6();
#ifdef __BCPLUSPLUS__
	int(Functor6::* fm)(int,int) = &Functor6::max;
	eval_mf(f,fm,1,2)();
#else
	eval_mf<int>(f,&Functor6::max,1,2)();
#endif

#ifdef __BCPLUSPLUS__
	double(Functor6::* fmd)(double,double) = &Functor6::max;
	eval_mf<double>(f,fmd,2.0,3.0)();
#else
	eval_mf<double>(f,&Functor6::max,2.0,3.0)();
#endif
    eval_mf<int>(f,&Functor6::cmax,1,2)();
    }
    { // on member functions from base & derived type
        lref<Derived> d = Derived();
        if(!eval_mf(d,&Base::foo,1)())
            throw "failed test_eval_mf 2";
        if(!eval_mf(d,&Derived::foo,1)())
            throw "failed test_eval_mf 2";
        if(!eval_mf(d,&Derived::foo_base,1,2)())
            throw "failed test_eval_mf 2";
        if(!eval_mf(d,&Derived::foo_derived,1,2,3)())
            throw "failed test_eval_mf 2";
    }
}



//--------------------------------------------------------
//  Tests  : begin and end relations to obtain iterators to lref<CollectionT>
//--------------------------------------------------------
void test_begin_end() {
    {
    list<int> li; li.push_back(1); li.push_back(2); li.push_back(3);
    lref<list<int> > lrli;
    lref<list<int>::iterator> itrB, itrE;
    relation r = eq(lrli, li) && begin(lrli,itrB) && end(lrli,itrE);
    if(!r())
        throw "failed test_begin_end 1";
    if(!equal(*itrB, *itrE, li.begin()))
        throw "failed test_begin_end 1";
    }
    { // const container with const_iterator
        lref<const string> s = "castor";
        string::const_iterator b = s->begin(), e = s->end();
        lref<string::const_iterator> lb, le;
        relation r = begin(s,lb) && end(s,le) && eq(b,lb) && eq(e,le);
        if(!r())
            throw "failed test_begin_end 2";
    }
    { // non const container with const_iterator
        lref<string> s = "castor";
        string::const_iterator b = s->begin(), e = s->end();
        lref<string::const_iterator> lb, le;
        relation r = begin(s,lb) && end(s,le) && eq(b,lb) && eq(e,le);
        if(!r())
            throw "failed test_begin_end 3";
    }
}



//--------------------------------------------------------
//  Tests  : Merge
//--------------------------------------------------------
void test_merge() {
    { // generate : left is empty vector
    int ai[] = {1,2,3};
    lref<vector<int> >  vi, vj = vector<int>(ai, ai+0);
    lref<vector<int> > m;
    relation r = castor::merge<vector<int> >(vi,vj,m);
    int i=0;
    for(; r(); ++i) {
        if((*m)!=(*vj))
            throw "failed test_merge 1";
    }
    if(i!=1)
        throw "failed test_merge 1";
    }

    { // generate : left&right are non-empty vector
    int aj[] ={1,3};
    int bj[]={2,4};
    lref</*const*/ vector<int> > rlj= vector<int>(aj+0, aj+2) , rrj= vector<int>(bj+0,bj+2); 
    lref<vector<int> > m2; 
    relation rj =  merge<vector<int> >(rlj,rrj,m2);
    int j=0;
    for(; rj(); ++j) {
        int expect[] = {1,2,3,4};
        vector<int> expected(expect+0, expect+4);
        if(*m2!=expected)
            throw "failed test_merge 2";
    }
    if(j!=1)
        throw "failed test_merge 2";
    }

    { // compare
    int ak[] ={1,3}, bk[]={2,4};
    int expect[] = {1,2,3,4};
    vector<int> expected3(expect+0, expect+4);

    lref</*const*/ vector<int> > rlk= vector<int>(ak+0, ak+2) , rrk= vector<int>(bk+0,bk+2);
    lref<vector<int> > m3; 
    relation rk =  merge<vector<int> >(rlk,rrk,m3);
    int k=0;
    for(; rk(); ++k) {
        if(*m3!=expected3)
            throw "failed test_merge 3";
    }
    if(k!=1)
        throw "failed test_merge 3";
    }

    { // generate first seq given a seq and a
    int ak[] ={1,3}, bk[]={2,4};
    int expect[] = {1,2,3,4};
    vector<int> expected3(expect+0, expect+4);

    lref</*const*/ vector<int> > rlk= vector<int>(ak+0, ak+2) , rrk= vector<int>(bk+0,bk+2);
    lref<vector<int> > m3; 
    relation rk =  merge<vector<int> >(rlk,rrk,m3);
    int k=0;
    for(; rk(); ++k) {
        if(*m3!=expected3)
            throw "failed test_merge 3";
    }
    if(k!=1)
        throw "failed test_merge 3";
    }
}


//--------------------------------------------------------
// Tests : next()  : 
//--------------------------------------------------------
void test_next() {
    {
    // should unify successfully
    int ai[] = {1,2,3};
    lref<int*> n;
    relation r = next(ai+0, n);
	int i=0;
	for(; r(); ++i) {
		if(i>0 || *(*n)!=2)
			throw "failed test_next 1";
	}
	if(i!=1 || n.defined())
		throw "failed test_next 1";
	}
	{
	// should compare successfully
	int aj[] = {1,2,3};
	lref<int*> n = aj+1;
	relation r2 = next(aj+0,n);
	int j=0;
	for(; r2(); ++j) {
		if(j>0 || *(*n)!=2)
			throw "failed test_next 2";
	}
	if(j!=1 || !n.defined())
		throw "failed test_next 2";
	}
	{
	// should not compare succesfully
	int ak[] = {1,2,3};
	lref<int*> n = ak+1;
	relation r3 = castor::next(n, n);  // element cannot be next of itself
    int k=0;
    for(; r3(); ++k);
    if(k!=0 || !n.defined())
        throw "failed test_next 3";
    }
    { //on simple values
    lref<int> n;
    next(1,n)();
    if(*n!=2)
        throw "failed test_next 4";
    }
    { //on simple values 2
    if(! next(1,2)())
        throw "failed test_next 4";
    }
}

void test_prev() {
    {// generate
    int ai[] = {1,2,3};
    lref<int*> p;
    relation r = prev(ai+1,p);
    int i=0;
    for(; r(); ++i) {
        if(i>0 || *(*p)!=1)
            throw "failed test_prev 1";
    }
    if(i!=1 || p.defined())
        throw "failed test_prev 1";
    }
    {// test  : aj+2 is next of aj+1
   int aj[] = {1,2,3};
    lref<int*> pj = aj+1;
    relation r2 = prev(aj+2, pj);
    int j=0;
    for(; r2(); ++j) {
        if(j>0 || *(*pj)!=2)
            throw "failed test_prev 2";
    }
    if(j!=1 || !pj.defined())
        throw "failed test_prev 2";
    }
    {// test - failing
    int ak[] = {1,2,3};
    lref<int*> pk = ak+1;
    relation r3 = prev(ak+1,pk);  // element cannot be prev of itself
    int k=0;
    for(; r3(); ++k);
    if(k!=0 || !pk.defined())
        throw "failed test_prev 3";
    }
}

//--------------------------------------------------------
// Tests : head(), tail(), head_tail()
//--------------------------------------------------------
void testHead() {
    {// generate
    std::vector<int> v;
    v.push_back(2);
    lref</*const*/ vector<int> > rvi =v; //= std::vector<int>(1);
    
    lref<int> h;
    relation r = head(rvi,h);
    int i=0;
    for(; r(); ++i) {
        if(*h!=2)
            throw "failed testHead 1";
    }
    if(i!=1 || h.defined())
        throw "failed testHead 1";
    }
    {//compare
    vector<int> vj;
    vj.push_back(2);
    lref</*const*/ vector<int> > rvj = vj;
    lref<int> h2=2;
    relation r2 = head(rvj,h2);
    int j=0;
    for(; r2(); ++j) {
        if(*h2!=2)
            throw "failed testHead 2";
    }
    if(j!=1 || !h2.defined())
        throw "failed testHead 2";
    }
    {// compare - failing
    std::vector<int> vk;
    vk.push_back(9);
    lref</*const*/ vector<int> > rvk = vk;
    lref<int> h3=2;
    relation r3 = head(rvk,h3);
    int k=0;
    for(; r3(); ++k);
    if(k!=0 || !h3.defined())
        throw "failed testHead 3";
    }
    {// head of empty list
    lref<int> hl;
    lref</*const*/ vector<int> >vl = vector<int>();
    relation rl =  head(vl,hl);
    if(rl())
        throw "failed testHead 4";
    }
    {// head of undefiend list
    lref<int> hm;
    lref</*const*/ vector<int> >vm;
    relation rm =  head(vm,hm);
    int passed=false;
    try { rm(); }
    catch (...) { passed=true; }
    if(!passed)
        throw "failed testHead 5";
    }
}

void test_head_list() {
    {// generate
    vector<int> items; items.push_back(1); items.push_back(2); items.push_back(3);
    lref</*const*/ vector<int> > v = items;
    lref<vector<int> > h;
#if defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r1 = head_n<vector<int>, vector<int> > (v,2,h);
#else
    relation r1 = head_n(v,2,h);
#endif
    int i=0;
    for(;r1(); ++i) {
        vector<int> expected; expected.push_back(1); expected.push_back(2);
        if(*h!=expected)
            throw "failed test_head_list";
    }
    if(i!=1 || h.defined())
        throw "failed test_head_list";
    }

    {// compare
    vector<int> items; items.push_back(1); items.push_back(2); items.push_back(3);
    lref</*const*/ vector<int> > v = items;
    lref<vector<int> > h = vector<int>(); (*h).push_back(1); (*h).push_back(2);
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r = head_n<vector<int>, vector<int> >(v,2,h);
#else
    relation r = head_n(v,2,h);
#endif
    int i=0;
    for(;r(); ++i);
    if(i!=1)
        throw "failed test_head_list";
    }
}

void test_tail_list() {
    {// generate
    vector<int> items; items.push_back(1); items.push_back(9); items.push_back(2); items.push_back(3);
    lref</*const*/ vector<int> > v = items;
    lref<vector<int> > t;
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r1 = tail_n<vector<int>, vector<int> >(v,2,t);
#else
    relation r1 = tail_n(v,2,t);
#endif

    int i=0;
    for(;r1(); ++i) {
        vector<int> expected; expected.push_back(2); expected.push_back(3);
        if(*t!=expected)
            throw "failed test_tail_list 1";
    }
    if(i!=1 || t.defined())
        throw "failed test_tail_list 1";
    }

    {// compare
    vector<int> items; items.push_back(1); items.push_back(2); items.push_back(3);
    lref</*const*/ vector<int> > v = items;
    lref<vector<int> > t = vector<int>(); (*t).push_back(2); (*t).push_back(3);
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
	relation r = tail_n<vector<int>, vector<int> >(v,2,t);
#else
	relation r = tail_n(v,2,t);
#endif
	int i=0;
	for(;r(); ++i);
	if(i!=1)
		throw "failed test_tail_list 2";
	}
	{ // tail of length 0 of a list is an empty list
	  lref<vector<int> > t, v=vector<int>();
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
	  relation r = tail_n<vector<int>, vector<int> >(v,0,t);
#else
	  relation r = tail_n(v,0,t);
#endif
      if(!r())
        throw "failed test_tail_list 3";
      if(!t.defined())
        throw "failed test_tail_list 3";
    }
}

void test_tail() {
    {
    // generate
    int arr[] = { 3, 4, 5 };
    lref</*const*/ vector<int> > rv = vector<int>(arr+0, arr+3);
    lref<vector<int> > tl;
    relation r = tail(rv,tl);
    int i=0;
    for(; r(); ++i) {
        vector<int> expected (arr+1, arr+3);
        if(*tl!=expected)
            throw "failed test_tail 1";
    }
    if(i!=1 || tl.defined())
       throw "failed test_tail 1";
    }
    {
    // test
    int arrj[] = { 1, 2, 3 };
    lref</*const*/ vector<int> > rvj = vector<int>(arrj+0, arrj+3);
    lref<vector<int> > tlj = vector<int>(arrj+1, arrj+3);
    relation r2 = tail(rvj,tlj);
    int j=0;
    for(; r2(); ++j);
    if(j!=1 || !tlj.defined())
        throw "failed test_tail 2";
    }
    {
    // tail of empty list
    lref<vector<int> > tk;
    lref<const vector<int> >vk = vector<int>();
    relation r3 =  tail(vk,tk);
    if(r3())
        throw "failed test_tail 3";
    }
    {
    // tail of undefined list
    lref<vector<int> > tm;
    lref</*const*/ vector<int> >vm;
    relation rm =  tail(vm,tm);
    int passed=false;
    try { rm(); }
    catch (...) { passed=true; }
    if(!passed)
        throw "failed test_tail 4";
    }
}


void test_head_tail() {
    {
    // generate
    int arr[] = {1,2,3};
    vector<int> v (arr+0, arr+3);
    lref</*const*/ vector<int> > rv = v;
    lref<int> h;
    lref<vector<int> > t;
    relation r = head_tail(rv,h,t);
    int i=0;
    for(; r(); ++i) {
        int expected_head = arr[0];
        vector<int> expected_tail (arr+1, arr+3);
        if(i>1 || *h!=expected_head || *t!=expected_tail)
            throw "failed test_head_tail 1";
    }
    if(i!=1 || h.defined() || t.defined() )
        throw "failed test_head_tail 1";
    }
    {
    //test
    int arrj[] = {1,2,3};
    lref</*const*/ vector<int> > rvj = vector<int> (arrj+0, arrj+3);
    lref<int> hj = arrj[0];
    lref<vector<int> > tj = vector<int> (arrj+1, arrj+3);
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r2 = head_tail<vector<int>,vector<int> >(rvj, arrj[0], tj);
#else
    relation r2 = head_tail(rvj, arrj[0], tj);
#endif
    int j=0;
    for(; r2(); ++j);
    if(j!=1 || !tj.defined() )
        throw "failed test_head_tail 2";
    }
}

void test_head_n_tail() {
    {//test
    int arr[] = {1,2,3};
    lref<vector<int> > lv = vector<int>(arr+0, arr+3);
    lref<vector<int> > h, t;
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r = head_n_tail<vector<int>, vector<int> >(lv, 1, h, t);
#else
    relation r = head_n_tail(lv, 1, h, t);
#endif
    int i=0;
    for(; r(); ++i) {
        if( h->size()!=1  || (*h)[0]!=1 || t->size()!=2 || (*t)[0]!=2 || (*t)[1]!=3 )
            throw "failed test_head_n_tail 1";
    }
    if(i!=1)
        throw "failed test_head_n_tail 1";
    }
    {//generate heads and tails
    int arr[] = {1,2,3};
    lref<vector<int> > lv = vector<int>(arr+0, arr+3);
    lref<vector<int> > h, t;
    lref<vector<int>::size_type> n;
    relation r = head_n_tail(lv, n, h, t);
    int i=0;
    for(; r(); ++i);
    if(i!=4)
        throw "failed test_head_n_tail 2";
    }
}

void test_insert() {
    { // 1 .. input list is simple list<int>
    list<int> li; li.push_back(1); li.push_back(2);
    lref<list<int> > insertedList;
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r = insert<list<int> >(9, li.begin(), li.end(), insertedList);
#else
    relation r = insert(9, li.begin(), li.end(), insertedList);
#endif    
    int expected[][3] = { {9,1,2}, {1,9,2}, {1,2,9} };
    int i=0;
    for(; r(); ++i)
        if( !equal(expected[i], expected[i]+3,insertedList->begin()) )
            throw "failed test_insert 1";
    if(i!=3)
        throw "failed test_insert 1";
    }
    { // 2 .. input list is simple lref<list<int> >
    lref<list<int> > li = list<int>(); li->push_back(1); li->push_back(2);
    lref<list<int> > insertedList;
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r = insert<list<int> >(9, li->begin(), li->end(), insertedList);
#else
    relation r = insert(9, li->begin(), li->end(), insertedList);
#endif
    int expected[][3] = { {9,1,2}, {1,9,2}, {1,2,9} };
    int i=0;
    for(; r(); ++i)
        if( !equal(expected[i], expected[i]+3,insertedList->begin()) )
            throw "failed test_insert 2";
    if(i!=3)
        throw "failed test_insert 2";
    }
    { // 3 .. test ...success
    lref<list<int> > li = list<int>(); li->push_back(1); li->push_back(2);
    int values[] = {1,9,2};
    lref<list<int> >insertedList = list<int>(values+0, values+3);
    relation r = insert<list<int> >(9, li->begin(), li->end(), insertedList);
    int i=0;
    for(; r(); ++i);
    if(i!=1)
        throw "failed test_insert 3";
    }
    { // 4 .. test.. failure
    lref<list<int> > li = list<int>(); li->push_back(1); li->push_back(2);
    int values[] = {1,2,3};
    lref<list<int> > insertedList = list<int>(values+0, values+3);
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r = insert<list<int> >(9, li->begin(), li->end(), insertedList);
#else
    relation r = insert(9, li->begin(), li->end(), insertedList);
#endif
    int i=0;
    for(; r(); ++i);
    if(i!=0)
        throw "failed test_insert 4";
    }
}

void test_insert_seq() {
    { // 1 .. input list is simple list<int>
    list<int> li; li.push_back(1); li.push_back(2);
    list<int> values; values.push_back(7); values.push_back(8);
    lref<list<int> > insertedList;
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r = insert_seq<list<int> >(values.begin(), values.end(), li.begin(), li.end(), insertedList);
#else
    relation r = insert_seq(values.begin(), values.end(), li.begin(), li.end(), insertedList);
#endif
    int expected[][4] = { {7,8,1,2}, {1,7,8,2}, {1,2,7,8} };
    int i=0;
    for(; r(); ++i)
        if( !equal(expected[i], expected[i]+3,insertedList->begin()) )
            throw "failed test_insert_seq 1";
    if(i!=3)
        throw "failed test_insert_seq 1";
    }
    { // 2 .. input list is simple lref<list<int> >
    lref<list<int> > li = list<int>(); li->push_back(1); li->push_back(2);
    list<int> values; values.push_back(7); values.push_back(8);
    lref<list<int> > insertedList;
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r = insert_seq<list<int> >(values.begin(), values.end(), li->begin(), li->end(), insertedList);
#else
    relation r = insert_seq(values.begin(), values.end(), li->begin(), li->end(), insertedList);
#endif
    int expected[][4] = { {7,8,1,2}, {1,7,8,2}, {1,2,7,8} };
    int i=0;
    for(; r(); ++i)
        if( !equal(expected[i], expected[i]+3,insertedList->begin()) )
            throw "failed test_insert _seq 2";
    if(i!=3)
        throw "failed test_insert_seq 2";
    }
    { // 3 .. test ...success
    lref<list<int> > li = list<int>(); li->push_back(1); li->push_back(2);
    list<int> values; values.push_back(7); values.push_back(8);
    int insValues[] = {1,7,8,2};
    lref<list<int> > insertedList = list<int>(insValues+0, insValues+4);
    relation r = insert_seq<list<int> >(values.begin(), values.end(), li->begin(), li->end(), insertedList);
    int i=0;
    for(; r(); ++i);
    if(i!=1)
        throw "failed test_insert_seq 3";
    }
    { // 4 .. test.. failure
    lref<list<int> > li = list<int>(); li->push_back(1); li->push_back(2);
    list<int> values; values.push_back(7); values.push_back(8);
    int insValues[] = {1,2,3,4};
    lref<list<int> > insertedList = list<int>(insValues+0, insValues+4);
    relation r = insert_seq<list<int> >(values.begin(), values.end(), li->begin(), li->end(), insertedList);
    int i=0;
    for(; r(); ++i);
    if(i!=0)
        throw "failed test_insert_seq 4";
    }
}

void test_inc_dec() {
    {// simple inc()
    lref<int> li = 2;
    relation r = inc(li);
    if( !r() )
        throw "failed test_inc_dec";
    if(*li!=3)  throw "failed test_inc_dec";
    if(r())     throw "failed test_inc_dec";
    if(*li!=2)   throw "failed test_inc_dec";
    }
    {// simple dec()
    lref<int> li = 2;
    relation r = dec(li);
    if( !r() )  throw "failed test_inc_dec";
    if(*li!=1)  throw "failed test_inc_dec";
    if(r())     throw "failed test_inc_dec";
    if(*li!=2)   throw "failed test_inc_dec";
    }    
    {// inc(lre<ptr>)
    int arr[] = {1,2,3,4};
    lref<int*> ptr = arr;
    relation r = inc(ptr);
    if( !r() )
        throw "failed test_inc_dec";
    if(**ptr!=2)
        throw "failed test_inc_dec";
    if(r())
        throw "failed test_inc_dec";
    }
    {// dec(lre<ptr>)
    int arr[] = {1,2,3,4};
    lref<int*> ptr = arr+2;
    relation r = dec(ptr);
    if( !r() )
        throw "failed test_inc_dec";
    if(**ptr!=2)
        throw "failed test_inc_dec";
    if(r())
        throw "failed test_inc_dec";
    }
}
//--------------------------------------------------------
//  Tests  : sequence
//--------------------------------------------------------
void test_sequence() { 
    { // copmare ... unequal number of elements .. L < R
    int a[] = {1,2,3};
    lref<list<int> > rli = list<int>(a+0,a+3);
    lref<int> i1 =1;
    list<int> il2; il2.push_back(2);
    relation r = sequence(rli)(i1)(il2.begin(), il2.end())(3)(4);
    if(r())
        throw "failed test_sequence 1";
    }
    { // compare ... unequal number of elements .. L > R
    int b[] = {1,2,3,4};
    lref<list<int> > rly = list<int>(b+0, b+4);
    lref<int> y1 =1;
    list<int> yl2; yl2.push_back(2);
    relation rm = sequence(rly)(y1)(yl2.begin(), yl2.end())(3);
    if(rm())
        throw "failed test_sequence 2";
    }
    { // compare ...  equal number of identical elements
    int c[] = {1,2,3,4};
    lref<list<int> > rlj = list<int>(c+0,c+4);
    lref<int> j2=2;
    list<int> jl3; jl3.push_back(3); jl3.push_back(4);
    relation rj = sequence(rlj)(1)(j2)(jl3.begin(), jl3.end());
    if(!rj())
        throw "failed test_sequence 3";
    }
    { // compare ...  differring elements
    int n[] = {1,2,3};
    lref<list<int> > rln = list<int>(n+0,n+3);
    lref<int> n2=2;
    relation rn = sequence(rln)(1)(n2)(4);
    if(rn())
        throw "failed test_sequence 4";

    }
    { // compare partial collection (using ignore())
    int d[] = {1,2,3,4};
    lref<list<int> > rlk = list<int>(d+0,d+4);
    lref<int> k2 = 2; 
    list<int> l3; l3.push_back(d[2]);
    lref<list<int> > k3 = l3;
    relation rk = sequence(rlk)(1)(k2)(k3).ignore();
    if(rk())
       throw "failed test_sequence 5";

    }
    { // generate list
    int e[] = {1,2,3};
    lref<list<int> > rlx ;
    lref<int> x1=1;
    list<int> lx3; lx3.push_back(e[2]);
    relation rx = sequence(rlx)(x1)(2)(lx3.begin(), lx3.end());
    int x=0;
    for(; rx(); ++x) {
        if(x>1 ||  *rlx != list<int>(e+0, e+3))
            throw "failed test_sequence 6";
    }
    if(x!=1 || rlx.defined())
       throw "failed test_sequence 6";
    }
    { // generate list : using mix of values, lref, lists,  lref<vector>
    lref<vector<string> > ln;
    string s = "One";
    lref<string> lrs = "Two";
    list<string> ls;
    ls.push_back("Three"); ls.push_back("Four");
    vector<string> lsTemp; 
    lsTemp.push_back("Five"); lsTemp.push_back("Six");
    lref<vector<string> > lrls=lsTemp;
    // create the sequence
    relation numbers = sequence(ln)("Zero")(s)(lrs)(ls.begin(), ls.end())(lrls);
    // see what it produces
    if(!numbers())
        throw "failed test_sequence 7";
    string expected[] ={ "Zero", "One", "Two", "Three", "Four", "Five", "Six"};
    if(!equal(expected, expected+7, ln->begin() ))
        throw "failed test_sequence 7";
    }
    { // generate using lref<iterator> 
    list<int> li; li.push_back(1); li.push_back(2); li.push_back(3);
    lref<list<int> > lrli1, lrli2=li;
    lref<list<int>::iterator> itrB, itrE;
    if(! (begin(lrli2,itrB) && end(lrli2, itrE) && sequence(lrli1)(itrB, itrE))() )
        throw "failed test_sequence 8";
    if(!equal(li.begin(), li.end(), lrli1->begin()))
        throw "failed test_sequence 8";
    }
    { // compare using lref<iterator>  
    list<int> li; li.push_back(1); li.push_back(2); li.push_back(3);
    lref<list<int> > lrli=li;
    lref<list<int>::iterator> itrB, itrE;
    if(! (begin(lrli,itrB) && end(lrli, itrE) && sequence(lrli)(itrB, itrE))() )
        throw "failed test_sequence 9";
    if(!equal(li.begin(), li.end(), lrli->begin()))
        throw "failed test_sequence 9";
    }
    { // generate using iterators
    list<int> li; li.push_back(1); li.push_back(2); li.push_back(3);
    lref<list<int>::iterator> itrB, itrE;
    lref<list<int> > lrli1, lrli2=li;
    if(! (begin(lrli2, itrB) && end(lrli2, itrE) && sequence(lrli1)(li.begin(), li.end()))() )
        throw "failed test_sequence 10";
    if(!equal(li.begin(), li.end(), lrli1->begin()))
        throw "failed test_sequence 10";
    }
    { // test for empty sequence
    lref<list<int> > li = list<int>();
    if(!sequence<list<int> >(li)())
        throw "failed test_sequence 11";
    }
    { // generate for empty sequence
    lref<list<int> > lrli;
    if(!sequence(lrli)())
        throw "failed test_sequence 12";
    if(!lrli->empty())
        throw "failed test_sequence 12";
    }

}


void test_range() {
    { // generate
    lref<int> li;
    int expected=0;
    relation r = range<int>(li, 0, 7);
    while(r()) {
        if(*li!=expected++)
            throw "failed test_range 1";
    }
    if(li.defined())
        throw "failed test_range 1";
    }
    { // test within range
    lref<int> i =2;
    relation r = range<int>(i, 0, 7);
    if(!r())
        throw "failed test_range 2";
    if(!i.defined())
        throw "failed test_range 2";
    if(r())
        throw "failed test_range 2";
    }
    { // test out of range
    lref<int> i =9, min =0 , max=7;
    relation r = range(i, min, max);
    if(r())
        throw "failed test_range 2";
    if(!i.defined())
        throw "failed test_range 2";
    if(r())
        throw "failed test_range 2";
    }
    { // empty range
        lref<int> i;
        relation r = range(i,10,2,-1);
        if(r())
            throw "failed test_range 3";
    }
}

void test_range_with_step() {
    { // generate
    lref<int> li;
	int expected=0, step=2;
	relation r = range<int>(li, 0, 7, step);
	while(r()) {
		if(*li!=expected)
			throw "failed test_range_with_step 1";
		expected+=step;
	}
	if(li.defined())
		throw "failed test_range_with_step 1";
	}
	{ // test within range
	lref<int> i =2;
	int step =8;
	relation r = range<int>(i, 0, 7, step);
	if(!r())
		throw "failed test_range_with_step 2";
	if(!i.defined())
		throw "failed test_range_with_step 2";
	if(r())
		throw "failed test_range_with_step 2";
	}
	{ // test out of range
	lref<int> i =9, min =0 , max=7, step=3;
    relation r = range(i, min, max, step);
    if(r())
        throw "failed test_range_with_step 3";
    if(!i.defined())
        throw "failed test_range_with_step 3";
    if(r())
        throw "failed test_range_with_step 3";
    }
    { // empty range
        relation r = range<int>(7,10,2,-1);
        if(r())
            throw "failed test_range_with_step 4";
    }
}

void test_range_dec() {
    { // generate
    lref<int> li;
    int expected=7;
    relation r = range_dec<int>(li, 7,0);
    while(r()) {
        if(*li!=expected--)
            throw "failed test_range_dec  1";
    }
    if(li.defined())
        throw "failed test_range_dec  1";
    }
    { // test 
    lref<int> i =2;
    relation r = range_dec(i, 7, 0);
    if(!r())
        throw "failed test_range_dec  2";
    if(!i.defined())
        throw "failed test_range_dec  2";
    if(r())
        throw "failed test_range_dec  2";
    }
    { // test out of range
    relation r = range_dec<int>(9, 7, 0);
    if(r())
        throw "failed test_range_dec  3";
    if(r())
        throw "failed test_range_dec  3";
    }
    { // empty range
        lref<int> i;
        relation r = range_dec(i,10,11);
        if(r())
            throw "failed test_range_dec  4";
    }
}

void test_range_dec_with_step() {
    { // generate
    lref<int> li;
	int expected=7, step=2;
	relation r = range_dec(li, 7, 0, step);
	while(r()) {
		if(*li!=expected)
			throw "failed test_range_dec_with_step 1";
		expected-=step;
	}
	if(li.defined())
		throw "failed test_range_dec_with_step 1";
	}
	{ // test  step>max
	relation r = range_dec<int>(2, 7, 0, 8);
	if(!r())
		throw "failed test_range_dec_with_step 2";
	}
	{ // gen  step>max
    lref<int> i;
	relation r = range_dec<int>(i, 7, 0, 8);
	if(!r())
		throw "failed test_range_dec_with_step 3";
	if(r())
		throw "failed test_range_dec_with_step 3";
	}
	{ // test out of range
	lref<int> i =9, min =0 , max=7, step=3;
    relation r = range(i, max, min, step);
    if(r())
        throw "failed test_range_dec_with_step 4";
    if(!i.defined())
        throw "failed test_range_dec_with_step 4";
    if(r())
        throw "failed test_range_dec_with_step 4";
    }
    { // empty range
        relation r = range<int>(7,10,12);
        if(r())
            throw "failed test_range_dec_with_step 5";
    }
}


void test_item() {
    {//1 - iterate over std::collection
    list<int> li;
	  li.push_back(1); li.push_back(2); li.push_back(3); li.push_back(4); li.push_back(5);
	  lref<int> obj;
	  relation r = item(obj, li.begin(), li.end());
    int j=1;
    while(r())
        if(*obj!= j++)
            throw "failed test_item 1";
    if(obj.defined())
        throw "failed test_item 1";
    }
    {//2 - lookup over std::collection (using lref)
    list<int> li;
    li.push_back(1); li.push_back(2); li.push_back(3); li.push_back(4); li.push_back(5); li.push_back(2);
    lref<int> obj=2;
    relation r = item(obj, li.begin(), li.end());
    int j=0;
    while(r()) {
        if(*obj!=2)
            throw "failed test_item 2";
        ++j;
    }
    if(j!=2 && *obj!=2)
        throw "failed test_item 2";
    }
    {//3 - iterator with array type
    int li[] = {1,2,3,4,5};
    lref<int> obj;
    relation r = item(obj, li+0, li+5);
    int j=1;
    while(r())
        if(*obj!= j++)
            throw "failed test_item 3";
    if(obj.defined())
        throw "failed test_item 3";
    }
    {//4 - lref<int*> over array type
    int ai[] = {1,2,3,4,5};
    lref<int> obj;
    lref<int*> b = ai+0, e = ai+5;
    relation r = item(obj, b, e);
    int j=1;
    while(r())
        if(*obj!= j++)
            throw "failed test_item 4";
    if(obj.defined())
        throw "failed test_item 4";
    }
    {//5 - using lref<vector<int>::iterator>
    int ai[] = {1,2,3,4,5};
    vector<int> vi(ai+0, ai+5);
    lref<int> obj;
    lref<vector<int>::iterator> b = vi.begin(), e = vi.end();
    relation r = item(obj, b, e);
    int j=1;
    while(r())
        if(*obj!= j++)
            throw "failed test_item 5";
    if(obj.defined())
        throw "failed test_item 5";
    }

    {//6 - find an value item that exist
    int ai[]  = { 1, 2, 3, 4 };
    vector<int> vi(ai+0, ai+4);
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    if( ! item<vector<int>::iterator>(4, vi.begin(), vi.end())() )
#else
    if( ! item(4, vi.begin(), vi.end())() )
#endif
        throw "failed test_item 6";
    }
    {//7- find a value item that doesnt exist
    int ai[]  = { 1, 2, 3, 4 };
    vector<int> vi(ai+0, ai+4);
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    if( item<vector<int>::iterator>(9, vi.begin(), vi.end())() )
#else
    if( item(9, vi.begin(), vi.end())() )
#endif
        throw "failed test_item 7";
    }
    {
    // 8- intersection of array1 and array2
    int arr1[] = {1,2,3,4};
    int arr2[] = {6,3,7,1};
    lref<int> obj;
    relation r = item(obj, arr1+0, arr1+4) && item(obj, arr2+0, arr2+4);
    if(!r() && *obj!=1)
        throw "failed test_item 8";
    if(!r() && *obj!=3)
        throw "failed test_item 8";
    if(r())
        throw "failed test_item 8";
    if(obj.defined())
        throw "failed test_item 8";
    }
    {
    // 9- iterate over container

    int ai[] = {1,2,3,4,5};
    lref<vector<int> > vi = vector<int>(ai+0, ai+5);
    lref<int> obj;
    relation r = item(obj, vi);
    int j=1;
    while(r())
        if(*obj!= j++)
            throw "failed test_item 9";
    if(obj.defined())
        throw "failed test_item 9";
    }
    {
    //10- lookup mode - with duplicates - using item(cont)
    int ai[] = {1,2,3,4,2};
    lref<vector<int> > vi = vector<int>(ai+0, ai+5);
#ifdef __GNUG__
    relation r = item<vector<int> >(2, vi);
#else
    relation r = item(2, vi);
#endif
    int j=0;
    while(r())
        ++j;
    if(j!=2)
        throw "failed test_item 10";
    }
    {
    //11- lookup mode - non existent value - using item(cont)
    int ai[] = {1,2,3,4,2};
    lref<vector<int> > vi = vector<int>(ai+0, ai+5);
#ifdef __GNUG__
    relation r = item<vector<int> >(9, vi);
#else
    relation r = item(9, vi);
#endif
    int j=0;
    while(r())
        ++j;
    if(j!=0)
        throw "failed test_item 11";
    }
    {//12 - compile time checks for overloads : item(i,std::set<>)  & item(i,std::multiset<>)
        lref<set<int> > s;
        lref<multiset<int> > ms;
        lref<const int> i;
        ItemSet_r<set<int> > r1 = item(i,s);        // ensure the call is forwarded to item_set
        ItemSet_r<multiset<int> > r2 = item(i,ms);  // ensure the call is forwarded to item_set
    }
    {  //13 - basic tests for overload - item(i,std::set<>)  & item(i,std::multiset<>)
    lref<const int> i;
    int ai[] = {1,2,3,4,2};
    lref<set<int> > si = set<int>(ai+0, ai+5);
    lref<multiset<int> > mi = multiset<int>(ai+0, ai+5);

    relation r = item(i,si);
    size_t j=0;
    while(r())
        ++j;
    if(j!=si->size())
        throw "failed test_item 13";

    r = item(i,mi);
    j=0;
    while(r())
        ++j;
    if(j!=mi->size())
        throw "failed test_item 13";

    }
    //{// const container and const_iterator
    //    int arr[] = {1,2,3,4};
    //    lref<const vector<int> > v = vector<int>(arr,arr+4);
    //    lref<const int> i;
    //    if(item(i,v)())
    //        throw "failed test_item 14";
    //    vector<int>::const_iterator b = v->begin(), e = v->end();
    //    lref<const int> j;
    //    if(item(j,b, e)())
    //        throw "failed test_item 14";
    // }
}

void test_item_set() {
    int ai[] = {1,2,3,4,2};
    lref<multiset<int> > mi = multiset<int>(ai+0, ai+5);
    lref<set<int> > si = set<int>(ai+0, ai+5);
    { // test mode - std::multiset
#ifdef __GNUG__
    relation r = item_set<multiset<int> >(2, mi);
#else
    relation r = item_set(2,mi);
#endif
    int j=0;
    while(r())
        ++j;
    if(j!=2)
        throw "failed test_item_set 1";
#ifdef __GNUG__
    if(item_set<multiset<int> >(-1,mi)())
#else
    if(item_set(-1,mi)())
#endif
        throw "failed test_item_set 1";
    }
    { // test mode - std::set
    lref<const int> i=2;
    relation r = item_set(i,si);
    int j=0;
    while(r())
        ++j;
    if(j!=1)
        throw "failed test_item_set 2";

#ifdef __GNUG__
    if(item_set<set<int> >(-1,si)())
#else
    if(item_set(-1,mi)())
#endif
        throw "failed test_item_set 2";
    }
    {  // gen mode - std::multiset
    if(mi->size()==si->size())  // require duplicates in mi
        throw "failed test_item_set 3";
    lref<const int> i;
    relation r = item_set(i,mi);
    size_t j=0;
    while(r())
        ++j;
    if(j!=mi->size())
        throw "failed test_item_set 3";
    }
    {  // gen mode - std::set
    lref<const int> i;
    relation r = item_set(i,si);
    size_t j=0;
    while(r())
        ++j;
    if(j!=si->size())
        throw "failed test_item_set 4";
    }
}

void test_item_map() {
    lref<multimap<char,int> > mm = multimap<char,int>();
    mm->insert ( make_pair('a',100) );
    mm->insert ( make_pair('z',150) ); 
    mm->insert ( make_pair('b',75) );
    mm->insert ( make_pair('c',100) );
    mm->insert ( make_pair('z',400) );
    mm->insert ( make_pair('c',100) );
    lref<map<char,int> > m = map<char,int>(mm->begin(), mm->end());

    { // lookup key & obj - multimap
#ifdef __GNUG__
        relation r = item_map<multimap<char,int> >('c',100,mm);
#else
        relation r = item_map('c',100,mm);
#endif
        int count=0;
        while(r())
            ++count;
        if(count!=2)
            throw "failed test_item_map 1";
        
#ifdef __GNUG__
        r = item_map<multimap<char,int> >('z',400,mm);
#else
        r = item_map('z',400,mm);
#endif
        count=0;
        while(r())
            ++count;
        if(count!=1)
            throw "failed test_item_map 1";
    }
    { // gen key & obj - multimap
        lref<const char> k;
        lref<int> v;
        relation r = item_map(k,v,mm);
        size_t count=0;
        while(r())
            ++count;
        if(count!=mm->size())
            throw "failed test_item_map 2";
    }
    { // lookup key, gen obj - multimap
        lref<const char> k='z';
        lref<int> v;
        relation r = item_map(k,v,mm);
        int count=0;
        while(r())
            ++count;
        if(count!=2)
            throw "failed test_item_map 3";
    }
    { // gen key, lookup obj - multimap
        lref<const char> k;
#ifdef __GNUG__
        relation r = item_map<multimap<char,int> >(k,100,mm);
#else
        relation r = item_map(k,100,mm);
#endif
        if(!r())
            throw "failed test_item_map 4";
        if(*k!='a')
            throw "failed test_item_map 4";
        if(!r())
            throw "failed test_item_map 4";
        if(*k!='c')
            throw "failed test_item_map 4";
        if(!r())
            throw "failed test_item_map 4";
        if(*k!='c')
            throw "failed test_item_map 4";
        if(r())
            throw "failed test_item_map 4";
    }
    { // gen key & obj - const multimap
        const multimap<char,int> cmm(mm->begin(), mm->end());
        lref<const multimap<char,int> > lcmm (&cmm, false);
        lref<const char> k;
        lref<int> v;
        relation r = item_map(k,v,mm);
        size_t count=0;
        while(r())
            ++count;
        if(count!=cmm.size())
            throw "failed test_item_map 2";
    }
}

void test_ritem() {
    {//1- lookup mode - with duplicates
    int ai[] = {1,2,3,4,2};
    lref<vector<int> > vi = vector<int>(ai+0, ai+5);
#ifdef __GNUG__
    relation r = ritem<vector<int> >(2, vi);
#else
    relation r = ritem(2, vi);
#endif
    int j=0;
    while(r())
        ++j;
    if(j!=2)
        throw "failed test_ritem 1";
    }
    {//2 - iterate over std::collection
    lref<list<int> > li = list<int>();
	li->push_back(1); li->push_back(2); li->push_back(3); li->push_back(4); li->push_back(5);
	lref<int> obj;
	relation r = ritem(obj, li);
    int j=5;
    while(r())
        if(*obj!= j--)
            throw "failed test_ritem 2";
    if(obj.defined() || j!=0)
        throw "failed test_ritem 2";
    }
}

void test_getValueCont() {
    {// going from from list<lref<int> > to vector<int>
    list<lref<int> > lri;
    lri.push_back(1); lri.push_back(2); lri.push_back(3);
    vector<int> vi = getValueCont<vector<int> >(lri);
    if(vi[0]!=1  || vi[1]!=2 || vi[2] !=3)
        throw "failed test_getValues 1";
    }
    {// going from from list<int*> to vector<int>
    int i[] = {1,2,3};
    list<int*>lri;
    lri.push_back(i+0);
    lri.push_back(i+1);
    lri.push_back(i+2);
    vector<int> vi = getValueCont<vector<int> >(lri);
    if(vi[0]!=1  || vi[1]!=2 || vi[2]!=3)
        throw "failed test_getValues 2";
    }
}

//--------------------------------------------------------
// Test : defined
//--------------------------------------------------------
void test_defined() {
    {
        lref<int> li;
        if(defined(li)())
            throw "failed test_defined 1";
        li=2;
        if(!defined(li)())
            throw "failed test_defined 1";
    }
}

//--------------------------------------------------------
// Test : undefined
//--------------------------------------------------------
void test_undefined() {
	lref<int> x;
	relation r= undefined(x);
	int i=0;
	for(; r(); ++i);
	if(i!=1)
		throw "failed test_undefined 1";

	lref<int> y = 2;
    relation r2= undefined(y);
	for(i=0; r2(); ++i);
	if(i!=0)
		throw "failed test_undefined 2";
}


//--------------------------------------------------------
// Test : unique, unique_f
//--------------------------------------------------------
void test_unique() {
    // print items appearing only once in arr
    int arr[] = {0, 1, 2, 3, 3, 2, 4};
    lref<int> i;
    int expected=0;
    relation r = item(i, arr+0, arr+7) && unique(i);
    int count=0;
    while(r()) {
        count++;
        if(*i!= expected++)
		   throw "failed test_unique 1";
    }
    if(count!=5)
        throw "failed test_unique 1";
}

void test_unique_f() {
	{
    // if i is an item in arr1 and j is an item in j
    // print all pairs of i and j such that i*j is unique
    int arr1[] = {0, 1, 2};
    int arr2[] = {3, 2, 1};
    lref<int> i, j;

    relation r = item(i, arr1+0, arr1+3) && item(j, arr2+0, arr2+3) && unique_f(i*j);
    if(!r())
        throw "failed test_unique_f 1";
    if(*i!=0 || *j!=3)
        throw "failed test_unique_f 1";
    if(!r())
        throw "failed test_unique_f 1";
    if(*i!=1 || *j!=3)
        throw "failed test_unique_f 1";
    if(!r())
        throw "failed test_unique_f 1";
    if(*i!=1 || *j!=2)
        throw "failed test_unique_f 1";
    if(!r())
        throw "failed test_unique_f 1";
    if(*i!=1 || *j!=1)
        throw "failed test_unique_f 1";
    if(!r())
        throw "failed test_unique_f 1";
    if(*i!=2 || *j!=3)
        throw "failed test_unique_f 1";
    if(!r())
        throw "failed test_unique_f 1";
    if(*i!=2 || *j!=2)
        throw "failed test_unique_f 1";
    if(r())
        throw "failed test_unique_f 1";
	}
}

void test_unique_mf() {
	{
    string words[] = {"mary", "had", "a", "little", "lamb"};
    lref<string> w;
    lref<int> c;
	relation r = (item(w,words,words+5) && unique_mf(w,&string::length) >>= count(c)) && predicate(c==4);
	if(!r())
		throw "failed test_unique_mf 1";
	}
}

struct person {
	string firstName, lastName;
	person (string firstName, string lastName) : firstName(firstName), lastName(lastName)
	{ }
	bool operator==(const person& rhs) const {
		return (firstName==rhs.firstName) && (lastName==rhs.lastName);
	}
};

void test_unique_mem() {
	{
    person people[] = { person("roshan","naik"), person("runa","naik"), person("harry","potter") };
    lref<person> p;
	lref<int> c;
	relation r = ( item(p, people, people+3) && unique_mem(p, &person::lastName) >>= count(c) ) && predicate(c==2);
	if(!r())
		throw "failed test_unique_mem 1";
	}
}

//--------------------------------------------------------
// Test : empty, not_empty
//--------------------------------------------------------

void test_empty() {
    { // 1- lref to vector
    lref<vector<int> > lvi;
    relation r = empty(lvi);
    if(!r())
        throw "failed test_empty 1";
    if(!lvi.defined() && !lvi->empty() )
        throw "failed test_empty 1";
    if(r())
        throw "failed test_empty 1";
    if(lvi.defined())
        throw "failed test_empty 1";
    }
    { // 2- not using lref
    vector<int> vi;
    relation r = empty(vi);
    if(!r())
        throw "failed test_empty 2";
    }
    { // 3- const Cont (not lref<Cont>)
    const vector<int> vi;
    relation r = empty(vi);
    if(!r())
        throw "failed test_empty 3";
    }
}


void test_not_emtpy() {
    { //1  - using lref<Cont>
    lref<vector<int> > lvi = vector<int>();
    relation r = not_empty(lvi);
    if(r())
        throw "failed test_not_empty 1";
    }
    { //2 - not using lref<Cont>
    vector<int> vi = vector<int>();
    relation r = not_empty(vi);
    if(r())
        throw "failed test_not_empty 2";
    }
    { //3 - const objects
    lref<const vector<int> > lvi = vector<int>();
    relation r = not_empty(lvi);
    if(r())
        throw "failed test_not_empty 3";
    }
}

//--------------------------------------------------------
// Test : dereference
//--------------------------------------------------------

void test_dereference() {
    {
    // dereferencing raw pointers... gen
    int two=2;
    int* pi= &two;
    lref<int> li;
    relation r = dereference<int*>(pi, li);
    if(!r() || *li!=2)
        throw "failed test_deref 1";
    }
    {
     //dereferencing logic references to iterators... test
    vector<int> vi; vi.push_back(4); vi.push_back(5);
	lref<vector<int>::iterator > lItr = vi.begin();
    lref<int> li=4;
    //relation r = deref<vector<int>::iterator>(lItr, vi.begin());
    relation r = dereference(lItr, li);
    if(!r())
        throw "failed test_deref 2"; 
    }
    {
     //dereferencing iterators... test
    vector<int> vi; vi.push_back(4); vi.push_back(5);
    relation r = dereference<vector<int>::iterator>(vi.begin(), 9);
    if(r())
        throw "failed test_deref 3"; 
    }

}

void test_repeat() {
		{// 1 - success
		lref<int> j;
		int s=0, times=3;
		relation r = repeat(1,times,j);
		while(r() && *j==1)
			++s;
		if(s!=times)
			throw "failed test_repeat 1";
		}
		{// 2 - failure
		lref<int> j;
		int s=0, times=3;
		for(relation r = repeat(1,times,j); r() && *j==2; ++s)
			;
		if(s!=0)
			throw "failed test_repeat 2";
		}
		{// 3 - (lref) - success
		lref<int> j , one=1;
		int s=0, times=3;
		for(relation r = repeat(one,times,j); r() && *j==2; ++s)
			;
		if(s!=0)
			throw "failed test_repeat 3";
		}
}

void test_pause() {
	{// basic compilation checks only
		pause("hello");
		string s("hi");
		pause(s);
		lref<string> ls="blah";
		pause(ls);
	}
}

void test_pause_f() {
	{// basic compilation checks only
        lref<string> s;
        lref<vector<string> > names;
        relation r = item(s, names) && pause_f(s + "\n");
	}
}
