// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_TLR_H
#define CASTOR_TLR_H 1

#include "coroutine.h"
#include "relation.h"
#include "lref.h"
#include "helpers.h"
#include <vector>
#include <utility>
#include <iterator>
#include <memory>
#include <algorithm>
#include <functional>

namespace castor {

    struct InvalidArg{};

class relation_tlr {
    struct impl {
        virtual ~impl(){}
        virtual impl* clone() const=0;
        virtual bool operator()(relation&)=0;
    };

    template<class F>
    struct wrapper : public impl {
        explicit wrapper(const F& f_) : f(f_)
        { }
        virtual impl* clone() const {
            return new wrapper<F>(this->f);
        }
        virtual bool operator()(relation& r) {
            return this->f(r);
        }
    private:
        F f;
    };

    std::auto_ptr<impl> pimpl;

    template<class Expr>  // disable instantiating relation_tlr from ILE
    relation_tlr(const Ile<Expr>& rhs);
public:
    typedef bool result_type;

    // Concept : F supports the method... bool F::operator()(relation&)
    template<class F>
    relation_tlr(F f) : pimpl(new wrapper<F>(f)) { 
        typedef bool (F::* boolMethod)(relation&);  // Compiler Error : f must support method bool F::operator()(relation)
    }

    relation_tlr(const relation_tlr& rhs) : pimpl(rhs.pimpl->clone())
    { }

    relation_tlr& operator=(const relation_tlr& rhs) {
        this->pimpl.reset(rhs.pimpl->clone());
        return *this;
    }

    bool operator()(relation & r) /*const*/ {
      return (*this->pimpl.get())(r); // we use get() instead of * in pimpl since constructors guarantee pimpl will be initialized
	}
};


class TakeLeft_r : public Coroutine {
	relation r;
	relation_tlr tlr;
public:
	TakeLeft_r(const relation& r, const relation_tlr& tlr) : r(r), tlr(tlr)
	{ }

	bool operator()(void) {
        co_begin();
        while(true)
		    co_yield( tlr(r) );
        co_end();
	}
};

inline
TakeLeft_r operator >>= (const relation& r, const relation_tlr& tlr) {
	return TakeLeft_r(r,tlr);
}


template<typename T, typename Pred>
struct Order_tlr : public Coroutine {
	lref<T> obj;
	std::vector<T> v;
	typename std::vector<T>::iterator itr;
	Pred pred;

	Order_tlr(const lref<T>& obj, Pred pred) : obj(obj), pred(pred)
	{}

	bool operator() (relation& r) {
		co_begin();
		if(obj.defined())
			throw InvalidArg();
		if(!r())
			co_return(false);
		do {
			v.push_back(*obj);
		} while(r());

		std::sort(v.begin(), v.end(), pred);
		for(itr=v.begin(); itr!=v.end(); ++itr) {
			obj.set_ptr(&*itr,false);
			co_yield(true);
		}
		obj.reset();
		co_end();
	}
};


// For use with >>= operator
// throws InvalidArg() if obj is initialized at the time evaluation
template<typename T>  inline
Order_tlr<T,std::less<T> > 
order(lref<T>& obj) {
	return Order_tlr<T,std::less<T> >(obj,std::less<T>());
}

// For use with >>= operator
// throws InvalidArg() if obj is initialized at the time evaluation
template<typename T, typename Pred>  inline
Order_tlr<T,Pred> 
order(lref<T>& obj, Pred cmp) {
	return Order_tlr<T,Pred>(obj,cmp);
}

namespace detail {
template<typename T, typename BinPred>
struct RevCmp {
	BinPred p;
	explicit RevCmp(BinPred p) : p(p)
	{}

	bool operator()(const T& l, const T& r)  {
		return p(r,l);
	}
	bool operator()(const T& l, const T& r)  const {
		return p(r,l);
	}
};
} // namespace detail



namespace detail {
template<typename T, typename MemFunc, typename BinPred>
struct MfPred {
	MemFunc mf;
	BinPred p;
	MfPred(MemFunc mf, BinPred p) : mf(mf), p(p)
	{}
	bool operator()(T& l, T& r) {
		return p( (l.*mf)(), (r.*mf)() );
	}
	bool operator()(const T& l, const T& r)  const {
		return p( (l.*mf)(), (r.*mf)() );
	}
};

// reverses order arguments to BinPred
template<typename T, typename MemFunc, typename BinPred>
struct MfPred2 {
	MemFunc mf;
	BinPred p;
	MfPred2(MemFunc mf, BinPred p) : mf(mf), p(p)
	{}
	bool operator()(T& l, T& r) {
		return p( (r.*mf)(), (l.*mf)() );
	}
	bool operator()(const T& l, const T& r)  const {
		return p( (r.*mf)(), (l.*mf)() );
	}
};

} // namespace detail

template<class T, class Pred>
struct OrderByBase : public Coroutine {
	lref<T> obj;
	typename std::vector<T>::iterator itr;
	Pred pred;
	std::vector<T> v;

	OrderByBase(const lref<T>& obj, Pred p) : obj(obj), pred(p)
	{}

	bool operator() (relation& r) {
		co_begin();
		if(obj.defined())
			throw InvalidArg();
		if(!r())
			co_return(false);
		do {
			v.push_back(*obj);
		} while(r());
		
		std::sort(v.begin(), v.end(), pred);
		for(itr=v.begin(); itr!=v.end(); ++itr) {
			obj.set_ptr(&*itr,false);
			co_yield(true);
		}
		obj.reset();
		co_end();
	}
};

//-------------------------------------------------
// order_mf(obj,mf,p) TLR
// throws InvalidArg() if obj is initialized at the time evaluation
//-------------------------------------------------
template<class T, class MemFunc, class Pred>
struct OrderMf_tlr : public OrderByBase<T, detail::MfPred<T,MemFunc,Pred> > {
	typedef OrderByBase<T, detail::MfPred<T,MemFunc,Pred> > BaseClass;
	OrderMf_tlr(const lref<T>& obj, MemFunc mf, Pred p) : BaseClass(obj, detail::MfPred<T,MemFunc,Pred>(mf,p))
	{}
};

template<typename T, typename MemFunc>  inline
OrderMf_tlr<T,MemFunc,std::less<typename detail::return_type<MemFunc>::result_type> > 
order_mf(lref<T>& obj, MemFunc f) {
	typedef typename detail::return_type<MemFunc>::result_type R;
	return OrderMf_tlr<T,MemFunc,std::less<R> >(obj,f,std::less<R>());
}

template<typename T, typename MemFunc, typename Pred>  inline
OrderMf_tlr<T,MemFunc,Pred> 
order_mf(lref<T>& obj, MemFunc f, Pred p) {
	return OrderMf_tlr<T,MemFunc,Pred>(obj,f,p);
}


namespace detail {
template<class T, class Mem, class BinaryPred>
struct CompareMember {
	Mem T::* mem;
	BinaryPred p;
	CompareMember(Mem T::* mem, BinaryPred pred) : mem(mem), p(pred)
	{}
	bool operator()(T& l, T& r) {
		return p( l.*mem, r.*mem );
	}
	bool operator()(const T& l, const T& r)  const {
		return p( l.*mem, r.*mem );
	}
};
} // namespace detail

//-------------------------------------------------
// order_mem
// For use with >>= operator
// throws InvalidArg() if obj is initialized at the time evaluation
//-------------------------------------------------
template<typename T, typename Mem, typename Pred>
struct OrderMem_tlr : public OrderByBase<T,detail::CompareMember<T,Mem,Pred> > {
	typedef OrderByBase<T,detail::CompareMember<T,Mem,Pred> > BaseClass;
	OrderMem_tlr(const lref<T>& obj, Mem T::* mem, Pred p) : BaseClass(obj, detail::CompareMember<T,Mem,Pred>(mem,p))
	{}
};

template<typename T, typename T2, typename MemberT2>  inline
OrderMem_tlr<T,MemberT2,std::less<MemberT2> > 
order_mem(lref<T>& obj, MemberT2 T2::* mem) {
	return OrderMem_tlr<T,MemberT2,std::less<MemberT2> >(obj,mem,std::less<MemberT2>());
}

template<typename T, typename T2, typename MemberT2, typename Pred>  inline
OrderMem_tlr<T,MemberT2,Pred> 
order_mem(lref<T>& obj, MemberT2 T2::* mem, Pred cmp) {
	return OrderMem_tlr<T,MemberT2,Pred>(obj,mem,cmp);
}

//-------------------------------------------------
// reverse(obj)
// For use with >>= operator
// throws InvalidArg() if obj is initialized at the time evaluation
//-------------------------------------------------
template<typename T>
struct Reverse_tlr : public Coroutine {
	lref<T> obj;
	std::vector<T> v;
	typename std::vector<T>::reverse_iterator itr;

	explicit Reverse_tlr(const lref<T>& obj) : obj(obj)
	{}

	bool operator() (relation& r) {
		co_begin();
		if(obj.defined())
			throw InvalidArg();
		if(!r())
			co_return(false);
		do {
			v.push_back(*obj);
		} while(r());

		for(itr=v.rbegin(); itr!=v.rend(); ++itr) {
			obj.set_ptr(&*itr, false);
			co_yield(true);
		}
        obj.reset();
        co_end();
	}
};

template<typename T>  inline
Reverse_tlr<T> reverse(lref<T>& obj) {
	return Reverse_tlr<T>(obj);
}


//-------------------------------------------------
// group_by(i, selector).then(..).then(..).item_order(..) - group seq using cond into (key,grp)
//-------------------------------------------------

namespace detail {

template<int N, class R>
struct nth {
	template<class T1, class T2>
	static R& key(std::pair<T1,T2>& p) {
		return nth<N-1,R>::key(p.second);
	}
};

template<class R>
struct nth<0,R> {
	template<class T>
	static T& key(T & p) {
		return p;
	}
	template<class T1, class T2>
	static T1& key(std::pair<T1,T2>& p) {
		return p.first;
	}
};

// Functor returns the v[index].first, where v is a vector<pair> v
template<class T>
class nthValue {
	lref<std::vector<T> > v;
public:
	typedef typename T::first_type& result_type;

	explicit nthValue(lref<std::vector<T> >& v) : v(v)
	{ }

	result_type operator()(typename std::vector<T>::size_type index) {
		return ((*v)[index]).first;
	}
};

template<class T>
class group_iterator {
	Func1<T&,size_t> getV;
	size_t curr;
public:
	typedef std::random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef size_t difference_type;
	typedef size_t distance_type;

	group_iterator(const Func1<T&,size_t>& getV, size_t curr) : getV(getV), curr(curr)
	{ }
	
	group_iterator(const group_iterator& rhs) : getV(rhs.getV), curr(rhs.curr)
	{ }

	group_iterator& operator =(const group_iterator& rhs) {
		curr=rhs.curr;
		getV=rhs.getV;
		return *this;
	}

	T& operator *() {
		return getV(curr);
	}

	T* operator ->() {
		return (&**this);
	}

	group_iterator& operator ++() {
		++curr;
		return *this;
	}

	group_iterator operator ++(int) {
		group_iterator tmp(*this);
		++curr;
		return tmp;
	}

	group_iterator& operator --() {
		--curr;
		return *this;
	}

	group_iterator operator --(int) {
		group_iterator tmp(*this);
		--curr;
		return tmp;
	}

	group_iterator& operator +=(difference_type amt) {
		curr+=amt;
		return *this;
	}

	group_iterator& operator -=(difference_type amt) {
		curr-=amt;
		return *this;
	}

	group_iterator operator +(difference_type amt) const {
		group_iterator tmp(*this);
		return tmp+=amt;
	}
    
    group_iterator operator -(difference_type amt) const {
		group_iterator tmp(*this);
		return tmp-=amt;
	}

	distance_type operator -(const group_iterator& rhs) const {
        return (rhs.curr<curr) ? (curr-rhs.curr) : (rhs.curr-curr);
	}

    const value_type& operator[](difference_type offset) const {
		return getV(curr+offset);
	}

	//todo: consider deep comparison (i.e. ensure referring to same vector)
	bool operator ==(const group_iterator& rhs) const {
		return curr==rhs.curr;
	}

	bool operator !=(const group_iterator& rhs) const {
		return !(*this==rhs);
	}

	bool operator >(const group_iterator& rhs) const {
		return curr>rhs.curr;
	}

	bool operator >=(const group_iterator& rhs) const {
		return curr>=rhs.curr;
	}

	bool operator <(const group_iterator& rhs) const {
		return curr<rhs.curr;
	}

	bool operator <=(const group_iterator& rhs) const {
		return curr<=rhs.curr;
	}
};

} // namespace detail

//------------------------------------------

template<class K, class V>
struct group {
	typedef K key_type;
	typedef V value_type;
	typedef detail::group_iterator<V> iterator;
    typedef typename std::vector<value_type>::size_type size_type;

    K key;
private:
	size_t first;
	size_t last;
	detail::Func1<V&,size_t> getV;

	template<class, class, class , class , class, class> friend class GroupBy;
	template<class, class, class , class , class> friend class GroupBy2;

	template<class, class> friend struct group;

	group(const K& key, size_t first, size_t last, detail::Func1<V&,size_t> getV) : key(key), first(first), last(last), getV(getV)
	{ }
public:
	
	size_t size() const {
		return last-first;
	}

	bool empty() const {
		return size()==0;
	}

	bool operator==(const group& rhs) const {
		return key==rhs.key && first==first && last==last;
	}

	iterator begin() const {
		return iterator(getV,first);
	}

	iterator end() const {
		return iterator(getV,last);
	}

#ifdef __GNUG__
	template<int N, class ElementT>
	struct GCC {
#else
	template<int N, class ElementT>
#endif
	static group get_group(lref<std::vector<ElementT> >& v, size_t first, size_t last) {
		using namespace detail;
		if( first>=last || v->size()<last)
			throw IndexOutOfBounds();

		size_t end1=0;
		std::vector<ElementT>& rvec = v.get();
		for(end1=first+1; end1<last; ++end1) {
			if( nth<N,K>::key(rvec[end1]) != nth<N,K>::key(rvec[first]) ) {
				break;
			}
		}
		return group( nth<N,K>::key((*v)[first]), first, end1, nthValue<ElementT>(v) );
	}
#ifdef __GNUG__
	};
#endif
};

template<class K, class V, class T>
struct group<group<K,V>, T >; // first template argument cannot be group<>


template<class K, class K2, class V2>
struct group<K, group<K2,V2> > {
	typedef K key_type;
	typedef group<K2,V2> value_type;
	typedef typename std::vector<value_type>::iterator iterator;
    typedef typename std::vector<value_type>::size_type size_type;
	K key;
private:
	lref<std::vector<value_type> > subgroups;
	size_t first;
	size_t last;

	template<class, class, class, class, class, class> friend class GroupBy;
	template<class, class, class , class , class> friend class GroupBy2;
	template<class, class> friend struct group;

    group(const K& key, lref<std::vector<value_type> >& subgroups, size_t first, size_t last/*, detail::Func1<value_type&,size_t> getV*/) : key(key), subgroups(subgroups), first(first), last(last)
	{ }
public:

	size_type size() const {
		return last-first;
	}

	bool empty() const {
		return first==last;
	}

	bool operator==(const group& rhs) const {
		return key==rhs.key && first==first && last==last && *subgroups==*rhs.subgroups;
	}

	iterator begin() const {
		return subgroups->begin();
	}

	iterator end() const {
		return subgroups->end();
	}
private:
#ifdef __GNUG__
	template<int N, class T>
	struct GCC {
#else
	template<int N, class T>
#endif
	static group get_group(lref<std::vector<T> >& v, size_t first, size_t last) {
		using namespace detail;
		if( first>=last || v->size()<last)
			throw IndexOutOfBounds();
		std::vector<T>& rvec = v.get();
		size_t end1=0;
		for(end1=first+1; end1!=last; ++end1) {
			if( nth<N,K>::key(rvec[end1]) != nth<N,K>::key(rvec[first]) )
				break;
		}
		lref<std::vector<value_type> > subgroups(new std::vector<value_type>(), true);
		for(size_t first2=first; first2<end1; first2=subgroups->rbegin()->last )
#ifdef __GNUG__
			subgroups->push_back( value_type::template GCC<N+1,T>::get_group(v, first2, end1) );
#else
			subgroups->push_back( value_type::get_group<N+1,T>(v, first2, end1) );
#endif

		return group( nth<N,K>::key(rvec[first]), subgroups, first, end1);
	}
#ifdef __GNUG__
	};
#endif
};

//------------------------------------------
namespace detail {

//FuncList: FuncList<selector1, FuncList<selector2, FuncList<selector3, FuncList<selector4, None> > > >
template<class Func, class Inner>
struct FuncList;

template<class Func>
struct FuncList<Func,None> {
	typedef typename return_type<Func>::result_type result_type;
	Func selector;

	explicit FuncList(Func f) : selector(f)
	{ }

	template<class Obj>
	result_type
	operator()(Obj& obj) {
		return selector(obj);
	}
};

template<class Func, class Func2>
struct FuncList<Func, FuncList<Func2,None> > {
	typedef FuncList<Func2,None> Inner;
	typedef std::pair<typename return_type<Func>::result_type, typename Inner::result_type> result_type;
	Func selector;
	Inner inner;

	FuncList(FuncList<Func,None> flist, Func2 lastF) : selector(flist.selector), inner(lastF)
	{ }

	template<class Obj>
	result_type operator()(Obj& obj) {
		return std::make_pair(selector(obj),inner(obj));
	}
};

template<class Func, class Func2, class Inner2>
struct FuncList <Func, FuncList<Func2,Inner2> > {
	typedef FuncList<Func2,Inner2> Inner;
	typedef std::pair<typename return_type<Func>::result_type, typename Inner::result_type> result_type;
	Func selector;
	Inner inner;

	template<class NewFunc, class OldInner>
	FuncList(FuncList<Func,OldInner> flist, NewFunc lastF) : selector(flist.selector), inner(flist.inner, lastF)
	{ }

	template<class Obj>
	result_type operator()(Obj& obj) {
		return std::make_pair(selector(obj),inner(obj));
	}
};

//--------------------------------------

template<class FList, class NewFunc>
struct AddFunc;

template<class Func, class Func2>
struct AddFunc<FuncList<Func,None>, Func2> {
	typedef FuncList<Func, FuncList<Func2,None> > result_type;
};

template<class Func, class Inner, class Func2>
struct AddFunc<FuncList<Func,Inner>, Func2> {
	typedef FuncList<Func, typename  AddFunc<Inner,Func2>::result_type> result_type;
};


//------------------------------------------

//FuncList: NestedCmp<cmp1, NestedCmp<cmp2, NestedCmp<cmp3,cmp4> > >
template<class Cmp, class Inner>
struct NestedCmp;

template<class Cmp1, class Cmp2, class Inner>
struct NestedCmp<NestedCmp<Cmp1,Cmp2>, Inner>;

template<class Cmp1, class Cmp2>
struct NestedCmp {
	typedef bool result_type;
	Cmp1 cmp;
	Cmp2 inner;

	NestedCmp(Cmp1 cmp, Cmp2 inner) : cmp(cmp), inner(inner)
	{ }

	template<class T>
	bool operator()(const T& lhs, const T& rhs) {
		if(cmp(lhs.first,rhs.first))
			return true;
		if(cmp(rhs.first,lhs.first))
			return false;

		return inner(lhs.second,rhs.second);
	}
};


template<class Cmp, class Cmp2, class Cmp3>
struct NestedCmp<Cmp, NestedCmp<Cmp2,Cmp3> > {
	typedef bool result_type;
	typedef NestedCmp<Cmp2,Cmp3> Inner;
	Cmp cmp;
	Inner inner;

	template<class NewCmp, class OldInner>
	NestedCmp(NestedCmp<Cmp,OldInner> flist, NewCmp lastCmp) : cmp(flist.cmp), inner(flist.inner, lastCmp)
	{ }

	template<typename T>
	bool operator()(const T& lhs, const T& rhs) {
		if(cmp(lhs.first,rhs.first))
			return true;
		if(cmp(rhs.first,lhs.first))
			return false;
		return inner(lhs.second,rhs.second);
	}
};


//--------------------------------------
template<class CmpList, class NewCmp>
struct AddCmp;

template<class T, class Cmp1, class Cmp2>
struct AddCmp<T, NestedCmp<Cmp1,Cmp2> >;

template<class Cmp1, class Cmp2>
struct AddCmp {
	typedef NestedCmp<Cmp1, Cmp2> result_type;
};

template<class Cmp1, class Cmp2, class Cmp3>
struct AddCmp<NestedCmp<Cmp1,Cmp2>, Cmp3> {
	typedef NestedCmp<Cmp1, typename AddCmp<Cmp2,Cmp3>::result_type> result_type;
};


//------------------------------------------
template<class KeyCmp, class ValCmp>
struct GroupElemCmp {
	KeyCmp keyCmp;
	ValCmp valCmp;
	GroupElemCmp(KeyCmp keyCmp, ValCmp valCmp) : keyCmp(keyCmp), valCmp(valCmp)
	{ }

	template<class T>
	bool operator ()(const T& lhs, const T& rhs) {
		if(keyCmp(lhs.second,rhs.second))
			return true;
		if(keyCmp(rhs.second,lhs.second))
			return false;
		return valCmp(lhs.first,rhs.first);
	}
};

template<class KeyCmp>
struct GroupElemCmp<KeyCmp,None> {
	KeyCmp keyCmp;
	GroupElemCmp(KeyCmp keyCmp, None) : keyCmp(keyCmp)
	{ }

	template<class T>
	bool operator ()(const T& lhs, const T& rhs) {
		return keyCmp(lhs.second,rhs.second);
	}
};

//------------------------------------------
//ElementType:  pair<Value, pair<Key1, pair<Key2, pair<Key3,Key4> > > >

template<class Group>
struct nested_keys;

template<class K, class V>
struct nested_keys<group<K,V> > {
	typedef K result_type;
};

template<class K1, class K2, class V2>
struct nested_keys<group<K1, group<K2,V2> > > {
	typedef std::pair<K1, typename nested_keys<group<K2,V2> >::result_type > result_type;
};


} //namespace detail

//------------------------------------------

template<class Item, class Grp, class K, class V, class Sel, class KCmp>
class GroupBy {
	lref<Item> i;
	Sel selectors;
	lref<Grp> g;
	KCmp keyCmps;
	
	typedef std::pair<Item, typename detail::nested_keys<Grp>::result_type> ElementType;
	lref<std::vector<ElementType> > elements;
public:	
	GroupBy(lref<Item> i, Sel selectors, lref<Grp> g, KCmp keyCmps) : i(i), selectors(selectors), g(g), keyCmps(keyCmps)
	{ }

	template<class Sel2>
	GroupBy<Item,Grp,K,typename V::value_type,typename detail::AddFunc<Sel,Sel2>::result_type, typename detail::AddCmp<KCmp,std::less<typename V::key_type> >::result_type>
    then(Sel2 keySelector) {
		typedef typename detail::AddFunc<Sel,Sel2>::result_type NewSelector;
		typedef typename detail::AddCmp<KCmp,std::less<typename V::key_type> >::result_type NewComparator;
		return GroupBy<Item,Grp,K,typename V::value_type,NewSelector,NewComparator>(i, NewSelector(selectors,keySelector), g, NewComparator(keyCmps, std::less<typename V::key_type>()));
	}

	template<class Sel2, class KCmp2>
	GroupBy<Item,Grp,K,typename V::value_type,typename detail::AddFunc<Sel,Sel2>::result_type, typename detail::AddCmp<KCmp,KCmp2>::result_type>
    then(Sel2 keySelector, KCmp2 keyCmp) {
		typedef typename detail::AddFunc<Sel,Sel2>::result_type NewSelector;
		typedef typename detail::AddCmp<KCmp,KCmp2>::result_type NewComparator;
		return GroupBy<Item,Grp,K,typename V::value_type,NewSelector,NewComparator>(i, NewSelector(selectors,keySelector), g, NewComparator(keyCmps, keyCmp));
	}
};

template<class Item, class Grp, class Sel, class KCmp, class ICmp>
class GroupBy2;

template<class Item, class Grp, class K, class Sel, class KCmp>
class GroupBy<Item, Grp, K, Item, Sel, KCmp> : public Coroutine {
	lref<Item> i;
	Sel selectors;
	lref<Grp> g;
	KCmp keyCmps;
	typedef std::pair<Item, typename detail::nested_keys<Grp>::result_type> ElementType;

	lref<std::vector<ElementType> > elements;
	typename std::vector<ElementType>::size_type first;
public:
	GroupBy(lref<Item> i, Sel selectors, lref<Grp> g, KCmp keyCmps) : i(i), selectors(selectors), g(g), keyCmps(keyCmps), first(0)
	{ }

	template<class ICmp>
	GroupBy2<Item,Grp,Sel,KCmp,ICmp>
    item_order(ICmp itemCmp) {
		return GroupBy2<Item,Grp,Sel,KCmp,ICmp>(i, selectors, g, keyCmps, itemCmp);
	}

	bool operator()(relation& rhs) {
		co_begin();
		if(g.defined() || i.defined() )
            throw InvalidArg();
		elements.set_ptr(new std::vector<ElementType>(), true);
		while(rhs())
			elements->push_back(std::make_pair(*i,selectors(*i)));
		if(elements->empty())
			co_return(false);

		typedef detail::GroupElemCmp<KCmp,detail::None> GCmp;
		std::sort(elements->begin(), elements->end(), GCmp(keyCmps,detail::None()));
 
		for(; first<elements->size(); first = g->last) {
#ifdef __GNUG__
			g = Grp::template GCC<1,ElementType>::get_group(elements, first, elements->size());
#else
			g = Grp::get_group<1>(elements, first, elements->size());
#endif
			co_yield(true);
		}
		g.reset();
		co_end();
	}
};

template<class Item, class Grp, class Sel, class KCmp, class ICmp>
class GroupBy2 : public Coroutine {
	lref<Item> i;
	Sel selectors;
	lref<Grp> g;
	KCmp keyCmps;
	ICmp itemCmp;
	typedef std::pair<Item, typename detail::nested_keys<Grp>::result_type> ElementType;

	lref<std::vector<ElementType> > elements;
	typename std::vector<ElementType>::size_type first;
public:
	GroupBy2(lref<Item> i, Sel selectors, lref<Grp> g, KCmp keyCmps, ICmp itemCmp) : i(i), selectors(selectors), g(g), keyCmps(keyCmps), itemCmp(itemCmp), first(0)
	{ }

	bool operator()(relation& rhs) {
		co_begin();
		if( g.defined()|| i.defined() )
			throw InvalidArg();
		elements.set_ptr(new std::vector<ElementType>(), true);
		while(rhs())
			elements->push_back(std::make_pair(*i,selectors(*i)));
		if(elements->empty())
			co_return(false);

		typedef detail::GroupElemCmp<KCmp,ICmp> GCmp;
		std::sort(elements->begin(), elements->end(), GCmp(keyCmps,itemCmp));
 
		for(; first<elements->size(); first = g->last) {
#ifdef __GNUG__
			g = Grp::template GCC<1,ElementType>::get_group(elements, first, elements->size());
#else
			g = Grp::get_group<1>(elements, first, elements->size());
#endif
			co_yield(true);
		}
		g.reset();
		co_end();
	}
};

//-------------------------------------------------
// group_by().then().then().item_order() - Multilevel grouping of objects
// For use with >>= operator
// throws InvalidArg() if obj is initialized at the time evaluation
// Concept: T: is an integral type
//-------------------------------------------------
template<class Item, class Sel, class K, class V>
GroupBy<Item,group<K,V>,K,V,detail::FuncList<Sel,detail::None>, std::less<K> > inline
group_by(lref<Item>& i_, Sel keySelector, lref<group<K,V> >& g) {
	using namespace detail;
	typedef typename return_type<Sel>::result_type ret_type;
	//ASSERT_SAME_TYPE(ret_type,K,"Group's key type does not match Selector's return type");
	return GroupBy<Item,group<K,V>,K,V,FuncList<Sel,None>,std::less<K> >(i_,FuncList<Sel,None>(keySelector),g, std::less<K>());
}

//-------------------------------------------------
// with keyCmp support
template<class Item, class Sel, class K, class V, class KCmp>
GroupBy<Item,group<K,V>, K,V,detail::FuncList<Sel,detail::None>,KCmp> inline
group_by(lref<Item>& i_, Sel keySelector, lref<group<K,V> >& g, KCmp keyCmp) {
	using namespace detail;
	typedef typename return_type<Sel>::result_type ret_type;
	//ASSERT_SAME_TYPE(ret_type,K,"Group's key type does not match Selector's return type");
	return GroupBy<Item,group<K,V>,K,V,FuncList<Sel,None>,KCmp>(i_,FuncList<Sel,None>(keySelector),g,keyCmp);
}


//-------------------------------------------------
// skip(n) - Skip first n solutions
//-------------------------------------------------

template<class Integral>
class Skip_tlr : public Coroutine {
	Integral n;
public:
	Skip_tlr (const Integral& n) : n(n)
	{ }

	bool operator()(relation& lhs) {
		co_begin();
        for( ; effective_value(n)>0; --effective_value(n) ) {
            if(!lhs())
                co_return(false);
        }
        while(lhs())
            co_yield(true);
		co_end();
	}
};

template<class Integral>
Skip_tlr<Integral> skip(Integral times) {
    return Skip_tlr<Integral>(times);
}

} // namespace castor

#endif
