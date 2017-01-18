// A semantically nicer way to return from possibly failing functions than using std::tuple.
// // Based on Folly's Try<>, but waaaaay smaller subset of features and with no use of exceptions.
// TODO: Move out of engine and into base_utils
// TODO: Add tribool support
// TODO: Implement tribool


#pragma once

#include <utility>

namespace noob
{
	template <typename T>
		struct results
		{
			//results() noexcept(true) = default;
			results(bool Valid, T Value) noexcept(true) : valid(Valid), value(Value) {};

			static noob::results<T> make_valid(T Value) noexcept(true)
			{
				return noob::results<T>(true, Value);
			}

			static noob::results<T> make_invalid() noexcept(true)
			{
				T t;
				return noob::results<T>(false, t);
			}

			const bool valid;
			T value;
		};
}
