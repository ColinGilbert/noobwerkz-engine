// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_REF_H
#define CASTOR_REF_H 1

#include "config.h"
#include "relation.h"
#include "refcountedptr.h"


namespace castor {


// Note:  lref::operator=(), lref.reset() will more or less always be called on lref objects whose reference count>=2. 
// So calling release() in reset will rarely ever invoke 'delete refPtr'. So inefficiency of disposing
// and creating a new RefCountedPtr between repeated invocations of reset() and set() is not a significant issue is most cases.
// At the same time for safety reasons we do check lref counts and delete refPtr if needed, for those rare
// cases where user just tries to call set() and reset() on a lref object having reference count=1

// concept: T is copy constructible
template <typename T>
class lref {
    detail::RefCountedPtr<T>* const refPtr;
	
public:
  typedef T result_type;

  //------ Constructors and Destructor
  lref() : refPtr(new detail::RefCountedPtr<T>)
  { }

  lref(const T& value) : refPtr(new detail::RefCountedPtr<T>(value))  
  { }

  // concept : T provides T::T(const T2&)
  template<typename T2>
  lref(const T2& value) : refPtr(new detail::RefCountedPtr<T>(value))
  { }

  lref(const lref<T>& rhs) : refPtr( incrementedRef(rhs.refPtr) )
  { }

private:
	template<typename T2> lref(const lref<T2>& rhs);

public:
  lref(T* ptr, bool manage) : refPtr(new detail::RefCountedPtr<T>(ptr,manage)) 
  { }

  ~lref() {
      if(refPtr->dec()==0) delete refPtr;
  }

  //------ Assignment
  lref& operator =(const T& newValue) {
      refPtr->set(newValue);
      return *this;
  }

  template<typename T2>
  lref& operator=(const T2& newValue) {
      refPtr->set(newValue);
      return *this;
  }

  // copies the value contained in rhs
  lref& operator=(const lref& rhs) {
	  if(rhs.defined())
		  refPtr->set(*rhs);
	  else
		  reset();
      return *this;
  }

  template<typename T2>
  lref& operator=(const lref<T2>& rhs) {
	  if(rhs.defined())
		  refPtr->set(*rhs);
	  else
		  reset();
      return *this;
  }

  template<typename T2>
  void set_ptr(T2* ptr, bool manage) {
	  refPtr->set_ptr(ptr, manage);	
  }

  //------ Checked access
  T& operator *() {
#ifndef LREF_QUICKACCESS
    return **refPtr;
#else
    return *refPtr->get();
#endif
  }

  const T& operator *() const {     
#ifndef LREF_QUICKACCESS
    return **refPtr;
#else
    return *refPtr->get();
#endif
  }

  detail::RefCountedPtr<T>& operator ->() {
      return *refPtr;
  }

  const detail::RefCountedPtr<T>& operator ->() const {
      return *refPtr;
  }

  //------ Unchecked access
  T& get() {
      return *refPtr->get();
  }

  const T& get() const {
      return *refPtr->get();
  }

  //------ Other
  bool defined() const {  // nothrow
      return refPtr->defined();
  }

  void reset() {   // nothrow
      refPtr->reset();
  }

  unsigned int use_count() const {  //nothrow
      return refPtr->use_count();
  }
  
  void swap(lref& other) { // no throw
      refPtr->swap(*other.refPtr);
  }

  bool bound(const lref& rhs) const { // no throw
	  return refPtr==rhs.refPtr;
  }

  //operator T() const {
  //    return operator *();
  //}

private:
  static detail::RefCountedPtr<T>* incrementedRef(detail::RefCountedPtr<T>* ptr) { // nothrow
      ptr->inc();
      return ptr;
  }

  //lref& operator=(const T&);
  //lref& operator=(const lref&);

}; // class lref

template<>
class lref<void>; // Disable instantiation of lref<void>

template<typename T>
class lref<T&>; // Disable instantiations of lref<T&>

// swap underlying values
template<typename T> inline 
void swap(lref<T>& l, lref<T>& r) {
    l.swap(r);
}


// Helper function overloads to abstract away details of acquiring the value of lref<T>, or other T
// This helps in avoiding multiple specializations of the operator classes that perform computation on lref<>

template <typename T> inline
T& effective_value(T& obj) {
    return obj;
}

template <typename T> inline
T& effective_value(lref<T>& obj) {
    return *obj;
}

// meta function effective_type

template<typename T>
struct effective_type {
    typedef T result_type;
};

template<typename T>
struct effective_type<lref<T> > {
    typedef typename lref<T>::result_type result_type;
};


namespace detail {
template<typename Itr>
struct Pointee {
    typedef typename Itr::value_type result_type;
};

template<typename T>
struct Pointee<T*> {
    typedef T result_type;
};

template<typename T>
struct Pointee<lref<T*> > {
    typedef T result_type;
};

template<typename Itr>
struct Pointee<lref<Itr> > {
    typedef typename Itr::value_type result_type;
};

}  // namespace detail
} // namespace castor

#endif
