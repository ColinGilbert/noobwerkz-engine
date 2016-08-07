//  Copyright (c) 2007 Red War Team. All Rights Reserved.  //
//ดจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจด//

#include "Shiny.h"

#ifdef _WIN32
#include <windows.h>
#else // assume POSIX
#include <unistd.h>
#endif

#include <iostream>
using namespace std;


//-----------------------------------------------------------------------------

void millisleep(unsigned int milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}


//-----------------------------------------------------------------------------

void Recursion(int calls_left) {
	PROFILE_FUNC(); // begin profile until end of block

	if (calls_left > 0) Recursion(calls_left - 1);
}


//-----------------------------------------------------------------------------

int main() {

	Recursion(12);

	PROFILER_UPDATE(); // update all profiles
	PROFILER_OUTPUT(); // print to cout

	return 0;
}
