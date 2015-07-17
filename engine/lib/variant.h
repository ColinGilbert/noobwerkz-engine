#include <stdexcept>
#include <typeinfo>

// Implementation details, the user should not import this:
namespace impl {

template<int N, typename... Ts>
struct storage_ops;

template<typename X, typename... Ts>
struct position;

template<typename... Ts>
struct type_info;

template<int N, typename T, typename... Ts>
struct storage_ops<N, T&, Ts...> {
	static void del(int n, void *data) {}
	
	template<typename visitor>
	static typename visitor::result_type apply(int n, void *data, visitor& v) {}
};

template<int N, typename T, typename... Ts>
struct storage_ops<N, T, Ts...> {
	static void del(int n, void *data) {
		if(n == N) reinterpret_cast<T*>(data)->~T();
		else storage_ops<N + 1, Ts...>::del(n, data);
	}
	template<typename visitor>
	static typename visitor::result_type apply(int n, void *data, visitor& v) {
		if(n == N) return v(*reinterpret_cast<T*>(data));
		else return storage_ops<N + 1, Ts...>::apply(n, data, v);
	}
};

template<int N>
struct storage_ops<N> {
	static void del(int n, void *data) {
		throw std::runtime_error(
			"Internal error: variant tag is invalid."
		);
	}
	template<typename visitor>
	static typename visitor::result_type apply(int n, void *data, visitor& v) {
		throw std::runtime_error(
			"Internal error: variant tag is invalid."
		);
	}
};

template<typename X>
struct position<X> {
	static const int pos = -1;
};

template<typename X, typename... Ts>
struct position<X, X, Ts...> {
	static const int pos = 0;
};

template<typename X, typename T, typename... Ts>
struct position<X, T, Ts...> {
	static const int pos = position<X, Ts...>::pos != -1 ? position<X, Ts...>::pos + 1 : -1;
};

template<typename T, typename... Ts>
struct type_info<T&, Ts...> {
	static const bool no_reference_types = false;
	static const bool no_duplicates = position<T, Ts...>::pos == -1 && type_info<Ts...>::no_duplicates;
	static const size_t size = type_info<Ts...>::size > sizeof(T&) ? type_info<Ts...>::size : sizeof(T&);
};

template<typename T, typename... Ts>
struct type_info<T, Ts...> {
	static const bool no_reference_types = type_info<Ts...>::no_reference_types;
	static const bool no_duplicates = position<T, Ts...>::pos == -1 && type_info<Ts...>::no_duplicates;
	static const size_t size = type_info<Ts...>::size > sizeof(T) ? type_info<Ts...>::size : sizeof(T&);
};

template<>
struct type_info<> {
	static const bool no_reference_types = true;
	static const bool no_duplicates = true;
	static const size_t size = 0;
};

} // namespace impl

template<typename... Types>
class variant {
	static_assert(impl::type_info<Types...>::no_reference_types, "Reference types are not permitted in variant.");
	static_assert(impl::type_info<Types...>::no_duplicates, "variant type arguments contain duplicate types.");
	
	int tag;
	char storage[impl::type_info<Types...>::size];
	
	variant() = delete;
	
	template<typename X>
	void init(const X& x) {
		tag = impl::position<X, Types...>::pos;
		new(storage) X(x);
	}
public:
	template<typename X>
	variant(const X& v) {
		static_assert(
			impl::position<X, Types...>::pos != -1,
			"Type not in variant."
		);
		init(v);
	}
	~variant() {
		impl::storage_ops<0, Types...>::del(tag, storage);
	}
	template<typename X>
	void operator=(const X& v) {
		static_assert(
			impl::position<X, Types...>::pos != -1,
			"Type not in variant."
		);
		this->~variant();
		init(v);
	}
	template<typename X>
	X& get() {
		static_assert(
			impl::position<X, Types...>::pos != -1,
			"Type not in variant."
		);
		if(tag == impl::position<X, Types...>::pos) {
			return *reinterpret_cast<X*>(storage);
		} else {
			throw std::runtime_error(
				std::string("variant does not contain value of type ") + typeid(X).name()
			);
		}
	}
	template<typename X>
	const X& get() const {
		static_assert(
			impl::position<X, Types...>::pos != -1,
			"Type not in variant."
		);
		if(tag == impl::position<X, Types...>::pos) {
			return *reinterpret_cast<const X*>(storage);
		} else {
			throw std::runtime_error(
				std::string("variant does not contain value of type ") + typeid(X).name()
			);
		}
	}
	template<typename visitor>
	typename visitor::result_type visit(visitor& v) {
		return impl::storage_ops<0, Types...>::apply(tag, storage, v);
	}
	
	int which() const {return tag;}
};

template<typename Result>
struct visitor {
	typedef Result result_type;
};
