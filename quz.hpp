#ifndef QUZ_HPP
#define QUZ_HPP

#include "quz_index.hpp"
#include "quz_string.hpp"

#include <type_traits>
#include <utility>

namespace quz
{

// Deparenthesize macro parameter
#define QUZ_DEPAREN(X) ESC(ISH X)
#define ISH(...) ISH __VA_ARGS__
#define ESC(...) ESC_(__VA_ARGS__)
#define ESC_(...) VAN ## __VA_ARGS__
#define VANISH

// Determine if type is a quz-struct
struct tag_t {};

template <typename StructT, typename = void>
struct is_quz_struct : std::false_type {};

template <typename StructT>
struct is_quz_struct<StructT, std::enable_if_t<std::is_same_v<typename StructT::quz_tag, quz::tag_t>>> : std::true_type {};

template <typename StructT>
constexpr bool is_quz_struct_v = quz::is_quz_struct<StructT>::value;

template <typename StructT>
concept quz_struct = quz::is_quz_struct_v<std::decay_t<StructT>>;

// Setup quz-struct
#define QUZ_BEGIN(STRUCT_NAME) \
static constexpr QUZ_STRING_T(STRUCT_NAME) struct_name() noexcept { return QUZ_STRING(STRUCT_NAME); } \
\
template <std::size_t N, typename T> \
struct counter; \
\
template <std::size_t Index, quz::quz_struct = STRUCT_NAME> \
struct field {}; \
\
template <std::size_t N, typename T, typename = void> \
struct is_unused_index : std::true_type {}; \
\
template <std::size_t N, typename T> \
struct is_unused_index<N, T, std::enable_if_t<sizeof(counter<N, int>)>> : std::false_type {}; \
\
template <std::size_t N, typename T> \
static constexpr bool is_unused_index_v = is_unused_index<N, T>::value; \
\
template <typename T, std::size_t Start = 0> \
struct find_unused_index : std::conditional_t<is_unused_index_v<Start, T>, std::integral_constant<std::size_t, Start>, find_unused_index<T, Start + 1>> {}; \
\
template <typename T> \
static constexpr std::size_t get_index = find_unused_index<T>::value;

// Endup quz-struct
#define QUZ_END \
using quz_tag = quz::tag_t; \
\
template <typename T> \
struct counter<get_index<quz_tag>, T> {}; \
\
static constexpr std::size_t size_v = get_index<quz_tag>; \
\
static constexpr std::size_t size() noexcept { return size_v; } \


// Define simple quz-struct field
#define QUZ_DEFINE_FIELD(TYPE, NAME) \
template <typename T> \
struct counter<get_index<QUZ_STRING_T(NAME)>, T> {}; \
\
static constexpr std::size_t NAME ##_index = get_index<QUZ_STRING_T(NAME)>; \
\
template <quz::quz_struct StructT>\
struct field<NAME ##_index, StructT> { \
    using struct_type = StructT; \
    using type = QUZ_DEPAREN(TYPE); \
    static constexpr QUZ_STRING_T(NAME) name() noexcept { return QUZ_STRING(NAME); } \
    static constexpr QUZ_STRING_T(TYPE) type_hint() noexcept { return QUZ_STRING(TYPE); } \
    static constexpr std::size_t index() noexcept { return NAME ##_index; } \
    static constexpr auto get(StructT& quz_object) { return quz_object[QUZ_INDEX(NAME ##_index)]; } \
    static constexpr auto get(const StructT& quz_object) { return quz_object[QUZ_INDEX(NAME ##_index)]; } \
}; \
\
constexpr std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_INDEX_T(NAME ##_index)) { return NAME; } \
constexpr const std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_INDEX_T(NAME ##_index)) const { return NAME; } \
constexpr std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_STRING_T(NAME)) { return NAME; } \
constexpr const std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_STRING_T(NAME)) const { return NAME; }

#define QUZ_FIELD(TYPE, NAME) \
QUZ_DEPAREN(TYPE) NAME; \
QUZ_DEFINE_FIELD(TYPE, NAME)

#define QUZ_FIELD_INIT(TYPE, NAME, INIT) \
QUZ_DEPAREN(TYPE) NAME = QUZ_DEPAREN(INIT); \
QUZ_DEFINE_FIELD(TYPE, NAME)

template <quz_struct StructT, std::size_t Index>
using field_t = typename StructT::field<Index>;

// Get
template<std::size_t Index, quz_struct StructT>
constexpr auto& get(StructT& quz_object) { return quz_object[quz::index<Index>]; }

template<std::size_t Index, quz_struct StructT>
constexpr auto& get(const StructT& quz_object) { return quz_object[quz::index<Index>]; }

// Fields
template<std::size_t Index, quz_struct StructT>
constexpr auto get_field() { return quz::field_t<StructT, Index>{}; }

template<quz_struct StructT, std::size_t... Is>
constexpr auto fields(std::index_sequence<Is...>) { return std::make_tuple(quz::get_field<Is, StructT>()...); }

template<quz_struct StructT>
constexpr auto fields() { return fields<StructT>(std::make_index_sequence<std::decay_t<StructT>::size()>{}); }

// For each quz-struct
template <typename Func, quz_struct StructT, std::size_t... Is>
constexpr void foreach(Func&& func, StructT&& quz_object, std::index_sequence<Is...>) { ( std::forward<Func>(func)(quz::get<Is>(std::forward<StructT>(quz_object))) , ...); }

template <typename Func, quz_struct StructT>
constexpr void foreach(Func&& func, StructT&& quz_object) { foreach(std::forward<Func>(func), std::forward<StructT>(quz_object), std::make_index_sequence<std::decay_t<StructT>::size()>{}); }

// For each tuple
template <typename Func, typename Tup, std::size_t... Is>
constexpr void foreach(Func&& func, Tup&& tup_object, std::index_sequence<Is...>) { ( std::forward<Func>(func)(std::get<Is>(std::forward<Tup>(tup_object))) , ... ); }

template <typename Func, typename Tup>
constexpr void foreach(Func&& func, Tup&& tup_object) { foreach(std::forward<Func>(func), std::forward<Tup>(tup_object), std::make_index_sequence<std::tuple_size<std::decay_t<Tup>>::value>{}); }

}

#endif // QUZ_HPP