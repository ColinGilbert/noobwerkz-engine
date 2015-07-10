// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_OPS_H
#define CASTOR_OPS_H 1

#include "relation.h"
#include "lref.h"
#include "workaround.h"
#include "coroutine.h"

namespace castor {

//---------------------------------------------------------------
//    Relational OR operator : provides backtracking
//---------------------------------------------------------------
//Concepts: L supports  bool operator()
//        : R supports  bool operator()

template<typename L, typename R>
class Or_r : public Coroutine {
	L l;
	R r;
public:
	Or_r(const L & l, const R & r) : l(l), r(r)
	{ }

	bool operator() (void) {
	  co_begin();
	  while(l())
		co_yield(true);
	  while(r())
		co_yield(true);
	  co_end();
	}
};

inline
Or_r<relation,relation> operator || (const relation& l, const relation& r) {
	return Or_r<relation,relation>(l, r);
}

//---------------------------------------------------------------
//    Relational AND operator
//---------------------------------------------------------------

template<typename L, typename R, bool fast=false>
class And_r : public Coroutine {
  L l;
  relation r; R rbegin;
public:
	And_r(const L& l, const R& r) : l(l), r(r), rbegin(r)
	{ }

  bool operator () (void) { // SLOW
    co_begin();
    while(l()) {
	  while(r())
        co_yield(true);
      r=rbegin;
    }
    co_end();
  }
};

template<typename L, typename R> 
class And_r <L,R,true> : public Coroutine {
  L l;
  R r;
public:
	And_r(const L& l, const R& r) : l(l), r(r)
	{ }

  bool operator () (void) {
    co_begin();
	while(l()) {
	  if(r())
		co_yield(true);
	  r.reset();
	}
	co_end();
  }
};

namespace detail {
struct twoBytes { char a[2]; };
template<typename T> char     fastAndCheck(typename T::UseFastAnd *);
template<typename T> twoBytes fastAndCheck(...);

} // namespace detail

template<typename T>
struct IsTestOnlyRelation {
  enum {result=( sizeof(::castor::detail::fastAndCheck<T>(0))==1 )};
};

template<typename Rel>
struct IsRelation_Constraint {
  static void constraints(Rel a) {
	relation r = a;
  }
};

#if defined(CASTOR_DISABLE_AND_OPTIMIZATION)
inline
And_r<relation,relation, false> operator && (const relation & l, const relation & r) {
  return And_r<relation, relation, false>(l, r);
}
#else
template<typename R> inline
And_r<relation,R,IsTestOnlyRelation<R>::result> operator && (const relation & l, const R & r) {
  return And_r<relation, R, IsTestOnlyRelation<R>::result>(l, r);
}
#endif


template<typename L, typename R>
class ExOr_r : public Coroutine {
    L l; 
    R r;
	bool lSucceeded;
public:
    ExOr_r(const L& l, const R& r) : l(l), r(r), lSucceeded(false)
    { }

    bool operator () (void) {
      co_begin();
      while(l()) {
        lSucceeded=true;
        co_yield(true);
      }
      if(lSucceeded)
        co_return(false);
	  while(r()) {
        co_yield(true);
	  }
      co_end();
    }
};


inline
ExOr_r<relation,relation> operator ^ (const relation & l, const relation & r) {
	return ExOr_r<relation,relation>(l, r);
}

} // namespace castor
#endif
