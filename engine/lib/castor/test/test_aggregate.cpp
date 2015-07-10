// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).
#include <castor.h>
#include <string>
#include <vector>

using namespace castor;
using namespace std;


namespace {

struct integer {
    int i;
    integer(int i) : i(i) 
    { }
    bool operator==(const integer& rhs) const {
        return i==rhs.i;
    }
    integer operator / (const integer& rhs) const {
        return i/rhs.i;
    }
};

struct integer_add {
    integer operator()(const integer& lhs, const integer& rhs) const {
        return integer(lhs.i + rhs.i);
    }
};

struct integer_mult {
    integer operator()(const integer& lhs, const integer& rhs) const {
        return integer(lhs.i * rhs.i);
    }
};

}

void test_count() {
	{ // generate
	int ai[] = { 10,9,8,7,6,5,4,3,2,1 };
	lref<int> j,c;
	relation r = item(j,ai,ai+10) >>= count(c);
	if(!r())
		throw "failed test_count 1";
	if(*c!=10)
		throw "failed test_count 1";
	if(r())
		throw "failed test_count 1";
	if(c.defined())
		throw "failed test_count 1";
	}
	{ // generate
		lref<int> j,c;
		relation r = range(j,1,-1) >>= count(c);
		if(!r())
			throw "failed test_count 2";
		if(*c!=0)
			throw "failed test_count 2";
		if(r())
			throw "failed test_count 2";
		if(c.defined())
			throw "failed test_count 2";
	}
}

// Test: Size of collections
void test_size() {
#ifdef CASTOR_ENABLE_DEPRECATED
    {
    // generate
    lref<vector<int>::size_type> s;
    lref</*const*/ vector<int> > coll = vector<int> ();
    relation r=size(coll,s);
    int i=0;
    for(; r(); ++i) {
        if(i>0 || *s!=0)
            throw "failed test_size 1";
    }
    if(i!=1)
        throw "failed test_size 1";
    }
    {
    // generate
    lref<vector<int>::size_type> s2;
    lref</*const*/ vector<int> > coll2;
    relation r2=size(coll2,s2);
    int passed2=false;
    try { r2(); }
    catch (...) { passed2=true; }
    if(!passed2)
        throw "failed test_size 2";
    }
    {
    // compare
    lref</*const*/ vector<int> > coll3 = vector<int> ();
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r3=size<vector<int> >(coll3,0);
#else
    relation r3=size(coll3,0);
#endif
    int j=0;
    for(; r3(); ++j) {
        if(j>0)
            throw "failed test_size 3";
    }
    if(j!=1)
        throw "failed test_size 3";
    }
#endif // #ifdef CASTOR_ENABLE_DEPRECATED
}


// Test: Size of collections
void test_size_of() {
    {
    // generate
    lref<vector<int>::size_type> s;
    lref<const vector<int> > coll = vector<int> ();
    relation r=size_of(coll,s);
    int i=0;
    for(; r(); ++i) {
        if(i>0 || *s!=0)
            throw "failed test_size_of 1";
    }
    if(i!=1)
        throw "failed test_size_of 1";
    }
    {
    // generate
    lref<vector<int>::size_type> s2;
    lref<vector<int> > coll2;
    relation r2=size_of(coll2,s2);
    int passed2=false;
    try { r2(); }
    catch (...) { passed2=true; }
    if(!passed2)
        throw "failed test_size_of 2";
    }
    {
    // compare
    lref</*const*/ vector<int> > coll3 = vector<int> ();
#if  defined(__BCPLUSPLUS__) || defined(__GNUG__)
    relation r3=size_of<vector<int> >(coll3,0);
#else
    relation r3=size_of(coll3,0);
#endif
    int j=0;
    for(; r3(); ++j) {
        if(j>0)
            throw "failed test_size_of 3";
    }
    if(j!=1)
        throw "failed test_size_of 3";
    }
}


void test_sum() {
	{
		lref<int> j;
		relation r = range(j,1,10) >>= sum(j);
		if(!r() || !j.defined())
			throw "failed test_sum 1";
		if(*j!=55)
			throw "failed test_sum 1";
		if(r())
			throw "failed test_sum 1";
		if(j.defined())
			throw "failed test_sum 1";
	}
	{
		lref<int> j;
		relation r = range(j,1,1) >>= sum(j);
		if(!r() || !j.defined())
			throw "failed test_sum 2";
		if(*j!=1)
			throw "failed test_sum 2";
		if(r())
			throw "failed test_sum 2";
		if(j.defined())
			throw "failed test_sum 2";
	}
	{
		lref<int> j;
		relation r = range(j,2,1) >>= sum(j);
		if(r() || j.defined())
			throw "failed test_sum 3";
	}
}

void test_reduce() {
	{ // multiple values in input
		lref<int> j;
		relation r = range(j,1,10) >>= reduce(j, std::multiplies<int>());
		if(!r() || !j.defined())
			throw "failed test_reduce 1";
		if(*j!=3628800)
			throw "failed test_reduce 1";
		if(r())
			throw "failed test_reduce 1";
		if(j.defined())
			throw "failed test_reduce 1";
	}
	{ // single input
		lref<int> j;
		relation r = range(j,1,1) >>= reduce(j, std::multiplies<int>());
		if(!r() || !j.defined())
			throw "failed test_reduce 2";
		if(*j!=1)
			throw "failed test_reduce 2";
		if(r())
			throw "failed test_reduce 2";
		if(j.defined())
			throw "failed test_reduce 2";
	}
	{ // no inputs
		lref<int> j;
		relation r = range(j,2,1) >>= reduce(j, std::plus<int>());
		if(r() || j.defined())
			throw "failed test_reduce 3";
	}
}

void test_max() {
    {// basic
        lref<int> i;
        relation r = range(i,1,100) >>= max(i);
        if(!r())
            throw "failed test_max 1";
        if(*i!=100)
            throw "failed test_max 1";
        if(r())
            throw "failed test_max 1";
        if(i.defined())
            throw "failed test_max 1";
    }
    {// with cmp
        lref<int> i;
        relation r = range(i,1,100) >>= max(i, std::greater<int>()); //max as min
        if(!r())
            throw "failed test_max 2";
        if(*i!=1)
            throw "failed test_max 2";
        if(r())
            throw "failed test_max 2";
        if(i.defined())
            throw "failed test_max 2";
    }
}

void test_min() {
    {// basic
        lref<int> i;
        relation r = range(i,1,100) >>= min(i);
        if(!r())
            throw "failed test_min 1";
        if(*i!=1)
            throw "failed test_min 1";
        if(r())
            throw "failed test_min 1";
        if(i.defined())
            throw "failed test_min 1";
    }
    {// with cmp
        lref<int> i;
        relation r = range(i,1,100) >>= min(i, std::greater<int>()); //min as max
        if(!r())
            throw "failed test_min 2";
        if(*i!=100)
            throw "failed test_min 2";
        if(r())
            throw "failed test_min 2";
        if(i.defined())
            throw "failed test_min 2";
    }
}

void test_average() {
    int nums[] = { 1, 2, 3, 4, 5 };
    lref<vector<int> > vi = vector<int>(nums,nums+5);
    {// gen 
        lref<int> i;
        relation r = item(i,vi) >>= average(i);
        if(!r())
            throw "failed test_average 1";
        if(*i!=3)
            throw "failed test_average 1";
        if(r())
            throw "failed test_average 1";
        if(i.defined())
            throw "failed test_average 1";
    }
    { // gen - with adder
        integer ints[] = {1,2,3,4,5};
        lref<vector<integer> > integers = vector<integer>(ints,ints+5);
        lref<integer> a;
        relation r = item(a,integers) >>= average(a,integer_add()) ;
        if(!r())
            throw "failed test_average 2";
        if(a->i!=3)
            throw "failed test_average 2";
    }
    { // gen - with adder & divider
        integer ints[] = {1,2,3,4,5};
        lref<vector<integer> > integers = vector<integer>(ints,ints+5);
        lref<integer> a;
        relation r = item(a,integers) >>= average(a,integer_add()) ;
        if(!r())
            throw "failed test_average 3";
        if(a->i!=3)
            throw "failed test_average 3";
    }
    {// empty container
        lref<int> s;
        lref<vector<int> > v2 = vector<int>();
        relation r =  item(s,v2) >>=  average(s) ;
        if(r())
            throw "failed test_average 5";
        if(s.defined())
            throw "failed test_average 5";
    }
}

namespace {
bool isEven(int i) {
    return i%2==0;
}

}

void test_max_of() {
    string arr[] = { "Prolog", "Leda", "castor", "C#" };
    lref<vector<string> > v = vector<string>(arr, arr+4);
    {// basic - gen
        lref<string> m;

        relation r =  max_of(v,m) ;
        if(!r())
            throw "failed test_max_of 1";
        if(*m!="castor")
            throw "failed test_max_of 1";
        if(r())
            throw "failed test_max_of 1";
        if(m.defined())
            throw "failed test_max_of 1";
    }
    {// basic - test
        lref<string> m = "castor";
        relation r =  max_of(v,m) ;
        if(!r())
            throw "failed test_max_of 2";
        if(*m!="castor")
            throw "failed test_max_of 2";
        if(r())
            throw "failed test_max_of 2";
        if(!m.defined())
            throw "failed test_max_of 2";
 
        // failure test
#ifdef __GNUG__
        r =  max_of<vector<string> >(v,"Prolog") ;
#else
        r =  max_of(v,"Prolog") ;
#endif
        if(r())
            throw "failed test_max_of 2";
    }
    {// basic - empty container
        lref<string> m;
        lref<vector<string> > v2 = vector<string>();
        relation r =  max_of(v2,m) ;
        if(r())
            throw "failed test_max_of 3";
        if(m.defined())
            throw "failed test_max_of 3";
    }
    {// with group_by
        lref<int> i,m;
        lref<group<bool,int> > g;
        relation r =  ( range(i,1,100) >>= group_by(i,isEven,g) ) && max_of(g,m) ;
        if(!r())
            throw "failed test_max_of 4";
        if(*m!=99)
            throw "failed test_max_of 4";
        if(!r())
            throw "failed test_max_of 4";
        if(*m!=100)
            throw "failed test_max_of 4";
        if(r())
            throw "failed test_max_of 4";
        if(m.defined())
            throw "failed test_max_of 4";
    }
    { // test - with cmp
#ifdef __GNUG__
        relation r =  max_of<vector<string> >(v,"C#", std::greater<string>() ) ;
#else
        relation r =  max_of(v,"C#", std::greater<string>() ) ;
#endif
        if(!r())
            throw "failed test_max_of 5";
    }
    {// basic - gen - with const container
        lref<string> m;
        lref<const vector<string> > cv = vector<string>(arr, arr+4);

        relation r =  max_of(cv,m) ;
        if(!r())
            throw "failed test_max_of 6";
        if(*m!="castor")
            throw "failed test_max_of 6";
        if(r())
            throw "failed test_max_of 6";
        if(m.defined())
            throw "failed test_max_of 6";
    }
}

void test_min_of() {
    string arr[] = { "Prolog", "Leda", "castor", "C#" };
    lref<vector<string> > v = vector<string>(arr, arr+4);
    {// basic - gen
        lref<string> m;

        relation r =  min_of(v,m) ;
        if(!r())
            throw "failed test_min_of 1";
        if(*m!="C#")
            throw "failed test_min_of 1";
        if(r())
            throw "failed test_min_of 1";
        if(m.defined())
            throw "failed test_min_of 1";
    }
    {// basic - test
        lref<string> m = "C#";
        relation r =  min_of(v,m) ;
        if(!r())
            throw "failed test_min_of 2";
        if(*m!="C#")
            throw "failed test_min_of 2";
        if(r())
            throw "failed test_min_of 2";
        if(!m.defined())
            throw "failed test_min_of 2";
 
        // failure test
#ifdef __GNUG__
        r =  min_of<vector<string> >(v,"Prolog");
#else
        r =  min_of(v,"Prolog");
#endif
        if(r())
            throw "failed test_min_of 2";
    }
    {// basic - empty container
        lref<string> m;
        lref<vector<string> > v2 = vector<string>();
        relation r =  min_of(v2,m) ;
        if(r())
            throw "failed test_min_of 3";
        if(m.defined())
            throw "failed test_min_of 3";
    }
    {// with group_by
        lref<int> i,m;
        lref<group<bool,int> > g;
        relation r =  ( range(i,1,100) >>= group_by(i,isEven,g) ) && min_of(g,m) ;
        if(!r())
            throw "failed test_min_of 4";
        if(*m!=1)
            throw "failed test_min_of 4";
        if(!r())
            throw "failed test_min_of 4";
        if(*m!=2)
            throw "failed test_min_of 4";
        if(r())
            throw "failed test_min_of 4";
        if(m.defined())
            throw "failed test_min_of 4";
    }
    { // test - with cmp
#ifdef __GNUG__
        relation r =  min_of<vector<string> >(v,"castor", std::greater<string>() ) ;
#else
        relation r =  min_of(v,"castor", std::greater<string>() ) ;
#endif
        if(!r())
            throw "failed test_min_of 5";
    }
    {// basic - gen - with const container
        lref<string> m;
        lref<vector<string> > cv = vector<string>(arr, arr+4);

        relation r =  min_of(cv,m) ;
        if(!r())
            throw "failed test_min_of 6";
        if(*m!="C#")
            throw "failed test_min_of 6";
        if(r())
            throw "failed test_min_of 6";
        if(m.defined())
            throw "failed test_min_of 6";
    }
}


void test_sum_of() {
    string arr[] = { "Prolog", "Leda", "castor", "C#" };
    lref<vector<string> > vs = vector<string>(arr, arr+4);
    int nums[] = { 1, 2, 3, 4, 5 };
    lref<vector<int> > vi = vector<int>(nums,nums+5);

    {// basic - gen - with strings
        lref<string> s;
        relation r =  sum_of(vs,s) ;
        if(!r())
            throw "failed test_sum_of 1";
        if(*s!="PrologLedacastorC#")
            throw "failed test_sum_of 1";
        if(r())
            throw "failed test_sum_of 1";
        if(s.defined())
            throw "failed test_sum_of 1";
    }
    {// basic - gen - with ints
        lref<int> s;
        relation r = sum_of(vi,s);
        if(!r())
            throw "failed test_sum_of 2";
        if(*s!=15)
            throw "failed test_sum_of 2";
        if(r())
            throw "failed test_sum_of 2";
        if(s.defined())
            throw "failed test_sum_of 2";
    }
    {// basic - test
#ifdef __GNUG__
        relation r = sum_of<vector<int> >(vi,15);
#else
        relation r = sum_of(vi,15);
#endif

        if(!r())
            throw "failed test_sum_of 3";
 
        // failure test
#ifdef __GNUG__
        r =  sum_of<vector<int> >(vi,10) ;
#else
        r =  sum_of(vi,10) ;
#endif
        if(r())
            throw "failed test_sum_of 3";
    }
    {// basic - empty container
        lref<int> s;
        lref<vector<int> > v2 = vector<int>();
        relation r =  min_of(v2,s) ;
        if(r())
            throw "failed test_sum_of 4";
        if(s.defined())
            throw "failed test_sum_of 4";
    }
    {// with group_by
        lref<int> i,s;
        lref<group<bool,int> > g;
        relation r =  ( range(i,1,10) >>= group_by(i,isEven,g) ) && sum_of(g,s) ;
        if(!r())
            throw "failed test_sum_of 5";
        if(*s!=25)
            throw "failed test_sum_of 5";
        if(!r())
            throw "failed test_sum_of 5";
        if(*s!=30)
            throw "failed test_sum_of 5";
        if(r())
            throw "failed test_sum_of 5";
        if(s.defined())
            throw "failed test_sum_of 5";
    }
}


void test_reduce_of() {
    string arr[] = { "Prolog", "Leda", "castor", "C#" };
    lref<vector<string> > vs = vector<string>(arr, arr+4);
    int nums[] = { 1, 2, 3, 4, 5 };
    lref<vector<int> > vi = vector<int>(nums,nums+5);

    { // test - with adder
        integer ints[] = {1,2,3,4};
        lref<vector<integer> > nums = vector<integer>(ints,ints+4);
        lref<integer> s;
        relation r = reduce_of(nums,s,integer_add()) ;
        if(!r())
            throw "failed test_reduce_of 1";
        if(s->i!=10)
            throw "failed test_reduce_of 1";
    }
    { // gen - with multiplier
        integer ints[] = {1,2,3,4};
        lref<vector<integer> > nums = vector<integer>(ints,ints+4);
        lref<integer> s = integer(24);
        relation r = reduce_of(nums,s,integer_mult()) ;
        if(!r())
            throw "failed test_reduce_of 2";
        if(s->i!=24)
            throw "failed test_reduce_of 2";
    }
}


void test_average_of() {
    int nums[] = { 1, 2, 3, 4, 5 };
    lref<vector<int> > vi = vector<int>(nums,nums+5);
    {// gen 
        lref<int> a;
        relation r = average_of(vi,a);
        if(!r())
            throw "failed test_average_of 1";
        if(*a!=3)
            throw "failed test_average_of 1";
        if(r())
            throw "failed test_average_of 1";
        if(a.defined())
            throw "failed test_average_of 1";
    }
    { // gen - with adder
        integer ints[] = {1,2,3,4,5};
        lref<vector<integer> > nums = vector<integer>(ints,ints+5);
        lref<integer> a;
        relation r = average_of(nums,a,integer_add()) ;
        if(!r())
            throw "failed test_average_of 2";
        if(a->i!=3)
            throw "failed test_average_of 2";
    }
    {// test
#ifdef __GNUG__
        relation r = average_of<vector<int> >(vi,3);
#else
        relation r = average_of(vi,3);
#endif
        if(!r())
            throw "failed test_average_of 3";
 
        // failure test
#ifdef __GNUG__
        r =  average_of<vector<int> >(vi,10);
#else
        r =  average_of(vi,10);
#endif
        if(r())
            throw "failed test_average_of 3";
    }
    {// empty container
        lref<int> s;
        lref<vector<int> > v2 = vector<int>();
        relation r =  average_of(v2,s) ;
        if(r())
            throw "failed test_average_of 4";
        if(s.defined())
            throw "failed test_average_of 4";
    }
    {// with group_by
        lref<int> i,a;
        lref<group<bool,int> > g;
        relation r =  ( range(i,1,10) >>= group_by(i,isEven,g) ) && average_of(g,a) ;
        if(!r())
            throw "failed test_average_of 5";
        if(*a!=5)
            throw "failed test_average_of 5";
        if(!r())
            throw "failed test_average_of 5";
        if(*a!=6)
            throw "failed test_average_of 5";
        if(r())
            throw "failed test_average_of 5";
        if(a.defined())
            throw "failed test_average_of 5";
    }
    {// gen - with const container
       lref<const vector<int> > cv = vector<int>(nums,nums+5);
        lref<int> a;
        relation r = average_of(cv,a);
        if(!r())
            throw "failed test_average_of 6";
        if(*a!=3)
            throw "failed test_average_of 6";
        if(r())
            throw "failed test_average_of 6";
        if(a.defined())
            throw "failed test_average_of 6";

    }
}
