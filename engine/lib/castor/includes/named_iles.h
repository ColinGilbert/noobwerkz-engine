// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_NAMED_ILES_H
#define CASTOR_NAMED_ILES_H 1

#include "ile.h"
#include "helpers.h"

namespace castor {

//-------------------------------------------------
// at() - Equivalent to operator[]
//-------------------------------------------------

template<typename Cont, typename Index>
class At_ILE {
	lref<Cont> c;
    Index i;
public:
	typedef typename Cont::value_type& result_type;
    At_ILE(const lref<Cont>& c, Index i) : c(c), i(i) 
    { }

	result_type operator()(void) {
        return (*c)[effective_value(i)];
    }
};

template<typename Obj, typename Index> inline
Ile<At_ILE<Obj,Index> >
at(lref<Obj>& obj, Index i) {
    return At_ILE<Obj,Index>(obj,i);
}

//-------------------------------------------------
// id() - Takes a reference to an object/lref
//-------------------------------------------------
template<class T>
class Id_ILE {
	T* obj;
public:
    typedef typename effective_type<T>::result_type& result_type;

    Id_ILE(T& obj) : obj(&obj)
    { }

	result_type operator()(void) {
        return effective_value(*obj);
    }
};

template<class T> inline
Ile<Id_ILE<T> >
id(T& obj) {
    return Id_ILE<T>(obj);
}


//-------------------------------------------------
// create() and Create<>::with()
//-------------------------------------------------

template<typename T>
class CreateWith0 {
public:
    typedef T result_type;
    T operator()(void) {
        return T();
    }
};

template<typename T, typename A1>
class CreateWith1 {
    A1 a1;
public:
    typedef T result_type;
    CreateWith1(const A1& a1) : a1(a1) 
    { }

    T operator()(void) {
        return T(effective_value(a1));
    }
};

template<typename T, typename A1, typename A2>
class CreateWith2 {
    A1 a1; A2 a2;
public:
    typedef T result_type;
    CreateWith2(const A1& a1, const A2& a2) : a1(a1), a2(a2)
    { }

    T operator()(void) {
        return T(effective_value(a1), effective_value(a2));
    }
};

template<typename T, typename A1, typename A2, typename A3>
class CreateWith3 {
    A1 a1; A2 a2; A3 a3;
public:
    typedef T result_type;
    CreateWith3(const A1& a1, const A2& a2, const A3& a3) : a1(a1), a2(a2), a3(a3)
    { }

    T operator()(void) {
        return T(effective_value(a1), effective_value(a2), effective_value(a3));
    }
};

template<typename T, typename A1, typename A2, typename A3, typename A4>
class CreateWith4 {
    A1 a1; A2 a2; A3 a3; A4 a4;
public:
    typedef T result_type;
    CreateWith4(const A1& a1, const A2& a2, const A3& a3, const A4& a4) : a1(a1), a2(a2), a3(a3), a4(a4)
    { }

    T operator()(void) {
        return T(effective_value(a1), effective_value(a2), effective_value(a3), effective_value(a4));
    }
};

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
class CreateWith5 {
    A1 a1; A2 a2; A3 a3; A4 a4; A5 a5;
public:
    typedef T result_type;
    CreateWith5(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5)
    { }

    T operator()(void) {
        return T(effective_value(a1), effective_value(a2), effective_value(a3), effective_value(a4), effective_value(a5));
    }
};

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class CreateWith6 {
    A1 a1; A2 a2; A3 a3; A4 a4; A5 a5; A6 a6;
public:
    typedef T result_type;
    CreateWith6(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6)
    { }

    T operator()(void) {
        return T(effective_value(a1), effective_value(a2), effective_value(a3), effective_value(a4), effective_value(a5), effective_value(a6));
    }
};


#ifdef CASTOR_ENABLE_DEPRECATED

template<typename T>
class Create {
public:
    static Ile<CreateWith0<T> > with() {
        return CreateWith0<T>();
    }

    template<typename A1>
    static Ile<CreateWith1<T,A1> > with(const A1& a1) {
        return CreateWith1<T,A1>(a1);
    }

    template<typename A1, typename A2>
    static Ile<CreateWith2<T,A1,A2> > with(const A1& a1, const A2& a2) {
        return CreateWith2<T,A1,A2>(a1,a2);
    }

    template<typename A1, typename A2, typename A3>
    static Ile<CreateWith3<T,A1,A2,A3> > with(const A1& a1, const A2& a2, const A3& a3) {
        return CreateWith3<T,A1,A2,A3>(a1,a2,a3);
    }

    template<typename A1, typename A2, typename A3, typename A4>
    static Ile<CreateWith4<T,A1,A2,A3,A4> > with(const A1& a1, const A2& a2, const A3& a3, const A4& a4) {
        return CreateWith4<T,A1,A2,A3,A4>(a1,a2,a3,a4);
    }

    template<typename A1, typename A2, typename A3, typename A4, typename A5>
    static Ile<CreateWith5<T,A1,A2,A3,A4,A5> > with(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) {
        return CreateWith5<T,A1,A2,A3,A4,A5>(a1,a2,a3,a4,a5);
    }

    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    static Ile<CreateWith6<T,A1,A2,A3,A4,A5,A6> > with(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) {
        return CreateWith6<T,A1,A2,A3,A4,A5,A6>(a1,a2,a3,a4,a5,a6);
    }
};

#endif


template<typename T> inline
Ile<CreateWith0<T> > create() {
    return CreateWith0<T>();
}

template<typename T, typename A1> inline
 Ile<CreateWith1<T,A1> > create(const A1& a1) {
    return CreateWith1<T,A1>(a1);
}

template<typename T, typename A1, typename A2> inline
 Ile<CreateWith2<T,A1,A2> > create(const A1& a1, const A2& a2) {
    return CreateWith2<T,A1,A2>(a1,a2);
}

template<typename T, typename A1, typename A2, typename A3> inline
 Ile<CreateWith3<T,A1,A2,A3> > create(const A1& a1, const A2& a2, const A3& a3) {
    return CreateWith3<T,A1,A2,A3>(a1,a2,a3);
}

template<typename T, typename A1, typename A2, typename A3, typename A4> inline
 Ile<CreateWith4<T,A1,A2,A3,A4> > create(const A1& a1, const A2& a2, const A3& a3, const A4& a4) {
    return CreateWith4<T,A1,A2,A3,A4>(a1,a2,a3,a4);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Ile<CreateWith5<T,A1,A2,A3,A4,A5> > create(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) {
    return CreateWith5<T,A1,A2,A3,A4,A5>(a1,a2,a3,a4,a5);
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Ile<CreateWith6<T,A1,A2,A3,A4,A5,A6> > create(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) {
    return CreateWith6<T,A1,A2,A3,A4,A5,A6>(a1,a2,a3,a4,a5,a6);
}
//-------------------------------------------------
// get() - get the value of data member
//-------------------------------------------------

template<typename Obj, typename Obj2, typename MemberT>
class Get_ILE {
    lref<Obj> obj_;
	MemberT Obj2::* mem;
public:
    typedef MemberT& result_type;

    Get_ILE(const lref<Obj>& obj_, MemberT Obj2::* mem) : obj_(obj_), mem(mem)
	{ }

    result_type operator()(void) {
        return (*obj_).*mem;
    }

    const MemberT& operator()(void) const {
        return (*obj_).*mem;
    }
};

template<typename Obj, typename Obj2, typename MemberT> inline
Ile<Get_ILE<Obj,Obj2,MemberT> > get(const lref<Obj>& obj_, MemberT Obj2::* mem) {
	return Get_ILE<Obj,Obj2,MemberT>(obj_,mem);
}


//-------------------------------------------------
// call() - invoke a function
//-------------------------------------------------

template<typename FuncT>
class Call_0 {
	FuncT f;
public:
    typedef typename detail::return_type<FuncT>::result_type result_type;

	Call_0(const FuncT& f) : f(f)
	{ }

    result_type operator()(void) {
        return f();
    }

    result_type operator()(void)  const {
        return f();
    }
};


template<typename FuncT, typename A1>
class Call_1 {
	FuncT f;
	A1 a1;
public:
	typedef typename detail::return_type<FuncT>::result_type result_type;

	Call_1(const FuncT& f, const A1& a1) : f(f), a1(a1)
	{ }

    result_type operator()(void)  {
        return f( effective_value(a1) );
    }

    result_type operator()(void)  const {
        return f( effective_value(a1) );
    }
};


template<typename FuncT, typename A1, typename A2>
class Call_2 {
	FuncT f;
	A1 a1; A2 a2;
public:
	typedef typename detail::return_type<FuncT>::result_type result_type;

	Call_2(const FuncT& f, const A1& a1, const A2& a2) : f(f), a1(a1), a2(a2)
	{ }

    result_type operator()(void) {
        return f( effective_value(a1),effective_value(a2) );
    }

    result_type operator()(void)  const {
        return f( effective_value(a1),effective_value(a2) );
    }
};

template<typename FuncT, typename A1, typename A2, typename A3>
class Call_3 {
	FuncT f;
	A1 a1; A2 a2; A3 a3;
public:
	typedef typename detail::return_type<FuncT>::result_type result_type;

	Call_3(const FuncT& f, const A1& a1, const A2& a2, const A3& a3) : f(f), a1(a1), a2(a2), a3(a3)
	{ }

    result_type operator()(void)  {
        return f( effective_value(a1),effective_value(a2),effective_value(a3) );
    }

    result_type operator()(void)  const {
        return f( effective_value(a1),effective_value(a2),effective_value(a3) );
    }
};


template<typename FuncT, typename A1, typename A2, typename A3, typename A4>
class Call_4 {
	FuncT f;
	A1 a1; A2 a2; A3 a3; A4 a4;
public:
	typedef typename detail::return_type<FuncT>::result_type result_type;

	Call_4(const FuncT& f, const A1& a1, const A2& a2, const A3& a3, const A4& a4) : f(f), a1(a1), a2(a2), a3(a3), a4(a4)
	{ }

    result_type operator()(void)  {
        return f(  effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4) );
    }

    result_type operator()(void)  const {
        return f(  effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4) );
    }
};


template<typename FuncT, typename A1, typename A2, typename A3, typename A4, typename A5>
class Call_5 {
	FuncT f;
	A1 a1; A2 a2; A3 a3; A4 a4; A5 a5;
public:
	typedef typename detail::return_type<FuncT>::result_type result_type;

	Call_5(const FuncT& f, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) : f(f), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5)
	{ }

    result_type operator()(void)  {
        return f( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5) );
    }

    result_type operator()(void)  const {
        return f( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5) );
    }
};

template<typename FuncT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class Call_6 {
	FuncT f;
	A1 a1; A2 a2; A3 a3; A4 a4; A5 a5; A6 a6;
public:
	typedef typename detail::return_type<FuncT>::result_type result_type;

	Call_6(const FuncT& f, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) : f(f), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6)
	{ }

    result_type operator()(void)  {
        return f( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5),effective_value(a6) );
    }

    result_type operator()(void)  const {
        return f( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5),effective_value(a6) );
    }
};


// For Nullary through sestiary(6-ary) functions
template<typename FuncT> inline
Ile<Call_0<FuncT> > call(const FuncT& f) {
	return Call_0<FuncT>(f);
}

template<typename FuncT, typename A1> inline
Ile<Call_1<FuncT,A1> > call(const FuncT& f, const A1& a1) {
	return Call_1<FuncT,A1>(f, a1);
}

template<typename FuncT, typename A1, typename A2> inline
Ile<Call_2<FuncT,A1,A2> > call(const FuncT& f, const A1& a1, const A2& a2) {
	return Call_2<FuncT,A1,A2>(f,a1,a2);
}

template<typename FuncT, typename A1, typename A2, typename A3> inline
Ile<Call_3<FuncT,A1,A2,A3> > call(const FuncT& f, const A1& a1, const A2& a2, const A3& a3) {
	return Call_3<FuncT,A1,A2,A3>(f,a1,a2,a3);
}

template<typename FuncT, typename A1, typename A2, typename A3, typename A4> inline
Ile<Call_4<FuncT,A1,A2,A3,A4> > call(const FuncT& f, const A1& a1, const A2& a2, const A3& a3, const A4& a4) {
	return Call_4<FuncT,A1,A2,A3,A4>(f,a1,a2,a3,a4);
}

template<typename FuncT, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Ile<Call_5<FuncT,A1,A2,A3,A4,A5> > call(const FuncT& f, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) {
	return Call_5<FuncT,A1,A2,A3,A4,A5>(f,a1,a2,a3,a4,a5);
}

template<typename FuncT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Ile<Call_6<FuncT,A1,A2,A3,A4,A5,A6> > call(const FuncT& f, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) {
	return Call_6<FuncT,A1,A2,A3,A4,A5,A6>(f,a1,a2,a3,a4,a5,a6);
}


// For nullary through sestiary(6-ary) function objects
template<typename Ret> inline
Ile<Call_0<Ret(*)(void)> > call(Ret(* f)(void)) {
	return Call_0<Ret(*)(void)>(f);
}

template<typename Ret, typename A1, typename P1> inline
Ile<Call_1<Ret(*)(P1),A1> > call(Ret(* f)(P1), const A1& a1) {
	return Call_1<Ret(*)(P1),A1>(f,a1);
}

template<typename Ret, typename P1, typename P2, typename A1, typename A2> inline
Ile<Call_2<Ret(*)(P1,P2),A1,A2> > call(Ret(* f)(P1,P2), const A1& a1, const A2& a2) {
	return Call_2<Ret(*)(P1,P2),A1,A2>(f,a1,a2);
}

template<typename Ret, typename P1, typename P2, typename P3, typename A1, typename A2, typename A3> inline
Ile<Call_3<Ret(*)(P1,P2,P3),A1,A2,A3> > call(Ret(* f)(P1,P2,P3), const A1& a1, const A2& a2, const A3& a3) {
	return Call_3<Ret(*)(P1,P2,P3),A1,A2,A3>(f,a1,a2,a3);
}

template<typename Ret, typename P1, typename P2, typename P3, typename P4, typename A1, typename A2, typename A3, typename A4> inline
Ile<Call_4<Ret(*)(P1,P2,P3,P4),A1,A2,A3,A4> > call(Ret(* f)(P1,P2,P3,P4), const A1& a1, const A2& a2, const A3& a3, const A4& a4) {
	return Call_4<Ret(*)(P1,P2,P3,P4),A1,A2,A3,A4>(f,a1,a2,a3,a4);
}

template<typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Ile<Call_5<Ret(*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5> > call(Ret(* f)(P1,P2,P3,P4,P5), const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) {
	return Call_5<Ret(*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5>(f,a1,a2,a3,a4,a5);
}

template<typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Ile<Call_6<Ret(*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6> > call(Ret(* f)(P1,P2,P3,P4,P5,P6), const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) {
	return Call_6<Ret(*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6>(f,a1,a2,a3,a4,a5,a6);
}




//-------------------------------------------------
// mcall() - invoke a method
//-------------------------------------------------
template<typename Obj, typename MemFunc, typename R>
class MCall_0 {
    lref<Obj> obj_;
	MemFunc mf;
public:
	typedef R result_type;
    MCall_0(lref<Obj>& obj_, MemFunc mf) :obj_(obj_), mf(mf)
    { }

    R operator()(void) {
        return ((*obj_).*mf)();
    }
    R operator()(void) const {
        return ((*obj_).*mf)();
    }
};

template<typename Obj, typename MemFunc, typename R, typename A1>
class MCall_1 {
    lref<Obj> obj_;
	MemFunc mf;
    A1 a1;
public:
	typedef R result_type;
	MCall_1(lref<Obj>& obj_, MemFunc mf, const A1& a1) :obj_(obj_), mf(mf), a1(a1)
    { }

    R operator()(void)  {
        return ((*obj_).*mf)( effective_value(a1) );
    }

    R operator()(void)  const {
        return ((*obj_).*mf)( effective_value(a1) );
    }
};

template<typename Obj, typename MemFunc, typename R, typename A1, typename A2>
class MCall_2  {
    lref<Obj> obj_;
	MemFunc mf;
    A1 a1; A2 a2;
public:
	typedef R result_type;
    MCall_2(lref<Obj>& obj_, MemFunc mf, const A1& a1, const A2& a2) :obj_(obj_), mf(mf), a1(a1), a2(a2)
    { }

    R operator()(void)  {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2) );
    }

    R operator()(void)  const {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2) );
    }
};

template<typename Obj, typename MemFunc, typename R, typename A1, typename A2, typename A3>
class MCall_3  {
    lref<Obj> obj_;
	MemFunc mf;
    A1 a1; A2 a2; A3 a3;
public:
	typedef R result_type;
    MCall_3(lref<Obj>& obj_, MemFunc mf, const A1& a1, const A2& a2, const A3& a3) :obj_(obj_), mf(mf), a1(a1), a2(a2), a3(a3)
    { }

    R operator()(void)  {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2),effective_value(a3) );
    }

    R operator()(void)  const {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2),effective_value(a3) );
    }
};

template<typename Obj, typename MemFunc, typename R, typename A1, typename A2, typename A3, typename A4>
class MCall_4  {
    lref<Obj> obj_;
	MemFunc mf;
    A1 a1; A2 a2; A3 a3; A4 a4;
public:
	typedef R result_type;
    MCall_4(lref<Obj>& obj_, MemFunc mf, const A1& a1, const A2& a2, const A3& a3, const A4& a4) :obj_(obj_), mf(mf), a1(a1), a2(a2), a3(a3), a4(a4)
    { }

    R operator()(void)  {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4) );
    }

    R operator()(void)  const {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4) );
    }
};

template<typename Obj, typename MemFunc, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
class MCall_5  {
    lref<Obj> obj_;
	MemFunc mf;
    A1 a1; A2 a2; A3 a3; A4 a4; A5 a5;
public:
	typedef R result_type;
    MCall_5(lref<Obj>& obj_, MemFunc mf, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) :obj_(obj_), mf(mf), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5)
    { }

    R operator()(void)  {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5) );
    }

    R operator()(void)  const {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5) );
    }
};

template<typename Obj, typename MemFunc, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class MCall_6  {
    lref<Obj> obj_;
	MemFunc mf;
    A1 a1; A2 a2; A3 a3; A4 a4; A5 a5; A6 a6;
public:
	typedef R result_type;
    MCall_6(lref<Obj> obj_, MemFunc mf, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) :obj_(obj_), mf(mf), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6)
    { }

    R operator()(void)  {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5),effective_value(a6) );
    }

    R operator()(void)  const {
        return ((*obj_).*mf)( effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5),effective_value(a6) );
    }
};


// For Nullary through sestiary(6-ary) member functions
// Overloads for non-const member functions
template<typename R, typename Obj, typename Obj2> inline
Ile<MCall_0<Obj,R(Obj2::*)(void),R> >
mcall(lref<Obj>& obj_, R(Obj2::*mf)(void) ) {
    return MCall_0<Obj,R(Obj2::*)(void),R>(obj_,mf);
}

template<typename R, typename P1, typename Obj, typename Obj2, typename A1> inline
Ile<MCall_1<Obj,R(Obj2::*)(P1),R,A1> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1), const A1& a1_) {
    return MCall_1<Obj,R(Obj2::*)(P1),R,A1>(obj_,mf,a1_);
}

template<typename R, typename P1, typename P2, typename Obj, typename Obj2, typename A1, typename A2> inline
Ile<MCall_2<Obj,R(Obj2::*)(P1,P2),R,A1,A2> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1,P2), const A1& a1_, const A2& a2_) {
    return MCall_2<Obj,R(Obj2::*)(P1,P2),R,A1,A2>(obj_,mf,a1_,a2_);
}

template<typename R, typename P1, typename P2, typename P3, typename Obj, typename Obj2, typename A1, typename A2, typename A3>  inline
Ile<MCall_3<Obj,R(Obj2::*)(P1,P2,P3),R,A1,A2,A3> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3), const A1& a1_, const A2& a2_, const A3& a3_) {
    return MCall_3<Obj,R(Obj2::*)(P1,P2,P3),R,A1,A2,A3>(obj_,mf,a1_,a2_,a3_);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4> inline
Ile<MCall_4<Obj,R(Obj2::*)(P1,P2,P3,P4),R,A1,A2,A3,A4> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) {
    return MCall_4<Obj,R(Obj2::*)(P1,P2,P3,P4),R,A1,A2,A3,A4>(obj_,mf,a1_,a2_,a3_,a4_);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4, typename A5>  inline
Ile<MCall_5<Obj,R(Obj2::*)(P1,P2,P3,P4,P5),R,A1,A2,A3,A4,A5> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4,P5), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) {
    return MCall_5<Obj,R(Obj2::*)(P1,P2,P3,P4,P5),R,A1,A2,A3,A4,A5>(obj_,mf,a1_,a2_,a3_,a4_,a5_);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Ile<MCall_6<Obj,R(Obj2::*)(P1,P2,P3,P4,P5,P6),R,A1,A2,A3,A4,A5,A6> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4,P5,P6), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) {
    return MCall_6<Obj,R(Obj2::*)(P1,P2,P3,P4,P5,P6),R,A1,A2,A3,A4,A5,A6>(obj_,mf,a1_,a2_,a3_,a4_,a5_,a6_);
}

// Overloads for const member functions
template<typename R, typename Obj, typename Obj2> inline
Ile<MCall_0<Obj,R(Obj2::*)(void) const,R> >
mcall(lref<Obj>& obj_, R(Obj2::*mf)(void) const) {
    return MCall_0<Obj,R(Obj2::*)(void) const,R>(obj_,mf);
}

template<typename R, typename P1, typename Obj, typename Obj2, typename A1> inline
Ile<MCall_1<Obj,R(Obj2::*)(P1) const,R,A1> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1) const, const A1& a1_) {
    return MCall_1<Obj,R(Obj2::*)(P1) const,R,A1>(obj_,mf,a1_);
}

template<typename R, typename P1, typename P2, typename Obj, typename Obj2, typename A1, typename A2> inline
Ile<MCall_2<Obj,R(Obj2::*)(P1,P2) const,R,A1,A2> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1,P2) const, const A1& a1_, const A2& a2_) {
    return MCall_2<Obj,R(Obj2::*)(P1,P2) const,R,A1,A2>(obj_,mf,a1_,a2_);
}

template<typename R, typename P1, typename P2, typename P3, typename Obj, typename Obj2, typename A1, typename A2, typename A3> inline
Ile<MCall_3<const Obj,R(Obj2::*)(P1,P2,P3) const,R,A1,A2,A3> >
mcall(lref<const Obj>& obj_, R(Obj2::* mf)(P1,P2,P3) const, const A1& a1_, const A2& a2_, const A3& a3_) {
    return MCall_3<const Obj,R(Obj2::*)(P1,P2,P3) const,R,A1,A2,A3>(obj_,mf,a1_,a2_,a3_);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4> inline
Ile<MCall_4<Obj,R(Obj2::*)(P1,P2,P3,P4) const,R,A1,A2,A3,A4> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4) const, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) {
    return MCall_4<Obj,R(Obj2::*)(P1,P2,P3,P4) const,R,A1,A2,A3,A4>(obj_,mf,a1_,a2_,a3_,a4_);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Ile<MCall_5<Obj,R(Obj2::*)(P1,P2,P3,P4,P5) const,R,A1,A2,A3,A4,A5> > 
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4,P5) const, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) {
    return MCall_5<Obj,R(Obj2::*)(P1,P2,P3,P4,P5) const,R,A1,A2,A3,A4,A5>(obj_,mf,a1_,a2_,a3_,a4_,a5_);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Ile<MCall_6<Obj,R(Obj2::*)(P1,P2,P3,P4,P5,P6) const,R,A1,A2,A3,A4,A5,A6> >
mcall(lref<Obj>& obj_, R(Obj2::* mf)(P1,P2,P3,P4,P5,P6) const, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) {
    return MCall_6<Obj,R(Obj2::*)(P1,P2,P3,P4,P5,P6) const,R,A1,A2,A3,A4,A5,A6>(obj_,mf,a1_,a2_,a3_,a4_,a5_,a6_);
}



} // namespace castor
#endif
