#ifndef VNITEST_MACRO_H
#define VNITEST_MACRO_H

#define VNITEST_STRINGIFY(...) #__VA_ARGS__

#define VNITEST_TO_STRING(...) VNITEST_STRINGIFY(__VA_ARGS__)

#define VNITEST_CONCAT_IMPL(lhs__,...) lhs__##__VA_ARGS__

#define VNITEST_CONCAT(lhs__,...) VNITEST_CONCAT_IMPL(lhs__,__VA_ARGS__)

#ifdef __COUNTER__
#   define VNITEST_UNIQUE_VAR_NAME(...) \
        VNITEST_CONCAT(__VA_ARGS__,__COUNTER__)
#else
#   define VNITEST_UNIQUE_VAR_NAME(...) \
        VNITEST_CONCAT(__VA_ARGS__,__LINE__)
#endif

#endif//VNITEST_MACRO_H
