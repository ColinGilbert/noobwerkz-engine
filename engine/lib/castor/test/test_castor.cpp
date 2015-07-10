// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <list>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <ctime>
#include <utility>
#include <set>
#include <map>

#include <castor.h>

#include "test_core.h"
#include "test_utils.h"
#include "test_predicate.h"
#include "test_eq.h"
#include "test_io.h"
#include "test_ile.h"
#include "test_collection.h"
#include "test_tlr.h"
#include "test_higher_order.h"
#include "test_aggregate.h"

using namespace std;
using namespace castor;


void runtests();


int main() 
{
  // clock_t start = clock();
  runtests();
  // cout << "\nTime taken " << (clock() - start) << "\n\n Testing Complete! \a\n";
  cout << "\n";
  return 0;
}


//--------------------------------------------------------
//   Family relationships
//--------------------------------------------------------

//  (mother1, father1) =>(son1 and daughter1); 
//  (mother2, father2) =>(son2 and daughter2) 
//  (gmother2, gfather2) =>(father2)
relation child( lref<string> progeny, lref<string> mother, lref<string> father ) {
    return     eq(progeny,"son1")      &&  eq(mother,"mother1")  && eq(father,"father1")
            || eq(progeny,"daughter1") &&  eq(mother,"mother1")  && eq(father,"father1")
            || eq(progeny,"son2")      &&  eq(mother,"mother2")  && eq(father,"father2")
            || eq(progeny,"daughter2") &&  eq(mother,"mother2")  && eq(father,"father2")
			|| eq(progeny,"father2")   &&  eq(mother,"gmother2") && eq(father,"gfather2")

			//|| eq(progeny,"gfather3")   &&  eq(mother,"ggmother2") && eq(father,"ggfather2")
			//|| eq(progeny,"gfather4")   &&  eq(mother,"ggmother2") && eq(father,"ggfather2")
			//|| eq(progeny,"gfather4")   &&  eq(mother,"ggmother2") && eq(father,"ggfather2")
	;
}


relation mother(lref<string> ch, lref<string> mom) {
    lref<string> x;
    return child(ch,mom,x);
}

relation father(lref<string> ch, lref<string> pop) {
    lref<string> x;
    return child(ch,x,pop);
}

relation parent(lref<string> par, lref<string> kid) {
    return father(kid,par) || mother(kid,par);
}

relation ancestor(lref<string> ans, lref<string> des) {
	lref<string> X;
	return parent(ans,des)
		|| parent(X,des) && recurse(ancestor,ans,X)
	;
}

relation spouse(lref<string> husband, lref<string> wife) {
	lref<string> child;
	return father(child,husband) && mother(child,wife);
}


//--------------------------------------------------------
//   Tests : Family relationship
//--------------------------------------------------------

void test_all_husband_wives() {
    lref<string> h,w;
    relation r=spouse(h,w);
    string expectedH[] = {"father1", "father1", "father2", "father2", "gfather2"};
    string expectedW[] = {"mother1", "mother1", "mother2", "mother2", "gmother2"};
    int i=0;
    for(; r(); ++i) {
        //cout << *h << " " << *w <<"\n";
        if(i>4 || *h!=expectedH[i] || *w!=expectedW[i])
            throw "failed test_all_husband_wives";
    }
    if(i==0 || h.defined() || w.defined())
        throw "failed test_all_husband_wives";
}

void test_all_mothers() {
    lref<string> w, ch;
    relation r=mother(ch,w);
    string expected[] = {"mother1", "mother1", "mother2", "mother2", "gmother2"};
    int i=0;
    for(; r(); ++i) {
        //cout << *w <<"\n";
        if(i>4 || expected[i]!=*w)
          throw "failed test_all_mothers";
    }
    if(i==0 || w.defined() || ch.defined())
        throw "failed test_all_mothers";
}

void test_wifeof_father1() {
    lref<string> w;
    relation r=spouse("father1",w);
    string expected[] = {"mother1", "mother1"};
    int i=0;
    for(; r(); ++i) {
        // cout << *w <<"\n";
        if(i>1 || expected[i]!=*w)
          throw "failed test_wifeof_father1";
    }
    if(i==0 || w.defined())
        throw "failed test_wifeof_father1";
}

void test_husbandof_wife2() {
    lref<string> h;
    relation r=spouse(h,"mother2");
    string expected[] = {"father2", "father2"};
    int i=0;
    for(; r(); ++i) {
        //cout << *h <<"\n";
        if(i>1 || expected[i]!=*h)
          throw "failed test_husbandof_wife2";
    }
    if(i==0 || h.defined())
        throw "failed test_husbandof_wife2";
}

void test_childrenof_f1_m1() {
    lref<string> ch;
    relation r=child(ch,"mother1","father1");

    string expected[] = { "son1", "daughter1"};
    int i=0;
    for(; r(); ++i) {
       //cout << *ch <<"\n";
      if(i>1 || expected[i]!=*ch)
          throw "failed test_childrenof_f1_m1";
    }
    if(i==0 || ch.defined())
        throw "failed test_childrenof_f1_m1";
}

void test_ischildof_f1_m1() {
    relation r=child("daughter1","mother1","father1");
    if(!r())
        throw "failed test_ischildof_f1_m1";
}

void test_parent_s1() {
    lref<string> p;
    relation r=parent(p,"son1");
    string expected[] = { "father1", "mother1"};
    int i=0;
    for(; r(); ++i) {
       //cout << *p << " is the parent of son1\n";
      if(i>1 || expected[i]!=*p)
          throw "failed test_parent_s1";
    }
    if(i==0 || p.defined())
        throw "failed test_parent_s1";
}

void test_ancestors_s2() {
    lref<string> a;
    relation r = ancestor(a,"son2");
    string expected[] = { "father2", "mother2", "gfather2", "gmother2" };
    int i=0;
    for(; r(); ++i) {
        // cout << *a << " is an ancestor of son2!\n";
        if(i>3 || *a!=expected[i])
            throw "failed test_ancestors_s2";
    }
    if(i==0 || a.defined() )
        throw "failed test_ancestors_s2";
}

void test_gf2_ancestor_s2() {
    lref<string> a;
    relation r = ancestor("gfather2","son2");
    if(!r())
        throw "failed test_gf2_ancestor_s2";
}

//--------------------------------------------------------
//   Graph Relations
//--------------------------------------------------------

relation edge(lref<int> n1, lref<int>n2) {
    return 
           eq(n1,1) && eq(n2,2)
        || eq(n1,2) && eq(n2,3)
        || eq(n1,3) && eq(n2,4)
        || eq(n1,4) && eq(n2,5)
    ;
}

relation path(lref<int> n1, lref<int> n2) {
    lref<int> intermediateNode;
    return  edge(n1, n2) 
         || edge(n1, intermediateNode) && recurse(path, intermediateNode, n2)
        ;
}

//--------------------------------------------------------
//   Test: Graph 
//--------------------------------------------------------

void test_allNodes_connectedTo_1() {
    lref<int> x;
    relation r=path(1,x);
	int expected[] = {2,3,4,5};
	int i=0;
	for(; r(); ++i) {
		// cout << "connected to " << *x << "\n";
		if(i>3 || *x!=expected[i])
			throw "failed test_allNodes_connectedTo_1";
	}
	if(i==0 || x.defined())
		throw "failed test_allNodes_connectedTo_1";

}

void test_path_1_2() {
	relation r=path(1,2);
	int i=0;
	for(; r(); ++i);
	if(i!=1)
		throw "failed test_path_1_2";
}

void test_path_1_5() {
	relation r=path(1,5);
	int i=0;
	for(; r(); ++i);
	if(i!=1)
		throw "failed test_path_1_5";
}

void test_nopath_1_5() {
	relation r=path(1,10);
	int i=0;
	for(; r(); ++i);
	if(i!=0)
		throw "failed test_path_1_5";
}


//--------------------------------------------------------
//   Integer Arithmetic  : Plus
//--------------------------------------------------------
// Note: Will return false if more than one argument is undefined.
relation plus_(lref<int> x, lref<int> y, lref<int> z) {
	return defined(x) && defined(y) && undefined(z) && eq_f(z,x+y)
	   ||  defined(x) && defined(z) && undefined(y) && eq_f(y,z-x)
	   ||  defined(y) && defined(z) && undefined(x) && eq_f(x,z-y)
	   ||  defined(x) && defined(y) && defined(z)   && eq_f(z,x+y)
	;
}

//--------------------------------------------------------
//   Tests : Integer Arithmetic  : Plus
//--------------------------------------------------------
void test_plus() {
	relation r = plus_(1,2,3);
	int i=0;
	for(; r(); ++i);
	if(i!=1)
		throw "failed test_plus";
}

void test_plus_z() {
	lref<int> z;
	relation r = plus_(1,2,z);
	int i=0;
	for(; r(); ++i) {
		if(i>0 || *z!=3)
			throw "failed test_plus_z";
	}
	if(i!=1 || z.defined())
		throw "failed test_plus_z";
}

void test_plus_y() {
	lref<int> y;
	relation r = plus_(-1,y,3);
	int i=0;
	for(; r(); ++i) {
		if(*y!=4)
			throw "failed test_plus_y";
	}
	if(i!=1 || y.defined())
		throw "failed test_plus_y";
}

void test_plus_x() {
	lref<int> x;
	relation r = plus_(x,-2,-3);
	int i=0;
	for(; r(); ++i) {
		if(*x!=-1)
			throw "failed test_plus_x";
	}
	if(i!=1 || x.defined())
		throw "failed test_plus_x";
}

//--------------------------------------------------------
//   Integer Arithmetic  : Multiply, Square
//--------------------------------------------------------

// Note : Is this a better model of implementation than plus() ?
relation multiply(int x, int y, int z) {
	if(z==x*y)
        return True();
	return False();
}
relation multiply(int x, int y, lref<int> z) {
	return eq(z,x*y);
}

relation multiply(int x, lref<int> y, int z) {
	return eq(y,z/x);
}

relation multiply(lref<int> x, int y, int z) {
	return eq(x,z/y);
}

// unlike plus(), above multiple() overloads correctly disallows ...
//        relation square(lref<int> n, lref<int> s) { .... }
relation square(int n, lref<int> s) {
	return multiply(n,n,s);
}

//--------------------------------------------------------
//   Tests : Integer Arithmetic  : Multiply, Square
//--------------------------------------------------------

void test_multiply() {
	relation r = multiply(1,1,1);
	int i=0;
	for(;r(); ++i);
	if(i!=1)
		throw "failed test_multiply";
}

void test_multiply_x() {
	lref<int> x;
	relation r = multiply(x,2,6);
	int i=0;
	for(;r(); ++i) {
		if(i>0 || *x!=3)
			throw "failed test_multiply_x";
	}
	if(i!=1)
		throw "failed test_multiply_x";
}

void test_multiply_z() {
	lref<int> z;
	relation r = multiply(3,2,z);
	int i=0;
	for(;r(); ++i) {
		if(i>0 || *z!=6)
			throw "failed test_multiply_z";
	}
	if(i!=1)
		throw "failed test_multiply_z";
}

void test_square() {
	relation r = square(7,49);
	int i=0;
	for(;r(); ++i);
	if(i!=1)
		throw "failed test_square";
}

void test_square_s() {
	lref<int> s;
	relation r = square(9,s);
	int i=0;
	for(;r(); ++i) {
		if(i>0 || *s!=81)
			throw "failed test_square_s";
	}
	if(i!=1)
		throw "failed test_square_s";
}


//--------------------------------------------------------
//   Relational looping using && False()
//--------------------------------------------------------

bool push(std::stack<std::string>& coll, std::string val) {
    coll.push(val);
    return true;
}

// some_relation && False()  ... for looping through all solutions in some_relation
void test_relational_loop() {
    lref<string> ch, f;
    lref<stack<string> > lst = stack<string>();
    relation r = child(ch,"mother1", f) && /*write_f(ch+string(" ")) && */predicate(&push, lst, ch) && False();
    if( r() )
        throw "failed test_relational_loop";

    if( lst->size()!=2 )
        throw "failed test_relational_loop";
    if(lst->top()!="daughter1")
        throw "failed test_relational_loop";
    lst->pop();
    if(lst->top()!="son1")
        throw "failed test_relational_loop";
}

//--------------------------------------------------------
//  Tests  : Finite Automata
//--------------------------------------------------------

struct Nfa {
// all tranistions in the NFA => ((ab)*ba)|b
static relation transition(lref<int> state1,  lref<char> input, lref<int> state2) {
return eq(state1, 0) && eq(input, 'a') && eq(state2, 1)
   ||  eq(state1, 0) && eq(input, 'b') && eq(state2, 2)
   ||  eq(state1, 0) && eq(input, 'b') && eq(state2, 4)
   ||  eq(state1, 1) && eq(input, 'b') && eq(state2, 0)
   ||  eq(state1, 2) && eq(input, 'a') && eq(state2, 3) ;
}
// all final states of the NFA
static relation final(lref<int> state) {
    return eq(state, 3) || eq(state, 4);
}
};
   
// rule determining successful exuecution of a FA
template<typename FA>
relation runFA(lref<string> input,  lref<int> startState=0)
{
  lref<char> firstChar;
  lref<string> rest;
  lref<int> nextState;
#ifdef __BCPLUSPLUS__
  relation (*self)(lref<string>,  lref<int>) = &runFA<FA>;
  return eq(input, "") && FA::final(startState)
	  || head(input,firstChar) && FA::transition(startState, firstChar, nextState) && tail(input, rest) && recurse(self, rest, nextState)
	  ;
#else
  return eq(input, "") && FA::final(startState)
	  || head(input,firstChar) && FA::transition(startState, firstChar, nextState) && tail(input, rest) && recurse(runFA<FA>, rest, nextState)
	  ;
#endif
}

void test_FA() {
	if( ! runFA<Nfa>("abba")() )
		throw "failed test_FA";
    if( runFA<Nfa>("aabba")() )
        throw "failed test_FA";
}


namespace {
int testCount=0;
int passCount=0;
int failCount=0;
}

void run_test( void(*testFunc)(void) ) {
	try {
		++testCount;
		testFunc();
		++passCount;
	}
    catch (const char* err) {
        ++failCount;
        cout << err <<"\n";
    }
}

void runtests() {
    // lref tests
    run_test( test_lref );
    run_test( test_begin_end ); // begin and end for lref<CollectionT>

    // test for || and && operators
    run_test( test_or_and );
	
    // family
	run_test( test_all_husband_wives );
	run_test( test_all_mothers );
	run_test( test_wifeof_father1 );
	run_test( test_husbandof_wife2 );
	run_test( test_childrenof_f1_m1 );
	run_test( test_ischildof_f1_m1 );
	run_test( test_parent_s1 );
	run_test( test_ancestors_s2 );
	run_test( test_gf2_ancestor_s2 );
    run_test( test_relational_loop );

	//graph
	run_test( test_allNodes_connectedTo_1 );
	run_test( test_path_1_2 );
	run_test( test_path_1_5 );
	run_test( test_nopath_1_5 );

	//// artithmetic  : plus
	run_test( test_plus );
	run_test( test_plus_z );
	run_test( test_plus_y );
	run_test( test_plus_x );

	// artithmetic  : multiply, square
	run_test( test_multiply );
	run_test( test_multiply_x );
	run_test( test_multiply_z );
	run_test( test_square );
	run_test( test_square_s );
    run_test( test_Prefix_UnaryOps_ILE );
    run_test( test_Postfix_UnaryOps_ILE );

    // eq
    run_test( test_eq );
    run_test( test_eq_values );
    run_test( test_eq_with_coll );
    run_test( test_eq_seq );
    run_test( test_eq_f );
    run_test( test_eq_mf );
    run_test( test_eq_mem );

    // ILE
    run_test( test_ILE );
	run_test( test_ILE_more );
    run_test( test_eq_ILE );
    run_test( test_Create_with );
    run_test( test_create );
    run_test( test_get );
    run_test( test_call );
	run_test( test_mcall );
	run_test( test_at );

    // merge
     run_test( test_merge );
     // run_test( test_merge_withcuts );

    // prev & next
    run_test( test_next );
    run_test( test_prev );

    // head & tail
    run_test( testHead );
    run_test( test_head_list );
    run_test( test_tail );
    run_test( test_tail_list );
    run_test( test_head_tail );
    run_test( test_head_n_tail );
	
	// inc/dec
    run_test( test_inc_dec );

	// insert
    run_test( test_insert );
    run_test( test_insert_seq );    

    //sequence()
    run_test( test_sequence );
    
    // runFA<Nfa>()
    run_test( test_FA );

    // recurse
    run_test( test_recurse_mf );
    
    // predicate
    run_test( test_predicate );
    run_test( test_predicate_mf );
    run_test( test_predicate_mem );
	
    // Dynamic relations
	run_test( test_ExDisjunctions );
    run_test( test_Disjunctions );
    run_test( test_Conjunctions );

    // Utils
    run_test( test_cut );

    run_test( test_range );
    run_test( test_range_with_step );
    run_test( test_range_dec );
    run_test( test_range_dec_with_step );

    run_test( test_item );
    run_test( test_item_set );
    run_test( test_item_map );
    run_test( test_ritem );

    run_test( test_getValueCont );

    run_test( test_defined );
    run_test( test_undefined );

    run_test( test_unique );
    run_test( test_unique_f );
	run_test( test_unique_mf );
	run_test( test_unique_mem );

    run_test( test_empty );
    run_test( test_not_emtpy );

    run_test( test_dereference );

    run_test( test_eval );
    run_test( test_eval_mf );

    run_test( test_readFrom );
    run_test( test_writeTo );
    run_test( test_writeAllTo );
    run_test( test_writeTo_f );
    run_test( test_writeTo_mf );
    run_test( test_writeTo_mem );

    run_test( test_writeTo_readFrom );

    run_test( test_op_exor );
	run_test( test_repeat );
	run_test( test_pause );
	run_test( test_pause_f );

    run_test( test_zip );

	run_test( test_True );

	// collection 
	run_test( test_shuffle );
    run_test( test_permutation );
    run_test( test_permutation_cmp );

	// TLRs
	run_test( test_order );
	run_test( test_order_mf );
	run_test( test_order_mem );
	run_test( test_reverse );
    run_test( test_skip );
    run_test( test_group_by );

    // aggregates
	run_test( test_count );
    run_test( test_size );
    run_test( test_size_of );
    run_test( test_max );
    run_test( test_min );
	run_test( test_sum );
	run_test( test_reduce );
    run_test( test_average );

    run_test( test_max_of );
    run_test( test_min_of );
    run_test( test_sum_of );
    run_test( test_reduce_of );
    run_test( test_average_of );

	cout << "\nFailed: " << failCount 
         << "\nPassed: " << passCount  
         << "\nTotal: " << testCount;
}
