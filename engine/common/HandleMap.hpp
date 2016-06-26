#pragma once

#include <limits>
#include <rdestl/rdestl.h>
#include <rdestl/fixed_array.h>

namespace noob
{
	template <typename T>
		class handle_map
		{
			public:
				static const T invalid = std::numeric_limits<T>::max();

				struct key_h
				{
					key_h() noexcept(true) : inner(invalid) {}
					key_h(T t) noexcept(true) : inner(t) {}
					T inner;
				};

				struct val_h
				{
					val_h() noexcept(true) : inner(invalid) {}
					val_h(T t) noexcept(true) : inner(invalid) {}
					T inner;
				};

				void init(uint32_t n) noexcept(true)
				{
					keys.resize(n);
					keys.resize(n);
					rde::fill_n<T>(&keys[0], keys.size(), invalid);
					rde::fill_n<T>(&values[0], values.size(), invalid);
				}

				val_h add(key_h k) noexcept(true)
				{
					++size;
					val_h val;
					val.v = size;
					values[val.v] = k;
					keys[k] = val.inner;
					return val;
				}

				rde::pair<val_h, val_h> erase(key_h k) noexcept(true)
				{
					val_h val, last_val;
					val.inner = keys[k.inner];
					last_val.inner = size - 1;
					--size;
					T last_key = values[last_val.inner];
					values[val] = last_key;
					keys[last_key] = val.inner;
					return rde::make_pair(last_val, val);
				}

				val_h get_val_for_key(key_h k) noexcept(true)
				{
					return val_h(keys[k.inner]);
				}

				key_h get_key_for_val(val_h v) noexcept(true)
				{
					return key_h(values[v.inner]);
				}


			protected:
				uint32_t size;
				rde::vector<T> keys;
				rde::vector<T> values;

		};
}
