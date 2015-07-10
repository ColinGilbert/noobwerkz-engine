// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_AGGREGATE_H
#define CASTOR_AGGREGATE_H 1

#include <algorithm>
#include <numeric>
#include <functional>
#include "relation.h"
#include "lref.h"
#include "coroutine.h"

namespace castor {

//-------------------------------------------------
// count(n) - n is number of times argument relation succeeded
// For use with >>= operator
// throws InvalidArg() if n is initialized at the time evaluation
// Concept: T: is an integral type
//-------------------------------------------------

template<class T>
struct Count_tlr : public Coroutine {
	lref<T> n;

	explicit Count_tlr(const lref<T>& n) : n(n)
	{}

	bool operator() (relation& r) {
		co_begin();
		if(n.defined())
            throw InvalidArg();
		n=0;
        while(r())
			++(n.get());
		co_yield(true);
		n.reset();
		co_end();
	}
};
    
template<class T> inline
Count_tlr<T> count(const lref<T>& n) {
	return Count_tlr<T>(n);
}


//-------------------------------------------------
// reduce(i,binFunc) - All i reduced using binFunc
// For use with >>= operator
// throws InvalidArg() if obj is initialized at the time evaluation
// Concept: BinFunc is a functor taking two args:  (Sum&, T&)
//-------------------------------------------------
template<class T, class BinFunc>
struct Reduce_tlr : public Coroutine {
	lref<T> i, total;
	BinFunc acc;

	Reduce_tlr(const lref<T>& i, const BinFunc& acc) : i(i), acc(acc)
	{}

	bool operator() (relation& r) {
		co_begin();
		if(i.defined())
			throw InvalidArg();
		if(!r())
			co_return(false);
		for(total=i; r(); total.get()=acc(total.get(),*i) );
		i=total;
		co_yield(true);
		i.reset();
		co_end();
	}
};

template<class T, class BinFunc> inline
Reduce_tlr<T,BinFunc> reduce(lref<T>& i, BinFunc acc) {
	return Reduce_tlr<T,BinFunc>(i,acc);
}


//-------------------------------------------------
// reduce_of(continer,s,binary_op) - reduction (using binary_op) of all values in container is s
// Concepts: 
//    Cont: is a container (23.1)
//    Cont::value_type: is CopyConstructible (20.1.3) and Assignable (23.1)
//    Func2: is a binary functor accepting arguments of type Cont::value_type and returning Cont::value_type
// throws InvalidDeref() if container is not initialized at the time evaluation
//-------------------------------------------------
template<class Cont, class Func2=std::plus<typename Cont::value_type> >
class ReduceOf_r : public Coroutine {
	lref<Cont> cont;
    lref<typename Cont::value_type> s;
    Func2 adder;
public:
    ReduceOf_r(lref<Cont> cont, const lref<typename Cont::value_type>& s, Func2 adder= std::plus<typename Cont::value_type>() ) : cont(cont), s(s), adder(adder)
	{}

	bool operator() () {
		co_begin();
        if( cont->size()==0 )
            co_return(false);
        if(s.defined()) {
            co_yield( *s == std::accumulate(++cont->begin(), cont->end(), *cont->begin(), adder) );
        } else {
            s = std::accumulate(++cont->begin(), cont->end(), *cont->begin(), adder);
            co_yield(true);
            s.reset();
        }
  		co_end();
	}
};


// reduce_of(continer,s) - reduction (using binary_op) of values in container = s
template<class Cont, class BinFunc> inline
ReduceOf_r<Cont,BinFunc>
reduce_of(lref<Cont>& cont_, const lref<typename Cont::value_type>& r, BinFunc acc) {
	return ReduceOf_r<Cont,BinFunc>(cont_,r,acc);
}

//-------------------------------------------------
// sum(i) - Sum of all i
// For use with >>= operator
// throws InvalidArg() if obj is initialized at the time evaluation
//-------------------------------------------------
template<class T> inline
Reduce_tlr<T,std::plus<T> > sum(lref<T>& i) {
	return Reduce_tlr<T,std::plus<T> >(i, std::plus<T>());
}


//-------------------------------------------------
// sum_of(continer,s) - sum of all values in container is s
// Concepts: 
//    Cont: is a container (23.1)
//    Cont::value_type: is CopyConstructible (20.1.3) and Assignable (23.1)
//    Func2:  is a binary functor accepting arguments of type Cont::value_type and returning Cont::value_type
// throws InvalidDeref() if container is not initialized at the time evaluation
//-------------------------------------------------

// sum_of(continer,s) - sum (using operator +) of values in container = s
template<class Cont> inline
ReduceOf_r<Cont> 
sum_of(lref<Cont>& cont_, const lref<typename Cont::value_type>& s) {
	return ReduceOf_r<Cont>(cont_, s);
}


//-------------------------------------------------
// max(n) - largest n
// For use with >>= operator
// throws InvalidArg() if n is initialized at the time evaluation
// Concept: T: is T is LessThanComparable (20.1.2) and CopyConstructible (20.1.3)
//-------------------------------------------------

template<class T, class Cmp>
class Max_tlr : public Coroutine {
	lref<T> n;
    lref<T> max;
    Cmp cmp;
public:
    Max_tlr(const lref<T>& n, Cmp cmp) : n(n), cmp(cmp)
	{}

	bool operator() (relation& r) {
		co_begin();
		if(n.defined())
            throw InvalidArg();
        if(r())
		    max=*n;
        else 
            co_return(false);
        while(r())
            max = std::max(max.get(),*n,cmp);
        n = max.get();
		co_yield(true);
		n.reset();
		co_end();
	}
};

// max(n) - largest n
template<class T> inline
Max_tlr<T, std::less<T> > max(const lref<T>& n) {
	return Max_tlr<T, std::less<T> >(n, std::less<T>());
}

// max(n) - largest n
template<class T, class Cmp> inline
Max_tlr<T,Cmp> max(const lref<T>& n, Cmp cmp) {
	return Max_tlr<T,Cmp>(n,cmp);
}


//-------------------------------------------------
// max_of(continer,m) - max value in container is m
// Concepts: 
//    Cont: is a container (23.1)
//    Cont::value_type is LessThanComparable(20.1.2)
// throws InvalidDeref() if container is not initialized at the time evaluation
//-------------------------------------------------
template<class Cont, class Cmp>
class MaxOf_r : public Coroutine {
	lref<Cont> cont;
    lref<typename Cont::value_type> m;
    Cmp cmp;
public:
    MaxOf_r(lref<Cont> cont, const lref<typename Cont::value_type>& m, Cmp cmp) : cont(cont), m(m), cmp(cmp)
	{}

	bool operator() () {
		co_begin();
        if(cont->size()==0)
            co_return(false);
        if(m.defined())
          co_return( m.get()== * std::max_element(cont->begin(), cont->end(), cmp) );

        m= *std::max_element(cont->begin(), cont->end(), cmp);
	    co_yield(true);
		m.reset();
  		co_end();
	}
};

// max_of(continer,m) - max value in container is m
template<class Cont> inline
MaxOf_r<Cont, std::less<typename Cont::value_type> > 
max_of(lref<Cont>& cont_, const lref<typename Cont::value_type>& m) {
	return MaxOf_r<Cont, std::less<typename Cont::value_type> >(cont_, m, std::less<typename Cont::value_type> ());
}

// max_of(continer,m) - max value in container is m
template<class Cont, class Cmp> inline
MaxOf_r<Cont,Cmp> 
max_of(lref<Cont>& cont_, const lref<typename Cont::value_type>& m, Cmp cmp) {
	return MaxOf_r<Cont,Cmp>(cont_,m,cmp);
}


//-------------------------------------------------
// min(n) - smallest n
// For use with >>= operator
// throws InvalidArg() if n is initialized at the time evaluation
// Concept: T: is T is LessThanComparable (20.1.2) and CopyConstructible (20.1.3)
//-------------------------------------------------
template<class T, class Cmp>
class Min_tlr : public Coroutine {
	lref<T> n;
    lref<T> min;
    Cmp cmp;
public:
    Min_tlr(const lref<T>& n, Cmp cmp) : n(n), cmp(cmp)
	{}

	bool operator() (relation& r) {
		co_begin();
		if(n.defined())
            throw InvalidArg();
        if(r())
		    min=*n;
        else 
            co_return(false);
        while(r())
            min = std::min(min.get(),*n,cmp);
        n = min.get();
		co_yield(true);
		n.reset();
		co_end();
	}
};

// min(n) - smallest n
template<class T> inline
Min_tlr<T,std::less<T> > min(const lref<T>& n) {
    return Min_tlr<T,std::less<T> >(n, std::less<T>());
}

// min(n) - smallest n
template<class T, class Cmp> inline
Min_tlr<T,Cmp> min(const lref<T>& n, Cmp cmp) {
    return Min_tlr<T,Cmp>(n, cmp);
}


//-------------------------------------------------
// min_of(continer,m) - min value in container is m
// Concepts: 
//    Cont: is a container (23.1)
//    Cont::value_type is LessThanComparable(20.1.2)
// throws InvalidDeref() if container is not initialized at the time evaluation
//-------------------------------------------------
template<class Cont, class Cmp=std::less<typename Cont::value_type> >
class MinOf_r : public Coroutine {
	lref<Cont> cont;
    lref<typename Cont::value_type> m;
    Cmp cmp;
public:
    MinOf_r(lref<Cont> cont, const lref<typename Cont::value_type>& m, Cmp cmp=std::less<typename Cont::value_type>()) : cont(cont), m(m), cmp(cmp)
	{}

	bool operator() () {
		co_begin();
        if(cont->size()==0)
            co_return(false);
        if(m.defined())
          co_return( m.get()== * std::min_element(cont->begin(), cont->end(), cmp) );

        m=*std::min_element(cont->begin(), cont->end(), cmp);
	    co_yield(true);
		m.reset();
  		co_end();
	}
};

// min_of(continer,m) - min value in container is m
template<class Cont> inline
MinOf_r<Cont> 
min_of(lref<Cont>& cont_, const lref<typename Cont::value_type>& m) {
	return MinOf_r<Cont>(cont_, m);
}

// min_of(continer,m) - min value in container is m
template<class Cont, class Cmp> inline
MinOf_r<Cont,Cmp> 
min_of(lref<Cont>& cont_, const lref<typename Cont::value_type>& m, Cmp cmp) {
	return MinOf_r<Cont,Cmp>(cont_,m,cmp);
}


//-------------------------------------------------
// average(i,binFunc) - Average i  (using binFunc)
// For use with >>= operator
// throws InvalidArg() if obj is initialized at the time evaluation
// Concepts: 
//   Cont : Satisfy requirements of standard c++ containers  [$23.1].
//   Adder: is a binary functor which returns a T and takes two args of type T
//   Divider: is a binary functor which returns a T and takes args: (T, size_t)
//-------------------------------------------------
template<class T, class Adder=std::plus<T> >
struct Average_tlr : public Coroutine {
	lref<T> i, total;
	Adder adder;

	Average_tlr( const lref<T>& i, Adder adder = std::plus<T>()) : i(i), adder(adder)
	{}

	bool operator() (relation& r) {
        size_t count=0;
		co_begin();
		if(i.defined())
			throw InvalidArg();
		if(!r())
			co_return(false);
		for(total=i; r(); ++count )
            total.get()=adder(total.get(),*i);
		i= *total / count;
		co_yield(true);
		i.reset();
		co_end();
	}
};

template<class T> inline
Average_tlr<T> average(lref<T>& i) {
    return Average_tlr<T>(i);
}

template<class T, class Adder> inline
Average_tlr<T,Adder> average(lref<T>& i, Adder adder) {
	return Average_tlr<T,Adder>(i,adder);
}


//-------------------------------------------------
// avg_of(continer,a) - avg values in container is a
// Concepts: 
//    Cont: is a container (23.1)
//    Cont::value_type is is CopyConstructible (20.1.3) and Assignable (23.1)
// throws InvalidDeref() if container is not initialized at the time evaluation
//-------------------------------------------------
template<class Cont, class Adder  = std::plus<typename Cont::value_type> >
class AvgOf_r : public Coroutine {
	lref<Cont> cont;
    lref<typename Cont::value_type> a;
    Adder adder;
public:
    AvgOf_r(lref<Cont> cont, const lref<typename Cont::value_type>& a, Adder adder= std::plus<typename Cont::value_type>() ) : cont(cont), a(a), adder(adder)
	{}

	bool operator() () {
		co_begin();
        if(cont->size()==0)
            co_return(false);

        if(a.defined()) {
            co_yield(*a== std::accumulate(++cont->begin(), cont->end(), *cont->begin(), adder) / cont->size() );
        } else {
            a = std::accumulate(++cont->begin(), cont->end(), *cont->begin(), adder) / cont->size();
            co_yield(true);
            a.reset();
        }
  		co_end();
	}
};

// average_of(continer,m) - max value in container is a
template<class Cont> inline
AvgOf_r<Cont> 
average_of(lref<Cont>& cont_, const lref<typename Cont::value_type>& a) {
    return AvgOf_r<Cont>(cont_, a);
}

// average_of(continer,m) - avg of values in container is a
template<class Cont, class Adder> inline
AvgOf_r<Cont,Adder> 
average_of(lref<Cont>& cont_, const lref<typename Cont::value_type>& a, Adder adder) {
    return AvgOf_r<Cont,Adder>(cont_,a,adder);
}


//--------------------------------------------------------
//  Size: s is the size of the collection coll
//--------------------------------------------------------

template<typename Cont>
class SizeOf_r : public Coroutine {
    lref<typename Cont::size_type> sz;
    lref<Cont> cont_;
public:
    SizeOf_r(const lref<Cont>& cont_, const lref<typename Cont::size_type>& sz) : sz(sz), cont_(cont_)
    { }

    bool operator() (void) {
      co_begin();
      if(sz.defined())
        co_return( *sz==cont_->size() );
      sz=cont_->size();
      co_yield(true);
      sz.reset();
      co_end();
    }
};
// Concept: Cont provides member function size and member typedef size_type
template<typename Cont> inline
SizeOf_r<Cont> size_of(lref<Cont>& cont_, lref<typename Cont::size_type> sz) {
    return SizeOf_r<Cont>(cont_, sz);
}

#ifdef CASTOR_ENABLE_DEPRECATED
// **  DEPRECATED ** : use size_of
// Concept: Cont provides member function size & member typedef size_type
template<typename Cont> inline
SizeOf_r<Cont> size(lref<Cont>& cont_, lref<typename Cont::size_type> sz) {
    return SizeOf_r<Cont>(cont_, sz);
}
#endif // CASTOR_ENABLE_DEPRECATED

} // namespace castor

#endif
