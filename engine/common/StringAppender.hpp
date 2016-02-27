#pragma once

namespace noob
{
	class string_appender
	{
		public:
			void operator+=(const std::string& s)
			{
				ww << s;
			}

			std::string get() const
			{
				return ww.str();
			}

		protected:

		fmt::MemoryWriter ww;
	}
}
