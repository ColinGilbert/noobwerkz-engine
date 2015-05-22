// This class keeps a running graph of all user commands and executes them in correct sequence.
#include <atomic>
#include "Lemon.hpp"
#include <lemon/list_graph.h>


namespace noob
{
	class commands
	{
		public:
			void input(char c);
		
		private:
			std::atomic<char> current_char;
			lemon::ListDigraph parse_graph
	};
}
