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

#include <concepts>
#include <functional>
#include <type_traits>

///
// Concepts
///

/// Concept that checks that the provided function is a member of the provided
/// class, AND is invocable with the given functions
template <typename T, typename F, typename... Args>
concept has_invocable_member_function =
    std::is_member_function_pointer_v<std::decay_t<F>> &&  // Check if function
                                                           // pointer is method
    std::invocable<F, T&, Args...>;  // Ensure function is invocable with
                                     // provided arguments

///
// Custom Types
///

/// Removes ALL qualifiers from a type
/// We remove pointers, const qualifiers, and references from the type
template <typename T>
using remove_qualifiers = std::remove_pointer_t<std::remove_cvref_t<T>>;

///
// Function Templates
///

/**
 * @brief Invokes the given function in the given class
 *
 * This function handles the process of invoking an arbitrary method in any
 * class. We preform the necessary operations to make sure the given function
 * gets called (It's kinda a syntax mess, so we encapsulate it away into this
 * function)
 *
 * We deduce this value at compile time via templates,
 * which allows us to simplify the implementation,
 * and could allow the compiler to implement some optimizations.
 * We also forward all provided variadic arguments to the target function.
 * Finally, we also deduce the return value of the provided function,
 * and return whatever that may be.
 *
 * We also enforce a concept that the provided function
 * can take the given arguments.
 * Your compiler should raise an error if this is not true!
 *
 * @tparam T Type of class to call function on
 * @tparam F Type of class method to call
 * @tparam A Variadic template parameter types
 * @param inst Class instance to work with
 * @param func Class method to call
 * @param args Arguments to pass to called function
 * @return Return type from provided function
 */
template <typename T, typename F, typename... A>
    requires has_invocable_member_function<T, F, A...>
auto invoke_method(T& inst, F&& func, A&&... args) {
    return std::invoke(std::forward<F>(func), inst, std::forward<A>(args)...);
}
