#pragma once


#include <unordered_map>
#include <memory>
#include <vector>

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

				bool operator==(const noob::component<T>::handle other) const
				{
					return (inner == other.inner);
				}

				bool operator!=(const noob::component<T>::handle other) const
				{
					return (inner != other.inner);
				}

				bool operator<(const noob::component<T>::handle other) const
				{
					return (inner < other.inner); 
				}

				bool operator>(const noob::component<T>::handle other) const
				{
					return (inner > other.inner); 
				}

				size_t get_inner() const
				{
					return inner;
				}

				protected:

				size_t inner;
			};

			T get(component<T>::handle h)
			{
				if (exists(h)) return items[h.inner];
				else 
				{
					logger::log("Invalid access to component");
					return items[0];
				}	
			}

			T get(size_t i)
			{
				handle h = make_handle(i);
				if (exists(h)) return items[h.inner];
				else 
				{
					logger::log("Invalid access to component");
					return items[0];
				}	
			}


			component<T>::handle add(const T& t)
			{
				items.push_back(t);
				handle h;
				h.inner = items.size() - 1;
				return h;
			}

			bool exists(component<T>::handle h)
			{
				return (h.inner < items.size());
			}

			void set(component<T>::handle h, const T t)
			{
				if (exists(h))
				{
					items[h.inner] = t;
				}
			}

			component<T>::handle make_handle(unsigned int i)
			{
				handle h;
				h.inner = i;
				return h;
			}

			void empty()
			{
				items.resize(0);
				names.empty();
			}

			protected:

			std::vector<T> items;
			std::unordered_map<std::string, size_t> names;
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

					bool operator==(const noob::component<std::unique_ptr<T>>::handle other) const
					{
						return (inner == other.inner);
					}

					bool operator!=(const noob::component<std::unique_ptr<T>>::handle other) const
					{
						return (inner != other.inner);
					}

					bool operator<(const noob::component<std::unique_ptr<T>>::handle other) const
					{
						return (inner < other.inner); 
					}

					bool operator>(const noob::component<std::unique_ptr<T>>::handle other) const
					{
						return (inner > other.inner); 
					}


					size_t get_inner() const
					{
						return inner;
					}

					protected:

					size_t inner;
				};

				T* get(component<std::unique_ptr<T>>::handle h)
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


				T* get(size_t i)
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

				component<std::unique_ptr<T>>::handle add(std::unique_ptr<T>&& t)
				{
					// items.emplace_back(std::move(t));
					items.push_back(std::move(t));
					handle h;
					h.inner = items.size() - 1;
					return h;
				}

				bool exists(component<std::unique_ptr<T>>::handle h)
				{
					bool results = h.inner < items.size();
					return results;
				}

				void set(component<std::unique_ptr<T>>::handle h, std::unique_ptr<T>&& t)
				{
					if (exists(h))
					{
						items[h.inner] = std::move(t);
					}
				}

				component<std::unique_ptr<T>>::handle make_handle(unsigned int i)
				{
					handle h;
					h.inner = i;
					return h;
				}

				void empty()
				{
					items.resize(0);
					names.empty();
				}

			protected:

				std::vector<std::unique_ptr<T>> items;
				std::unordered_map<std::string, size_t> names;
		};
}
