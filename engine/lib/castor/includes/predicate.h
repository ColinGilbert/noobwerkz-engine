// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#ifndef CASTOR_PREDICATE_H
#define CASTOR_PREDICATE_H 1

#include "relation.h"
#include "helpers.h"
#include "workaround.h"

namespace castor {

//----------------------------------------------------------------------------
// predicate : Adaptor relation for treating predicate functions as relations
//----------------------------------------------------------------------------
template<typename Pred>
struct Predicate0_r : public detail::TestOnlyRelation<Predicate0_r<Pred> > {
	Pred pred;

	Predicate0_r (const Pred& pred) : pred(pred)
	{ }

	bool apply() {
		return pred();
	}
};

template<typename Pred, typename A1>
struct Predicate1_r : public detail::TestOnlyRelation<Predicate1_r<Pred,A1> > {
	Pred pred;
	A1 a1;
	Predicate1_r (const Pred& pred, const A1& a1) : pred(pred), a1(a1)
	{ }

	bool apply() {
	    return pred(effective_value(a1));
	}	
};

template<typename Pred, typename A1, typename A2>
struct Predicate2_r : public detail::TestOnlyRelation<Predicate2_r<Pred,A1,A2> > {
	Pred pred;
	A1 a1;	A2 a2;

	Predicate2_r (const Pred& pred, const A1& a1, const A2& a2) : pred(pred), a1(a1), a2(a2)
	{ }

	bool apply() {
	    return pred(effective_value(a1),effective_value(a2));
	}
};

template<typename Pred, typename A1, typename A2, typename A3>
struct Predicate3_r : public detail::TestOnlyRelation<Predicate3_r<Pred,A1,A2,A3> > {
	Pred pred;
	A1 a1;	A2 a2;	A3 a3;

	Predicate3_r (const Pred& pred, const A1& a1, const A2& a2, const A3& a3) : pred(pred), a1(a1), a2(a2), a3(a3)
	{ }

	bool apply() {
	    return pred(effective_value(a1),effective_value(a2),effective_value(a3));
	}
};


template<typename Pred, typename A1, typename A2, typename A3, typename A4>
struct Predicate4_r : public detail::TestOnlyRelation<Predicate4_r<Pred,A1,A2,A3,A4> > {
	Pred pred;
	A1 a1;	A2 a2;	A3 a3;	A4 a4;

	Predicate4_r (const Pred& pred, const A1& a1, const A2& a2, const A3& a3, const A4& a4) : pred(pred), a1(a1), a2(a2), a3(a3), a4(a4)
	{ }

	bool apply() {
	    return pred(effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4));
	}
};

template<typename Pred, typename A1, typename A2, typename A3, typename A4, typename A5>
struct Predicate5_r : public detail::TestOnlyRelation<Predicate5_r<Pred,A1,A2,A3,A4,A5> > {
	Pred pred;
	A1 a1;	A2 a2;	A3 a3;	A4 a4;	A5 a5;

	Predicate5_r (const Pred& pred, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) : pred(pred), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5)
	{ }

	bool apply() {
	    return pred(effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5));
	}
};

template<typename Pred, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct Predicate6_r : public detail::TestOnlyRelation<Predicate6_r<Pred,A1,A2,A3,A4,A5,A6> > {
	Pred pred;
	A1 a1;	A2 a2;	A3 a3;	A4 a4;	A5 a5;  A6 a6;

	Predicate6_r (const Pred& pred, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) : pred(pred), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6)
	{ }

	bool apply() {
	    return pred(effective_value(a1),effective_value(a2),effective_value(a3),effective_value(a4),effective_value(a5),effective_value(a6));
	}
};

// support for function objects
template<typename Pred> inline
Predicate0_r<Pred> predicate(Pred pred) {
	return Predicate0_r<Pred>(pred);
}

template<typename Pred, typename A1> inline 
Predicate1_r<Pred,A1> predicate(Pred pred, const A1& a1_) {
	return Predicate1_r<Pred,A1>(pred,a1_);
}

template<typename Pred, typename A1, typename A2> inline
Predicate2_r<Pred,A1,A2> predicate(Pred pred, const A1& a1_, const A2& a2_) {
	return Predicate2_r<Pred,A1,A2>(pred,a1_,a2_);
}

template<typename Pred, typename A1, typename A2, typename A3> inline
Predicate3_r<Pred,A1,A2,A3> predicate(Pred pred, const A1& a1_, const A2& a2_, const A3& a3_) {
	return Predicate3_r<Pred,A1,A2,A3>(pred,a1_,a2_,a3_);
}

template<typename Pred, typename A1, typename A2, typename A3, typename A4> inline 
Predicate4_r<Pred,A1,A2,A3,A4> predicate(Pred pred, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) {
	return Predicate4_r<Pred,A1,A2,A3,A4>(pred,a1_,a2_,a3_,a4_);
}

template<typename Pred, typename A1, typename A2, typename A3, typename A4 ,typename A5> inline 
Predicate5_r<Pred,A1,A2,A3,A4,A5> predicate(Pred pred, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) {
	return Predicate5_r<Pred,A1,A2,A3,A4,A5>(pred,a1_,a2_,a3_,a4_,a5_);
}

template<typename Pred, typename A1, typename A2, typename A3, typename A4 ,typename A5, typename A6> inline 
Predicate6_r<Pred,A1,A2,A3,A4,A5,A6> predicate(Pred pred, const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) {
	return Predicate6_r<Pred,A1,A2,A3,A4,A5,A6>(pred,a1_,a2_,a3_,a4_,a5_,a6_);
}


// support for function pointers
template<typename R> inline
Predicate0_r<R(*)(void)> 
predicate(R(* pred)(void)) {
    return Predicate0_r<R(*)(void)>(pred);
}

template<typename R, typename P1, typename A1> inline
Predicate1_r<R(*)(P1),A1> 
predicate(R(* pred)(P1), const A1& a1_) {
    return Predicate1_r<R(*)(P1),A1>(pred,a1_);
}

template<typename R, typename P1, typename P2, typename A1, typename A2> inline
Predicate2_r<R(*)(P1,P2),A1,A2> 
predicate(R(* pred)(P1,P2), const A1& a1_, const A2& a2_) {
    return Predicate2_r<R(*)(P1,P2),A1,A2>(pred,a1_,a2_);
}

template<typename R, typename P1, typename P2, typename P3, typename A1, typename A2, typename A3> inline
Predicate3_r<R(*)(P1,P2,P3),A1,A2,A3> 
predicate(R(* pred)(P1,P2,P3), const A1& a1_, const A2& a2_, const A3& a3_) {
    return Predicate3_r<R(*)(P1,P2,P3),A1,A2,A3>(pred,a1_,a2_,a3_);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename A1, typename A2, typename A3, typename A4> inline
Predicate4_r<R(*)(P1,P2,P3,P4),A1,A2,A3,A4> 
predicate(R(* pred)(P1,P2,P3,P4), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) {
    return Predicate4_r<R(*)(P1,P2,P3,P4),A1,A2,A3,A4>(pred,a1_,a2_,a3_,a4_);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Predicate5_r<R(*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5> 
predicate(R(* pred)(P1,P2,P3,P4,P5), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) {
    return Predicate5_r<R(*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5>(pred,a1_,a2_,a3_,a4_,a5_);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Predicate6_r<R(*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6> 
predicate(R(* pred)(P1,P2,P3,P4,P5,P6), const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) {
	return Predicate6_r<R(*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6>(pred,a1_,a2_,a3_,a4_,a5_,a6_);
}

    
//----------------------------------------------------------------------------
// predicate_mf : Adaptor relation for treating predicate methods as relations
//----------------------------------------------------------------------------
template<typename Obj, typename MemPred>
class MemPredicate0_r : public detail::TestOnlyRelation<MemPredicate0_r<Obj,MemPred> > {
    lref<Obj> obj_;
	MemPred pred;
public:
	MemPredicate0_r (lref<Obj> obj_, MemPred pred) : obj_(obj_), pred(pred)
	{ }

	bool apply() {
		return ((*obj_).*pred)();
	}
};

template<typename Obj, typename MemPred, typename A1>
class MemPredicate1_r : public detail::TestOnlyRelation<MemPredicate1_r<Obj,MemPred,A1> > {
    lref<Obj> obj_;
	MemPred pred;
	A1 arg1;
public:
	MemPredicate1_r (lref<Obj> obj_, MemPred pred, const A1& arg1) : obj_(obj_), pred(pred), arg1(arg1)
	{ }

	bool apply() {
		return ((*obj_).*pred)(effective_value(arg1));
	}
};

template<typename Obj, typename MemPred, typename A1, typename A2>
class MemPredicate2_r : public detail::TestOnlyRelation<MemPredicate2_r<Obj,MemPred,A1,A2> > {
    lref<Obj> obj_;
	MemPred pred;
	A1 arg1; A2 arg2;
public:
	MemPredicate2_r (lref<Obj> obj_, MemPred pred, const A1& arg1, const A2& arg2) : obj_(obj_), pred(pred), arg1(arg1), arg2(arg2)
	{ }

	bool apply() {
		return  ((*obj_).*pred)( effective_value(arg1), effective_value(arg2) );
	}
};

template<typename Obj, typename MemPred, typename A1, typename A2, typename A3>
class MemPredicate3_r : public detail::TestOnlyRelation<MemPredicate3_r<Obj,MemPred,A1,A2,A3> > {
    lref<Obj> obj_;
	MemPred pred;
	A1 arg1; A2 arg2; A3 arg3;
public:
	MemPredicate3_r (lref<Obj> obj_, MemPred pred, const A1& arg1, const A2& arg2, const A2& arg3) : obj_(obj_), pred(pred), arg1(arg1), arg2(arg2), arg3(arg3)
	{ }

	bool apply() {
		return  ((*obj_).*pred)(effective_value(arg1), effective_value(arg2), effective_value(arg3));
	}
};

template<typename Obj, typename MemPred, typename A1, typename A2, typename A3, typename A4>
class MemPredicate4_r : public detail::TestOnlyRelation<MemPredicate4_r<Obj,MemPred,A1,A2,A3,A4> > {
	lref<Obj> obj_;
	MemPred pred;
	A1 arg1; A2 arg2; A3 arg3; A4 arg4;
public:
	MemPredicate4_r (lref<Obj> obj_, MemPred pred, const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4) :obj_(obj_), pred(pred), arg1(arg1), arg2(arg2), arg3(arg3), arg4(arg4)
	{ }

	bool apply() {
		return  ((*obj_).*pred)(effective_value(arg1), effective_value(arg2), effective_value(arg3), effective_value(arg4) );
	}
};

template<typename Obj, typename MemPred, typename A1, typename A2, typename A3, typename A4, typename A5>
class MemPredicate5_r : public detail::TestOnlyRelation<MemPredicate5_r<Obj,MemPred,A1,A2,A3,A4,A5> > {
    lref<Obj> obj_;
	MemPred pred;
	A1 arg1; A2 arg2; A3 arg3; A4 arg4; A5 arg5;
public:
	MemPredicate5_r (lref<Obj> obj_, MemPred pred, const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4, const A5& arg5) : obj_(obj_), pred(pred), arg1(arg1), arg2(arg2), arg3(arg3), arg4(arg4), arg5(arg5)
	{ }

	bool apply() {
		return ((*obj_).*pred)(effective_value(arg1), effective_value(arg2), effective_value(arg3), effective_value(arg4), effective_value(arg5) );
	}
};

template<typename Obj, typename MemPred, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class MemPredicate6_r : public detail::TestOnlyRelation<MemPredicate6_r<Obj,MemPred,A1,A2,A3,A4,A5,A6> > {
    lref<Obj> obj_;
	MemPred pred;
	A1 arg1; A2 arg2; A3 arg3; A4 arg4; A5 arg5; A6 arg6;
public:
	MemPredicate6_r (const lref<Obj>& obj_, MemPred pred, const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4, const A5& arg5, const A6& arg6) : obj_(obj_), pred(pred), arg1(arg1), arg2(arg2), arg3(arg3), arg4(arg4), arg5(arg5), arg6(arg6)
	{ }

	bool apply() {
		return  ((*obj_).*pred)(effective_value(arg1), effective_value(arg2), effective_value(arg3), effective_value(arg4), effective_value(arg5), effective_value(arg6));
	}
};


// 0
template<typename R, typename Obj, typename Obj2> inline
MemPredicate0_r<Obj,R(Obj::*)(void)> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(void) ) {
	return MemPredicate0_r<Obj,R(Obj::*)(void)>(obj_, mempred);
}

template<typename R, typename Obj, typename Obj2> inline
MemPredicate0_r<Obj,R(Obj::*)(void) const> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(void) const) {
	return MemPredicate0_r<Obj,R(Obj::*)(void) const>(obj_, mempred);
}

// 1
template<typename R, typename P1, typename Obj, typename Obj2, typename A1> inline
MemPredicate1_r<Obj,R(Obj::*)(P1),A1> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1), const A1& arg1) {
	return MemPredicate1_r<Obj,R(Obj::*)(P1),A1>(obj_, mempred, arg1);
}

template<typename R, typename P1, typename Obj, typename Obj2, typename A1> inline
MemPredicate1_r<Obj,R(Obj::*)(P1) const,A1> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1) const, const A1& arg1) {
	return MemPredicate1_r<Obj,R(Obj::*)(P1) const,A1>(obj_, mempred, arg1);
}

// 2
template<typename R, typename P1, typename P2, typename Obj, typename Obj2, typename A1, typename A2> inline
MemPredicate2_r<Obj,R(Obj::*)(P1,P2),A1,A2> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2), const A1& arg1, const A2& arg2) {
	return MemPredicate2_r<Obj,R(Obj::*)(P1,P2),A1,A2>(obj_, mempred, arg1, arg2);
}

template<typename R, typename P1, typename P2, typename Obj, typename Obj2, typename A1, typename A2> inline
MemPredicate2_r<Obj,R(Obj::*)(P1,P2) const,A1,A2> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2) const, const A1& arg1, const A2& arg2) {
	return MemPredicate2_r<Obj,R(Obj::*)(P1,P2) const,A1,A2>(obj_, mempred, arg1, arg2);
}

// 3
template<typename R, typename P1, typename P2, typename P3, typename Obj, typename Obj2, typename A1, typename A2, typename A3> inline
MemPredicate3_r<Obj,R(Obj::*)(P1,P2,P3),A1,A2,A3> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2,P3), const A1& arg1, const A2& arg2, const A3& arg3) {
	return MemPredicate3_r<Obj,R(Obj::*)(P1,P2,P3),A1,A2,A3>(obj_, mempred, arg1, arg2, arg3);
}

template<typename R, typename P1, typename P2, typename P3, typename Obj, typename Obj2, typename A1, typename A2, typename A3> inline
MemPredicate3_r<Obj,R(Obj::*)(P1,P2,P3) const,A1,A2,A3> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2,P3) const, const A1& arg1, const A2& arg2, const A3& arg3) {
	return MemPredicate3_r<Obj,R(Obj::*)(P1,P2,P3) const,A1,A2,A3>(obj_, mempred, arg1, arg2, arg3);
}

// 4
template<typename R, typename P1, typename P2, typename P3, typename P4, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4>  inline
MemPredicate4_r<Obj,R(Obj::*)(P1,P2,P3,P4),A1,A2,A3,A4> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2,P3,P4), const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4) {
	return MemPredicate4_r<Obj,R(Obj::*)(P1,P2,P3,P4),A1,A2,A3,A4>(obj_, mempred, arg1, arg2, arg3, arg4);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4>  inline
MemPredicate4_r<Obj,R(Obj::*)(P1,P2,P3,P4) const,A1,A2,A3,A4> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2,P3,P4) const, const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4) {
	return MemPredicate4_r<Obj,R(Obj::*)(P1,P2,P3,P4) const,A1,A2,A3,A4>(obj_, mempred, arg1, arg2, arg3, arg4);
}


// 5
template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4, typename A5> inline
MemPredicate5_r<Obj,R(Obj::*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2,P3,P4,P5), const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4, const A5& arg5) {
	return MemPredicate5_r<Obj,R(Obj::*)(P1,P2,P3,P4,P5),A1,A2,A3,A4,A5>(obj_, mempred, arg1, arg2, arg3, arg4, arg5);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4, typename A5> inline
MemPredicate5_r<Obj,R(Obj::*)(P1,P2,P3,P4,P5) const,A1,A2,A3,A4,A5> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2,P3,P4,P5) const, const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4, const A5& arg5) {
	return MemPredicate5_r<Obj,R(Obj::*)(P1,P2,P3,P4,P5) const,A1,A2,A3,A4,A5>(obj_, mempred, arg1, arg2, arg3, arg4, arg5);
}

// 6
template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
MemPredicate6_r<Obj,R(Obj::*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2,P3,P4,P5,P6), const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4, const A5& arg5, const A6& arg6) {
	return MemPredicate6_r<Obj,R(Obj::*)(P1,P2,P3,P4,P5,P6),A1,A2,A3,A4,A5,A6>(obj_, mempred, arg1, arg2, arg3, arg4, arg5, arg6);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename Obj, typename Obj2, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
MemPredicate6_r<Obj,R(Obj::*)(P1,P2,P3,P4,P5,P6) const,A1,A2,A3,A4,A5,A6> 
predicate_mf(lref<Obj>& obj_, R(Obj2::* mempred)(P1,P2,P3,P4,P5,P6) const, const A1& arg1, const A2& arg2, const A3& arg3, const A4& arg4, const A5& arg5, const A6& arg6) {
	return MemPredicate6_r<Obj,R(Obj::*)(P1,P2,P3,P4,P5,P6) const,A1,A2,A3,A4,A5,A6>(obj_, mempred, arg1, arg2, arg3, arg4, arg5, arg6);
}


//----------------------------------------------------------------
// predicate_mem : Unify with a member variable
//----------------------------------------------------------------

template<typename Obj, typename MemberT>
class Predicate_mem_r : public detail::TestOnlyRelation<Predicate_mem_r<Obj,MemberT> > {
	lref<Obj> obj_;
	MemberT Obj::*  mem;
public:
	Predicate_mem_r(const lref<Obj>& obj_, MemberT Obj::* mem) : obj_(obj_), mem(mem)
	{ }

	bool apply() {
		return (*obj_).*mem;
	}
};


template<typename Obj, typename Obj2, typename MemberT> inline
Predicate_mem_r<Obj, MemberT> 
predicate_mem(lref<Obj>& obj_, MemberT Obj2::* mem) {
	return Predicate_mem_r<Obj, MemberT>(obj_, mem);
}

} // namespace castor


#endif
