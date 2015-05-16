// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#include <castor.h>
#include <string>
#include <stack>
#include <list>
#include <utility>

using namespace std;
using namespace castor;
//--------------------------------------------------------
//  Tests  : basic lref test for compilation errors
//--------------------------------------------------------
struct Base{
	//virtual void p() { cout << "Base\n"; }
	virtual ~Base() { }
};
struct Derived : public Base {
	//virtual void p() { cout << "Derived\n"; }
	virtual ~Derived() { }
};

struct A {
	static int i;
	A(){ ++i; }
	A(const A&) { ++i; }
	~A(){ --i; }
};

int A::i=0;

void test_lref() {
	{
	lref<string> r2 = "123";
    // these only check for compiler errors
	lref<string> rs; lref<const char*> rc = "world";
	rs = "hello";
	if(*rs!="hello")
		throw "failed test_lref 1";
	rs = rc;
	if(*rs!="world")
		throw "failed test_lref 1";
	}
	{ // Derived type assignment -- compiler error check
		lref<Base> b(new Base(), true); 
		b.set_ptr( new Base(), true);
	}

	//  -- pointer stuff --
	{  // basic initialization
		{
		A::i=0;
		lref<A> r(new A(),true);
		if(A::i!=1)
			throw "failed test_lref 2";
		}
		if(A::i!=0)
			throw "failed test_lref 2";
		
		A* pa = new A();
		{
		lref<A> r(pa,false);
		if(A::i!=1)
			throw "failed test_lref 2";
		}
		if(A::i!=1)
			throw "failed test_lref 2";
		delete pa;
	}
    { // unmanaged to managed transition
		A::i=0;
        A a;
        lref<A> r(&a, false);
        r=a;
		if(A::i!=2)
			throw "failed test_lref 3";
    }
    { // managed to unmanaged transition
		A::i=0;
        lref<A> r(new A, true);
        A a;
        r.set_ptr(&a,false);
		if(A::i!=1)
			throw "failed test_lref 4";
    }
	{  // basic assignment
		A::i=0;
		{
		lref<A> r; r.set_ptr(new A(),true);
		if(A::i!=1)
			throw "failed test_lref 5";
		}
		if(A::i!=0)
			throw "failed test_lref 5";
		{
		lref<A> r; r.set_ptr(new A(),false);
		if(A::i!=1)
			throw "failed test_lref 5";
		}
		if(A::i!=1)
			throw "failed test_lref 5";
	}
	{  // init and assignment
	    A::i=0;
	    lref<A> r(new A(),true);
	    r.set_ptr(new A(), true);
	    if(A::i!=1)
		    throw "failed test_lref 6";
	}
}


//--------------------------------------------------------
//  Tests  : for relational || and && operators
//--------------------------------------------------------
void test_or_and() {
    list<pair<int,int> > expected , actual; 
    expected.push_back(make_pair(1,1));
    expected.push_back(make_pair(1,2));
    expected.push_back(make_pair(2,1));
    expected.push_back(make_pair(2,2));

    lref<int> a, b;
    relation r =  (eq(a,1) || eq(a,2))
               && (eq(b,1) || eq(b,2));
    while(r())
        actual.push_back( make_pair(*a,*b));
    if(actual!=expected)
        throw "failed test_or_and 1";
}


//--------------------------------------------------------
//  Tests  : recurse relation for member relations
//--------------------------------------------------------
struct MyRel {
    relation decrementToUnity(lref<int> j) {
        return  eq(j,1)
            || predicate(j!=1) && dec(j) && recurse_mf(this, &MyRel::decrementToUnity, j);
    }
    
    relation decrementToUnity_c(lref<int> j) const {
        return eq(j,1)
            || predicate(j!=1) && dec(j) && recurse_mf(this, &MyRel::decrementToUnity_c, j);
    }

    relation decrementToUnity_c2(lref<int> j, lref<int>& blah)  const {
        return eq(j,1)
            || predicate(j!=1) && dec(j) && recurse_mf(this, &MyRel::decrementToUnity_c2, j, blah);
    }

};

void test_recurse_mf() {
    {//non const obj, non const member relation
    MyRel mr;
    lref<int> value=4;
    mr.decrementToUnity(value)();
    if(*value!=1)
        throw "failed test_recurse_mf 1";
    }
    { //non const obj, const member relation
    MyRel mr;
    lref<int> value=4;
    mr.decrementToUnity_c(value)();
    if(*value!=1)
        throw "failed test_recurse_mf 2";
    }
    { //const obj, const member relation
#if defined(__GNUG__)
    const MyRel mr = MyRel();
#else
    const MyRel mr;
#endif
    lref<int> value=4;
    mr.decrementToUnity_c(value)();
    if(*value!=1)
        throw "failed test_recurse_mf 3";
    }
}


//--------------------------------------------------------
//   Dynamic Relations
//--------------------------------------------------------


void test_Disjunctions() {
    {// push_back
    Disjunctions males, males2;
    lref<string> p;
    males.push_back(eq(p, "roshan"));
    males.push_back(eq(p, "shivaram"));
    if(!males()) 
        throw "failed test_Disjunctions 1";
    if(*p!="roshan") 
        throw "failed test_Disjunctions 1";
	males2=males;
    if(!males()) 
        throw "failed test_Disjunctions 1";
	if(*p!="shivaram") 
        throw "failed test_Disjunctions 1";
	if(males()) 
        throw "failed test_Disjunctions 1"; // all done
    if(p.defined()) 
        throw "failed test_Disjunctions 1";

    if(!males2()) 
        throw "failed test_Disjunctions 2";
	if(*p!="shivaram") 
        throw "failed test_Disjunctions 2";
	if(males2()) 
        throw "failed test_Disjunctions 2"; // all done
    }
    {// push_front
    Disjunctions males, males2;
    lref<string> p;
    males.push_front(eq(p, "shivaram"));
    males.push_front(eq(p, "roshan"));
    if(!males()) 
        throw "failed test_Disjunctions 3";
    if(*p!="roshan") 
        throw "failed test_Disjunctions 3";
	males2=males;
    if(!males()) 
        throw "failed test_Disjunctions 3";
	if(*p!="shivaram") 
        throw "failed test_Disjunctions 3";
	if(males()) 
        throw "failed test_Disjunctions 3"; // all done
    if(p.defined()) 
        throw "failed test_Disjunctions 3";

    if(!males2()) 
        throw "failed test_Disjunctions 4";
	if(*p!="shivaram") 
        throw "failed test_Disjunctions 4";
	if(males2()) 
        throw "failed test_Disjunctions 4"; // all done
    }
    {
    Disjunctions books;
    lref<string> t, a;
    books.push_back(eq(t,"Tuesdays with Morrie") && eq(a,"Mitch Albom"));
    books.push_back(eq(t,"Bertrand Russell in 90 minutes") && eq(a,"Paul Strathern"));
    if(!books())
        throw "failed test_Disjunctions 5";
    if(*a != "Mitch Albom")
        throw "failed test_Disjunctions 5";
    if(!books())
        throw "failed test_Disjunctions 5";
    if(*a != "Paul Strathern")
        throw "failed test_Disjunctions 5";
    if(books())
        throw "failed test_Disjunctions 5"; // all done
    }
}

void test_ExDisjunctions() {
    {// push_back
    ExDisjunctions males, males2;
    lref<string> p;
    males.push_back(eq(p, "roshan"));
    males.push_back(eq(p, "shivaram"));
    if(!males()) 
        throw "failed test_ExDisjunctions 1";
    if(*p!="roshan") 
        throw "failed test_ExDisjunctions 1";
	males2=males;
	if(males()) 
        throw "failed test_ExDisjunctions 1"; // all done
    if(p.defined()) 
        throw "failed test_ExDisjunctions 1";
    
	if(males2()) 
        throw "failed test_ExDisjunctions 2"; // all done
    }
    {// push_front
    ExDisjunctions males, males2;
    lref<string> p;
    males.push_back(eq(p, "shivaram"));
    males.push_front(eq(p, "roshan"));
    if(!males()) 
        throw "failed test_ExDisjunctions 2";
    if(*p!="roshan") 
        throw "failed test_ExDisjunctions 2";
	males2=males;
	if(males()) 
        throw "failed test_ExDisjunctions 2"; // all done
    if(p.defined()) 
        throw "failed test_ExDisjunctions 2";
    
	if(males2()) 
        throw "failed test_ExDisjunctions 3"; // all done
    }
    {
    ExDisjunctions books;
    lref<string> t, a;
    books.push_back(eq(t,"Tuesdays with Morrie") && eq(a,"Mitch Albom"));
    books.push_back(eq(t,"Bertrand Russell in 90 minutes") && eq(a,"Paul Strathern"));
    if(!books())
        throw "failed test_ExDisjunctions 4";
    if(*a != "Mitch Albom")
        throw "failed test_ExDisjunctions 4";
    if(books())
        throw "failed test_ExDisjunctions 4"; // all done
    }
}

void test_Conjunctions() {
    { // push_back
    Conjunctions gender, gender2;
    lref<string> p, m;
    gender.push_back(eq(p, "roshan"));
    gender.push_back(eq(m, "male"));
    if(!gender()) 
        throw "failed test_Conjunctions 1";
    if(*p!="roshan") 
        throw "failed test_Conjunctions 1";
    gender2 = gender;
	if(gender()) 
        throw "failed test_Conjunctions 1"; // all done

    if(gender2())
        throw "failed test_Conjunctions 2"; // all done
    }
    { // push_front
    Conjunctions gender, gender2;
    lref<string> p, m;
    gender.push_front(eq(m, "male"));
    gender.push_front(eq(p, "roshan"));
    if(!gender()) 
        throw "failed test_Conjunctions 3";
    if(*p!="roshan") 
        throw "failed test_Conjunctions 3";
    gender2 = gender;
	if(gender()) 
        throw "failed test_Conjunctions 3"; // all done

    if(gender2())
        throw "failed test_Conjunctions 4"; // all done
    }
    {
    Conjunctions books;
    lref<string> t, a;
    books.push_back(eq(t,"Tuesdays with Morrie") && eq(a,"Mitch Albom"));
    if(!books())
        throw "failed test_Conjunctions 5";
    if(*a != "Mitch Albom")
        throw "failed test_Conjunctions 5";
    if(books())
        throw "failed test_Conjunctions 5"; // all done
    }
}


//--------------------------------------------------------
//   Cuts
//--------------------------------------------------------
namespace {
bool push(std::stack<std::string>& coll, std::string val) {
    coll.push(val);
    return true;
}
}
void test_cut() {
#ifndef __BCPLUSPLUS__   // cut() && cut() not supported in BCB
	{ // cut() && cut()
	relation r = cutexpr( cut() && cut() );
	if( r() )
		throw "failed test_cut";
	}
#endif // __BCPLUSPLUS__
	{ // cut() && ...       TODO : test for all solns here
	lref<stack<string> > lss = stack<string>(); lref<string> ls = "blah";
	relation r = cutexpr(cut() && predicate(&push,lss,ls));
	if( !r() )
		throw "failed test_cut 1";
	if(lss->empty())
		throw "failed test_cut 2";
	}
	{ // ... && cut()
	lref<stack<string> > lss = stack<string>(); lref<string> s = "blah";
	relation r = cutexpr( cut() && predicate(&push,lss,s) );
	if( !r() )
		throw "failed test_cut 3";
	if( lss->empty())
		throw "failed test_cut 4";
	if( r() )
		throw "failed test_cut 5";
	}
#ifndef __BCPLUSPLUS__   // cut() || cut() not supported in BCB
	{ // cut() || cut()
	relation r = cutexpr( cut() || cut() );
	if( r() )
		throw "failed test_cut 6";
	}
#endif // __BCPLUSPLUS__
	{ // cut() || ...
    lref<stack<string> > lss = stack<string>(); lref<string> s = "blah";
    relation r = cutexpr( cut() || predicate(&push,lss,s) );
    if( r() )
        throw "failed test_cut 7";
    if(!lss->empty())
        throw "failed test_cut 8";
    }
    { // .. || cut()
    lref<stack<string> > lss = stack<string>(); lref<string> s = "blah";
    relation r = cutexpr( predicate(&push,lss,s) || cut() );
    if(! r() )
        throw "failed test_cut 9";
    if(lss->size()!=1)
       throw "failed test_cut 10";
    }
    {// full blown cut expression
    lref<stack<string> > lss = stack<string>(); lref<string> s1="one", s2="two", s3="three";
    lref<int> i =2, j=3;
    relation r = cutexpr(
            predicate(&push,lss,s1) &&  predicate(i<j)  &&  cut() //&& write("i<j")
         || predicate(&push,lss,s2) &&  predicate(i>j)  &&  cut() && True() //&& write("i>j")
         || predicate(&push,lss,s3) &&  predicate(i==j)  &&  cut() //&& write("i==j")
      );
    relation temp = r;
    while(r());
    if(lss->size()!=1)
        throw "failed test_cut 11";

    r = temp;
    i=3; j=2; lss = stack<string>();
    while(r());
    if(lss->size()!=2)
        throw "failed test_cut 12";

    r = temp; lss = stack<string>();
    i=2; j=2;
    while(r());
    if(lss->size()!=3)
        throw "failed test_cut 13";
    }
}


void test_op_exor() {
    {
    lref<int> i;
    relation r = (range(i,1,3) ^ range(i,5,7) ^ range(i,9,10) );
    if(!r()) throw "failed test_op_exor 1";
    if(*i!=1) throw "failed test_op_exor 1";

    if(!r()) throw "failed test_op_exor 1";
    if(*i!=2) throw "failed test_op_exor 1";

    if(!r()) throw "failed test_op_exor 1";
    if(*i!=3) throw "failed test_op_exor 1";

    if(r()) throw "failed test_op_exor 1";
    if(i.defined()) throw "failed test_op_exor 1";
    }
    {
    lref<int> i;
    relation r = (range(i,1,0) ^ range(i,5,7) ^ range(i,9,10) );
    if(!r()) throw "failed test_op_exor 2";
    if(*i!=5) throw "failed test_op_exor 2";

    if(!r()) throw "failed test_op_exor 2";
    if(*i!=6) throw "failed test_op_exor 2";

    if(!r()) throw "failed test_op_exor 2";
    if(*i!=7) throw "failed test_op_exor 2";

    if(r()) throw "failed test_op_exor 2";
    if(i.defined()) throw "failed test_op_exor 2";
    }
        
}

void test_True() {
	{
	  lref<int> c;
	  relation r = True(20) >>= count(c);
	  if(!r())
		  throw "failed test_times 1";
	  if(*c!=20)
		  throw "failed test_times 1";
	  if(r() || c.defined() )
		  throw "failed test_times 1";
	}
	{
	  lref<int> c;
	  relation r = True() >>= count(c);
	  if(!r())
		  throw "failed test_times 2";
	  if(*c!=1)
		  throw "failed test_times 2";
	}
}