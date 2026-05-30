/**
 * @file meta.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various metaprogramming components
 * @version 0.1
 * @date 2025-06-15
 *
 * @copyright Copyright (c) 2025
 *
 * This header contains some metaprogramming components in use across maec.
 * Most of these are generic enough to be used in many contexts!
 */

#pragma once

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>

namespace dsp::meta {

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

/// Concept that checks that the provided type is numerical.
/// Integer and floating point types will satisfy this concept
/// TODO: This can be replaced with std::arithmetic?
template <typename T>
concept is_numerical =
    std::floating_point<T> ||  // Check if function is floating point or,
    std::integral<T>;          // Check if function is integral type

/// Concept that checks that the provided type is floating point
template <typename T>
concept is_fp = std::is_floating_point_v<T>;

/// Concept that enforces field types
/// Field sets have addition, subtraction, multiplication, and division
/// operations defined, and they behave identically as the real numbers.
/// This concept ensures two types can preform these operations with each other
template <typename T, typename U = T>
concept field = requires(T x, U y) {
    // We must be able to preform addition

    x + y;
    y + x;

    // We must be able to preform subtraction
    // (In both ways)

    x - y;
    y - x;

    // We must be able to negate the field

    -x;

    // We must be able to multiply the field

    x * y;
    y * x;

    // We must be able to divide the field

    x / y;
    y / x;
};

///
// Custom types
///

/// Removes ALL qualifiers from a type
/// We remove pointers, const qualifiers, and references from the type
template <typename T>
using remove_qualifiers = std::remove_pointer_t<std::remove_cvref_t<T>>;

///
// Meta utilities
///

/**
 * @brief Statically iterates over a range of numbers at compile time
 *
 * We will iterate from I to N, calling the provided function with each index.
 * The index will be provided as a template parameter,
 * allowing for compile time iteration.
 *
 * The provided function MUST accept a single template parameter of the index,
 * and must not have ANY arguments!
 *
 * @tparam I Starting value
 * @tparam N Stopping value
 * @tparam F Type of function to invoke
 * @param f Function to invoke
 */
template <int I, int N, class F>
constexpr void static_for(F f) {
    if constexpr (I < N) {
        f.template operator()<I>();
        static_for<I + 1, N>(f);
    }
}

/**
 * @brief A binary decision between two types
 *
 * We choose between two types based upon a boolean.
 * If true is provided, then we will choose the first type.
 * If false is provided, then we will choose the second type.
 *
 * You can access the choosen type under the 'type' attribute.
 *
 * @tparam flag Boolean to decide which type to use
 * @tparam T Type selected if true
 * @tparam F Type selected if false
 */
template <bool flag, typename T, typename F>
struct choose_type;

template <typename T, typename F>
struct choose_type<true, T, F> {

    using type = T;
};

template <typename T, typename F>
struct choose_type<false, T, F> {

    using type = F;
};

/**
 * @brief Choose a value based upon a constexpr flag
 *
 * This structure will choose a value based upon a given flag.
 * If the flag is true, then we will choose the first value.
 * If the flag is false, then we will choose the second value.
 * The value can be retrieved using the 'val' parameter.
 *
 * This decision will be done at compile time,
 * allowing for values to be selected in a very efficient manner.
 *
 * @tparam flag Flag to choose from
 * @tparam T Type of value to choose
 * @tparam val1 First value, selected if true
 * @tparam val2 Second value, selected if false
 */
template <bool flag, typename T, T val1, T val2>
struct choose_value;

template <typename T, T val1, T val2>
struct choose_value<true, T, val1, val2> {

    T val = val1;
};

template <typename T, T val1, T val2>
struct choose_value<false, T, val1, val2> {

    T val = val2;
};

/**
 * @brief Determines the proper qualifiers for the given type
 *
 * This class determines the proper qualifiers for a type.
 * We check to see if the type is numerical.
 * If so, we specify that the type should be passed by value.
 * Otherwise, we specify that the type should be passed by reference,
 * or constant reference is provided.
 * You can access the determined type by using 'param_type',
 * or 'param_type_c' if you want constant references.
 *
 * This component is used to resolve issues where numerical values are passed
 * by constant reference, which can lead to slowdowns in some cases.
 *
 * @tparam T Type to check
 */
template <typename T>
struct det_qualifier {

    /// Define the type value, without any qualifiers
    using value_type = remove_qualifiers<T>;

    /// Define reference of this type
    using reference = T&;

    /// Define const reference of this type
    using const_reference = const T&;

    /// Determine which type to used based upon numerical value
    using param_type = choose_type<is_numerical<T>, value_type, reference>;

    /// Determine which type to use, but add constant qualifiers
    using param_type_c =
        choose_type<is_numerical<T>, value_type, const_reference>;
};

/**
 * @brief Chooses a type based upon an enum value
 *
 * Given an enum value and a list of types,
 * we choose a type associated with the respective enum.
 * The enum values MUST start at zero and must sequentially increase!
 * Not doing this could lead to index errors, or mismatched types.
 *
 * The types passed are related to the enum order,
 * so the 1st enum will be associated with the 1st type and so on.
 *
 * @tparam E Enum type
 * @tparam V Enum value
 * @tparam Args Args to select
 */
template <typename E, E V, typename... Args>
    requires(std::is_enum_v<E>)
struct enum_type {

    /// Tuple of all arguments
    /// TODO: For later versions, use direct indexing
    using TypeTuple = std::tuple<Args...>;

    /// Type associated with this enum value
    using type = std::tuple_element_t<static_cast<std::size_t>(V), TypeTuple>;
};

}  // namespace dsp::meta
