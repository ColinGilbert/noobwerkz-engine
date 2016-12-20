// A semantically nicer way to return from arguments than using std::tuple. For use in returning a type from a function that may fail to properly do something.
// Based on Folly's Try<>, but far smaller subset of features and without any support for exceptions.
// TODO: Move out of engine and into base_utils
// TODO: Add tribool support

#pragma once

namespace noob
{
	template <typename T>
		struct return_type
		{
			return_type(bool Valid, T Value) noexcept(true) : valid(Valid), value(Value) {}

			static noob::return_type<T> make_invalid()
			{
				T t;
				return noob::return_type<T>(false, t);
			}
			const bool valid;
			T value;
		};


}
