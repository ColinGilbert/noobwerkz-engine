// From http://chadaustin.me/2014/06/emscripten-callbacks-and-c11-lambdas/
#pragma once
/*
namespace noob
{

	template<typename T>
		class lambda_callback
		{
			public:
				// perfect forwarding
				lambda_callback(T&& t) : t_inner(std::forward<T>(t))
			{

			}

				virtual void overloaded(const Response& response) override
				{
					t_inner(response);
				}

			private:
				T t_inner;
		};

	// this function exists so T’s type can be inferred, as lambdas have anonymous types
	template<typename T>
		lambda_callback<T>* make_callback(T&& t)
		{
			return new lambda_callback(std::forward<T>(t));
		}

}
*/
