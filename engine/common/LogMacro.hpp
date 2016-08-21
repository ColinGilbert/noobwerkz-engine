#include "Logging.hpp"//<format.h>

namespace noob
{
static void logging_function(const std::string& msg)
{
	logger::log(msg);
}

	enum class log_severity : uint32_t
	{
		ERROR = 0,
		WARNING = 1,
		MESSAGE = 2,
	};

	enum class log_msg : uint32_t
		// Details
		BAD_DATA = 0,
		BAD_VALUE = 1,	
	};

	class log
	{
		public:
		log(const std::string& class_name_arg) constexpr(true) : class_name(class_name_arg), ww(class_name) {}
		
		friend std::ostream &operator<<(const std::string& in)
		{
			ww << in;
		}
		void log() const noexcept(true)
		{
			logger::log(ww.str());
		}

		const std::string class_name
	
		protected:
		fmt::MemoryWriter ww;
	}
