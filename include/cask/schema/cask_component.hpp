#pragma once

#include <cask/schema/describe.hpp>

#define CASK_PAIR_TYPE(type, name) type
#define CASK_PAIR_NAME(type, name) name
#define CASK_PAIR_NAME_STR(type, name) #name

#define CASK_FIELD_DECL(pair) CASK_PAIR_TYPE pair CASK_PAIR_NAME pair;

#define CASK_FIELD_DESC(StructName, pair) \
    cask::field(CASK_PAIR_NAME_STR pair, &StructName::CASK_PAIR_NAME pair)

#define CASK_CONCAT(a, b) CASK_CONCAT_IMPL(a, b)
#define CASK_CONCAT_IMPL(a, b) a ## b

#define CASK_NARGS(...) CASK_NARGS_IMPL(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define CASK_NARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,N,...) N

#define CASK_FOR_EACH_1(macro, x) macro(x)
#define CASK_FOR_EACH_2(macro, x, ...) macro(x) CASK_FOR_EACH_1(macro, __VA_ARGS__)
#define CASK_FOR_EACH_3(macro, x, ...) macro(x) CASK_FOR_EACH_2(macro, __VA_ARGS__)
#define CASK_FOR_EACH_4(macro, x, ...) macro(x) CASK_FOR_EACH_3(macro, __VA_ARGS__)
#define CASK_FOR_EACH_5(macro, x, ...) macro(x) CASK_FOR_EACH_4(macro, __VA_ARGS__)
#define CASK_FOR_EACH_6(macro, x, ...) macro(x) CASK_FOR_EACH_5(macro, __VA_ARGS__)
#define CASK_FOR_EACH_7(macro, x, ...) macro(x) CASK_FOR_EACH_6(macro, __VA_ARGS__)
#define CASK_FOR_EACH_8(macro, x, ...) macro(x) CASK_FOR_EACH_7(macro, __VA_ARGS__)
#define CASK_FOR_EACH_9(macro, x, ...) macro(x) CASK_FOR_EACH_8(macro, __VA_ARGS__)
#define CASK_FOR_EACH_10(macro, x, ...) macro(x) CASK_FOR_EACH_9(macro, __VA_ARGS__)
#define CASK_FOR_EACH_11(macro, x, ...) macro(x) CASK_FOR_EACH_10(macro, __VA_ARGS__)
#define CASK_FOR_EACH_12(macro, x, ...) macro(x) CASK_FOR_EACH_11(macro, __VA_ARGS__)
#define CASK_FOR_EACH_13(macro, x, ...) macro(x) CASK_FOR_EACH_12(macro, __VA_ARGS__)
#define CASK_FOR_EACH_14(macro, x, ...) macro(x) CASK_FOR_EACH_13(macro, __VA_ARGS__)
#define CASK_FOR_EACH_15(macro, x, ...) macro(x) CASK_FOR_EACH_14(macro, __VA_ARGS__)
#define CASK_FOR_EACH_16(macro, x, ...) macro(x) CASK_FOR_EACH_15(macro, __VA_ARGS__)
#define CASK_FOR_EACH(macro, ...) CASK_CONCAT(CASK_FOR_EACH_, CASK_NARGS(__VA_ARGS__))(macro, __VA_ARGS__)

#define CASK_DESC_1(S, x) CASK_FIELD_DESC(S, x)
#define CASK_DESC_2(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_1(S, __VA_ARGS__)
#define CASK_DESC_3(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_2(S, __VA_ARGS__)
#define CASK_DESC_4(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_3(S, __VA_ARGS__)
#define CASK_DESC_5(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_4(S, __VA_ARGS__)
#define CASK_DESC_6(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_5(S, __VA_ARGS__)
#define CASK_DESC_7(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_6(S, __VA_ARGS__)
#define CASK_DESC_8(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_7(S, __VA_ARGS__)
#define CASK_DESC_9(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_8(S, __VA_ARGS__)
#define CASK_DESC_10(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_9(S, __VA_ARGS__)
#define CASK_DESC_11(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_10(S, __VA_ARGS__)
#define CASK_DESC_12(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_11(S, __VA_ARGS__)
#define CASK_DESC_13(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_12(S, __VA_ARGS__)
#define CASK_DESC_14(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_13(S, __VA_ARGS__)
#define CASK_DESC_15(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_14(S, __VA_ARGS__)
#define CASK_DESC_16(S, x, ...) CASK_FIELD_DESC(S, x), CASK_DESC_15(S, __VA_ARGS__)
#define CASK_DESC(S, ...) CASK_CONCAT(CASK_DESC_, CASK_NARGS(__VA_ARGS__))(S, __VA_ARGS__)

#define CASK_COMPONENT(StructName, ...) \
    struct StructName { \
        CASK_FOR_EACH(CASK_FIELD_DECL, __VA_ARGS__) \
        static cask::RegistryEntry describe() { \
            return cask::describe<StructName>(#StructName, { \
                CASK_DESC(StructName, __VA_ARGS__) \
            }); \
        } \
    };
