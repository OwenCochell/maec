/**
 * @file meta.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various metaprograming components
 * @version 0.1
 * @date 2025-10-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <type_traits>

///
// Custom Types
///

/// Removes ALL qualifiers from a type
/// We remove pointers, const qualifiers, and references from the type
template <typename T>
using remove_qualifiers = std::remove_pointer_t<std::remove_cvref_t<T>>;
