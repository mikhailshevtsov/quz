#ifndef QUZ_PREPROCESSOR_HPP
#define QUZ_PREPROCESSOR_HPP

// Recursive macro
#define PARENS ()

#define EXPAND(...)  EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define QUZ_FOR_EACH(MACRO, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(MACRO, __VA_ARGS__)))
#define FOR_EACH_HELPER(MACRO, ARG1, ...)                         \
  MACRO ARG1                                                     \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (MACRO, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

// Deparenthesize macro parameter
#define QUZ_DEPAREN(ARG) ESC(ISH ARG)
#define ISH(...) ISH __VA_ARGS__
#define ESC(...) ESC_(__VA_ARGS__)
#define ESC_(...) VAN ## __VA_ARGS__
#define VANISH

#endif