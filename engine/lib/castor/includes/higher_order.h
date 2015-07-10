// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_HIGHER_ORDER_H
#define CASTOR_HIGHER_ORDER_H 1

#include "coroutine.h"

namespace castor {

//-------------------------------------------------
// zip(l,r) - succeeds each time l() and r() succeed
//-------------------------------------------------

template<class L, class R>
class Zip_r : public Coroutine {
	L l;
	R r;
public:
	Zip_r (const L& l, const R& r) : l(l), r(r) 
	{ }

	bool operator()(void) {
		co_begin();
		while(true) {
			if(!l()) 
				co_return(false);
			if(r()) {
				co_yield(true);
			}
			else { 
				co_return(false);
			}
		}
		co_end();
	}
};

template<class L, class R>
Zip_r<L,R> zip(const L& l, const R& r) {
	return Zip_r<L,R>(l,r);
}

} // namespace castor
#endif
