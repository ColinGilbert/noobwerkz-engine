// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).


#include <castor.h>
#include <iostream>
#include <vector>

using namespace castor;
using namespace std;


void test_order() {
	{
		int ai[] = { 10,9,8,7,6,5,4,3,2,1 };
		vector<int> vi(ai,ai+10); 
		sort(vi.begin(), vi.end());

		lref<int> j;
		relation r = item(j,ai,ai+10) >>= order(j);
		for(int i=0; r(); ++i) {
			if(vi.at(i)!=*j)
				throw "failed test_order 1";
		}
		if(j.defined())
			throw "failed test_order 1";
	}
	{// with comparator
		int ai[] = { 10,9,8,7,6,5,4,3,2,1 };
		vector<int> vi(ai,ai+10); 
		sort(vi.begin(), vi.end());

		lref<int> j;
        relation r = item(j,ai,ai+10) >>= order(j, std::greater<int>());
		for(int i=9; r(); --i) {
			if(vi.at(i)!=*j)
				throw "failed test_order 2";
		}
		if(j.defined())
			throw "failed test_order 2";
	}
}


void test_order_mf() {
	{
		string values[] = {"four","one","three"};
		string svalues[] = {"one","four","three"};
		vector<string> sorted_values(svalues,svalues+3);

		lref<string> v;
		relation r = item(v, values, values+3) >>= order_mf(v, &string::length );
		for(int i=0; r(); ++i) {
			if(sorted_values.at(i)!=*v)
				throw "failed test_order_mf 1";
		}
		if(v.defined())
			throw "failed test_order_mf 1";
	}
	{ // with comparator
		string values[] = {"four","one","three"};
		string svalues[] = {"three","four","one"};
		vector<string> sorted_values(svalues,svalues+3);

		lref<string> v;
		relation r = item(v, values, values+3) >>= order_mf(v, &string::length, greater<size_t>() );
		for(int i=0; r(); ++i) {
			if(sorted_values.at(i)!=*v)
				throw "failed test_order_mf 2";
		}
		if(v.defined())
			throw "failed test_order_mf 2";
	}
}


namespace {
struct Number {
	int i;
	Number(int i) : i(i)
	{ }

	bool operator==(const Number & rhs) {
		return i==rhs.i;
	}
}; 
} //namespace

void test_order_mem() {
	{
		Number values[] = {3,1,4,2};
		Number svalues[] = {1,2,3,4};
		vector<Number> sorted_values(svalues,svalues+4);

		lref<Number> v;
		relation r = item(v, values, values+4) >>= order_mem(v, &Number::i );
		for(int i=0; r(); ++i) {
			if(sorted_values.at(i).i!=v->i)
				throw "failed test_order_mem 1";
		}
		if(v.defined())
			throw "failed test_order_mem 1";
	}
	{
		Number values[] = {3,1,4,2};
		Number svalues[] = {4,3,2,1};
		vector<Number> sorted_values(svalues,svalues+4);

		lref<Number> v;
		relation r = item(v, values, values+4) >>= order_mem(v, &Number::i, greater<int>() );
		for(int i=0; r(); ++i) {
			if(sorted_values.at(i).i!=v->i)
				throw "failed test_order_mem 2";
		}
		if(v.defined())
			throw "failed test_order_mem 2";
	}
}

void test_reverse() {
	{
		string values[] = {"four","one","two","three"};
		vector<string> rev(values,values+4);
		std::reverse(rev.begin(), rev.end());

		lref<string> s;
		relation r = item(s,values, values+4) >>= reverse(s);
		for(int i=0; r(); ++i) {
			if(rev.at(i)!=*s)
				throw "failed test_reverse 1";
		}
	}
}


void test_skip() {
	{ // int argument
	lref<int> i;
	int ai[] = { 1,2,3,4 };
    relation r = item(i,ai,ai+4) >>= skip(2);
    int j=3;
	while( r() ) {
		if(*i!=j++)
			throw "failed test_skip 1";
	}
	}
	{ // lref<int> argument
	    lref<int> i;
        lref<unsigned long> s=2;
        relation r = range(i,1,4) >>= skip(s);
        int j=3;
	    while( r() ) {
		    if(*i!=j++)
			    throw "failed test_skip 2";
	    }
    }
    { // skip first two evens in the array
    int ai[] = { 1,2,3,4,5,6,7,8 };
    lref<int> i;
    relation r = item(i,ai,ai+8) && predicate(i%2==0) >>= skip(2) ;
    if(! r() )
        throw "failed test_skip 3";
    if( *i!=6 )
        throw "failed test_skip 3";
    if(! r() )
        throw "failed test_skip 3";
    if( *i!=8 )
        throw "failed test_skip 3";
    if( r() )
        throw "failed test_skip 3";
    if( i.defined() )
        throw "failed test_skip 3";
    }
	{ // skip - more than items generated
	    lref<int> i;
        relation r = range(i,1,4) >>= skip(5);
	    if( r() )
		    throw "failed test_skip 4";
    }
}


char firstChar(string& s) {
	return s.at(0);
}

size_t slength(string& s) {
	return s.length();
}

bool palind(string& s) {
	return std::equal(s.begin(), s.end(), s.rbegin());
}


bool myCmp(pair<string,pair<char,int> >& lhs, pair<string,pair<char,int> >& rhs) {
	if(lhs.second.first < rhs.second.first)
		return true;
	if(lhs.second.first == rhs.second.first)
		return lhs.second.second <= rhs.second.second;
	return false;
}


void test_group_by() {
	// common data for all tests
	string numbers[] = { "One", "Three", "Two", "Four", "Five", "Six" , "million", "madam" };
	size_t len = sizeof(numbers)/sizeof(string);

	vector<string> nums(numbers,numbers+len);
	lref<vector<string> > lnums(&nums,false);

	{ // nested grouping (3 levels)
		lref<string> num;
		string results[] = { "Four", "Five", "One", "Six", "Two", "Three", "madam", "million" };
		char keys1[] = { 'F', 'O', 'S', 'T', 'm' };
		size_t  keys2[] = { 4, 3, 3, 3, 5, 5, 7 };
		bool keys3[] = { false, false, false, false, false, true, false };
		int  v=0, k1=0, k2=0, k3=0;

		lref<group<char,group<size_t,group<bool,string> > > > g1;
		lref<group<size_t,group<bool,string> > > g2;
		lref<group<bool,string> > g3;

		relation r1 = item(num,lnums) >>= group_by(num, &firstChar, g1).then(slength).then(palind) ;
		lref<string> s;
		while(r1()) {
			if(g1->key!=keys1[k1++])
				throw "failed test_group_by 1";
			relation r2 = item(g2,g1);
			while(r2()) {
				if(g2->key!=keys2[k2++])
					throw "failed test_group_by 1";
				relation r3 = item(g3,g2);
				while(r3()) {
					if(g3->key!=keys3[k3++])
							throw "failed test_group_by 1";
					relation values = item(s,g3);
					while(values())
						if(*s!=results[v++])
							throw "failed test_group_by 1";
				}
			}
		}
		if(g1.defined() || g2.defined() || g3.defined() || s.defined() || num.defined())
			throw "failed test_group_by 1";

		 //- Following two lines of code must emit compiler error - "then() is not a member of .. "
		 //group_by(num, &firstChar, g2).then(slength).then(palind);  
		 //group_by(num, &firstChar, g3).then(slength).then(palind).then(palind);
	}
	{ // nested grouping - 2 levels
		lref<string> num;
		string results[] = { "Four", "Five", "One", "Six", "Two", "Three", "madam", "million" };
		char keys1[] = { 'F', 'O', 'S', 'T', 'm' };
		size_t keys2[] = { 4, 3, 3, 3, 5, 5, 7 };
		int v=0, k1=0, k2=0;

		lref<group<char,group<size_t,string> > > g1;
		lref<group<size_t,string> > g2;
		relation outer = item(num,lnums) >>= group_by(num, &firstChar, g1).then(slength);
		lref<string> s;
		while(outer()) {
			if((g1->key)!=keys1[k1++])
				throw "failed test_group_by 2";
			relation inner = item(g2,g1);
			while(inner()) { 
				if(g2->key!=keys2[k2++])
					throw "failed test_group_by 2";
				relation values = item(s,g2);
				while(values())
					if(*s!=results[v++])
						throw "failed test_group_by 2";
			}
		}
		if(g1.defined() || g2.defined() || s.defined() || num.defined())
			throw "failed test_group_by 2";
	}
	{ // basic grouping - 1 levels
		lref<string> num;
		string results[] = { "Four", "Five", "One", "Six", "Three", "Two", "million", "madam" };
		char keys[] = { 'F', 'O', 'S', 'T', 'm' };
		int v=0, k=0;
		lref<group<char,string> > g;
		relation r = item(num,lnums) >>= group_by(num, &firstChar, g) ;
		lref<string> s;
		while(r()) {
			if((g->key)!=keys[k++])
				throw "failed test_group_by 3";
			relation values = item(s,g);
			while(values()) {
				if(*s!=results[v++])
					throw "failed test_group_by 3";
			}
		}
		if(g.defined() || s.defined() || num.defined())
			throw "failed test_group_by 3";
	}
	{ // basic grouping with keyCmp - 1 levels
		lref<string> num;
		string results[] = { "million", "madam", "Three", "Two", "Six", "One", "Four", "Five"};
		char keys[] = { 'm', 'T', 'S', 'O', 'F' };
		int v=0, k=0;
		lref<group<char,string> > g;
		relation r = item(num,lnums) >>= group_by(num, &firstChar, g, std::greater<char>());
		lref<string> s;
		while(r()) {
			if((g->key)!=keys[k++])
				throw "failed test_group_by 4";
			relation values = item(s,g);
			while(values()) {
				if(*s!=results[v++])
					throw "failed test_group_by 4";
			}
		}
		if(g.defined() || s.defined() || num.defined())
			throw "failed test_group_by 4";
	}
	{ // nested grouping with keyCmp - 2 levels
		lref<string> num;
		string results[] = { "million", "madam", "Three", "Two", "Six", "One", "Four", "Five"};
		char keys1[] = { 'm', 'T', 'S', 'O', 'F' };
		size_t keys2[] = { 7,5,5,3,3,3,4 };
		int v=0, k1=0, k2=0;

		lref<group<char,group<size_t,string> > > g1;
		lref<group<size_t,string> > g2;
		relation outer = item(num,lnums) >>= group_by(num, &firstChar, g1, std::greater<char>()).then(slength, std::greater<size_t>());
		lref<string> s;
		while(outer()) {
			if((g1->key)!=keys1[k1++])
				throw "failed test_group_by 5";
			relation inner = item(g2,g1);
			while(inner()) { 
				if(g2->key!=keys2[k2++])
					throw "failed test_group_by 5";
				relation values = item(s,g2);
				while(values()) {
					if(*s!=results[v++])
						throw "failed test_group_by 5";
				}
			}
		}
		if(g1.defined() || g2.defined() || s.defined() || num.defined())
			throw "failed test_group_by 2";
	}
	{ // nested grouping with keyCmp & item_order() - 2 levels
		lref<string> num;
		string results[] = { "million", "madam", "Three", "Two", "Six", "One", "Five", "Four" };
		char keys1[] = { 'm', 'T', 'S', 'O', 'F' };
		size_t keys2[] = { 7,5,5,3,3,3,4 };
		int v=0, k1=0, k2=0;

		lref<group<char,group<size_t,string> > > g1;
		lref<group<size_t,string> > g2;
        relation outer = item(num,lnums) >>= group_by(num, &firstChar, g1, std::greater<char>()).then(slength, std::greater<size_t>()).item_order(std::less<string>());
		lref<string> s;
		while(outer()) {
			if((g1->key)!=keys1[k1++])
				throw "failed test_group_by 6";
			relation inner = item(g2,g1);
			while(inner()) {
				if(g2->key!=keys2[k2++])
					throw "failed test_group_by 6";
				relation values = item(s,g2);
				while(values()) {
					if(*s!=results[v++])
						throw "failed test_group_by 6";
				}
			}
		}
		if(g1.defined() || g2.defined() || s.defined() || num.defined())
			throw "failed test_group_by 6";
	}
    { // empty input range
        lref<string> num;
		lref<group<char,group<size_t,string> > > g;
        relation r = item(num,numbers,numbers) >>= group_by(num, firstChar, g).then(slength);
        if(r())
			throw "failed test_group_by 7";
    }
}
