// This class performs full bounds-checking on each char access. It is for buffering user-supplied data. Stores text as
// TODO: Add proper UTF8 support via ICU (normalization, canonicalization, spoof-checking)
// TODO: Add hash-based canary to detect illegitimate changes at runtime.
// TODO: Add thread-safety via atomics.

#pragma once

// #include <atomic>
#include <string>
#include <memory>

#include <noob/strings/string_funcs.hpp>

#include "ReturnType.hpp"

namespace noob
{
	class managed_string
	{
		public:
			// Wipes the old contents.
			void init(const std::string&) noexcept(true);
			noob::return_type<uint8_t> at(size_t) noexcept(true);
			bool set(size_t) noexcept(true);
		protected:
			std::unique_ptr<std::basic_string<uint8_t>> text;
	};


}
