#ifndef QUZ_INDEX_HPP
#define QUZ_INDEX_HPP

#include <cstddef>
#include <utility>
#include <type_traits>

namespace quz
{

#define QUZ_INDEX(INDEX) quz::index<INDEX>
#define QUZ_INDEX_T(INDEX) decltype(QUZ_INDEX(INDEX))

template <std::size_t N>
using index_t = std::integral_constant<std::size_t, N>;

template <std::size_t N>
constexpr index_t<N> index = index_t<N>{};

// Make reverse index sequence
template <std::size_t, typename>
struct make_reverse_index_sequence_helper;

template <std::size_t N, std::size_t... Is>
struct make_reverse_index_sequence_helper<N, std::index_sequence<Is...>> : std::index_sequence<(N - Is)...> {};

template <std::size_t N>
struct make_reverse_index_sequence : make_reverse_index_sequence_helper<N - 1, decltype(std::make_index_sequence<N>{})> {};

// Compile-time power
consteval std::size_t static_pow(std::size_t base, std::size_t exp) noexcept { return exp == 0 ? 1 : base * static_pow(base, exp - 1); }

// Compile-time index implementaion
template <char... Cs>
struct index_impl
{
    template <std::size_t... Is>
    static constexpr auto get(std::index_sequence<Is...>) noexcept { return index<(... + ((Cs - '0') * static_pow(10, Is)))>; }
};

namespace literals
{
// Integral constant literal
template <char... Cs>
constexpr auto operator""_qi() { return index_impl<Cs...>::get(make_reverse_index_sequence<sizeof...(Cs)>{}); }
}

}

#endif // QUZ_INDEX_HPP