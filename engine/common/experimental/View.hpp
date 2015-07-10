// This is the interface that defines a view.
// A view refers to a given aspect of the program. For example: The start screen, the terrain editor, and the main game have their own views.
// As far as the main app is concerned, each view is responsible for handling its own input and drawing itself. However, the programmer should put in as much business logic as desired. 

#pragma once

#include <string>


namespace noob
{
	class view
	{
		public:
			virtual void init() = 0;
			virtual void draw() = 0;
			virtual void input(const std::string& in) = 0;
			virtual void load(const std::string& file) = 0;
			virtual void save(const std::string& file) = 0;
		protected:
			size_t height, width;
			bool initialized;
	};
}
