// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#ifndef CASTOR_RECURSE_H
#define CASTOR_RECURSE_H 1

#include "relation.h"
#include "helpers.h"
#include "workaround.h"

namespace castor {

template<typename Rel_0>
struct Recurse0_r {
	Rel_0 r;
	relation rTmp;
	bool initialized;

	Recurse0_r (Rel_0 r) : r(r), rTmp(False()), initialized(false)
	{ }

	bool operator()(void) {
		if(!initialized) {
			rTmp = r();
			initialized=true;
		}
		return rTmp();
	}
};

template<typename Rel_1, typename A1>
struct Recurse1_r {
	Rel_1 r;
	lref<A1> a1;
	relation rTmp;
	bool initialized;

	Recurse1_r (Rel_1 r, const lref<A1>& a1) : r(r), a1(a1), rTmp(False()), initialized(false)
	{ }

	bool operator()(void) {
		if(!initialized) {
			rTmp = r(a1);
			initialized=true;
		}
		return rTmp();
	}
};

template<typename Rel_2, typename A1,  typename A2>
struct Recurse2_r {
	Rel_2 r;
	lref<A1> a1; lref<A2> a2;
	relation rTmp;
	bool initialized;

	Recurse2_r (Rel_2 r, const lref<A1>& a1, const lref<A2>& a2) : r(r), a1(a1), a2(a2), rTmp(False()), initialized(false)
	{ }

	bool operator()(void) {
		if(!initialized) {
			rTmp = r(a1, a2);
			initialized=true;
		}
		return rTmp();
	}
};

template<typename Rel_3, typename A1,  typename A2,  typename A3>
struct Recurse3_r {
	Rel_3 r;
	lref<A1> a1; lref<A2> a2; lref<A3> a3;
	relation rTmp;
	bool initialized;

	Recurse3_r (Rel_3 r, const lref<A1>& a1, const lref<A2>& a2, const lref<A3>& a3) : r(r), a1(a1), a2(a2), a3(a3), rTmp(False()), initialized(false)
	{ }

	bool operator()(void) {
		if(!initialized) {
			rTmp = r(a1, a2, a3);
			initialized=true;
		}
		return rTmp();
	}
};


template<typename Rel_4, typename A1,  typename A2,  typename A3,  typename A4>
struct Recurse4_r {
	Rel_4 r;
	lref<A1> a1; lref<A2> a2; lref<A3> a3; lref<A4> a4;
	relation rTmp;
	bool initialized;

	Recurse4_r (Rel_4 r, const lref<A1>& a1, const lref<A2>& a2, const lref<A3>& a3, const lref<A4>& a4) : r(r), a1(a1), a2(a2), a3(a3), a4(a4), rTmp(False()), initialized(false)
	{ }

	bool operator()(void) {
		if(!initialized) {
			rTmp = r(a1, a2, a3, a4);
			initialized=true;
		}
		return rTmp();
	}
};


template<typename Rel_5, typename A1,  typename A2,  typename A3,  typename A4,  typename A5>
struct Recurse5_r {
	Rel_5 r;
	lref<A1> a1; lref<A2> a2; lref<A3> a3; lref<A4> a4; lref<A5> a5;
	relation rTmp;
	bool initialized;

	Recurse5_r (Rel_5 r, const lref<A1>& a1, const lref<A2>& a2, const lref<A3>& a3, const lref<A4>& a4, const lref<A5>& a5) : r(r), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), rTmp(False()), initialized(false)
	{ }

	bool operator()(void) {
		if(!initialized) {
			rTmp = r(a1, a2, a3, a4, a5);
			initialized=true;
		}
		return rTmp();
	}
};


template<typename Rel_6, typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6>
struct Recurse6_r {
	Rel_6 r;
	lref<A1> a1; lref<A2> a2; lref<A3> a3; lref<A4> a4; lref<A5> a5;  lref<A6> a6;
	relation rTmp;
	bool initialized;

	Recurse6_r (Rel_6 r, const lref<A1>& a1, const lref<A2>& a2, const lref<A3>& a3, const lref<A4>& a4, const lref<A5>& a5, const lref<A6>& a6) : r(r), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6), rTmp(False()), initialized(false)
	{ }

	bool operator()(void) {
		if(!initialized) {
			rTmp = r(a1, a2, a3, a4, a5, a6);
			initialized=true;
		}
		return rTmp();
	}
};


template<typename Rel_0> inline
Recurse0_r<Rel_0> recurse( Rel_0 r ) {
	return Recurse0_r<Rel_0>(r);
}

template<typename Rel_1, typename A1> inline
Recurse1_r<Rel_1,A1> recurse(Rel_1 r, lref<A1>& a1) {
	return Recurse1_r<Rel_1,A1>(r,a1);
}

template<typename Rel_2, typename A1, typename A2> inline
Recurse2_r<Rel_2,A1,A2> recurse(Rel_2 r, lref<A1>& a1, lref<A2>& a2) {
	return Recurse2_r<Rel_2,A1,A2>(r,a1,a2);
}

template<typename Rel_3, typename A1, typename A2, typename A3> inline
Recurse3_r<Rel_3,A1,A2,A3> recurse(Rel_3 r, lref<A1>& a1, lref<A2>& a2, lref<A3>& a3) {
	return Recurse3_r<Rel_3,A1,A2,A3>(r,a1,a2,a3);
}

template<typename Rel_4, typename A1, typename A2, typename A3, typename A4> inline
Recurse4_r<Rel_4,A1,A2,A3,A4> recurse(Rel_4 r, lref<A1>& a1, lref<A2>& a2, lref<A3>& a3, lref<A4>& a4) {
	return Recurse4_r<Rel_4,A1,A2,A3,A4>(r,a1,a2,a3,a4);
}

template<typename Rel_5, typename A1, typename A2, typename A3, typename A4, typename A5> inline
Recurse5_r<Rel_5,A1,A2,A3,A4,A5> recurse(Rel_5 r, lref<A1>& a1, lref<A2>& a2, lref<A3>& a3, lref<A4>& a4, lref<A5>& a5) {
	return Recurse5_r<Rel_5,A1,A2,A3,A4,A5>(r,a1,a2,a3,a4,a5);
}

template<typename Rel_6, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
Recurse6_r<Rel_6,A1,A2,A3,A4,A5,A6> recurse(Rel_6 r, lref<A1>& a1, lref<A2>& a2, lref<A3>& a3, lref<A4>& a4, lref<A5>& a5, lref<A6>& a6) {
	return Recurse6_r<Rel_6,A1,A2,A3,A4,A5,A6>(r,a1,a2,a3,a4,a5,a6);
}


template<typename Obj, typename MemRel>
struct RecurseMem0_r {
  Obj* obj;
  MemRel mf;
  relation r;
  bool initialized;

  RecurseMem0_r(Obj* obj, MemRel mf) : obj(obj), mf(mf), r(False()), initialized(false)
  { }

  bool operator()(void) {
    if(!initialized) {
      r = ( (obj->*mf)() );
      initialized=true;
    }
    return r();
  }
};

template<typename Obj, typename MemRel, typename A1>
struct RecurseMem1_r {
  Obj* obj;
  MemRel mf;
  lref<A1> a1;
  relation r;
  bool initialized;

  RecurseMem1_r(Obj* obj, MemRel mf, const lref<A1>& a1) : obj(obj), mf(mf), a1(a1), r(False()), initialized(false)
  { }

  bool operator()(void) {
    if(!initialized) {
      r = ((obj->*mf)(a1));
      initialized=true;
    }
    return r();
  }
};

template<typename Obj, typename MemRel, typename A1, typename A2>
struct RecurseMem2_r {
  Obj* obj;
  MemRel mf;
  lref<A1> a1;	lref<A2> a2;
  relation r;
  bool initialized;

  RecurseMem2_r(Obj* obj, MemRel mf, const lref<A1>& a1, const lref<A2>& a2) : obj(obj), mf(mf), a1(a1), a2(a2), r(False()), initialized(false)
  { }

  bool operator()(void) {
    if(!initialized) {
      r = ((obj->*mf)(a1,a2));
      initialized=true;
    }
    return r();
  }
};

template<typename Obj, typename MemRel, typename A1, typename A2, typename A3>
struct RecurseMem3_r {
  Obj* obj;
  MemRel mf;
  lref<A1> a1;	lref<A2> a2;
  lref<A3> a3;
  relation r;
  bool initialized;

  RecurseMem3_r(Obj* obj, MemRel mf, const lref<A1>& a1, const lref<A2>& a2, const lref<A3>& a3) : obj(obj), mf(mf), a1(a1), a2(a2), a3(a3), r(False()), initialized(false)
  { }

  bool operator()(void) {
    if(!initialized) {
      r = ((obj->*mf)(a1,a2,a3));
      initialized=true;
    }
    return r();
  }
};

template<typename Obj, typename MemRel, typename A1, typename A2, typename A3, typename A4>
struct RecurseMem4_r {
  Obj* obj;
  MemRel mf;
  lref<A1> a1;	lref<A2> a2;	lref<A3> a3;	lref<A4> a4;
  relation r;
  bool initialized;

  RecurseMem4_r(Obj* obj, MemRel mf, const lref<A1>& a1, const lref<A2>& a2, const lref<A3>& a3, const lref<A4>& a4) : obj(obj), mf(mf), a1(a1), a2(a2), a3(a3), a4(a4), r(False()), initialized(false)
  { }

  bool operator()(void) {
    if(!initialized) {
      r = ((obj->*mf)(a1,a2,a3,a4));
      initialized=true;
    }
    return r();
  }
};

template<typename Obj, typename MemRel, typename A1, typename A2, typename A3, typename A4, typename A5>
struct RecurseMem5_r {
  Obj* obj;
  MemRel mf;
  lref<A1> a1;	lref<A2> a2;	lref<A3> a3;	lref<A4> a4;	lref<A5> a5;
  relation r;
  bool initialized;

  RecurseMem5_r(Obj* obj, MemRel mf, const lref<A1>& a1, const lref<A2>& a2, const lref<A3>& a3, const lref<A4>& a4, const lref<A5>& a5) : obj(obj), mf(mf), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), r(False()), initialized(false)
  { }

  bool operator()(void) {
    if(!initialized) {
      r = ((obj->*mf)(a1,a2,a3,a4,a5));
      initialized=true;
    }
    return r();
  }
};

template<typename Obj, typename MemRel, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct RecurseMem6_r {
  Obj* obj;
  MemRel mf;
  lref<A1> a1;	lref<A2> a2;	lref<A3> a3;	lref<A4> a4;	lref<A5> a5;  lref<A6> a6;
  relation r;
  bool initialized;

  RecurseMem6_r(Obj* obj, MemRel mf, const lref<A1>& a1, const lref<A2>& a2, const lref<A3>& a3, const lref<A4>& a4, const lref<A5>& a5, const lref<A6>& a6) : obj(obj), mf(mf), a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(6), r(False()), initialized(false)
  { }

  bool operator()(void) {
    if(!initialized) {
      r = ((obj->*mf)(a1,a2,a3,a4,a5,a6));
      initialized=true;
    }
    return r();
  }
};


//-- 0
template<typename Obj, typename MemRel_0> inline
RecurseMem0_r<Obj,MemRel_0> recurse_mf(Obj* obj, MemRel_0 mr) {
	return RecurseMem0_r<Obj,MemRel_0>(obj,mr);
}

//-- 1
template<typename Obj, typename MemRel_1, typename A1> inline
RecurseMem1_r<Obj,MemRel_1,A1> recurse_mf(Obj* obj, MemRel_1 mr, lref<A1>& a1) {
	return RecurseMem1_r<Obj,MemRel_1,A1>(obj,mr,a1);
}

//-- 2
template<typename Obj, typename MemRel_2, typename A1, typename A2> inline
RecurseMem2_r<Obj,MemRel_2,A1,A2> recurse_mf(Obj* obj, MemRel_2 mr, lref<A1>& a1, lref<A2>& a2) {
	return RecurseMem2_r<Obj,MemRel_2,A1,A2>(obj,mr,a1,a2);
}

//-- 3
template<typename Obj, typename MemRel_3, typename A1, typename A2, typename A3> inline
RecurseMem3_r<Obj,MemRel_3,A1,A2,A3> recurse_mf(Obj* obj, MemRel_3 mr, lref<A1>& a1, lref<A2>& a2, lref<A3>& a3) {
	return RecurseMem3_r<Obj,MemRel_3,A1,A2,A3>(obj,mr,a1,a2,a3);
}

//-- 4
template<typename Obj, typename MemRel_4, typename A1, typename A2, typename A3, typename A4> inline
RecurseMem4_r<Obj,MemRel_4,A1,A2,A3,A4> recurse_mf(Obj* obj, MemRel_4 mr, lref<A1>& a1, lref<A2>& a2, lref<A3>& a3, lref<A4>& a4) {
	return RecurseMem4_r<Obj,MemRel_4,A1,A2,A3,A4>(obj,mr,a1,a2,a3,a4);
}

//-- 5
template<typename Obj, typename MemRel_5, typename A1, typename A2, typename A3, typename A4, typename A5> inline
RecurseMem5_r<Obj,MemRel_5,A1,A2,A3,A4,A5> recurse_mf(Obj* obj, MemRel_5 mr, lref<A1>& a1, lref<A2>& a2, lref<A3>& a3, lref<A4>& a4, lref<A5>& a5) {
	return RecurseMem5_r<Obj,MemRel_5,A1,A2,A3,A4,A5>(obj,mr,a1,a2,a3,a4,a5);
}

//-- 6
template<typename Obj, typename MemRel_6, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> inline
RecurseMem6_r<Obj,MemRel_6,A1,A2,A3,A4,A5,A6> recurse_mf(Obj* obj, MemRel_6 mr, lref<A1>& a1, lref<A2>& a2, lref<A3>& a3, lref<A4>& a4, lref<A5>& a5, lref<A6>& a6) {
	return RecurseMem6_r<Obj,MemRel_6,A1,A2,A3,A4,A5,A6>(obj,mr,a1,a2,a3,a4,a5,a6);
}

} // namespace castor

#endif
