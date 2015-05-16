// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#ifndef CASTOR_FUNCTIONAL_H
#define CASTOR_FUNCTIONAL_H 1

namespace castor { namespace detail {

// Currying functions
// Concepts: P1 should be copy constructible
//           F1 supports operator() without one argument A such that P1 is same as A or convertible to A
template<typename RetType, typename F, typename P1>
struct Binder1 {
	F f;
	P1 p1;

	Binder1 (const F & f, const P1 & p1) : f(f), p1(p1)
	{ }

	RetType operator()(void) {
		return f(p1);
	}
};

template<typename RetType, typename F, typename P1, typename P2>
struct Binder2 {
	F f;
	P1 p1;	P2 p2;

	Binder2 (const F& f, const P1& p1, const P2& p2) : f(f), p1(p1), p2(p2)
	{ }

	RetType operator()(void) {
		return f(p1,p2);
	}
};

template<typename RetType, typename F, typename P1, typename P2, typename P3>
struct Binder3 {
	F f;
	P1 p1;	P2 p2;	P3 p3;

	Binder3 (const F& f, const P1& p1, const P2& p2, const P3& p3) : f(f), p1(p1), p2(p2), p3(p3)
	{ }

	RetType operator()(void) {
		return f(p1,p2,p3);
	}
};


template<typename RetType, typename F, typename P1, typename P2, typename P3, typename P4>
struct Binder4 {
	F f;
	P1 p1;	P2 p2;	P3 p3;	P4 p4;

	Binder4 (const F& f, const P1& p1, const P2& p2, const P3& p3, const P4& p4) : f(f), p1(p1), p2(p2), p3(p3), p4(p4)
	{ }

	RetType operator()(void) {
		return f(p1,p2,p3,p4);
	}
};

template<typename RetType, typename F, typename P1, typename P2, typename P3, typename P4, typename P5>
struct Binder5 {
	F f;
	P1 p1;	P2 p2;	P3 p3;	P4 p4;	P5 p5;

    Binder5 (const F& f, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) : f(f), p1(p1), p2(p2), p3(p3), p4(p4), p5(p5)
	{ }

	RetType operator()(void) {
		return f(p1,p2,p3,p4,p5);
	}
};

template<typename RetType, typename F, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct Binder6 {
	F f;
	P1 p1;	P2 p2;	P3 p3;	P4 p4;	P5 p5;   P6 p6;

    Binder6 (const F& f, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6) : f(f), p1(p1), p2(p2), p3(p3), p4(p4), p5(p5), p6(6)
	{ }

	RetType operator()(void) {
		return f(p1,p2,p3,p4,p5,p6);
	}
};

template<typename RetType, typename F, typename P1> inline 
Binder1<RetType,F,P1> bind(F f, const P1 & p1) {
	return Binder1<RetType,F,P1>(f,p1);
}

template<typename RetType, typename F, typename P1, typename P2> inline 
Binder2<RetType,F,P1,P2> bind(F f, const P1 & p1, const P2 & p2) {
	return Binder2<RetType,F,P1,P2>(f,p1,p2);
}

template<typename RetType, typename F, typename P1, typename P2, typename P3> inline 
Binder3<RetType,F,P1,P2,P3> bind(F f, const P1 & p1, const P2 & p2, const P3 & p3) {
	return Binder3<RetType,F,P1,P2,P3>(f,p1,p2,p3);
}

template<typename RetType, typename F, typename P1, typename P2, typename P3, typename P4> inline 
Binder4<RetType,F,P1,P2,P3,P4> bind(F f, const P1 & p1, const P2 & p2, const P3 & p3, const P4 & p4) {
	return Binder4<RetType,F,P1,P2,P3,P4>(f,p1,p2,p3,p4);
}

template<typename RetType, typename F, typename P1, typename P2, typename P3, typename P4 ,typename P5> inline 
Binder5<RetType,F,P1,P2,P3,P4,P5> bind(F f, const P1 & p1, const P2 & p2, const P3 & p3, const P4 & p4, const P5 & p5) {
	return Binder5<RetType,F,P1,P2,P3,P4,P5>(f,p1,p2,p3,p4,p5);
}

template<typename RetType, typename F, typename P1, typename P2, typename P3, typename P4 ,typename P5, typename P6> inline 
Binder6<RetType,F,P1,P2,P3,P4,P5,P6> bind(F f, const P1 & p1, const P2 & p2, const P3 & p3, const P4 & p4, const P5 & p5, const P6 & p6) {
	return Binder6<RetType,F,P1,P2,P3,P4,P5,P6>(f,p1,p2,p3,p4,p5,p6);
}

} } // namespace castor::detail

#endif
