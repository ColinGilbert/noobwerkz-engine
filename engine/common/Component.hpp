#pragma once

#include <memory>
#include <vector>
#include <rdestl/vector.h>

#include "Logger.hpp"

namespace noob
{
	class application;

	template <typename T>
		class handle
		{
			public:

			handle() noexcept(true) : inner(0) {}

			bool operator==(const noob::handle<T> other) const noexcept(true)
			{
				return (inner == other.inner);
			}

			bool operator!=(const noob::handle<T> other) const noexcept(true)
			{
				return (inner != other.inner);
			}

			bool operator<(const noob::handle<T> other) const noexcept(true)
			{
				return (inner < other.inner); 
			}

			bool operator>(const noob::handle<T> other) const noexcept(true)
			{
				return (inner > other.inner); 
			}

			uint32_t get_inner() const noexcept(true)
			{
				return inner;
			}

			static handle make(uint32_t i)
			{
				handle h;
				h.inner = i;
				return h;
			}

			protected:

			uint32_t inner;
		};

	template <typename T>
		class component 
		{
			friend class application;
			public:

			inline T get(noob::handle<T> h) const  noexcept(true)
			{
				if (exists(h)) return items[h.get_inner()];
				else 
				{
					logger::log("Invalid access to component");
					return items[0];
				}	
			}
/*
			T& get_ref(handle<T> h) noexcept(true)
			{
				return get_ref(h.get_inner());
			}
*/
			inline handle<T> add(const T& t) noexcept(true)
			{
				items.push_back(t);
				
				return handle<T>::make(items.size() - 1);
			}

			inline bool exists(handle<T> h) const noexcept(true)
			{
				return (h.get_inner() < items.size());
			}

			inline void set(handle<T> h, const T t) noexcept(true)
			{
				if (exists(h))
				{
					items[h.get_inner()] = t;
				}
			}

			void empty() noexcept(true)
			{
				items.resize(0);
			}

			inline uint32_t count() const noexcept(true)
			{
				return items.size();
			}

			protected:

			rde::vector<T> items;
		};

	template <typename T>
		class component_dynamic
		{
			public:
				T* get(handle<T> h) const noexcept(true)
				{
					if (exists(h)) 
					{
						T* temp = items[h.get_inner()].get();
						return temp;
					}
					else 
					{
						logger::log("Invalid access to component");
						return items[0].get();
					}
				}

				handle<T> add(std::unique_ptr<T>&& t) noexcept(true)
				{
					// items.emplace_back(std::move(t));
					items.push_back(std::move(t));
					return handle<T>::make(items.size()-1);
				}

				bool exists(handle<T> h) const noexcept(true)
				{
					bool results = h.get_inner() < items.size();
					return results;
				}

				void set(handle<T> h, std::unique_ptr<T>&& t) noexcept(true)
				{
					if (exists(h))
					{
						items[h.get_inner()] = std::move(t);
					}
				}

				void empty() noexcept(true)
				{
					items.resize(0);
				}

				uint32_t count() const noexcept(true)
				{
					return items.size();
				}

			protected:

				std::vector<std::unique_ptr<T>> items;
		};
}
