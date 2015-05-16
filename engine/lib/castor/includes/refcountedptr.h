// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#ifndef CASTOR_REFCOUNTEDPTR_H
#define CASTOR_REFCOUNTEDPTR_H 1

#include <algorithm>
#include "helpers.h"

namespace castor {	
	struct InvalidDeref{}; 
	struct Underflow{};
}

namespace castor {	namespace detail {

// IsConvertibleStrict<T1,T2>::result==true  if T1 is convertible to T2, and T1!=T2
#ifdef __BCPLUSPLUS__
namespace ICS_detail {
// BCB workaround: sizeof trick doesnt directly work in BCB. call to overloaded test() 
// inside sizeof()produces compiler error 
// These members actually belong inside IsConvertibleStrict.
    class Big { char dummy[2]; };
    template<typename T1> T1 MakeT1();
    typedef char Small;
    template<typename T2> Small test(T2);
    template<typename T2> Big test(...);
};

// IsConvertibleStrict<T1,T2>::result==true  if T1 is convertible to T2, and T1!=T2
template <typename T1, typename T2>
class IsConvertibleStrict {
public:
	bool static const result = (sizeof(ICS_detail::Small)==sizeof( ICS_detail::test<T2>(ICS_detail::MakeT1<T1>()) ) )
				 && !SameType<T1,T2>::result ;
};
#else // __BCPLUSPLUS__
template <typename T1, typename T2>
class IsConvertibleStrict {
	static T1 MakeT1();
	typedef char Small;
    class Big { char dummy[2]; };
	static Small test(T2);
	static Big test(...);
public:
	enum { result = (sizeof(Small)==sizeof( test(MakeT1()) ) )
                 && !SameType<T1,T2>::result 
         };
};
#endif // __BCPLUSPLUS__

// IsDerivedStrict<T1,T2>::result==true  if T1 is derived from T2 and T1!=T2
template <typename D, typename B>
struct IsDerivedStrict {
	enum { result = IsConvertibleStrict<const D*, const B*>::result
                && !SameType<const D*, const void*>::result 
        };
};


// Class RefCountedPtr
//-------------------------------------------------------

// alloc_ should have been (as originally implemented in v0.8) a nested template inside RefCountedPtr<T>
// such that T in alloc<T>_ is same as T in RefCountedPtr. But BCB compiler couldnt handle it
template<typename T, bool allocateT2> struct alloc_;
template<typename T>
struct alloc_<T, true> {
    template<typename T2>
    static T* exec( const T2& value ) { return new typename RemoveConst<T2>::result_type(value); }
};

template<typename T>
struct alloc_<T, false> {
    template<typename T2>
    static T* exec( const T2& value ) {
//        typedef typename RemoveConst<T>::result_type NonConstT;
        return new T(value);
    }
};

template<typename T>
class RefCountedPtr {
protected:
    T* ptr_;
	unsigned long count_;
    bool reset_, exactType_, manage_;

    RefCountedPtr(const RefCountedPtr&);
    RefCountedPtr& operator =(const RefCountedPtr&);

    template<typename T2>
    static T* initPtr(const T2& value) {
        return detail::alloc_<T, IsDerivedStrict<T2,T>::result> ::exec(value); // return new T2(value);
    }
public:
    typedef T pointee_type;
    RefCountedPtr() : ptr_(0), count_(1), reset_(true), exactType_(false), manage_(true)
    { }

    explicit RefCountedPtr(const T& value) : ptr_(new T(value)), count_(1), reset_(false), exactType_(true), manage_(true)
    { }

	explicit RefCountedPtr(T* ptr, bool manage) : ptr_(ptr), count_(1), reset_(false), exactType_(true), manage_(manage)
    { }

	template<typename T2>
    explicit RefCountedPtr(const T2& value) : ptr_(initPtr(value)), count_(1), reset_(false), exactType_(!IsDerivedStrict<T2,T>::result), manage_(true)
    { }

    ~RefCountedPtr() {
       if(ptr_ && manage_) 
		   delete ptr_;
    }

    // Here we avoid invoking 'new' as far as possible for efficiency reasons
    // The new value is assigned to an existing T object pointed to by ptr_
    // If ptr_==0 then we invoke new.
    // TODO: how well do set(T) and set(X) mix
    void set(const T& value) {
        if(exactType_) {
            if(manage_)
                *ptr_ = value;
            else
                ptr_ = new T(value);
        }
        else {
			T* temp = new T(value);
            if(ptr_ && manage_) 
				delete ptr_;
            ptr_= temp;
        }
        reset_=false;
        exactType_=true;
		manage_=true;
    }

    template<typename T2>
    void set(const T2& value) {
		T* temp = initPtr(value);
        if(ptr_ && manage_)
            delete ptr_;
        ptr_= temp;
        reset_=false;
        exactType_=!IsDerivedStrict<T2,T>::result;
		manage_=true;
    }

	void set_ptr(T* ptr, bool manage) {
        if(ptr_ && manage_) 
			delete ptr_;
        ptr_= ptr;
        reset_=false;
        exactType_=true;
		manage_=manage;
	}

	template<typename T2>
    void set_ptr(T2* ptr, bool manage) {
        if(ptr_ && manage_)
			delete ptr_;
        ptr_= ptr;
        reset_=false;
        exactType_=true;
		manage_=manage;		
    }

    void reset() { // nothrow
        reset_=true;
    }

    T& operator*() const {
        if(reset_) throw InvalidDeref();
        return *ptr_;
    }

    T* operator->() const {
        if(reset_) throw InvalidDeref();
        return ptr_;
    }

    T* get() const {
        return ptr_;
    }

    void inc() { // nothrow
        ++count_; 
    }

    unsigned long dec() {
        if(count_==0)
            throw Underflow();
        return --count_;
    }

    unsigned long use_count() const { // nothrow
        return count_;
    }

    bool defined() const {  // nothrow
        return !reset_;
    }

    void swap(RefCountedPtr& other) { // no throw
        ::std::swap(ptr_,other.ptr_);
        ::std::swap(reset_,other.reset_);
        ::std::swap(exactType_,other.exactType_);
    }
}; // class RefCountedPtr


} } // namespace castor::detail

#endif
