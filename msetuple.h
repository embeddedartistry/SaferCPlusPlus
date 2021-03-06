
#pragma once
# ifndef MSETUPLE_H_
# define MSETUPLE_H_

#ifndef MSE_TUPLE_NO_XSCOPE_DEPENDENCE
#include "msescope.h"
#endif // !MSE_TUPLE_NO_XSCOPE_DEPENDENCE
#include "msepointerbasics.h"

#include<tuple>
#include<type_traits>
#include<functional>

#ifdef MSE_SELF_TESTS
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#endif // MSE_SELF_TESTS


#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4702 4189 )
#endif /*_MSC_VER*/

#ifdef MSE_CUSTOM_THROW_DEFINITION
#include <iostream>
#define MSE_THROW(x) MSE_CUSTOM_THROW_DEFINITION(x)
#else // MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) throw(x)
#endif // MSE_CUSTOM_THROW_DEFINITION

#ifndef _NODISCARD
#ifdef MSE_HAS_CXX17
#define _NODISCARD [[nodiscard]]
#else // MSE_HAS_CXX17
#define _NODISCARD
#endif // MSE_HAS_CXX17
#endif // !_NODISCARD


namespace mse {

	namespace impl {
		template<class>
		// false value attached to a dependent name (for static_assert)
		MSE_INLINE_VAR constexpr bool _Always_false = false;

		template<class _Ty>
		struct _Unrefwrap_helper
		{	// leave unchanged if not a reference_wrapper
			using type = _Ty;
		};
		template<class _Ty>
		struct _Unrefwrap_helper<std::reference_wrapper<_Ty>>
		{	// make a reference from a reference_wrapper
			using type = _Ty & ;
		};
		// decay, then unwrap a reference_wrapper
		template<class _Ty>
		using _Unrefwrap_t = typename _Unrefwrap_helper<std::decay_t<_Ty>>::type;
	}

	namespace mstd {

		namespace impl {
			namespace tuple {
				template<class _Ty>
				void s_invoke_T_valid_if_not_an_xscope_type_on_each_type() {
					mse::impl::T_valid_if_not_an_xscope_type<_Ty>();
				}
				template<class _Ty, class _Ty2, class... _Args>
				void s_invoke_T_valid_if_not_an_xscope_type_on_each_type() {
					mse::impl::T_valid_if_not_an_xscope_type<_Ty>();
					s_invoke_T_valid_if_not_an_xscope_type_on_each_type<_Ty2, _Args...>();
				}
			}
		}

		template<class... _Types>
		class tuple;

		template<>
		class tuple<> : public std::tuple<> {
		public:
			typedef std::tuple<> base_class;

			using base_class::base_class;

			//using base_class::operator=;
			//MSE_USING(tuple, base_class);

			void async_shareable_tag() const {}
			void async_passable_tag() const {}
		};

		template<class _This, class... _Rest>
		class tuple<_This, _Rest...> : public std::tuple<_This, _Rest...> {
		public:
			typedef std::tuple<_This, _Rest...> base_class;

			//MSE_USING(tuple, base_class);
			using base_class::base_class;

			~tuple() {
				mse::mstd::impl::tuple::s_invoke_T_valid_if_not_an_xscope_type_on_each_type<_This, _Rest...>();
			}

			//using base_class::operator=;

			template<class dummyT = int, class = typename std::enable_if<(std::is_same<dummyT, int>::value)
				&& (mse::impl::is_marked_as_shareable_msemsearray<_This>::value)
				&& (mse::impl::conjunction<mse::impl::is_marked_as_shareable_msemsearray<_Rest>...>::value), void>::type>
			void async_shareable_tag() const {}
			template<class dummyT = int, class = typename std::enable_if<(std::is_same<dummyT, int>::value)
				&& (mse::impl::is_marked_as_passable_msemsearray<_This>::value)
				&& (mse::impl::conjunction<mse::impl::is_marked_as_passable_msemsearray<_Rest>...>::value), void>::type>
			void async_passable_tag() const {}
		};

#ifdef MSE_HAS_CXX17
		template<class... _Types> tuple(_Types...)->tuple<_Types...>;
		template<class _Ty1, class _Ty2> tuple(std::pair<_Ty1, _Ty2>)->tuple<_Ty1, _Ty2>;
		template<class _Alloc, class... _Types> tuple(std::allocator_arg_t, _Alloc, _Types...)->tuple<_Types...>;
		template<class _Alloc, class _Ty1, class _Ty2> tuple(std::allocator_arg_t, _Alloc, std::pair<_Ty1, _Ty2>)->tuple<_Ty1, _Ty2>;
		template<class _Alloc, class... _Types> tuple(std::allocator_arg_t, _Alloc, tuple<_Types...>)->tuple<_Types...>;
#endif /* MSE_HAS_CXX17 */

		template<class... _Types>
		constexpr tuple<typename std::decay<_Types>::type...> make_tuple(_Types&&... _Args) {
			typedef tuple<mse::impl::_Unrefwrap_t<_Types>...> _Ttype;
			return (_Ttype(std::forward<_Types>(_Args)...));
		}
	}

	template<class... Types>
	using tuple = mstd::tuple<Types...>;


#ifndef MSE_TUPLE_NO_XSCOPE_DEPENDENCE

	template<class... _Types>
	class xscope_tuple;

	template<>
	class xscope_tuple<> : public std::tuple<>, public mse::us::impl::XScopeTagBase {
	public:
		typedef std::tuple<> base_class;

		MSE_USING(xscope_tuple, base_class);
		using base_class::base_class;

		//using base_class::operator=;

		void async_xscope_shareable_tag() const {}
		void async_xscope_passable_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template <class _This, class... _Rest>
	class xscope_tuple<_This, _Rest...> : public std::tuple<_This, _Rest...>, public mse::us::impl::XScopeTagBase
		//, public std::conditional<std::is_base_of<mse::us::impl::ReferenceableByScopePointerTagBase, _Ty>::value, mse::us::impl::ReferenceableByScopePointerTagBase, mse::impl::TPlaceHolder_msescope<xscope_tuple<_Ty> > >::type
		//, public std::conditional<std::is_base_of<mse::us::impl::ContainsNonOwningScopeReferenceTagBase, _Ty>::value, mse::us::impl::ContainsNonOwningScopeReferenceTagBase, mse::impl::TPlaceHolder2_msescope<xscope_tuple<_Ty> > >::type
	{
	public:
		typedef std::tuple<_This, _Rest...> base_class;

		MSE_USING(xscope_tuple, base_class);
		using base_class::base_class;

		template<class dummyT = int, class = typename std::enable_if<(std::is_same<dummyT, int>::value)
			&& (mse::impl::is_marked_as_xscope_shareable_msemsearray<_This>::value)
			&& (mse::impl::conjunction<mse::impl::is_marked_as_xscope_shareable_msemsearray<_Rest>...>::value), void>::type>
			void async_xscope_shareable_tag() const {}
		template<class dummyT = int, class = typename std::enable_if<(std::is_same<dummyT, int>::value)
			&& (mse::impl::is_marked_as_xscope_passable_msemsearray<_This>::value)
			&& (mse::impl::conjunction<mse::impl::is_marked_as_xscope_passable_msemsearray<_Rest>...>::value), void>::type>
			void async_xscope_passable_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class... _Types> xscope_tuple(_Types...)->xscope_tuple<_Types...>;
	template<class _Ty1, class _Ty2> xscope_tuple(std::pair<_Ty1, _Ty2>)->xscope_tuple<_Ty1, _Ty2>;
	template<class _Alloc, class... _Types> xscope_tuple(std::allocator_arg_t, _Alloc, _Types...)->xscope_tuple<_Types...>;
	template<class _Alloc, class _Ty1, class _Ty2> xscope_tuple(std::allocator_arg_t, _Alloc, std::pair<_Ty1, _Ty2>)->xscope_tuple<_Ty1, _Ty2>;
	template<class _Alloc, class... _Types> xscope_tuple(std::allocator_arg_t, _Alloc, xscope_tuple<_Types...>)->xscope_tuple<_Types...>;
#endif /* MSE_HAS_CXX17 */

	template<class... _Types>
	constexpr xscope_tuple<typename std::decay<_Types>::type...> make_xscope_xscope_tuple(_Types&&... _Args) {
		typedef xscope_tuple<mse::impl::_Unrefwrap_t<_Types>...> _Ttype;
		return (_Ttype(std::forward<_Types>(_Args)...));
	}
#endif // !MSE_TUPLE_NO_XSCOPE_DEPENDENCE
}

namespace std {
	template<size_t _Index, class... _Types>
	_NODISCARD constexpr tuple_element_t<_Index, tuple<_Types...>>& get(mse::mstd::tuple<_Types...>& _Tuple) noexcept {
		return get<_Index>(static_cast<typename mse::mstd::tuple<_Types...>::base_class&>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr const tuple_element_t<_Index, mse::mstd::tuple<_Types...>>& get(const mse::mstd::tuple<_Types...>& _Tuple) noexcept {
		return get<_Index>(static_cast<const typename mse::mstd::tuple<_Types...>::base_class&>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr tuple_element_t<_Index, mse::mstd::tuple<_Types...>>&& get(mse::mstd::tuple<_Types...>&& _Tuple) noexcept {
		return get<_Index>(std::forward<typename mse::mstd::tuple<_Types...>::base_class>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr const tuple_element_t<_Index, mse::mstd::tuple<_Types...>>&& get(const mse::mstd::tuple<_Types...>&& _Tuple) noexcept {
		return get<_Index>(std::forward<const typename mse::mstd::tuple<_Types...>::base_class>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr _Ty& get(mse::mstd::tuple<_Types...>& _Tuple) noexcept {
		return get<_Ty>(static_cast<typename mse::mstd::tuple<_Types...>::base_class&>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr const _Ty& get(const mse::mstd::tuple<_Types...>& _Tuple) noexcept {
		return get<_Ty>(static_cast<const typename mse::mstd::tuple<_Types...>::base_class&>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr _Ty&& get(mse::mstd::tuple<_Types...>&& _Tuple) noexcept {
		return get<_Ty>(std::forward<typename mse::mstd::tuple<_Types...>::base_class>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr const _Ty&& get(const mse::mstd::tuple<_Types...>&& _Tuple) noexcept {
		return get<_Ty>(std::forward<const typename mse::mstd::tuple<_Types...>::base_class>(_Tuple));
	}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif /*__clang__*/

	template<class... _Types>
	struct tuple_size<mse::mstd::tuple<_Types...>>
		: integral_constant<size_t, sizeof...(_Types)>
	{	// size of tuple
	};

	template<size_t _Index>
	struct tuple_element<_Index, mse::mstd::tuple<>>
	{	// enforce bounds checking
		static_assert(mse::impl::_Always_false<integral_constant<size_t, _Index>>,
			"tuple index out of bounds");
	};

	template<class _This,
		class... _Rest>
		struct tuple_element<0, mse::mstd::tuple<_This, _Rest...>>
	{	// select first element
		using type = _This;
		using _Ttype = mse::mstd::tuple<_This, _Rest...>;
	};

	template<size_t _Index,
		class _This,
		class... _Rest>
		struct tuple_element<_Index, mse::mstd::tuple<_This, _Rest...>>
		: public tuple_element<_Index - 1, mse::mstd::tuple<_Rest...>>
	{	// recursive tuple_element definition
	};

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/


	template<size_t _Index, class... _Types>
	_NODISCARD constexpr tuple_element_t<_Index, tuple<_Types...>>& get(mse::xscope_tuple<_Types...>& _Tuple) noexcept {
		return get<_Index>(static_cast<typename mse::xscope_tuple<_Types...>::base_class&>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr const tuple_element_t<_Index, mse::xscope_tuple<_Types...>>& get(const mse::xscope_tuple<_Types...>& _Tuple) noexcept {
		return get<_Index>(static_cast<const typename mse::xscope_tuple<_Types...>::base_class&>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr tuple_element_t<_Index, mse::xscope_tuple<_Types...>>&& get(mse::xscope_tuple<_Types...>&& _Tuple) noexcept {
		return get<_Index>(std::forward<typename mse::xscope_tuple<_Types...>::base_class>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr const tuple_element_t<_Index, mse::xscope_tuple<_Types...>>&& get(const mse::xscope_tuple<_Types...>&& _Tuple) noexcept {
		return get<_Index>(std::forward<const typename mse::xscope_tuple<_Types...>::base_class>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr _Ty& get(mse::xscope_tuple<_Types...>& _Tuple) noexcept {
		return get<_Ty>(static_cast<typename mse::xscope_tuple<_Types...>::base_class&>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr const _Ty& get(const mse::xscope_tuple<_Types...>& _Tuple) noexcept {
		return get<_Ty>(static_cast<const typename mse::xscope_tuple<_Types...>::base_class&>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr _Ty&& get(mse::xscope_tuple<_Types...>&& _Tuple) noexcept {
		return get<_Ty>(std::forward<typename mse::xscope_tuple<_Types...>::base_class>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr const _Ty&& get(const mse::xscope_tuple<_Types...>&& _Tuple) noexcept {
		return get<_Ty>(std::forward<const typename mse::xscope_tuple<_Types...>::base_class>(_Tuple));
	}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif /*__clang__*/

	template<class... _Types>
	struct tuple_size<mse::xscope_tuple<_Types...>>
		: integral_constant<size_t, sizeof...(_Types)>
	{	// size of tuple
	};

	template<size_t _Index>
	struct tuple_element<_Index, mse::xscope_tuple<>>
	{	// enforce bounds checking
		static_assert(mse::impl::_Always_false<integral_constant<size_t, _Index>>,
			"tuple index out of bounds");
	};

	template<class _This,
		class... _Rest>
		struct tuple_element<0, mse::xscope_tuple<_This, _Rest...>>
	{	// select first element
		using type = _This;
		using _Ttype = mse::xscope_tuple<_This, _Rest...>;
	};

	template<size_t _Index,
		class _This,
		class... _Rest>
		struct tuple_element<_Index, mse::xscope_tuple<_This, _Rest...>>
		: public tuple_element<_Index - 1, mse::xscope_tuple<_Rest...>>
	{	// recursive tuple_element definition
	};

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/
}

namespace mse {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif /*__GNUC__*/
#endif /*__clang__*/

	namespace self_test {
		class CTupleTest1 {
		public:
			template<class Tuple, std::size_t N>
			struct TuplePrinter {
				static void print(const Tuple& t)
				{
					TuplePrinter<Tuple, N - 1>::print(t);
					std::cout << ", " << std::get<N - 1>(t);
				}
			};

			template<class Tuple>
			struct TuplePrinter<Tuple, 1> {
				static void print(const Tuple& t)
				{
					std::cout << std::get<0>(t);
				}
			};
			// helper function to print a tuple of any size
			template<class... Args>
			static void print(const std::tuple<Args...>& t)
			{
				std::cout << "(";
				TuplePrinter<decltype(t), sizeof...(Args)>::print(t);
				std::cout << ")\n";
			}
			// end helper function

			static void s_test1() {
#ifdef MSE_SELF_TESTS
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple */
					struct CB {
						static mse::mstd::tuple<double, char, std::string> get_student(int id)
						{
							if (id == 0) return mse::mstd::make_tuple(3.8, 'A', "Lisa Simpson");
							if (id == 1) return mse::mstd::make_tuple(2.9, 'C', "Milhouse Van Houten");
							if (id == 2) return mse::mstd::make_tuple(1.7, 'D', "Ralph Wiggum");
							throw std::invalid_argument("id");
						}
					};

					{
						auto student0 = CB::get_student(0);
						std::cout << "ID: 0, "
							<< "GPA: " << std::get<0>(student0) << ", "
							<< "grade: " << std::get<1>(student0) << ", "
							<< "name: " << std::get<2>(student0) << '\n';

						double gpa1;
						char grade1;
						std::string name1;
						std::tie(gpa1, grade1, name1) = CB::get_student(1);
						std::cout << "ID: 1, "
							<< "GPA: " << gpa1 << ", "
							<< "grade: " << grade1 << ", "
							<< "name: " << name1 << '\n';

#ifdef MSE_HAS_CXX17
						// C++17 structured binding:
						auto[gpa2, grade2, name2] = CB::get_student(2);
						std::cout << "ID: 2, "
							<< "GPA: " << gpa2 << ", "
							<< "grade: " << grade2 << ", "
							<< "name: " << name2 << '\n';
#endif // MSE_HAS_CXX17
					}
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/tuple */
					{
						mse::mstd::tuple<int, std::string, double> t1;
						std::cout << "Value-initialized: "; print(t1);
						mse::mstd::tuple<int, std::string, double> t2(42, "Test", -3.14);
						std::cout << "Initialized with values: "; print(t2);
						//mse::mstd::tuple<char, std::string, int> t3(t2);
						//std::cout << "Implicitly converted: "; print(t3);
						mse::mstd::tuple<int, double> t4(std::make_pair(42, 3.14));
						std::cout << "Constructed from a pair"; print(t4);
					}
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/swap */
					mse::mstd::tuple<int, std::string, double> p1, p2;
					p1 = mse::mstd::make_tuple(10, "test", 3.14);
					p2.swap(p1);
					std::cout << "(" << std::get<0>(p2)
						<< ", " << std::get<1>(p2)
						<< ", " << std::get<2>(p2) << ")\n";
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/swap */
					struct CB {
						static mse::mstd::tuple<int, int> f() // this function returns multiple values
						{
							int x = 5;
							return mse::mstd::make_tuple(x, 7); // return {x,7}; in C++17
						}
					};

					{
						// heterogeneous tuple construction
						int n = 1;
						auto t = mse::mstd::make_tuple(10, "Test", 3.14, std::ref(n), n);
						n = 7;
						std::cout << "The value of t is " << "("
							<< std::get<0>(t) << ", " << std::get<1>(t) << ", "
							<< std::get<2>(t) << ", " << std::get<3>(t) << ", "
							<< std::get<4>(t) << ")\n";

						// function returning multiple values
						int a, b;
						std::tie(a, b) = CB::f();
						std::cout << a << " " << b << "\n";
					}
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/tuple_cat */
					{
						std::tuple<int, std::string, double> t1(10, "Test", 3.14);
						int n = 7;
						auto t2 = std::tuple_cat(t1, std::make_pair("Foo", "bar"), t1, std::tie(n));
						n = 10;
						print(t2);
					}
				}
#endif // MSE_SELF_TESTS
			}

#ifdef __clang__
#pragma clang diagnostic pop
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /*__GNUC__*/
#endif /*__clang__*/
#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

		};
	}

} // namespace mse

# endif //MSETUPLE_H_
