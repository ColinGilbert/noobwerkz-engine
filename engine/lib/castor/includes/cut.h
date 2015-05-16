// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_CUT_H
#define CASTOR_CUT_H 1

#include "relation.h"
#include "coroutine.h"

namespace castor {

class cut{};

template<typename LRel, typename RRel>
class CutAnd;
template<typename LRel, typename RRel>
class CutOr;

namespace detail {
    template<typename Expr1, typename Expr2> inline
    bool exec(CutAnd<Expr1,Expr2>& cut_expr, bool& arg) {
        return cut_expr.exec(arg);
    }

    template<typename Expr1, typename Expr2> inline
    bool exec(CutOr<Expr1,Expr2>& cut_expr, bool& arg) {
        return cut_expr.exec(arg);
    }

    template<typename Rel> inline
    bool exec(Rel& rel, bool&) {
        return rel();
    }
}

//-----------------------------------------
// Operators && and || for Cut expressions
//-----------------------------------------

struct InvalidState{};

template<typename LRel, typename RRel>
class CutAnd : public Coroutine {
    LRel l;
    relation r;
    RRel rbegin;
	enum {A, A1, A2, B, C} state;
public:
    CutAnd(const LRel& l, const RRel& r) : l(l), r(r), rbegin(r), state(A)
    { }

    bool exec(bool& cut_occurred) {
		switch(state) {
		case A:
            if( !detail::exec(l,cut_occurred) ) {
				state=B;
				return false;
			}
			state=A1;
			// .. drop down to A1
		case A1:
            if(detail::exec(r,cut_occurred)) {
				state=C;
				return true;
			}
			state=A2;
			// .. drop down to A2
		case A2:
            if(detail::exec(l,cut_occurred)) {
				r=rbegin;
				state=A1;
                return exec(cut_occurred);
			}
			state=B;
			// .. drop down to B
		case B:
			return false;
		case C:
			if(detail::exec(r,cut_occurred))
				return true;
			state=A2;
            return exec(cut_occurred);
		default:
			throw InvalidState();
		}
    }
};

template<typename LRel, typename RRel/*=cut*/>
class CutOr  {
    LRel l;
    RRel r;
    bool leftdone, rightdone;
public:
    CutOr(const LRel& l, const RRel& r) : l(l), r(r), leftdone(false), rightdone(false)
    { }

    bool exec(bool& cut_occurred) {
        if(!leftdone){
            leftdone=!detail::exec(l,cut_occurred);
            rightdone=cut_occurred;
            if(!leftdone)
                return true;
        }
        if(!rightdone) {
            rightdone=!detail::exec(r,cut_occurred);
            return !rightdone;
        }
        return false;
    }
};

// Support for : ... && cut()
// One solution from left of &&
template<typename LRel>
class CutAnd<LRel,cut> {
    LRel l;
    bool done;
public:
    CutAnd(const LRel& l) : l(l), done(false)
    { }

    CutAnd(const LRel& l, cut) : l(l), done(false)
    { }

    bool exec(bool& cut_occurred) {
        if(done)
            return false;
        done=true;
        cut_occurred=detail::exec(l, cut_occurred);
        return cut_occurred;
    }
};

// Support for : ... || cut()
// Produce all solutions from foo
template<typename LRel>
class CutOr<LRel,cut> {
    LRel l;
    bool done;
public:
    CutOr(const LRel& l) : l(l), done(false)
    { }

    CutOr(const LRel& l, cut) : l(l), done(false)
    { }

    bool exec(bool& cut_occurred) {
        if(done)
            return false;
        if( detail::exec(l, cut_occurred) )
            return true;
        cut_occurred = true;
        done = true;
        return false;
    }
};


// Support for : cut() && ...
// All solutions from right of &&
template<typename RRel>
class CutAnd<cut,RRel> {
    RRel r;
    bool done;
public:
    CutAnd(const RRel& r) : r(r), done(false)
    { }

    CutAnd(const RRel& r, cut) : r(r), done(false)
    { }

    bool exec(bool& cut_occurred) {
        if(done)
            return false;
        cut_occurred=true;
        if( detail::exec(r, cut_occurred) )
            return true;
        done=true;
        return false;
    }
};


// Support for : cut() || .....
// ignore everything to the right of ||
template<typename RRel>
struct CutOr<cut,RRel>  {
    CutOr(const RRel&)
    { }
    CutOr(cut, const RRel&)
    { }

    bool exec(bool& cut_occurred) const {
        cut_occurred=true;
        return false;
    }
};



// Support for : cut() && cut()
// No solutions for this expression
template<>
struct CutAnd<cut,cut> {
    CutAnd(cut, cut)
    { }

    CutAnd()
    { }

    bool exec(bool /*cut_occurred*/) const {
        return false;
    }
};

// Support for : cut() || cut()
// No solutions for this expression
template<>
struct CutOr<cut,cut> {
    CutOr(cut, cut)
    { }

    CutOr()
    { }

    bool exec(bool& cut_occurred) const {
        cut_occurred=true;
        return false;
    }
};

template<typename RRel> inline
CutAnd<cut,RRel> operator && (cut, const RRel & r) {
	return CutAnd<cut, RRel>(r);
}


template<typename LRel> inline
CutAnd<LRel,cut> operator && (const LRel & l, cut ) {
	return CutAnd<LRel, cut>(l);
}


inline
CutAnd<cut,cut> operator && (cut, cut ) {
	return CutAnd<cut,cut>();
}

template<typename LRel1, typename LRel2, typename RRel> inline
CutAnd<CutAnd<LRel1,LRel2>, RRel> operator && (const CutAnd<LRel1,LRel2> & l, const RRel & r) {
	return CutAnd<CutAnd<LRel1,LRel2>, RRel>(l,r);
}

template<typename LRel, typename RRel1, typename RRel2> inline
CutAnd<LRel, CutAnd<RRel1,RRel2> > operator && (const LRel & l, const CutAnd<RRel1,RRel2> & r) {
	return CutAnd<LRel, CutAnd<RRel1,RRel2> >(l,r);
}

template<typename LRel1, typename LRel2, typename RRel1, typename RRel2> inline
CutAnd<CutAnd<LRel1,LRel2>, CutAnd<RRel1,RRel2> > operator && (const CutAnd<LRel1,LRel2> & l, const CutAnd<RRel1,RRel2> & r) {
	return CutAnd<CutAnd<LRel1,LRel2>, CutAnd<RRel1,RRel2> >(l,r);
}

template<typename LRel1, typename LRel2, typename RRel1, typename RRel2> inline
CutAnd<CutOr<LRel1,LRel2>, CutOr<RRel1,RRel2> > operator && (const CutOr<LRel1,LRel2> & l, const CutOr<RRel1,RRel2> & r) {
	return CutAnd<CutOr<LRel1,LRel2>, CutOr<RRel1,RRel2> >(l,r);
}

template<typename LRel1, typename LRel2, typename RRel1, typename RRel2> inline
CutAnd<CutAnd<LRel1,LRel2>, CutOr<RRel1,RRel2> > operator && (const CutAnd<LRel1,LRel2> & l, const CutOr<RRel1,RRel2> & r) {
	return CutAnd<CutAnd<LRel1,LRel2>, CutOr<RRel1,RRel2> >(l,r);
}

template<typename LRel> inline
CutOr<LRel,cut> operator || (const LRel & l, cut ) {
	return CutOr<LRel, cut>(l);
}

template<typename RRel> inline
CutOr<cut,RRel> operator || (cut , const RRel & r) {
	return CutOr<cut, RRel>(r);
}

inline
CutOr<cut,cut> operator || (cut , cut ) {
	return CutOr<cut,cut>();
}

template<typename LRel1, typename LRel2, typename RRel> inline
CutOr<CutOr<LRel1,LRel2>, RRel> operator || (const CutOr<LRel1,LRel2> & l, const RRel & r) {
	return CutOr<CutOr<LRel1,LRel2>, RRel>(l,r);
}

template<typename LRel1, typename LRel2, typename RRel> inline
CutOr<CutAnd<LRel1,LRel2>,RRel> operator || (const CutAnd<LRel1,LRel2> & l, const RRel & r) {
	return CutOr<CutAnd<LRel1,LRel2>, RRel>(l,r);
}

template<typename LRel, typename RRel1, typename RRel2> inline
CutOr<LRel, CutOr<RRel1,RRel2> > operator || (const LRel & l, const CutOr<RRel1,RRel2> & r) {
	return CutOr<LRel, CutOr<RRel1,RRel2> >(l,r);
}

template<typename LRel, typename RRel1, typename RRel2> inline
CutOr<LRel,CutAnd<RRel1,RRel2> > operator || (const LRel & l, const CutAnd<RRel1,RRel2> & r) {
	return CutOr<LRel, CutAnd<RRel1,RRel2> >(l,r);
}

template<typename LRel1, typename LRel2, typename RRel1, typename RRel2> inline
CutOr<CutAnd<LRel1,LRel2>, CutAnd<RRel1,RRel2> > operator || (const CutAnd<LRel1,LRel2> & l, const CutAnd<RRel1,RRel2> & r) {
	return CutOr<CutAnd<LRel1,LRel2>, CutAnd<RRel1,RRel2> >(l,r);
}

template<typename LRel1, typename LRel2, typename RRel1, typename RRel2> inline 
CutOr<CutOr<LRel1,LRel2>, CutOr<RRel1,RRel2> > operator || (const CutOr<LRel1,LRel2> & l, const CutOr<RRel1,RRel2> & r) {
	return CutOr<CutOr<LRel1,LRel2>, CutOr<RRel1,RRel2> >(l,r);
}

template<typename LRel1, typename LRel2, typename RRel1, typename RRel2> inline 
CutOr<CutAnd<LRel1,LRel2>, CutOr<RRel1,RRel2> > operator || (const CutAnd<LRel1,LRel2> & l, const CutOr<RRel1,RRel2> & r) {
	return CutOr<CutAnd<LRel1,LRel2>, CutOr<RRel1,RRel2> >(l,r);
}

template<typename LRel1, typename LRel2, typename RRel1, typename RRel2> inline 
CutOr<CutOr<LRel1,LRel2>, CutAnd<RRel1,RRel2> > operator || (const CutOr<LRel1,LRel2> & l, const CutAnd<RRel1,RRel2> & r) {
	return CutOr<CutOr<LRel1,LRel2>, CutAnd<RRel1,RRel2> >(l,r);
}

// Requires: 
// ExprWithCut : must implement method bool exec(bool&).
 template<typename ExprWithCut>
class CutExpr_r {
    ExprWithCut expr;
    bool cut_occurred;
public:
    CutExpr_r(const ExprWithCut& expr) : expr(expr), cut_occurred(false)
    { }

    bool operator()() {
        return expr.exec(cut_occurred);
    }
};

template<typename ExprWithCut> inline
CutExpr_r<ExprWithCut> cutexpr(const ExprWithCut& cut_expr) {
    return CutExpr_r<ExprWithCut>(cut_expr);
}

} // namespace castor

#endif
