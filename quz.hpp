#ifndef QUZ_HPP
#define QUZ_HPP

#include "preprocessor.hpp"
#include "index.hpp"
#include "string.hpp"

#include <type_traits>
#include <utility>
#include <variant>

namespace quz
{

// Determine if type is a quz-struct
template <typename StructT, typename = void>
struct is_quz_struct : std::false_type {};

template <typename StructT>
struct is_quz_struct<StructT, std::enable_if_t<StructT::quz_struct_v>> : std::true_type {};

template <typename StructT>
constexpr bool is_quz_struct_v = quz::is_quz_struct<StructT>::value;

template <typename StructT>
concept quz_struct = quz::is_quz_struct_v<std::decay_t<StructT>>;


// Setup quz-struct
#define QUZ_BEGIN(STRUCT_NAME) \
static constexpr bool quz_struct_v = true; \
\
static constexpr QUZ_STRING_T(STRUCT_NAME) struct_name() noexcept { return {}; } \
\
template <std::size_t Index, bool = false> \
class field {}; \
\
struct basic_field { using struct_type = STRUCT_NAME; }; \
\
static constexpr QUZ_INDEX_T(0)


// Endup quz-struct
#define QUZ_END \
size_v {}; \
\
static constexpr std::size_t size() noexcept { return size_v; } \


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


template <quz_struct StructT, std::size_t Index>
using field_t = typename StructT::field<Index>;

// Get
template<std::size_t Index, quz_struct StructT>
constexpr auto& get(StructT& quz_object) { return quz_object[quz::index<Index>]; }

template<std::size_t Index, quz_struct StructT>
constexpr auto& get(const StructT& quz_object) { return quz_object[quz::index<Index>]; }

// Get field
template<std::size_t Index, quz_struct StructT>
constexpr auto get_field(StructT& quz_object) { return quz_object.get_field(quz::index<Index>); }

template<std::size_t Index, quz_struct StructT>
constexpr auto get_field(const StructT& quz_object) { return quz_object.get_field(quz::index<Index>); }

// Fields
template<quz_struct StructT, std::size_t... Is>
constexpr auto fields(StructT&& quz_object, std::index_sequence<Is...>) { return std::make_tuple(std::forward<StructT>(quz_object).get_field(quz::index<Is>)...); }

template<quz_struct StructT>
constexpr auto fields(StructT&& quz_object) { return fields(std::forward<StructT>(quz_object), std::make_index_sequence<quz_object.size()>{}); }

// For each quz-struct
template <typename Func, quz_struct StructT, std::size_t... Is>
constexpr void for_each(Func&& func, StructT&& quz_object, std::index_sequence<Is...>) { ( std::forward<Func>(func)(quz::get<Is>(std::forward<StructT>(quz_object))) , ...); }

template <typename Func, quz_struct StructT>
constexpr void for_each(Func&& func, StructT&& quz_object) { for_each(std::forward<Func>(func), std::forward<StructT>(quz_object), std::make_index_sequence<std::decay_t<StructT>::size()>{}); }

// For each quz-struct
template <typename Func, quz_struct StructT, std::size_t... Is>
constexpr void for_each_field(Func&& func, StructT&& quz_object, std::index_sequence<Is...>) { ( std::forward<Func>(func)(quz::get_field<Is>(std::forward<StructT>(quz_object))) , ...); }

template <typename Func, quz_struct StructT>
constexpr void for_each_field(Func&& func, StructT&& quz_object) { for_each_field(std::forward<Func>(func), std::forward<StructT>(quz_object), std::make_index_sequence<std::decay_t<StructT>::size()>{}); }

}

#endif // QUZ_HPP