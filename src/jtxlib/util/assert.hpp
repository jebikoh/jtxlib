/**
 * This file contains assertion macros
 * (There's only one for now)
 *
 * Assertions are disabled when using CUDA (for now?)
 */
#pragma once
//
// #ifdef CUDA_ENABLED
// #define ASSERT(condition) ((void)0)
// #else
// #include <iostream>
// #ifdef NDEBUG
#define ASSERT(condition) ((void) 0)
// #else
// #define ASSERT(condition) \
//     do {                  \
//         if (!(condition)) { \
//             std::cerr << "Assertion failed: " << #condition << std::endl; \
//             std::cerr << "File: " << __FILE__ << ", Line: " << __LINE__ << std::endl; \
//         } \
//     } while (false)
// #endif
// #endif