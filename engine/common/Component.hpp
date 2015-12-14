#pragma once


#include <unordered_map>
#include <memory>
#include <vector>

namespace noob
{
	template<typename T>
		class component 
		{
			public:
				class handle
				{
					friend class component;
					public:
					handle() : valid(false), inner(0) {}
					protected:
					bool valid;
					size_t inner;
				};

				T get(component<T>::handle h)
				{
					if (exists(h)) return items[h.inner];
					else return items[0];
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

				void set(component<T>::handle h, const std::string& name)
				{
					if (exists(h))
					{
						names[name] = h.inner;
					}
				}

				component<T>::handle get_handle(const std::string& name)
				{
					handle h;
					auto search = names.find(name);
					if (search != names.end())
					{
						h.valid = true;
						h.inner = search->second;
					}
					return h;
				}

				bool exists(const std::string& name)
				{
					return (names.find(name) != names.end());
				}


			protected:

				std::vector<T> items;
				std::unordered_map<std::string, size_t> names;
		};


	template<typename T>
		class component<std::unique_ptr<T>> 
		{
			public:
				class handle
				{
					friend class component;
					protected:
					bool valid;
					size_t inner;
				};

				T* get(component<std::unique_ptr<T>>::handle h)
				{
					if (exists(h)) return items[h.inner].get();
					else return items[0].get();
				}

				component<std::unique_ptr<T>>::handle add(std::unique_ptr<T>&& t)
				{
					items.emplace_back(std::move(t));
					handle h;
					h.inner = items.size() - 1;
					return h;
				}

				component<std::unique_ptr<T>>::handle add(std::unique_ptr<T>&& t, const std::string& name)
				{
					handle h;
					auto search = names.find(name);
					if (search != names.end())
					{
						h.inner = search->second;
						items[h.inner] = std::move(t);
						h.valid = true;
						return h;
					}

					items.emplace_back(std::move(t));
					h.inner = items.size() - 1;
					return h;
				}


				bool exists(component<std::unique_ptr<T>>::handle h)
				{
					return (h.inner < items.size());
				}

				void set(component<std::unique_ptr<T>>::handle h, const std::string& name)
				{
					if (exists(h))
					{
						names[name] = h.inner;
					}
				}

				component<std::unique_ptr<T>>::handle get_handle(const std::string& name)
				{
					handle h;
					auto search = names.find(name);
					if (search != names.end())
					{
						h.valid = true;
						h.inner = search->second;
					}
					return h;
				}

				bool exists(const std::string& name)
				{
					return (names.find(name) != names.end());
				}

			protected:

				std::vector<std::unique_ptr<T>> items;
				std::unordered_map<std::string, size_t> names;
		};

}
