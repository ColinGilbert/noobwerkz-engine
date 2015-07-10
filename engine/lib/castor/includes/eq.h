// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_EQ_H
#define CASTOR_EQ_H 1

#include "coroutine.h"
#include "lref.h"
#include "ile.h"
#include "relation.h"
#include "helpers.h"
#include <cstring>
#include <string>
namespace castor {

template<typename L, typename R, typename Cmp=::castor::detail::None>
class UnifyBase {
    Cmp cmp;
public:
    UnifyBase(const Cmp& cmp) : cmp(cmp)
    { }

    bool compare(const L& l, const R& r) {
        return cmp(l,r);
    }
};

template<typename L, typename R>
struct UnifyBase<L, R, ::castor::detail::None> {
    UnifyBase(const ::castor::detail::None&)
    { }

    bool compare(const L& l, const R& r) {
        return l==r; 
    }
};

template<typename L, typename R, typename Cmp=::castor::detail::None>
struct UnifyL : public UnifyBase<L, R, Cmp>, public Coroutine {
    lref<L> l;
    R r;
    typedef UnifyBase<L, R, Cmp> Base;

    UnifyL(const lref<L>& l, const R& r, const Cmp& cmp) : Base(cmp), l(l), r(r)
    { }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            co_return( Base::compare(*l,r) ) ;
        l=r;
        co_yield(true);
        l.reset();
        co_end();
    }
};

template<typename L, typename R, typename Cmp=::castor::detail::None>
class UnifyLR: public UnifyBase<L,R,Cmp> , public Coroutine {
    lref<L> l;
    lref<R> r;
    typedef UnifyBase<L,R,Cmp> Base;
public:
    UnifyLR(const lref<L>& l, const lref<R>& r, const Cmp& cmp) : Base(cmp), l(l), r(r)
    { }

    bool operator() (void) { // throws if  both l and r are not defined
        co_begin();
		if(l.defined()) {
			if(r.defined())
                co_return( Base::compare(*l,*r) );
			r=*l;
			co_yield(true);
            r.reset();
            co_return(false);
		}
        l=*r; // throws if r is not defined
        co_yield(true);
        l.reset();
        co_end();
    }    
};

template<typename L, typename R> inline
UnifyLR<L,R> eq(lref<L>& l, lref<R>& r) {
    return UnifyLR<L,R>(l,r, ::castor::detail::None());
}

template<typename L, typename R, typename Cmp> inline
UnifyLR<L,R,Cmp> eq(lref<L>& l, lref<R>& r, Cmp cmp) {
    return UnifyLR<L,R,Cmp>(l,r, cmp);
}

template<typename L, typename R> inline
Boolean eq(const L& l, const R& r) {
    return Boolean(l==r);
}
// eq : treat char* as a string
inline
Boolean eq(const char* l, const char* r) {
    return Boolean( std::strcmp(l,r)==0 );
}

template<typename Cmp> inline
Boolean eq(const char* l, const char* r, Cmp cmp) {
    return Boolean( cmp(l,r) );
}

template<typename L, typename R, typename Cmp> inline
Boolean eq(const L& l, const R& r, Cmp cmp) {
    return Boolean( cmp(l,r) );
}

template<typename L, typename R> inline
UnifyL<L,R> eq(const lref<L>& l, const R& r) {
    return UnifyL<L,R>(l,r, ::castor::detail::None());
}

template<typename L, typename R, typename Cmp> inline
UnifyL<L,R,Cmp> eq(const lref<L>& l, const R& r, Cmp cmp) {
    return UnifyL<L,R,Cmp>(l,r, cmp);
}

template<typename L, typename R> inline
UnifyL<R,L> eq(const L& l, const lref<R>& r) {
    return UnifyL<R,L>(r,l, ::castor::detail::None());
}

template<typename L, typename R, typename Cmp> inline
UnifyL<R,L,Cmp> eq(const L& l, const lref<R>& r, Cmp cmp) {
    return UnifyL<R,L,Cmp>(r,l,cmp);
}

template<typename T> inline
UnifyL<T,T> eq(const lref<T>& l, const char* r) {
	return UnifyL<T,T>(l, r, ::castor::detail::None());
}
template<typename T, typename Cmp> inline
UnifyL<T,T,Cmp> eq(const lref<T>& l, const char* r, Cmp cmp) {
	return UnifyL<T,T,Cmp>(l,r,cmp);
}

template<typename T> inline
UnifyL<T,T> eq(const char* l, const lref<T>& r) {
	return UnifyL<T,T>(r,l, ::castor::detail::None());
}

template<typename T, typename Cmp> inline
UnifyL<T,T,Cmp> eq(const char* l, const lref<T>& r, Cmp cmp) {
	return UnifyL<T,T,Cmp>(r,l,cmp);
}



namespace detail {

    template<typename T1, typename T2> inline
    bool compare(const T1& t1, const T2& t2, const ::castor::detail::None&) {
        return t1==t2;
    }
#ifdef __BCPLUSPLUS__
    template<typename T1, typename T2, typename Cmp> inline
    bool compare(const T1& t1, const T2& t2, const Cmp& cmp) {
        return cmp(t1,t2);
    }
#else
    template<typename T1, typename T2, typename Cmp> inline
    bool compare(const T1& t1, const T2& t2, Cmp cmp) {
        return cmp(t1,t2);
    }
#endif

    template<typename Iter1, typename Iter2, typename Cmp>
    bool compare_seq(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2, Cmp cmp=::castor::detail::None()) {
        do{
            if(first1==last1) 
                return first2==last2;
            else {
                if(first2==last2)
                    return false;
                if(!::castor::detail::compare(*first1,*first2, cmp))
                    return false;
            }
            ++first1; ++first2;
        }while(true);
    }
} // namespace detail

//----------------------------------------------------------------
// eq_seq : Unify Collection with sequence
//----------------------------------------------------------------
template<typename Cmp, typename Collection, typename Iter>
struct EqSeq_Base {
private:
    Cmp cmp;
public:
    EqSeq_Base(const Cmp& cmp) : cmp(cmp)
    { }

    bool compare(const lref<Collection>& coll, const Iter& beg, const Iter& end) {
        return ::castor::detail::compare_seq(coll->begin(), coll->end(), beg, end, cmp);
    }
};

template<typename Collection, typename Iter>
#ifdef __BCPLUSPLUS__
struct EqSeq_Base<detail::None, Collection, Iter>  {
#else
struct EqSeq_Base< ::castor::detail::None, Collection, Iter>  {
#endif
    EqSeq_Base(const ::castor::detail::None&)
    { }

    bool compare(const lref<Collection>& coll, const Iter& beg, const Iter& end) {
        return ::castor::detail::compare_seq(coll->begin(), coll->end(), beg, end, ::castor::detail::None());
    }
};

template<typename Collection, typename Iter, typename Cmp=::castor::detail::None>
class EqSeq_r : public EqSeq_Base<Cmp, Collection, Iter> , public Coroutine {
    lref<Collection> c;
    Iter begin_, end_;
    typedef EqSeq_Base<Cmp, Collection, Iter> Base;
public:
    typedef Collection collection_type;
    typedef Iter iterator_type;

    EqSeq_r(const lref<Collection>& c, const Iter& begin_, const Iter& end_, const Cmp& cmp=::castor::detail::None()) : Base(cmp) ,c(c), begin_(begin_), end_(end_)
    { }

    bool operator() (void) {
        co_begin();
        if(c.defined())
            co_return( Base::compare(c, begin_, end_) );
        c=Collection(begin_, end_);
        co_yield(true);
        c.reset();
        co_end();
    }
};

template<typename Cont, typename Iter> inline 
EqSeq_r<Cont, Iter> eq_seq(const lref<Cont>& c, Iter begin_, Iter end_) {
    return EqSeq_r<Cont, Iter>(c, begin_, end_);
}

template<typename Cont, typename Iter, typename Cmp> inline 
EqSeq_r<Cont, Iter, Cmp> eq_seq(const lref<Cont>& c, Iter begin, Iter end, Cmp cmp) {
    return EqSeq_r<Cont, Iter, Cmp>(c, begin, end, cmp);
}


//----------------------------------------------------------------
// eq_f : Unify with a value obtained by calling a function
//----------------------------------------------------------------
template<typename L, typename Func>
class Eq_f_r : public Coroutine {
    lref<L> l;
    Func func;
public:
    Eq_f_r (const lref<L>& l, const Func& func) : l(l), func(func)
    { }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            co_return( *l==func() );
        l=func();
        co_yield(true);
        l.reset();
        co_end();
	}
};

template<typename L, typename Func1, typename A1>
class Eq_f_r1 : public Coroutine {
    lref<L> l;
    Func1 func;
    A1 arg1;
public:
    Eq_f_r1 (const lref<L>& l, const Func1& func, const A1& arg1) : l(l), func(func), arg1(arg1)
    { }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            co_return( *l==func(effective_value(arg1)) );
        l=func(effective_value(arg1));
        co_yield(true);
        l.reset();
        co_end();
	}
};

template<typename L, typename Func2, typename A1, typename A2>
class Eq_f_r2 : public Coroutine {
    lref<L> l;
    Func2 func;
    A1 arg1; A2 arg2; 
public:
    Eq_f_r2 (const lref<L>& l, const Func2& func, const A1& arg1, const A2& arg2) : l(l), func(func), arg1(arg1), arg2(arg2)
    { }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            co_return( *l==func(effective_value(arg1), effective_value(arg2)) );
        l=func(effective_value(arg1), effective_value(arg2));
        co_yield(true);
        l.reset();
        co_end();
	}
};


template<typename L, typename Func3, typename A1, typename A2, typename A3>
class Eq_f_r3 : public Coroutine {
    lref<L> l;
    Func3 func;
    A1 arg1; A2 arg2; A3 arg3;
public:
    Eq_f_r3 (const lref<L>& l, const Func3& func, const A1& arg1, const A2& arg2, const A3& arg3) : l(l), func(func), arg1(arg1), arg2(arg2), arg3(arg3)
    { }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            co_return( *l==func(effective_value(arg1), effective_value(arg2), effective_value(arg3)) );
        l=func(effective_value(arg1), effective_value(arg2), effective_value(arg3));
        co_yield(true);
        l.reset();
        co_end();
	}
};

template<typename L, typename Func4, typename A1, typename A2, typename A3, typename A4>
class Eq_f_r4 : public Coroutine {
    lref<L> l;
    Func4 func;
    A1 arg1; A2 arg2; A3 arg3; A4 arg4;
public:
    Eq_f_r4 (const lref<L>& l, const Func4& func, const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4) : l(l), func(func), arg1(arg1), arg2(arg2), arg3(arg3), arg4(arg4)
    { }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            co_return( *l==func(effective_value(arg1), effective_value(arg2), effective_value(arg3), effective_value(arg4)) );
        l=func(effective_value(arg1), effective_value(arg2), effective_value(arg3), effective_value(arg4));
        co_yield(true);
        l.reset();
        co_end();
	}
};


template<typename L, typename Func5, typename A1, typename A2, typename A3, typename A4, typename A5>
class Eq_f_r5 : public Coroutine {
    lref<L> l;
    Func5 func;
    A1 arg1; A2 arg2; A3 arg3; A4 arg4; A5 arg5;
public:
    Eq_f_r5 (const lref<L>& l, const Func5& func, const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4, const A5& arg5) : l(l), func(func), arg1(arg1), arg2(arg2), arg3(arg3), arg4(arg4), arg5(arg5)
    { }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            co_return( *l==func(effective_value(arg1), effective_value(arg2), effective_value(arg3), effective_value(arg4), effective_value(arg5)) );
        l=func(effective_value(arg1), effective_value(arg2), effective_value(arg3), effective_value(arg4), effective_value(arg5));
        co_yield(true);
        l.reset();
        co_end();
	}
};

template<typename L, typename Func6, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class Eq_f_r6 : public Coroutine {
    lref<L> l;
    Func6 func;
    A1 arg1; A2 arg2; A3 arg3; A4 arg4; A5 arg5; A6 arg6;
public:
    Eq_f_r6 (const lref<L>& l, const Func6& func, const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4, const A5& arg5, const A6& arg6) : l(l), func(func), arg1(arg1), arg2(arg2), arg3(arg3), arg4(arg4), arg5(arg5), arg6(arg6)
    { }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            return *l==func(effective_value(arg1), effective_value(arg2), effective_value(arg3), effective_value(arg4), effective_value(arg5), effective_value(arg6));
        l=func(effective_value(arg1), effective_value(arg2), effective_value(arg3), effective_value(arg4), effective_value(arg5), effective_value(arg6));
        co_yield(true);
        l.reset();
        co_end();
	}
};


// overloads for function objects
template<typename T, typename Func> inline
Eq_f_r<T, Func> eq_f(lref<T> l, Func f) {
    return Eq_f_r<T, Func>(l,f);
}

template<typename T, typename Func1, typename A1> inline
Eq_f_r1<T, Func1, A1> eq_f(lref<T> l, Func1 f, const A1& a1_) {
    return Eq_f_r1<T, Func1, A1>(l,f, a1_);
}

template<typename T, typename Func2, typename A1, typename A2> inline
Eq_f_r2<T, Func2, A1, A2> eq_f(lref<T> l, Func2 f, const A1& a1_, const A2& a2_) {
    return Eq_f_r2<T, Func2, A1, A2>(l,f, a1_, a2_);
}

template<typename T, typename Func3, typename A1, typename A2, typename A3> inline
Eq_f_r3<T, Func3, A1, A2, A3> eq_f(lref<T> l, Func3 f, const A1& a1_, const A2& a2_, const A3& a3_) {
    return Eq_f_r3<T, Func3, A1, A2, A3>(l,f, a1_, a2_, a3_);
}

template<typename T, typename Func4, typename A1, typename A2, typename A3, typename A4> inline
Eq_f_r4<T, Func4, A1, A2, A3, A4> eq_f(lref<T> l, Func4 f, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) {
    return Eq_f_r4<T, Func4, A1, A2, A3, A4>(l,f, a1_, a2_, a3_, a4_);
}

template<typename T, typename Func5, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Eq_f_r5<T, Func5, A1, A2, A3, A4, A5> eq_f(lref<T> l, Func5 f, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) {
    return Eq_f_r5<T, Func5, A1, A2, A3, A4, A5>(l,f, a1_, a2_, a3_, a4_, a5_);
}

template<typename T, typename Func6, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Eq_f_r6<T, Func6, A1, A2, A3, A4, A5, A6> eq_f(lref<T> l, Func6 f, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) {
	return Eq_f_r6<T, Func6, A1, A2, A3, A4, A5, A6>(l,f, a1_, a2_, a3_, a4_, a5_, a6_);
}


// overloads for function pointers
template<typename T, typename R> inline
Eq_f_r<T,R(*)(void)>
eq_f(lref<T> l, R(* f)(void)) {
	return Eq_f_r<T,R(*)(void)>(l,f);
}

template<typename T, typename R, typename P1, typename A1> inline
Eq_f_r1<T,R(*)(P1),A1>
eq_f(lref<T> l, R(* f)(P1), const A1& a1_) {
	return Eq_f_r1<T,R(*)(P1),A1>(l,f,a1_);
}

template<typename T, typename R, typename P1, typename P2, typename A1, typename A2> inline
Eq_f_r2<T,R(*)(P1,P2),A1,A2>
eq_f(lref<T> l, R(* f)(P1,P2), const A1& a1_, const A2& a2_) {
	return Eq_f_r2<T,R(*)(P1,P2),A1,A2>(l,f,a1_,a2_);
}

template<typename T, typename R, typename P1, typename P2, typename P3, typename A1, typename A2, typename A3> inline
Eq_f_r3<T,R(*)(P1,P2,P3),A1,A2,A3>
eq_f(lref<T> l, R(* f)(P1,P2,P3), const A1& a1_, const A2& a2_, const A3& a3_) {
	return Eq_f_r3<T,R(*)(P1,P2,P3),A1,A2,A3>(l,f,a1_,a2_,a3_);
}

template<typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename A1, typename A2, typename A3, typename A4> inline
Eq_f_r4<T,R(*)(P1,P2,P3,P4),A1,A2,A3,A4>
eq_f(lref<T> l, R(* f)(P1,P2,P3,P4), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) {
    return Eq_f_r4<T,R(*)(P1,P2,P3,P4),A1,A2,A3,A4>(l,f,a1_,a2_,a3_,a4_);
}

template<typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Eq_f_r5<T,R(*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5>
eq_f(lref<T> l, R(* f)(P1,P2,P3,P4,P5), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) {
    return Eq_f_r5<T,R(*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5>(l,f,a1_,a2_,a3_,a4_,a5_);
}

template<typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Eq_f_r6<T,R(*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6>
eq_f(lref<T> l, R(* f)(P1,P2,P3,P4,P5,P6), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) {
	return Eq_f_r6<T,R(*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6>(l,f,a1_,a2_,a3_,a4_,a5_,a6_);
}

//----------------------------------------------------------------
// eq_mf : Unify with a value obtained by calling a member function
//----------------------------------------------------------------

template<typename L, typename Obj, typename MemFunT>
class Eq_mf_r0 : public Coroutine {
    lref<L> l;
	lref<Obj> obj_;
    MemFunT mf;
public:
    Eq_mf_r0(const lref<L>& l, lref<Obj> obj_, MemFunT mf) : l(l), obj_(obj_), mf(mf)
	{ }

    bool operator() (void) {
        co_begin();
        if(l.defined())
             co_return( *l==( ((*obj_).*mf)() ) );
        l=( ((*obj_).*mf)() );
        co_yield(true);
        l.reset();
        co_end();
	}
};


template<typename L, typename Obj, typename MemFunT, typename A1>
class Eq_mf_r1 : public Coroutine {
    lref<L> l;
	lref<Obj> obj_;
    MemFunT mf;
	A1 a1_;
public:
    Eq_mf_r1(const lref<L>& l, lref<Obj> obj_, MemFunT mf, const A1& a1_) : l(l), obj_(obj_), mf(mf), a1_(a1_)
	{ }

    bool operator() (void) {
        co_begin();
        if(l.defined())
             co_return( *l==( ((*obj_).*mf)(effective_value(a1_)) ) );
        l=( ((*obj_).*mf)(effective_value(a1_)) );
        co_yield(true);
        l.reset();
        co_end();
	}
};


template<typename L, typename Obj, typename MemFunT, typename A1, typename A2>
class Eq_mf_r2 : public Coroutine {
    lref<L> l;
	lref<Obj> obj_;
    MemFunT mf;
	A1 a1_; A2 a2_;
public:
    Eq_mf_r2(const lref<L>& l, lref<Obj> obj_, MemFunT mf, const A1& a1_, const A2& a2_) : l(l), obj_(obj_), mf(mf), a1_(a1_), a2_(a2_)
	{ }

    bool operator() (void) {
        co_begin();
        if(l.defined())
             co_return( *l==( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_)) ) );
        l=( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_)) );
        co_yield(true);
        l.reset();
        co_end();
	}
};


template<typename L, typename Obj, typename MemFunT, typename A1, typename A2, typename A3>
class Eq_mf_r3 : public Coroutine {
    lref<L> l;
	lref<Obj> obj_;
    MemFunT mf;
	A1 a1_; A2 a2_; A3 a3_;
public:
    Eq_mf_r3(const lref<L>& l, lref<Obj> obj_, MemFunT mf, const A1& a1_, const A2& a2_, const A3& a3_) : l(l), obj_(obj_), mf(mf), a1_(a1_), a2_(a2_), a3_(a3_)
	{ }

    bool operator() (void) {
        co_begin();
        if(l.defined())
             co_return( *l==( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_),effective_value(a3_)) ) );
        l=( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_),effective_value(a3_)) );
        co_yield(true);
        l.reset();
        co_end();
	}
};


template<typename L, typename Obj, typename MemFunT, typename A1, typename A2, typename A3, typename A4>
class Eq_mf_r4 : public Coroutine {
    lref<L> l;
	lref<Obj> obj_;
    MemFunT mf;
	A1 a1_; A2 a2_; A3 a3_; A4 a4_;
public:
    Eq_mf_r4(const lref<L>& l, lref<Obj> obj_, MemFunT mf, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) : l(l), obj_(obj_), mf(mf), a1_(a1_), a2_(a2_), a3_(a3_), a4_(a4_)
	{ }

    bool operator() (void) {
        co_begin();
        if(l.defined())
             co_return( *l==( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_),effective_value(a3_),effective_value(a4_)) ) );
        l=( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_),effective_value(a3_),effective_value(a4_)) );
        co_yield(true);
        l.reset();
        co_end();
	}
};


template<typename L, typename Obj, typename MemFunT, typename A1, typename A2, typename A3, typename A4, typename A5>
class Eq_mf_r5 : public Coroutine {
    lref<L> l;
	lref<Obj> obj_;
    MemFunT mf;
	A1 a1_; A2 a2_; A3 a3_; A4 a4_; A5 a5_;
public:
    Eq_mf_r5(const lref<L>& l, lref<Obj> obj_, MemFunT mf, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) : l(l), obj_(obj_), mf(mf), a1_(a1_), a2_(a2_), a3_(a3_), a4_(a4_), a5_(a5_)
	{ }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            return *l==( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_),effective_value(a3_),effective_value(a4_),effective_value(a5_)) );
        l=( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_),effective_value(a3_),effective_value(a4_),effective_value(a5_)) );
        co_yield(true);
        l.reset();
        co_end();
	}
};


template<typename L, typename Obj, typename MemFunT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class Eq_mf_r6 : public Coroutine {
    lref<L> l;
	lref<Obj> obj_;
    MemFunT mf;
	A1 a1_; A2 a2_; A3 a3_; A4 a4_; A5 a5_; A6 a6_;
public:
    Eq_mf_r6(const lref<L>& l, lref<Obj> obj_, MemFunT mf, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) : l(l), obj_(obj_), mf(mf), a1_(a1_), a2_(a2_), a3_(a3_), a4_(a4_), a5_(a5_), a6_(a6_)
	{ }

    bool operator() (void) {
        co_begin();
        if(l.defined())
            co_return( *l==( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_),effective_value(a3_),effective_value(a4_),effective_value(a5_),effective_value(a6_)) ) );
        l=( ((*obj_).*mf)(effective_value(a1_),effective_value(a2_),effective_value(a3_),effective_value(a4_),effective_value(a5_),effective_value(a6_)) );
        co_yield(true);
        l.reset();
        co_end();
	}
};



// overloads for non-const member functions
template<typename L, typename Obj, typename Obj2, typename R> inline
Eq_mf_r0<L,Obj,R(Obj::*)(void)> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(void) ) {
    return Eq_mf_r0<L,Obj,R(Obj::*)(void)>(l,obj_,mf);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename A1> inline
Eq_mf_r1<L,Obj,R(Obj::*)(P1),A1> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1), const A1& a1_) {
    return Eq_mf_r1<L,Obj,R(Obj::*)(P1),A1>(l,obj_,mf,a1_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename A1, typename A2> inline
Eq_mf_r2<L,Obj,R(Obj::*)(P1,P2),A1,A2> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2), const A1& a1_, const A2& a2_) {
    return Eq_mf_r2<L,Obj,R(Obj::*)(P1,P2),A1,A2>(l,obj_,mf,a1_,a2_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename P3, typename A1, typename A2, typename A3> inline
Eq_mf_r3<L,Obj,R(Obj::*)(P1,P2,P3),A1,A2,A3> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3), const A1& a1_, const A2& a2_, const A3& a3_) {
    return Eq_mf_r3<L,Obj,R(Obj::*)(P1,P2,P3),A1,A2,A3>(l,obj_,mf,a1_,a2_,a3_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename P3, typename P4, typename A1, typename A2, typename A3, typename A4> inline
Eq_mf_r4<L,Obj,R(Obj::*)(P1,P2,P3,P4),A1,A2,A3,A4> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) {
    return Eq_mf_r4<L,Obj,R(Obj::*)(P1,P2,P3,P4),A1,A2,A3,A4>(l,obj_,mf,a1_,a2_,a3_,a4_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Eq_mf_r5<L,Obj,R(Obj::*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4,P5), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) {
    return Eq_mf_r5<L,Obj,R(Obj::*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5>(l,obj_,mf,a1_,a2_,a3_,a4_,a5_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Eq_mf_r6<L,Obj,R(Obj::*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4,P5,P6), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) {
    return Eq_mf_r6<L,Obj,R(Obj::*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6>(l,obj_,mf,a1_,a2_,a3_,a4_,a5_,a6_);
}


// overloads for const member functions
template<typename L, typename Obj, typename Obj2, typename R> inline
Eq_mf_r0<L,Obj,R(Obj::*)(void) const> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(void) const) {
    return Eq_mf_r0<L,Obj,R(Obj::*)(void) const>(l,obj_,mf);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename A1> inline
Eq_mf_r1<L,Obj,R(Obj::*)(P1) const,A1> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1) const, const A1& a1_) {
    return Eq_mf_r1<L,Obj,R(Obj::*)(P1) const,A1>(l,obj_,mf,a1_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename A1, typename A2> inline
Eq_mf_r2<L,Obj,R(Obj::*)(P1,P2) const,A1,A2> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2) const, const A1& a1_, const A2& a2_) {
    return Eq_mf_r2<L,Obj,R(Obj::*)(P1,P2) const,A1,A2>(l,obj_,mf,a1_,a2_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename P3, typename A1, typename A2, typename A3> inline
Eq_mf_r3<L,Obj,R(Obj::*)(P1,P2,P3) const,A1,A2,A3> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3) const, const A1& a1_, const A2& a2_, const A3& a3_) {
    return Eq_mf_r3<L,Obj,R(Obj::*)(P1,P2,P3) const,A1,A2,A3>(l,obj_,mf,a1_,a2_,a3_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename P3, typename P4, typename A1, typename A2, typename A3, typename A4> inline
Eq_mf_r4<L,Obj,R(Obj::*)(P1,P2,P3,P4) const,A1,A2,A3,A4> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4) const, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) {
    return Eq_mf_r4<L,Obj,R(Obj::*)(P1,P2,P3,P4) const,A1,A2,A3,A4>(l,obj_,mf,a1_,a2_,a3_,a4_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Eq_mf_r5<L,Obj,R(Obj::*)(P1,P2,P3,P4,P5) const,A1,A2,A3,A4,A5> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4,P5) const, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) {
    return Eq_mf_r5<L,Obj,R(Obj::*)(P1,P2,P3,P4,P5) const,A1,A2,A3,A4,A5>(l,obj_,mf,a1_,a2_,a3_,a4_,a5_);
}

template<typename L, typename Obj, typename Obj2, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Eq_mf_r6<L,Obj,R(Obj::*)(P1,P2,P3,P4,P5,P6) const,A1,A2,A3,A4,A5,A6> 
eq_mf(lref<L> l, lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4,P5,P6) const, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) {
    return Eq_mf_r6<L,Obj,R(Obj::*)(P1,P2,P3,P4,P5,P6) const,A1,A2,A3,A4,A5,A6>(l,obj_,mf,a1_,a2_,a3_,a4_,a5_,a6_);
}

//----------------------------------------------------------------
// eq_mem : Unify with a member variable
//----------------------------------------------------------------

template<typename L, typename Obj, typename MemberT>
class Eq_mem_r : public Coroutine {
    lref<L> l;
	lref<Obj> obj_;
	MemberT Obj::*  mem;
public:
    Eq_mem_r(const lref<L>& l, const lref<Obj>& obj_, MemberT Obj::* mem) : l(l), obj_(obj_), mem(mem)
	{ }

    bool operator() (void) {
		co_begin();
        if(l.defined())
			co_return( *l== (*obj_).*mem );
        l= (*obj_).*mem;
		co_yield(true);
		l.reset();
		co_end();
	}
};


template<typename L, typename Obj, typename Obj2, typename MemberT> inline
Eq_mem_r<L, Obj, MemberT> eq_mem(lref<L> l, lref<Obj>& obj_, MemberT Obj2::* mem) {
    return Eq_mem_r<L, Obj, MemberT>(l, obj_, mem);
}

template<typename Obj, typename Obj2, typename MemberT> inline
Eq_mem_r<std::string, Obj, MemberT> eq_mem(const char* l, lref<Obj>& obj_, MemberT Obj2::* mem) {
    return Eq_mem_r<std::string, Obj, MemberT>(lref<std::string>(l), obj_, mem);
}

} // namespace castor

#endif
