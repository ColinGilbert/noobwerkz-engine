#pragma once

#include <noob/logger/logger.hpp>
#include <noob/strings/strings.hpp>

#include <iostream>
#include <fstream>

// With any luck, this class should become obsolete.
namespace noob
{
	struct utils
	{
		static std::string load_file_as_string(const std::string& filename);
	};



#if NOOB_PLATFORM_ANDROID
#include <android/log.h>

	struct log_funct
	{
		static void log(noob::importance imp, const std::string& s) noexcept(true)
		{
			std::string msg = noob::concat(noob::to_string(imp), ": ", s);
			__android_log_print(ANDROID_LOG_DEBUG,"NOOB", msg.c_str());
			std::cout << msg << "\n";
		}
	};

#else
	struct log_funct
	{
		static void log(noob::importance imp, const std::string& s) noexcept(true)
		{

			std::string msg = noob::concat(noob::to_string(imp), ": ", s);
			//__android_log_print(ANDROID_LOG_DEBUG,"NOOB", msg.c_str());
			std::cout << msg << "\n";
		}
	};

#endif
	typedef logger_impl<log_funct> logger;
}
