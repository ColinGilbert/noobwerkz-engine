// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#include <castor.h>
#include <iostream>
#include <vector>

using namespace castor;
using namespace std;


void test_shuffle() {
	{ // generate mode, lref<Itr>
	  lref<int> c;
	  lref<string> s = "hello", ss;
	  lref<string::iterator> sb = s->begin(), se = s->end();
	  relation r = zip(True(20), shuffle(sb,se,ss)) >>= count(c);
	  if(!r())
		  throw "failed test_shuffle 1";
	  if(*c!=20)
		  throw "failed test_shuffle 1";
	  if(r() || c.defined() || !ss.defined())
		  throw  "failed test_shuffle 1";
	}
	{ // generate mode, Itr
	  lref<int> c;
	  lref<string> s = "hello", ss;
	  string::iterator sb = s->begin(), se = s->end();
	  relation r = zip(True(20), shuffle(sb,se,ss)) >>= count(c);
	  if(!r())
		  throw "failed test_shuffle 2";
	  if(*c!=20)
		  throw "failed test_shuffle 2";
	  if(r() || c.defined() || !ss.defined())
		  throw  "failed test_shuffle 2";
	}	
	{ // test mode - container
	  lref<int> c;
	  lref<string> s = "hello", ps;
	  relation r = (permutation(s,ps) && shuffle(ps,s)) >>= count(c);
	  if(!r())
		  throw "failed test_shuffle 3";
	  if(*c!=60)
		  throw  "failed test_shuffle 3";
	  if(r() || c.defined() || ps.defined())
		  throw  "failed test_shuffle 3";
	}
	{ // gen mode - const container, const_iterator
	  lref<int> c;
	  const string s = "hello";
      lref<const string> ls(&s,false);
      lref<string> sh;
	  relation r = shuffle(s.begin(), s.end(), sh); //const_iterator
	  if(!r())
		  throw  "failed test_shuffle 4";
      sh.reset();
      r = shuffle(ls,sh);  // const container
	  if(!r())
		  throw  "failed test_shuffle 4";
	}
}


void test_permutation() {
	{ // test mode
	  lref<string> s = "hello", ps="olleh";
	  relation r = permutation(s,ps) ;
	  if(!r())
		  throw "failed test_permute 1";
	  if(r())
		  throw  "failed test_permute 1";
	}
	{ // generate mode
	  lref<int> c=0;
	  lref<string> s = "hello", ps, ss="helol";
	  relation r = permutation(s,ps) && eq(ps,ss) && eval(++c);
	  if(!r() && *c!=1)
		  throw "failed test_permute 2";
	  ss="heoll";
	  if(!r() && *c!=2)
		  throw "failed test_permute 2";
	  if(r())
		  throw "failed test_permute 2";
	}
	{ // gen mode - const container, const_iterator
	  lref<int> c;
	  const string s = "hello";
      lref<const string> ls(&s,false);
      lref<string> sh;
	  relation r = permutation(s.begin(), s.end(), sh); //const_iterator
	  if(!r())
		  throw  "failed test_permute 3";
      sh.reset();
      r = permutation(ls,sh);  // const container
	  if(!r())
		  throw  "failed test_permute 3";
	}
}

void test_permutation_cmp() {
	{
        lref<string> s="ab", ps;
        relation r = permutation_cmp(s,ps,std::less<char>() );
        if(!r())
		  throw "failed test_permute_cmp 1";
        if(*ps!="ba")
		  throw "failed test_permute_cmp 1";
        if(!r())
		  throw "failed test_permute_cmp 1";
        if(*ps!="ab")
		  throw "failed test_permute_cmp 1";
	}
	{ // test mode 
	  string s="ba";
      lref<vector<char> > ps;
      relation r = permutation_cmp(s.begin(), s.end(), ps, std::less<char>() ) ;
	  if(!r())
		  throw "failed test_permute_cmp 2";
      if((*ps)[0]!='b')
		  throw  "failed test_permute_cmp 2";
      if((*ps)[1]!='a')
		  throw  "failed test_permute_cmp 2";
	  if(!r())
		  throw "failed test_permute_cmp 2";
      if((*ps)[0]!='a')
		  throw  "failed test_permute_cmp 2";
      if((*ps)[1]!='b')
		  throw  "failed test_permute_cmp 2";
	}
	{ // gen mode - greater<>
	  lref<string> s="ab", ps;
      relation r = permutation_cmp(s, ps, std::greater<char>() ) ;
	  if(!r())
		  throw "failed test_permute_cmp 3";
      if(*ps!="ab")
		  throw  "failed test_permute_cmp 3";
	  if(!r())
		  throw "failed test_permute_cmp 3";
      if(*ps!="ba")
		  throw  "failed test_permute_cmp 3";
	}
}