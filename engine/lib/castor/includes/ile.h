// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_ILE_H
#define CASTOR_ILE_H 1

#include "relation.h"
#include "lref.h"
#include "ops.h"
#include "cut.h"
#include "workaround.h"
#include "helpers.h"

namespace castor {
//-------------------------------------------------------------------------
// Creation of 'Expression templates' from expressions involving lref<>
//-------------------------------------------------------------------------
template <typename Expr>
struct Ile {
    Expr expr;

    typedef typename Expr::result_type result_type;
    Ile(const Expr& expr) : expr(expr)
    { }

    result_type operator()(void) {
        return expr();
    }

    result_type operator()(void) const {
        return expr();
    }
};


template <class T>
struct Value_ILE {
    typedef T result_type;
    T val;
    Value_ILE(const T& val) : val(val)
    { }

private:
    Value_ILE(const lref<T>& val);
};

namespace detail {
    
    template <typename T> inline
    typename T::result_type evaluate(T& t) {
        return t();
    }

    template <typename T> inline
    T& evaluate(Value_ILE<T>& t){
        return t.val;
    }

    template <typename T> inline
    typename lref<T>::result_type& evaluate(lref<T>& lr) {
        return *lr;
    }

    //template <typename T> inline
    //    const typename lref<T>::result_type& evaluate(const lref<T>& lref) {
    //    return *lref;
    //}

} //namespace detail


#define LREF_BINARYOP_CLASS_DECL(OPERATOR, OP_CLASS, RESULT)              \
template <typename L, typename R>                                         \
struct OP_CLASS {                                                         \
    typedef RESULT result_type;                                           \
    L left;                                                               \
    R right;                                                              \
                                                                          \
    OP_CLASS(const L& left, const R& right) : left(left), right(right)    \
    { }                                                                   \
                                                                          \
    result_type operator()(void) {                                        \
    return detail::evaluate(left) OPERATOR detail::evaluate(right);       \
    }                                                                     \
                                                                          \
    result_type operator()(void) const {                                  \
        return detail::evaluate(left) OPERATOR detail::evaluate(right);   \
    }                                                                     \
};

#define LREF_BINARYOP_FUNC_DECL(OPERATOR, OP_CLASS)                      \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<lref<L>, Value_ILE<R> > >                                   \
operator OPERATOR (lref<L>& left, const R& right)  {                     \
    return OP_CLASS<lref<L>, Value_ILE<R> >(left,right);                 \
}                                                                        \
                                                                         \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<lref<L>, Value_ILE<R*> > >                                  \
operator OPERATOR (lref<L>& left, R* right)  {                           \
    return OP_CLASS<lref<L>, Value_ILE<R*> >(left,right);                \
}                                                                        \
                                                                         \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<Value_ILE<L>, lref<R> > >                                   \
operator OPERATOR (const L& left, lref<R>& right)  {                     \
    return OP_CLASS<Value_ILE<L>, lref<R> >(left,right);                 \
}                                                                        \
                                                                         \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<Value_ILE<L>, lref<R> > >                                   \
operator OPERATOR (L* left, lref<R>& right)  {                           \
    return OP_CLASS<Value_ILE<L*>, lref<R> >(left,right);                \
}                                                                        \
                                                                         \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<lref<L>, lref<R> > >                                        \
operator OPERATOR (lref<L>& left, lref<R>& right)  {                     \
    return OP_CLASS<lref<L>, lref<R> >(left,right);                      \
}                                                                        \
                                                                         \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<L, Value_ILE<R> > >                                         \
operator OPERATOR (const Ile<L>& left, const R& right)  {                \
	return OP_CLASS<L, Value_ILE<R> >(left.expr,right);                  \
}                                                                        \
														                 \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<L, Value_ILE<R*> > >                                        \
operator OPERATOR (const Ile<L>& left, R* right)  {                      \
	return OP_CLASS<L, Value_ILE<R*> >(left.expr,right);                 \
}                                                                        \
														                 \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<Value_ILE<L>, R> >                                          \
operator OPERATOR (const L& left, const Ile<R>& right)  {                \
	return OP_CLASS<Value_ILE<L>, R>(left,right.expr);                   \
}                                                                        \
														                 \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<Value_ILE<L*>, R> >                                         \
operator OPERATOR (L* left, const Ile<R>& right)  {                      \
	return OP_CLASS<Value_ILE<L*>, R>(left,right.expr);                  \
}                                                                        \
																         \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<L,R> >                                                      \
operator OPERATOR (const Ile<L>& left, const Ile<R>& right)  {           \
    return OP_CLASS<L, R>(left.expr,right.expr);                         \
}                                                                        \
                                                                         \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<L, lref<R> > >                                              \
operator OPERATOR (const Ile<L>& left, lref<R>& right)  {                \
	return OP_CLASS<L, lref<R> >(left.expr,right);                       \
}                                                                        \
																         \
template <typename L, typename R> inline                                 \
Ile<OP_CLASS<lref<L>, R> >                                               \
operator OPERATOR (lref<L>& left, const Ile<R>& right)  {                \
    return OP_CLASS<lref<L>,R>(left,right.expr);                         \
}


#define LREF_BINARYOP_DECL(OPERATOR, OP_CLASS, RESULT) \
LREF_BINARYOP_CLASS_DECL(OPERATOR, OP_CLASS, RESULT)   \
LREF_BINARYOP_FUNC_DECL(OPERATOR, OP_CLASS) 

namespace detail {

    template<class T>
    struct GetResult {
        typedef typename NonRef<typename T::result_type>::type type;
    };

}// namespace detail

LREF_BINARYOP_DECL(+,  Plus_ILE, typename detail::GetResult<L>::type)
LREF_BINARYOP_DECL(-,  Minus_ILE, typename detail::GetResult<L>::type)
LREF_BINARYOP_DECL(*,  Mult_ILE, typename detail::GetResult<L>::type)
LREF_BINARYOP_DECL(/,  Div_ILE, typename detail::GetResult<L>::type)
LREF_BINARYOP_DECL(%,  Mod_ILE, typename detail::GetResult<L>::type)
LREF_BINARYOP_DECL(|,  BitOr_ILE, typename detail::GetResult<L>::type)
LREF_BINARYOP_DECL(^,  BitExOr_ILE, typename detail::GetResult<L>::type)
LREF_BINARYOP_DECL(&,  BitAnd_ILE, typename detail::GetResult<L>::type)
LREF_BINARYOP_DECL(<<, ShiftLeft_ILE, typename detail::GetResult<L>::type&)
LREF_BINARYOP_DECL(>>, ShiftRight_ILE, typename detail::GetResult<L>::type&)

LREF_BINARYOP_DECL(==, Equal_ILE, bool)
LREF_BINARYOP_DECL(!=, UnEqual_ILE, bool)
LREF_BINARYOP_DECL(<,  Less_ILE, bool)
LREF_BINARYOP_DECL(>,  Greater_ILE, bool)
LREF_BINARYOP_DECL(<=, LessEqual_ILE, bool)
LREF_BINARYOP_DECL(>=, GreaterEqual_ILE, bool)
LREF_BINARYOP_DECL(&&, And_ILE, bool)
LREF_BINARYOP_DECL(||, Or_ILE, bool)


// Prefix unary operators come in two varieties: either they return a value(e.g -, +, !)
//  or they return a reference (e.g. ++,--)
#define LREF_PREFIX_UNARYOP_CLASS_DECL_RETREF(OPERATOR, OP_CLASS)                               \
template <typename Expr>                                                                        \
class OP_CLASS {                                                                                \
    Expr expr;                                                                                  \
public:                                                                                         \
    typedef typename detail::RefType<typename Expr::result_type>::result_type result_type;      \
                                                                                                \
    OP_CLASS(const Expr& expr) : expr(expr)                                                     \
    { }                                                                                         \
                                                                                                \
    result_type operator()(void) {                                                              \
        return OPERATOR (detail::evaluate(expr));                                               \
    }                                                                                           \
                                                                                                \
    result_type operator()(void) const  {                                                       \
        return OPERATOR (detail::evaluate(expr));                                               \
    }                                                                                           \
};

#define LREF_PREFIX_UNARYOP_CLASS_DECL_RETVAL(OPERATOR, OP_CLASS, RESULT)           \
template <typename Expr>                                                            \
class OP_CLASS {                                                                    \
    Expr expr;                                                                      \
public:                                                                             \
    typedef RESULT result_type;                                                     \
                                                                                    \
    OP_CLASS(const Expr& expr) : expr(expr)                                         \
    { }                                                                             \
                                                                                    \
    result_type operator()(void)  {                                                 \
        return OPERATOR (detail::evaluate(expr));                                   \
    }                                                                               \
    result_type operator()(void) const {                                            \
        return OPERATOR (detail::evaluate(expr));                                   \
    }                                                                               \
};


#define LREF_PREFIX_UNARYOP_FUNCS_DECL(OPERATOR, OP_CLASS)                          \
template <typename T> inline                                                        \
Ile<OP_CLASS<lref<T> > >                                                            \
operator OPERATOR (const lref<T>& obj) {                                            \
    return OP_CLASS<lref<T> >(obj);                                                 \
}                                                                                   \
                                                                                    \
template <typename T> inline                                                        \
Ile<OP_CLASS<T> >                                                                   \
operator OPERATOR (const Ile<T>& expr) {                                            \
    return OP_CLASS<T>(expr.expr);                                                  \
}


#define LREF_PREFIX_UNARYOP_DECL_RETREF(OPERATOR, OP_CLASS)                         \
LREF_PREFIX_UNARYOP_CLASS_DECL_RETREF(OPERATOR, OP_CLASS)                           \
LREF_PREFIX_UNARYOP_FUNCS_DECL(OPERATOR, OP_CLASS)

#define LREF_PREFIX_UNARYOP_DECL_RETVAL(OPERATOR, OP_CLASS, RESULT)                 \
LREF_PREFIX_UNARYOP_CLASS_DECL_RETVAL(OPERATOR, OP_CLASS, RESULT)                   \
LREF_PREFIX_UNARYOP_FUNCS_DECL(OPERATOR, OP_CLASS)



LREF_PREFIX_UNARYOP_DECL_RETVAL(+, Prefix_Plus_ILE, typename detail::GetResult<Expr>::type )
LREF_PREFIX_UNARYOP_DECL_RETVAL(-, Prefix_Minus_ILE, typename detail::GetResult<Expr>::type )
LREF_PREFIX_UNARYOP_DECL_RETVAL(~, Prefix_Complement_ILE, typename detail::GetResult<Expr>::type )
LREF_PREFIX_UNARYOP_DECL_RETVAL(!, Prefix_Not_ILE, bool)

LREF_PREFIX_UNARYOP_DECL_RETREF(++, Prefix_Inc_ILE)
LREF_PREFIX_UNARYOP_DECL_RETREF(--, Prefix_Dec_ILE)


#define LREF_POSTFIX_UNARYOP_CLASS_DECL(OPERATOR, OP_CLASS)                                      \
template <typename IleExpr>                                                                      \
class OP_CLASS {                                                                                 \
    IleExpr expr;                                                                                \
public:                                                                                          \
    typedef typename detail::NonRef<typename IleExpr::result_type>::type result_type;            \
                                                                                                 \
    OP_CLASS(const IleExpr& expr) : expr(expr)                                                   \
    { }                                                                                          \
                                                                                                 \
    result_type operator()(void) {                                                               \
        return (detail::evaluate(expr)) OPERATOR;                                                \
    }                                                                                            \
    result_type  operator()(void) const {                                                        \
        return (detail::evaluate(expr)) OPERATOR;                                                \
    }                                                                                            \
};

#define LREF_POSTFIX_UNARYOP_FUNCS_DECL(OPERATOR, OP_CLASS)                         \
template <typename T> inline                                                        \
Ile<OP_CLASS<lref<T> > >                                                            \
operator OPERATOR (lref<T> const & obj, int) {                                      \
    return OP_CLASS<lref<T> >(obj);                                                 \
}                                                                                   \
                                                                                    \
template <typename T> inline                                                        \
Ile<OP_CLASS<T> >                                                                   \
operator OPERATOR (Ile<T> const & expr, int) {                                      \
    return Ile<OP_CLASS<T> >(expr.expr);                                            \
}

#define LREF_POSTFIX_UNARYOP_DECL(OPERATOR, OP_CLASS)                               \
LREF_POSTFIX_UNARYOP_CLASS_DECL(OPERATOR, OP_CLASS)                                 \
LREF_POSTFIX_UNARYOP_FUNCS_DECL(OPERATOR, OP_CLASS)


LREF_POSTFIX_UNARYOP_DECL(++, Postfix_Inc_ILE)
LREF_POSTFIX_UNARYOP_DECL(--, Postfix_Dec_ILE)


} // namespace castor
#endif
