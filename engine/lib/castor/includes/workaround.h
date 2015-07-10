// Castor : Logic Programming Library
// Copyright © 2007-2010 Roshan Naik (roshan@mpprogramming.com).
// This software is governed by the MIT license (http://www.opensource.org/licenses/mit-license.php).

#ifndef CASTOR_WORKAROUND_H
#define CASTOR_WORKAROUND_H 1

// const ref arguments in templates not very well supported in BCB
#ifdef __BCPLUSPLUS__
#define CREF  const &
#define CASTOR_DISABLE_AND_OPTIMIZATION
#else
#define CREF
#endif

#endif //#ifndef CASTOR_WORKAROUND_H
