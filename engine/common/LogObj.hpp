#pragma once

#include "Logger.hpp"//<format.h>

namespace noob
{

	static void logging_function(const std::string& msg)
	{
		logger::log(msg);
	};


	enum class log_severity : uint32_t
	{
		ERROR = 0,
		WARNING = 1,
		MESSAGE = 2

	};

	static std::string to_string(noob::log_severity arg) noexcept(true)
	{
		switch (arg)
		{
			case (noob::log_severity::ERROR):
				{
					return "Error";

				}
			case (noob::log_severity::WARNING):
				{
					return "Warning";
				}
			case (noob::log_severity::MESSAGE): 
				{
					return "Message";
				}

			default:
				{
					return "INVALID LOG SEVERITY LEVEL!";
				}
		}
	};

	enum class log_msg : uint32_t
	{
		COMMENT = 0,
		BAD_DATA = 1,
		BAD_VALUE = 2,
	};

	static std::string to_string(noob::log_msg arg) noexcept(true)
	{
		switch(arg)
		{
			case(noob::log_msg::COMMENT):
				{
					return "Comment";
				}
			case(noob::log_msg::BAD_DATA):
				{
					return "Bad data";
				}
			case (noob::log_msg::BAD_VALUE):
				{
					return "INVALID LOG MESSAGE VALUE!";
				}
		}
	};

	class log_obj
	{
		public:
			log_obj(const std::string& class_name_arg, noob::log_severity severity_arg, noob::log_msg msg_arg) noexcept(true) : class_name(class_name_arg), severity(severity_arg), msg(msg_arg) {}

			void operator<<(const std::string& in) noexcept(true)
			{
				ww << in;
			}

			void log_now() const noexcept(true)
			{
				//logger::log(ww.str());
				fmt::MemoryWriter results;
				results << "[" << class_name << "] : " << noob::to_string(severity) << ", " << noob::to_string(msg) << ". " << ww.str();
				logger::log(results.str());
			}

			const std::string class_name;
			const noob::log_severity severity;
			const noob::log_msg msg;

		protected:
			fmt::MemoryWriter ww;
	};
}
