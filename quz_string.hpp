#ifndef QUZ_STRING_HPP
#define QUZ_STRING_HPP

#include <cstddef>
#include <utility>
#include <ostream>

namespace quz
{

#define QUZ_STRING(STR) []<std::size_t... Is>(std::index_sequence<Is...>) -> quz::string<#STR[Is]...> { return {}; }(std::make_index_sequence<std::size(#STR) - 1>{})
#define QUZ_STRING_T(STR) decltype(QUZ_STRING(STR))

// String equality
template <std::size_t N, std::size_t... Is>
static constexpr bool str_equal_impl(const char (&left)[N], const char (&right)[N], std::index_sequence<Is...>) noexcept { return ((left[Is] == right[Is]) && ...); }

template <std::size_t N>
constexpr bool str_equal(const char (&left)[N], const char (&right)[N]) noexcept { return str_equal_impl(left, right, std::make_index_sequence<N>{}); }

constexpr bool str_equal(const char* left, const char* right) noexcept { return *left == *right && (*left == '\0' || str_equal(left + 1, right + 1)); }

template <char... Cs>
struct string
{
    static constexpr const char data[sizeof...(Cs) + 1] { Cs..., '\0' };
    static constexpr std::size_t size() noexcept { return sizeof...(Cs); }
    constexpr operator const char*() const noexcept { return data; }
};

namespace literals
{
template <typename Char, Char... Cs>
constexpr string<Cs...> operator"" _qs() { return {}; }
}

template <char... Cs>
constexpr bool operator==(const string<Cs...>& left, const string<Cs...>& right) noexcept { return true; }

template <std::size_t N, char... Cs>
constexpr bool operator==(const char (&left)[N], const string<Cs...>& right) noexcept { return N - 1 == right.size() && str_equal(left, right.data); }

template <std::size_t N, char... Cs>
constexpr bool operator==(const string<Cs...>& left, const char (&right)[N]) noexcept { return left.size() == N - 1 && str_equal(left.data, right); }

template <char... Cs>
constexpr bool operator==(const char* left, const string<Cs...>& right) noexcept { return str_equal(left, right.data); }

template <char... Cs>
constexpr bool operator==(const string<Cs...>& left, const char* right) noexcept { return str_equal(left.data, right); }

}

#endif // QUZ_STRING_HPP
