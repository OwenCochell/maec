/**
 * @file utils.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations of various utility components
 * @version 0.1
 * @date 2023-03-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "utils.hpp"

// template<class T>
// void Collection<T>::add_object(std::unique_ptr<T> ptr) {

//     // Add the unique pointer to our collection:

//     this->objs.push_back(std::move(ptr));
// }

template <class T>
void Collection<T>::add_object(std::unique_ptr<T> &ptr) {

    // Add and move the unique pointer to our collection:

    this->objs.push_back(std::move(ptr));
}

template<class T>
T* Collection<T>::release_object(int index) {

    // Grab and release the object at the position:

    return this->objs.at(index).release();
}

template<class T>
void Collection<T>::free_object(int index) {

    // Grab and release the object at the position:

    this->objs.erase(this->objs.begin() + index);
}
