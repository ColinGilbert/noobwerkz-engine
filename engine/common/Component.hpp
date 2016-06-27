#pragma once

#include <memory>
#include <vector>
#include <rdestl/vector.h>

#include "Logger.hpp"

namespace noob
{
	class application;

	template <typename T>
	class component 
		{
			friend class application;
			public:
			class handle
			{
				friend class component;
				friend class application;

				public:

				handle() noexcept(true) : inner(0) {}

				bool operator==(const noob::component<T>::handle other) const noexcept(true)
				{
					return (inner == other.inner);
				}

				bool operator!=(const noob::component<T>::handle other) const noexcept(true)
				{
					return (inner != other.inner);
				}

				bool operator<(const noob::component<T>::handle other) const noexcept(true)
				{
					return (inner < other.inner); 
				}

				bool operator>(const noob::component<T>::handle other) const noexcept(true)
				{
					return (inner > other.inner); 
				}

				uint32_t get_inner() const noexcept(true)
				{
					return inner;
				}

				protected:

				uint32_t inner;
			};

			T get(component<T>::handle h) noexcept(true)
			{
				if (exists(h)) return items[h.inner];
				else 
				{
					logger::log("Invalid access to component");
					return items[0];
				}	
			}

			T get(uint32_t i) noexcept(true)
			{
				handle h = make_handle(i);
				if (exists(h)) return items[h.inner];
				else 
				{
					logger::log("Invalid access to component");
					return items[0];
				}	
			}

			T& get_ref(uint32_t i) noexcept(true)
			{
				handle h = make_handle(i);
				if (exists(h)) return items[h.inner];
				else
				{
					logger::log("Invalid access to component ref");
				}
			}

			T& get_ref(component<T>::handle h) noexcept(true)
			{
				return get_ref(h.inner);
			}

			component<T>::handle add(const T& t) noexcept(true)
			{
				items.push_back(t);
				handle h;
				h.inner = items.size() - 1;
				return h;
			}

			bool exists(component<T>::handle h) noexcept(true)
			{
				return (h.inner < items.size());
			}

			void set(component<T>::handle h, const T t) noexcept(true)
			{
				if (exists(h))
				{
					items[h.inner] = t;
				}
			}

			component<T>::handle make_handle(unsigned int i) noexcept(true)
			{
				handle h;
				h.inner = i;
				return h;
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

			rde::vector<T> items;
		};


	template<typename T>
		class component <std::unique_ptr<T>> 
		{
			public:
				class handle
				{
					friend class application;					
					friend class component;

					public:

					handle() noexcept(true) : inner(0) {}

					bool operator==(const noob::component<std::unique_ptr<T>>::handle other) const noexcept(true)
					{
						return (inner == other.inner);
					}

					bool operator!=(const noob::component<std::unique_ptr<T>>::handle other) const noexcept(true)
					{
						return (inner != other.inner);
					}

					bool operator<(const noob::component<std::unique_ptr<T>>::handle other) const noexcept(true)
					{
						return (inner < other.inner); 
					}

					bool operator>(const noob::component<std::unique_ptr<T>>::handle other) const noexcept(true)
					{
						return (inner > other.inner); 
					}


					uint32_t get_inner() const noexcept(true)
					{
						return inner;
					}

					protected:

					uint32_t inner;
				};
				
				T* get(component<std::unique_ptr<T>>::handle h) noexcept(true)
				{
					if (exists(h)) 
					{
						T* temp = items[h.inner].get();
						return temp;
					}
					else 
					{
						logger::log("Invalid access to component");
						return items[0].get();
					}
				}


				T* get(uint32_t i) noexcept(true)
				{
					handle h = make_handle(i);
					if (exists(h)) 
					{
						T* temp = items[h.inner].get();
						return temp;
					}
					else 
					{
						logger::log("Invalid access to component");
						return items[0].get();
					}
				}

				component<std::unique_ptr<T>>::handle add(std::unique_ptr<T>&& t) noexcept(true)
				{
					// items.emplace_back(std::move(t));
					items.push_back(std::move(t));
					handle h;
					h.inner = items.size() - 1;
					return h;
				}

				bool exists(component<std::unique_ptr<T>>::handle h) noexcept(true)
				{
					bool results = h.inner < items.size();
					return results;
				}

				void set(component<std::unique_ptr<T>>::handle h, std::unique_ptr<T>&& t) noexcept(true)
				{
					if (exists(h))
					{
						items[h.inner] = std::move(t);
					}
				}

				component<std::unique_ptr<T>>::handle make_handle(unsigned int i) noexcept (true)
				{
					handle h;
					h.inner = i;
					return h;
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
