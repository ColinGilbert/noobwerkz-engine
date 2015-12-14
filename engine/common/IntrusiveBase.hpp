#pragma once


#include <boost/intrusive_ptr.hpp>

template<typename T>
inline void intrusive_ptr_add_ref(T* expr){
	++expr->references;
}

template<typename T>
inline void intrusive_ptr_release(T* expr){
	if(--expr->references == 0)
		delete expr;
}


//static inline void instrusive_ptr_add_ref

/*
#include "boost/intrusive_ptr.hpp"
// forward declarations
class intrusive_base;
namespace boost
{
void intrusive_ptr_add_ref(intrusive_base * p);
void intrusive_ptr_release(intrusive_base * p);
};

//namespace noob
//{
// My Class
class intrusive_base
{
private:
size_t references;
friend void boost::intrusive_ptr_add_ref(intrusive_base * p);
friend void boost::intrusive_ptr_release(intrusive_base * p);

public:
intrusive_base() : references(0) {}   // initialize references to 0
// virtual ~intrusive_base();
};

// class specific addref/release implementation
// the two function overloads must be in the boost namespace on most compilers:
namespace boost
{
inline void intrusive_ptr_add_ref(intrusive_base * p)
{
// increment reference count of object *p
++(p->references);
}



inline void intrusive_ptr_release(intrusive_base * p)
{
// decrement reference count, and delete object when reference count reaches 0
if (--(p->references) == 0)
delete p;
} 
} // namespace boost
//}
//
*/
