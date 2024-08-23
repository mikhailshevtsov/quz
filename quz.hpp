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



// Setup quz-struct
#define QUZ_STRUCT(STRUCT_NAME, SIZE) \
static constexpr QUZ_STRING_T(STRUCT_NAME) struct_name {}; \
\
template <std::size_t Index, typename = STRUCT_NAME> \
struct field {}; \
\
static constexpr std::size_t size() noexcept { return SIZE; } \
\
using quz_tag = quz::tag_t;


// Define simple quz-struct field
#define QUZ_FIELD(TYPE, NAME, INDEX) \
QUZ_DEPAREN(TYPE) NAME; \
\
constexpr std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_INDEX_T(INDEX)) { return NAME; } \
constexpr const std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_INDEX_T(INDEX)) const { return NAME; } \
constexpr std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_STRING_T(NAME)) { return NAME; } \
constexpr const std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_STRING_T(NAME)) const { return NAME; } \
\
template <typename StructT>\
struct field<INDEX, StructT> { \
    using struct_type = StructT; \
    using type = QUZ_DEPAREN(TYPE); \
    static constexpr std::size_t index() noexcept { return INDEX; } \
    static constexpr QUZ_STRING_T(NAME) name {}; \
    static constexpr QUZ_STRING_T(TYPE) type_hint {}; \
    static constexpr auto get(StructT& object) { return object[QUZ_INDEX(INDEX)]; } \
    static constexpr auto get(const StructT& object) { return object[QUZ_INDEX(INDEX)]; } \
};

// Define quz-struct field with initializer
#define QUZ_FIELD_INIT(TYPE, NAME, INITIALIZER, INDEX) \
QUZ_DEPAREN(TYPE) NAME = QUZ_DEPAREN(INITIALIZER); \
\
constexpr std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_INDEX_T(INDEX)) { return NAME; } \
constexpr const std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_INDEX_T(INDEX)) const { return NAME; } \
constexpr std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_STRING_T(NAME)) { return NAME; } \
constexpr const std::remove_reference_t<QUZ_DEPAREN(TYPE)>& operator[](QUZ_STRING_T(NAME)) const { return NAME; } \
\
template <typename StructT>\
struct field<INDEX, StructT> { \
    using struct_type = StructT; \
    using type = QUZ_DEPAREN(TYPE); \
    static constexpr std::size_t index() noexcept { return INDEX; } \
    static constexpr QUZ_STRING_T(NAME) name {}; \
    static constexpr QUZ_STRING_T(TYPE) type_hint {}; \
};

template <typename StructT, std::size_t Index>
using field_t = typename StructT::field<Index>;

template <typename StructT>
concept quz_struct = quz::is_quz_struct_v<std::decay_t<StructT>>;

// Get
template<std::size_t Index, quz_struct StructT>
constexpr auto& get(StructT& object) { return object[quz::index<Index>]; }

template<std::size_t Index, quz_struct StructT>
constexpr auto& get(const StructT& object) { return object[quz::index<Index>]; }

// Fields
template<std::size_t Index, quz_struct StructT>
constexpr auto get_field(StructT&& object) { return quz::field_t<StructT, Index>{}; }

template<quz_struct StructT, std::size_t... Is>
constexpr auto fields(StructT&& object, std::index_sequence<Is...>) { return std::make_tuple(quz::get_field<Is>(std::forward<std::decay_t<StructT>>(object))...); }

template<quz_struct StructT>
constexpr auto fields(StructT&& object) { return fields(std::forward<StructT>(object), std::make_index_sequence<std::decay_t<StructT>::size()>{}); }

// For each quz-struct
template <typename F, quz_struct StructT, std::size_t... Is>
constexpr void for_each(F&& f, StructT&& object, std::index_sequence<Is...>) { ( std::forward<F>(f)(quz::get<Is>(std::forward<StructT>(object))) , ...); }

template <typename F, quz_struct StructT>
constexpr void for_each(F&& f, StructT&& object) { return for_each(std::forward<F>(f), std::forward<StructT>(object), std::make_index_sequence<std::decay_t<StructT>::size()>{}); }

// For each tuple
template <typename F, typename Tup, std::size_t... Is>
constexpr void for_each(F&& f, Tup&& object, std::index_sequence<Is...>) { ( std::forward<F>(f)(std::get<Is>(std::forward<Tup>(object))) , ...); }

template <typename F, typename Tup>
constexpr void for_each(F&& f, Tup&& object) { return for_each(std::forward<F>(f), std::forward<Tup>(object), std::make_index_sequence<std::tuple_size<std::decay_t<Tup>>::value>{}); }

}

#endif // QUZ_HPP