/**
 * Minimal unit testing library
 *
 * @author Antloop Studios, 2020 <https://antloop.world>
 * @license Unlicense <https://choosealicense.com/licenses/unlicense>
 */

#ifndef ANTLOOP_UNIT_H
#define ANTLOOP_UNIT_H

#include <stdio.h>

/**
 * Usage:
 *
 * void test1() {
 *      test_assert(1 == 1);
 *      test_ok();
 * }
 *
 * void test2() {
 *      test_assert(2 != 1);
 *      test_ok();
 * }
 *
 * int main() {
 *      test_run_all(
 *          &test1,
 *          &test2,
 *      );
 * }
 */

#define test_assert(condition)                                                              \
    if (!(condition))                                                                       \
    {                                                                                       \
        printf("\033[31m KO  \033[34m%s[%d]: \033[0m%s\n", __func__, __LINE__, #condition); \
        return;                                                                             \
    }

#define test_ok()                                             \
    {                                                         \
        printf("\033[32m OK  \033[34m%s\033[0m\n", __func__); \
        return;                                               \
    }

#define test_run_all(...)                                   \
    {                                                       \
        void (*tests[])() = {__VA_ARGS__};                  \
        unsigned ntests = sizeof(tests) / sizeof(tests[0]); \
        for (unsigned t = 0; t < ntests; t++)               \
            tests[t]();                                     \
        printf("\033[35mDONE\033[0m\n");                    \
    }

#endif
