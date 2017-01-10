#pragma once

namespace noob
{
	template<typename T>
		class singleton
		{
			public:
				T& get() noexcept(true)
				{
					static T t;
					return t;
				}
		};
}
