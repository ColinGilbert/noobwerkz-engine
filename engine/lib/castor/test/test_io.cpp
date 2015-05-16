// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#include "test_io.h"

#include <castor.h>
#include <vector>
#include <string>
#include <sstream>

using namespace std;
using namespace castor;

int getN(int /*i1*/, int /*i2*/, int /*i3*/, int /*i4*/, int /*i5*/, int /*i6*/)     { return 6; }

void test_writeTo() {
	{ // write value to stream
	stringstream sstrm;
	if(!writeTo(sstrm, "Hello")())
        throw "failed test_writeTo 1"; 
    string s;
    sstrm >> s;
    if(s!="Hello")
        throw "failed test_writeTo 1"; 
    }
}

void test_writeAllTo() {
  string as[] = {"1","2","3","4"};
  { // write to stream using pointers
	stringstream sstrm;
	if(!writeAllTo(sstrm,as,as+4, " ")())
        throw "failed test_writeAllTo 1"; 
    string s; int i=0;
    while(sstrm >> s) {
      if(s!=as[i++])
          throw "failed test_writeAllTo 1"; 
    }
  }
  { // write to stream using iterators
	stringstream sstrm;
    vector<string> vs (as,as+4);
    if(!writeAllTo(sstrm,vs.begin(),vs.end(),"")())
        throw "failed test_writeAllTo 2"; 
    string s;
    while(sstrm >> s) {
      if(s!="1234")
          throw "failed test_writeAllTo 2"; 
    }
  }	
  { // write container to stream
	stringstream sstrm;
    lref<const vector<string> > vls = vector<string>(as,as+4);
    if(!writeAllTo(sstrm,vls,"")())
        throw "failed test_writeAllTo 3"; 
    string s;
    while(sstrm >> s) {
      if(s!="1234")
          throw "failed test_writeAllTo 3"; 
    }
  }
  {
    // acquire lref<iterators> relationally and then use them with writeAllTo
    stringstream sstrm;
    lref<vector<string> > lvs = vector<string>(as,as+4);
    lref<vector<string>::iterator> b,e;
    relation r = begin(lvs,b) && end(lvs,e) && writeAllTo(sstrm,b,e,"");
    while(r());
    string s;
    while(sstrm >> s) {
      if(s!="1234")
          throw "failed test_writeAllTo 4"; 
    }
  }
}

namespace  {
struct num {
    int i;

    num(int i) : i(i)
    { }

    num(const num& n) : i(n.i)
    { }

    num operator + (const num& r) const {
        return num(i + r.i);
    }
    static num static_compute(num i, num j, num k) {
        return num(i.i + j.i / k.i);
    }

    num nonstatic_compute(num i, num j, num k) {
        return num(i.i + j.i / k.i);
    }
};

num operator *(num l, num r) {
    return num(l.i * r.i);
}

ostream& operator << (ostream &o, const num& n) {
    return o << n.i;
}

struct Functor6 {
    typedef int result_type;
    static int result;

    int operator() (void)                                               { return 0; }
    int operator() (int i1)                                             { return i1; }
    int operator() (int i1, int i2)                                     { return i1+i2; }
    int operator() (int i1, int i2, int i3)                             { return i1+i2+i3; }
    int operator() (int i1, int i2, int i3, int i4)                     { return i1+i2+i3+i4; }
    int operator() (int i1, int i2, int i3, int i4, int i5)             { return i1+i2+i3+i4+i5; }
    int operator() (int i1, int i2, int i3, int i4, int i5, int i6)     { return i1+i2+i3+i4+i5+i6; }

    int method (void)                                               { return 0; }
    int method (int i1)                                             { return i1; }
    int method (int i1, int i2)                                     { return i1+i2; }
    int method (int i1, int i2, int i3)                             { return i1+i2+i3; }
    int method (int i1, int i2, int i3, int i4)                     { return i1+i2+i3+i4; }
    int method (int i1, int i2, int i3, int i4, int i5)             { return i1+i2+i3+i4+i5; }
    int method (int i1, int i2, int i3, int i4, int i5, int i6)     { return i1+i2+i3+i4+i5+i6; }

    int method (int i1, int i2, int i3)   const                     { return i1+i2+i3; }

	int max(int i, int j) { return (i>j)?i:j; }
    double max(double i, double j) { return (i>j)?i:j; }
};
}//namespace {}


void test_writeTo_f() {
	{ // write value of computing an ILE to stream
	stringstream sstrm;
	lref<num> i=3;
	writeTo_f(sstrm, i+i*i)();
	if(sstrm.str()!="12")
		throw "failed test_writeTo_f 1";
	}
	{ // write value of computing a func
	stringstream sstrm;
	lref<num> i=3;
	writeTo_f(sstrm, &num::static_compute, i,3,i)();
	if(sstrm.str()!="4")
		throw "failed test_writeTo_f 2";
	}
    {
	stringstream sstrm;
	lref<int> i=3;
	writeTo_f(sstrm, Functor6(), i,i,i)();
	if(sstrm.str()!="9")
		throw "failed test_writeTo_f 3";
    }
}

void test_writeTo_mf() {
	{
	stringstream sstrm;
	lref<num> i=3;
	lref<num> n(5);
	writeTo_mf(sstrm, n,&num::nonstatic_compute, i,i,i)();
	if(sstrm.str()!="4")
		throw "failed test_writeTo_mf 1";
	}
}

namespace {
struct Base { 
    int b; 
    Base() : b(0) { }
};
struct Derived : Base { 
    int d; 
    Derived() : d(1), Base() { }
};
}
void test_writeTo_mem() {
	{
    typedef pair<string,string> name;
	lref<name> me = name("Roshan","Naik");
	stringstream sstrm;
    writeTo_mem(sstrm, me, &name::first)();
	if(sstrm.str()!="Roshan")
		throw "failed test_writeTo_mem 1";
	}
    { // base class member
	lref<Derived> d = Derived();
	stringstream sstrm;
    writeTo_mem(sstrm, d, &Base::b)();
    writeTo_mem(sstrm, d, &Derived::b)();
    writeTo_mem(sstrm, d, &Derived::d)();
	if(sstrm.str()!="001")
		throw "failed test_writeTo_mem 2";
    }
}

void test_writeTo_readFrom() {
  { // write value to stream and then read it
    stringstream sstrm;
    relation r = writeTo(sstrm, "Hello") && readFrom(sstrm,"Hello");
    if(!r())
      throw "failed test_writeTo_readFrom 1"; 
  }
  {// copy words from one stream to another
    stringstream inputData, outputData;
    inputData << "Hello World.";
    lref<string> ls;
    relation copyWords = readFrom(inputData, ls) && writeTo(outputData, ls);
    int count=0;
    while(copyWords()) { ++count; }
    if(count!=2 ||  ls.defined())
      throw "failed test_writeTo_readFrom 2";
  }
}


void test_readFrom() {
    { // read value into undefined lref
	stringstream sstrm;
	sstrm << "Hello";
	lref<string> s;
	if(!readFrom(sstrm,s)())
		throw "failed test_readFrom 1";
	}
	{ // read value into defined lref
	stringstream sstrm;
	sstrm << "Hello";
	lref<string> s = "Hello";
	if(!readFrom(sstrm,s)())
		throw "failed test_readFrom 2";
	}
	{ // read value into defined lref
	stringstream sstrm;
	sstrm << "Hello";
	if(readFrom(sstrm,"World")())
		throw "failed test_readFrom 3";
	}
	{ // read all values from stream
	stringstream sstrm;
	sstrm << "Hello World";
	lref<string> s;
	relation r = readFrom(sstrm,s);
	if(!r() || *s!="Hello")
		throw "failed test_readFrom 4";
	if(!r() || *s!="World")
		throw "failed test_readFrom 4";
	}
}
