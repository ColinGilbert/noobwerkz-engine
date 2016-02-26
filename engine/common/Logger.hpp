#pragma once
#include "format.h"
namespace noob
{
	class logger
	{
		public:
			enum importance
			{
				CRITICAL = 0, ERROR = 1, WARNING = 2, EVENT = 3, INFO = 4
			};

			static void log(const std::string&, noob::logger::importance i = noob::logger::importance::INFO);
	};
}
