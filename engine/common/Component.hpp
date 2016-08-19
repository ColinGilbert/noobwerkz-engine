#pragma once

#include <memory>
#include <vector>
#include <limits>

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

				uint32_t index() const noexcept(true)
				{
					return inner;
				}

				static handle make(uint32_t i)
				{
					handle h;
					h.inner = i;
					return h;
				}

				static handle make_invalid()
				{
					handle h;
					h.inner = std::numeric_limits<uint32_t>::max();
				}

			protected:

				uint32_t inner;
		};

	template <typename T>
		class component 
		{
			friend class application;
			public:

			T get(noob::handle<T> h) const noexcept(true)
			{
				if (exists(h)) return items[h.index()];
				else 
				{
					logger::log("Invalid access to component");
					return items[0];
				}	
			}


			std::tuple<bool, const T*> get_ptr(handle<T> h) const noexcept(true)
			{
				if (exists(h))
				{
					return std::make_tuple(true, &(items[h.index()]));
				}
				else
				{
					return std::make_tuple(false, &(items[0]));
				}

			}

			std::tuple<bool, T*> get_ptr_mutable(handle<T> h) const noexcept(true)
			{
				if (exists(h))
				{
					return std::make_tuple(true, const_cast<T*>(&(items[h.index()])));
				}
				else
				{
					return std::make_tuple(false, const_cast<T*>(&(items[0])));
				}
			}

			handle<T> add(const T& t) noexcept(true)
			{
				items.push_back(t);

				return handle<T>::make(items.size() - 1);
			}

			bool exists(handle<T> h) const noexcept(true)
			{
				return (h.index() < items.size());
			}

			void set(handle<T> h, const T t) noexcept(true)
			{
				if (exists(h))
				{
					items[h.index()] = t;
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

			std::tuple<bool, uint32_t> index_from_ptr(const T* ptr) const noexcept(true)
			{
				const ptrdiff_t max_size = (items.size() * sizeof(T)) - sizeof(T);
				const ptrdiff_t diff = ptr - &(items[0]);
				if (diff < max_size)
				{
					return std::make_tuple(true, diff / sizeof(T));
				}
				else
				{
					return std::make_tuple(false, 0);
				}
			}

			uint32_t index_from_ptr_unsafe(const T* ptr) const noexcept(true)
			{
				const ptrdiff_t diff = ptr - &(items[0]);
				return diff / sizeof(T);
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
						T* temp = items[h.index()].get();
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
					bool results = h.index() < items.size();
					return results;
				}

				void set(handle<T> h, std::unique_ptr<T>&& t) noexcept(true)
				{
					if (exists(h))
					{
						items[h.index()] = std::move(t);
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
