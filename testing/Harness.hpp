namespace noob
{
	class harness
	{
		public:
			virtual void init_world() = 0;
			virtual void destroy_world() = 0;
			virtual bool run_test(const noob::test& t) = 0;
	};

};
