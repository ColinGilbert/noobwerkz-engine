// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).


//--------------------------------------------------------
//  Tests  : predicate relation for converting predicates to relations
//--------------------------------------------------------
#include <castor.h>
#include <iostream>

using namespace castor;
using namespace std;

//namespace {

bool isEven(int num) {
    return num%2 == 0;
}

bool isEven_lref(lref<int> num) {
    return (*num)%2 == 0;
}

struct Functor {
    int j;
    bool t,f;
    Functor() : j(4), t(true), f(false)
    { }

    bool isEven(int i) {
        return (i+j)%2 == 0;
    }

    bool isEven_c(int i) const {
        return i%2 == 0;
    }

    bool allEqual(int i, const int j, int k, int l, lref<int> m, const int n) const {
        return i==j==k==l==*m==n;
    }

    bool ternaryConstPred(int, const int, int) const {
        return true;
    }
    static bool staticMembPred(int) {
        return true;
    }
};

bool myPred (void)                                               { return true; }
bool myPred (int i1)                                             { return 0<i1; }
bool myPred (int i1, int i2)                                     { return myPred(i1) && i1<i2; }
bool myPred (int i1, lref<int> i2, int i3)                       { return myPred(i1,*i2) && myPred(*i2,i3); }
bool myPred (int i1, int i2, int i3, int i4)                     { return myPred(i1,i2,i3) && myPred(i3,i4); }
bool myPred (int i1, int i2, int i3, int i4, int i5)             { return myPred(i1,i2,i3,i4) && myPred(i4,i5); }
bool myPred (int i1, int i2, int i3, int i4, int i5, int i6)     { return myPred(i1,i2,i3,i4,i5) && myPred(i5,i6); }


struct MyPred {
    bool operator() (void)                                            { return myPred(); }
    bool operator() (int i1)                                          { return myPred(i1); }
    bool operator() (int i1, int i2)                                  { return myPred(i1,i2); }
    bool operator() (int i1, lref<int> i2, int i3)                    { return myPred(i1,*i2,i3); }
    bool operator() (int i1, int i2, int i3, int i4)                  { return myPred(i1,i2,i3,i4); }
    bool operator() (int i1, int i2, int i3, int i4, int i5)          { return myPred(i1,i2,i3,i4,i5); }
    bool operator() (int i1, int i2, int i3, int i4, int i5, int i6)  { return myPred(i1,i2,i3,i4,i5,i6); }

    bool method (void)                                               { return myPred(); }
    bool method (int i1)                                             { return myPred(i1); }
    bool method (int i1, int i2)                                     { return myPred(i1,i2); }
    bool method (int i1, lref<int> i2, int i3)                       { return myPred(i1,*i2,i3); }
    bool method (int i1, int i2, int i3, int i4)                     { return myPred(i1,i2,i3,i4); }
    bool method (int i1, int i2, int i3, int i4, int i5)             { return myPred(i1,i2,i3,i4,i5); }
    bool method (int i1, int i2, int i3, int i4, int i5, int i6)     { return myPred(i1,i2,i3,i4,i5,i6); }

};

//} // namespace {}

void test_predicate() {
    {// global predicate : success
    relation r = predicate(isEven,2);
    int i=0;
    for(; r(); ++i);
    if(i!=1)
        throw "failed test_predicate 1";
    }
    { // global predicate : failure
    relation r = predicate(isEven,3);
    int i=0;
    for(; r(); ++i);
    if(i!=0)
        throw "failed test_predicate 2";
    }
    {// global predicate taking lref : success
    lref<int> j=2;
    relation r = predicate(isEven_lref,j);
    int i=0;
    for(; r(); ++i);
    if(i!=1)
        throw "failed test_predicate 3";
    }
    {// global predicate taking lref : failure
    relation r = predicate(isEven_lref,3);
    int i=0;
    for(; r(); ++i);
    if(i!=0)
        throw "failed test_predicate 4";
    }
    { // static member predicate
    relation r = predicate(&Functor::staticMembPred, 2);
    int i=0;
    for(; r(); ++i);
    if(i!=1)
        throw "failed test_predicate 5";
    }
    { //  function pointers
    if(!predicate(myPred)())
        throw "failed test_predicate 6";
    if(!predicate(myPred,1)())
        throw "failed test_predicate 6";
    if(!predicate(myPred,1,2)())
        throw "failed test_predicate 6";
    if(!predicate(myPred,1,2,3)())
        throw "failed test_predicate 6";
    if(!predicate(myPred,1,2,3,4)())
        throw "failed test_predicate 6";
    if(!predicate(myPred,1,2,3,4,5)())
        throw "failed test_predicate 6";
    if(!predicate(myPred,1,2,3,4,5,6)())
        throw "failed test_predicate 6";
    if(predicate(myPred,3,2)())
        throw "failed test_predicate 6";
    }
    { //  function objects
    if(!predicate(MyPred())())
        throw "failed test_predicate 7";
    if(!predicate(MyPred(),1)())
        throw "failed test_predicate 7";
    if(!predicate(MyPred(),1,2)())
        throw "failed test_predicate 7";
    if(!predicate(MyPred(),1,2,3)())
        throw "failed test_predicate 7";
    if(!predicate(MyPred(),1,2,3,4)())
        throw "failed test_predicate 7";
    if(!predicate(MyPred(),1,2,3,4,5)())
        throw "failed test_predicate 7";
    if(!predicate(MyPred(),1,2,3,4,5,6)())
        throw "failed test_predicate 7";
    if(predicate(MyPred(),3,2)())
        throw "failed test_predicate 7";
    }
}

void test_predicate_mf() {
    {// member predicate function
    lref<Functor> f = Functor();
    lref<int> two = 2;
    relation r = predicate_mf(f, &Functor::isEven, two);
    int i=0;
    for(; r(); ++i);
    if(i!=1)                        
        throw "failed test_predicate_mf 1";
    }
    {// failing member predicate function
    lref<Functor> f = Functor();
    relation r = predicate_mf(f, &Functor::isEven, 5);
    int i=0;
    for(; r(); ++i);
    if(i!=0)
        throw "failed test_predicate_mf 2";
    }
#ifdef __BCPLUSPLUS__
#else
	{// non-const obj, const member predicate
	lref<Functor> f = Functor();
	lref<int> one = 1;
	typedef bool(Functor::* mpred)(int,const int,int,int,lref<int>,const int) const;
	relation r = predicate_mf(f, &Functor::allEqual , 1,1, 1,1, 1,one);
	int i=0;
	for(; r(); ++i);
	if(i!=1)
		throw "failed test_predicate_mf 3";
	}
#endif

#ifdef __BCPLUSPLUS__
#else
	{// const obj, const member predicate
	lref<const Functor> f = Functor();
	relation r = predicate_mf(f, &Functor::allEqual, 1,1, 1,1, 1,1);
	int i=0;
	for(; r(); ++i);
	if(i!=1)
		throw "failed test_predicate_mf 4";
	}
#endif

#ifdef __BCPLUSPLUS__
#else
	{// const object, const member predicate
	lref<const Functor> f = Functor();
	relation r = predicate_mf(f, &Functor::ternaryConstPred, 2, 3, 6);
	int i=0;
	for(; r(); ++i);
	if(i!=1)
		throw "failed test_predicate_mf 5";
	}
#endif

	{
	lref<MyPred> lp = MyPred();
#ifdef __BCPLUSPLUS__
	bool(MyPred::* meth)(void) = &MyPred::method;
	if(!predicate_mf(lp, meth)())
#else
	if(!predicate_mf(lp, &MyPred::method)())
#endif
		throw "failed test_predicate_mf 6";

#ifdef __BCPLUSPLUS__
	bool(MyPred::* meth1)(int) = &MyPred::method;
	if(!predicate_mf(lp, meth1,1)())
#else
	if(!predicate_mf(lp, &MyPred::method,1)())
#endif
		throw "failed test_predicate_mf 6";


#ifdef __BCPLUSPLUS__
	bool(MyPred::* meth2)(int,int) = &MyPred::method;
	if(!predicate_mf(lp, meth2,1,2)())
#else
	if(!predicate_mf(lp, &MyPred::method,1,2)())
#endif
		throw "failed test_predicate_mf 6";


#ifdef __BCPLUSPLUS__
	bool(MyPred::* meth3)(int,lref<int>,int) = &MyPred::method;
	if(!predicate_mf(lp, meth3,1,2,3)())
#else
	if(!predicate_mf(lp, &MyPred::method,1,2,3)())
#endif
		throw "failed test_predicate_mf 6";



#ifdef __BCPLUSPLUS__
	bool(MyPred::* meth4)(int,int,int,int) = &MyPred::method;
	if(!predicate_mf(lp, meth4,1,2,3,4)())
#else
	if(!predicate_mf(lp, &MyPred::method,1,2,3,4)())
#endif
		throw "failed test_predicate_mf 6";


#ifdef __BCPLUSPLUS__
	bool(MyPred::* meth5)(int,int,int,int,int) = &MyPred::method;
	if(!predicate_mf(lp, meth5,1,2,3,4,5)())
#else
	if(!predicate_mf(lp, &MyPred::method,1,2,3,4,5)())
#endif
		throw "failed test_predicate_mf 6";



#ifdef __BCPLUSPLUS__
	bool(MyPred::* meth6)(int,int,int,int,int,int) = &MyPred::method;
	if(!predicate_mf(lp, meth6,1,2,3,4,5,6)())
#else
	if(!predicate_mf(lp, &MyPred::method,1,2,3,4,5,6)())
#endif
		throw "failed test_predicate_mf 6";


#ifdef __BCPLUSPLUS__
	bool(MyPred::* meth2a)(int,int) = &MyPred::method;
	if(predicate_mf(lp, meth2a,3,2)())
#else
	if(predicate_mf(lp, &MyPred::method,3,2)())
#endif
        throw "failed test_predicate_mf 6";
    }
}



void test_predicate_mem() {
    {
    lref<Functor> lf = Functor();
    if(!predicate_mem(lf, &Functor::t)())
        throw "failed test_predicate_mem 1";
    if(predicate_mem(lf, &Functor::f)())
        throw "failed test_predicate_mem 1";
    }
}

