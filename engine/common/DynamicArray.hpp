#pragma once

#include <rdestl/vector.h>
#include <rdestl/sort.h>

namespace noob
{
	template <typename T>
	struct dynamic_array
	{
			void insertion_sort() noexcept(true)
			{
				rde::insertion_sort(&items[0], &items[items.size()-1]); 
			}

			void quick_sort() noexcept(true)
			{
				rde::quick_sort(&items[0], &items[items.size()-1]);
			}

			void heap_sort() noexcept(true)
			{
				rde::heap_sort(&items[0], &items[items.size()-1]);
			}
		
			void is_sorted() noexcept(true)
			{
				rde::is_sorted(&items[0], &items[items.size()-1]);
			}

			rde::vector<T> items;
	};
}
