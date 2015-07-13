// This class is used to insert a bunch of items by name and allows you to search by name. The current implementation uses the STL, but further optimizations are definitely worth exploring.
// It is only as thread-safe as the std::vector allows.

#pragma once

#include <vector>
#include <tuple>

namespace noob
{
	template<T> class string_indexer
	{
		public:
			noob::string_indexer(): ready(true) {}
			
			std::tuple<bool, T&> get(const std::string& name) const
			{

			}


			void add(const std::string& name, T item, bool sort = true)
			{

			}


			void add(const std::vector<std::tuple<const std::string&, T>> items, bool sort = true)
			{

			}


			void remove(const std::string& name, T item, bool sort = true)
			{

			}


			void remove(const std::vector<std::tuple<const std::string&, T>> items, bool sort = true)
			{

			}


			void reindex()
			{

			}


		protected:
			std::vector<std::tuple<const std::string, size_t>> index; 
			bool ready;
	}
}
