// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#if !defined CASTOR_COLLECTION_H
#define CASTOR_COLLECTION_H 1

#include "coroutine.h"
#include "relation.h"
#include "lref.h"
#include "utils.h"
#include <algorithm>
#include <vector>
#include <functional>
#include <iterator>

namespace castor {

namespace detail {
template<typename RandItr>
bool equal_heap(RandItr first_b, RandItr first_e, RandItr second_b, RandItr second_e) {
	if( std::distance(first_b, first_e) != std::distance(second_b, second_e) )
		return false;
	while(first_b!=first_e) {
		if(*first_b!=*second_b)
			return false;
		std::pop_heap(first_b,first_e);
		std::pop_heap(second_b,second_e);
		--first_e; 
		--second_e;
	}
	return true;
}

// Cmp : less than ordering 
template<class InItr, class InItr2, class Cmp>
bool equal_bags(InItr first_b, InItr first_e, InItr2 second_b, InItr2 second_e, Cmp cmp = std::less<typename std::iterator_traits<InItr>::value_type>() ) {
    typename std::iterator_traits<InItr>::difference_type s1 = std::distance(first_b,first_e), s2 = std::distance(second_b,second_e);
	if(s1!=s2)
		return false;
    typedef typename std::iterator_traits<InItr>::value_type value_type;
    typedef typename std::iterator_traits<InItr2>::value_type value_type2;
	std::vector<value_type>  tmp1(s1);
	std::vector<value_type2>  tmp2(s2);
    tmp1.insert(tmp1.begin(),first_b, first_e);
    tmp2.insert(tmp2.begin(),second_b, second_e);

    std::make_heap(tmp1.begin(),tmp1.end(), cmp);
	std::make_heap(tmp2.begin(),tmp2.end(), cmp);
	return equal_heap(tmp1.begin(),tmp1.end(),tmp2.begin(),tmp2.end());
}

} // namespace detail

//-------------------------------------------------
// shuffle(seq_i,shuf) - shuffling (randomize) sequence seq_i, yields shuf
//-------------------------------------------------
template<class InItr, class RandSeq>
class Shuffle_r : public Coroutine {
	typedef typename std::iterator_traits<typename effective_type<InItr>::result_type>::value_type value_type;
	InItr begin_i, end_i;
	lref<RandSeq> shuf;

public:
	Shuffle_r(InItr begin_i, InItr end_i, const lref<RandSeq>& shuf) : begin_i(begin_i), end_i(end_i), shuf(shuf) 
	{ }

	bool operator()(void) {
		co_begin();
		if(effective_value(begin_i)==effective_value(end_i))
            co_return(false);
		if(shuf.defined())
            co_return( detail::equal_bags(effective_value(begin_i), effective_value(end_i), shuf->begin(), shuf->end(), std::less<value_type>() ) );
        shuf.set_ptr(new RandSeq(effective_value(begin_i),effective_value(end_i) ), true);
		while (true) {
			std::random_shuffle(shuf->begin(), shuf->end());
			co_yield(true);
		}
		shuf.reset();
		co_end();
	}
};

template<class InItr, class RandSeq> inline
Shuffle_r<InItr,RandSeq> shuffle(const InItr& begin_i, const InItr& end_i, const lref<RandSeq>& shuf) {
	return Shuffle_r<InItr,RandSeq>(begin_i,end_i,shuf);
}


template<class Cont, class RandSeq> inline
relation shuffle(lref<Cont>& seq_i, const lref<RandSeq>& shuf) {
	lref<typename Cont::const_iterator> b,e;
	return begin(seq_i,b) && end(seq_i,e) && shuffle(b,e,shuf);
}


//-------------------------------------------------
// permutation(.., p_seq) - permutation of input sequence is p_seq (uses std::less as the ordering contraint)
//-------------------------------------------------
template<class FwdItr, class RandSeq, class Cmp = std::less<typename RandSeq::value_type> >
struct Permutation_r : public Coroutine {
	typedef typename std::iterator_traits<typename effective_type<FwdItr>::result_type>::value_type value_type;
	FwdItr begin_i, end_i;
	lref<RandSeq> p_seq;
	Cmp cmp;
public:
	Permutation_r(FwdItr begin_i, FwdItr end_i, const lref<RandSeq>& p_seq, Cmp cmp=Cmp() ) : begin_i(begin_i), end_i(end_i), p_seq(p_seq), cmp(cmp)
	{ }

    bool operator()(void) {
		co_begin();

        if(effective_value(begin_i)==effective_value(end_i))
			co_return(false);

        if(p_seq.defined())
            co_return( detail::equal_bags(effective_value(begin_i), effective_value(end_i), p_seq->begin(), p_seq->end(), std::less<value_type>() ) );

        p_seq.set_ptr(new RandSeq(effective_value(begin_i),effective_value(end_i) ), true);
		while ( std::next_permutation(p_seq->begin(), p_seq->end(), cmp) )  // gen all next permutations
			co_yield(true);
		p_seq->clear();
        p_seq->insert(p_seq->begin(), effective_value(begin_i),effective_value(end_i));    // reload
		co_yield(true);
		while ( std::prev_permutation(p_seq->begin(), p_seq->end(), cmp) )  // gen all prev permutations
			co_yield(true);
		p_seq.reset();
		co_end();
	}
};

template<class FwdItr, class RandSeq> inline
Permutation_r<FwdItr,RandSeq>
permutation(const FwdItr& begin_i, const FwdItr& end_i, lref<RandSeq>& p_seq) {
	return Permutation_r<FwdItr,RandSeq>(begin_i,end_i,p_seq);
}

template<class Cont, class RandSeq> inline
relation permutation(lref<Cont>& seq_i, lref<RandSeq>& p_seq) {
	lref<typename Cont::const_iterator> b,e;
	return begin(seq_i,b) && end(seq_i,e) && permutation(b,e,p_seq);
}

//-------------------------------------------------
// permutation_cmp(.., p_seq) - permutation of input sequence using 'order' constraint is p_seq
// note: This relations could not be called simply permutation due to overload resolution 
//       ambiguities with above relations
//-------------------------------------------------

template<class FwdItr, class RandSeq, class Cmp> inline
Permutation_r<FwdItr,RandSeq,Cmp> 
permutation_cmp(const FwdItr& begin_i, const FwdItr& end_i, lref<RandSeq>& p_seq, Cmp order) {
	return Permutation_r<FwdItr,RandSeq,Cmp>(begin_i, end_i, p_seq, order);
}


template<class Cont, class RandSeq, class Cmp> inline
relation permutation_cmp(lref<Cont>& seq_i, lref<RandSeq>& p_seq, Cmp order) {
	lref<typename Cont::const_iterator> b,e;
	return begin(seq_i,b) && end(seq_i,e) && permutation_cmp(b,e,p_seq,order);
}


} // namespace castor

#endif
