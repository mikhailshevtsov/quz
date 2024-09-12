#ifndef QUZ_META_STRUCT_HPP
#define QUZ_META_STRUCT_HPP

#include "preprocessor.hpp"
#include "index.hpp"
#include "string.hpp"

#include <type_traits>
#include <utility>

namespace quz
{

// Determine if type is a quz-struct
template <typename S, typename = void>
struct is_meta_struct : std::false_type {};

template <typename S>
struct is_meta_struct<S, std::enable_if_t<S::meta_struct_v>> : std::true_type {};

template <typename S>
constexpr bool is_meta_struct_v = is_meta_struct<S>::value;

template <typename S>
concept meta_struct = is_meta_struct_v<std::decay_t<S>>;


// Setup quz-struct
#define QUZ_BEGIN(STRUCT_NAME) \
static constexpr bool meta_struct_v = true; \
\
static constexpr QUZ_STRING_T(STRUCT_NAME) struct_name() noexcept { return {}; } \
\
struct basic_field { using struct_type = STRUCT_NAME; }; \
\
template <std::size_t Index, bool = false> \
class field {}; \
\
static constexpr QUZ_INDEX_T(0)


// Endup quz-struct
#define QUZ_END \
size() noexcept { return {}; }


// Define simple quz-struct field
#define QUZ_FIELD(TYPE, NAME) \
NAME ##_index {}; \
QUZ_DEPAREN(TYPE) NAME; \
\
template <bool IsConst> \
class field<NAME ##_index, IsConst> : public basic_field { \
public: \
    using type = QUZ_DEPAREN(TYPE); \
    static constexpr QUZ_STRING_T(NAME) name() noexcept { return {}; } \
    static constexpr QUZ_STRING_T(TYPE) type_hint() noexcept { return {}; } \
    static constexpr std::size_t index() noexcept { return NAME ##_index; } \
    \
    constexpr decltype(auto) get() { return value; } \
    constexpr decltype(auto) get() const { return value; } \
    \
    constexpr operator decltype(auto)() { return value; } \
    constexpr operator decltype(auto)() const { return value; } \
    \
    constexpr field(std::conditional_t<IsConst, \
        const std::remove_reference_t<type>&, \
        std::remove_reference_t<type>&> value) noexcept : value{value} {} \
    \
private: \
    std::conditional_t<IsConst, \
        const std::remove_reference_t<type>&, \
        std::remove_reference_t<type>&> value; \
}; \
\
constexpr std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_INDEX_T(NAME ##_index)) { return NAME; } \
constexpr const std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_INDEX_T(NAME ##_index)) const { return NAME; } \
constexpr std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_STRING_T(NAME)) { return NAME; } \
constexpr const std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_STRING_T(NAME)) const { return NAME; } \
\
constexpr auto get_field(QUZ_INDEX_T(NAME ##_index)) { return field<NAME ##_index, false>{NAME}; } \
constexpr auto get_field(QUZ_INDEX_T(NAME ##_index)) const { return field<NAME ##_index, true>{NAME}; } \
constexpr auto get_field(QUZ_STRING_T(NAME)) { return field<NAME ##_index, false>{NAME}; } \
constexpr auto get_field(QUZ_STRING_T(NAME)) const { return field<NAME ##_index, true>{NAME}; } \
\
static constexpr QUZ_INDEX_T(NAME ##_index + 1)


#define QUZ_DEFINE_STRUCT(STRUCT_NAME, ...) \
QUZ_BEGIN(STRUCT_NAME) \
QUZ_FOR_EACH(QUZ_FIELD, __VA_ARGS__) \
QUZ_END


#define QUZ_STRUCT(STRUCT_NAME, ...) \
struct STRUCT_NAME { QUZ_DEFINE_STRUCT(STRUCT_NAME, __VA_ARGS__) }


template <meta_struct S, std::size_t Index>
using field_t = typename S::field<Index>;

// Get
template<std::size_t Index, meta_struct S>
constexpr auto& get(S& s) { return s[quz::index<Index>]; }

template<std::size_t Index, meta_struct S>
constexpr auto& get(const S& s) { return s[quz::index<Index>]; }

// Get field
template<std::size_t Index, meta_struct S>
constexpr auto get_field(S& s) { return s.get_field(quz::index<Index>); }

template<std::size_t Index, meta_struct S>
constexpr auto get_field(const S& s) { return s.get_field(quz::index<Index>); }

// For each simple field
template <typename F, meta_struct S, std::size_t... Is>
constexpr void for_each(F&& f, S&& s, std::index_sequence<Is...>) { ( std::forward<F>(f)(quz::get<Is>(std::forward<S>(s))) , ...); }

template <typename F, meta_struct S>
constexpr void for_each(F&& f, S&& s) { for_each(std::forward<F>(f), std::forward<S>(s), std::make_index_sequence<std::decay_t<S>::size()>{}); }

// For each quz-struct
template <typename F, meta_struct S, std::size_t... Is>
constexpr void for_each_field(F&& f, S&& s, std::index_sequence<Is...>) { ( std::forward<F>(f)(quz::get_field<Is>(std::forward<S>(s))) , ...); }

template <typename F, meta_struct S>
constexpr void for_each_field(F&& f, S&& s) { for_each_field(std::forward<F>(f), std::forward<S>(s), std::make_index_sequence<std::decay_t<S>::size()>{}); }

}

#endif // QUZ_META_STRUCT_HPP