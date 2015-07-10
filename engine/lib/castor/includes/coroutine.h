// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_COROUTINE_H
#define CASTOR_COROUTINE_H 1

namespace castor {

class Coroutine {
protected:
  long co_entry_pt;
  void zero() {
	  co_entry_pt=0;
  }
public:
  Coroutine() : co_entry_pt(0)
  { }
};


// macros for implementing relations imperatively (as coroutines)
#define co_begin()    switch (co_entry_pt) { case 0:
#define co_yield(x)   { co_entry_pt = (x)? __LINE__ : -1; return (co_entry_pt==__LINE__); case __LINE__: ; }
#define co_return(x)  { co_entry_pt = -1; return (x); }
#define co_end()      break; } co_entry_pt = -1; return false;



} // namespace castor
#endif
