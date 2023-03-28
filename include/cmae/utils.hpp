/**
 * @file utils.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various utility components
 * @version 0.1
 * @date 2023-03-23
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file contains misc. components not necessarily related to audio generation.
 * These components can be used to aid the the development of components,
 * and can take some edge off the developer.
 * 
 */

#pragma once

#include <memory>
#include <vector>

/**
 * @brief Keeps a collection of objects
 * 
 * This class can be used to keep a collection
 * of arbitrary objects using smart pointers.
 * This allows for objects to be collected and kept,
 * with a guarantee that objects will be freed when they are not needed.
 * 
 * This class can be used in many contexts,
 * but the most popular one is making a container or collection,
 * such as ChainEnvelope, the owner of submitted in components,
 * which will keep and manage them for the developer.
 * Objects can be submitted via smart pointer,
 * or via raw pointer,
 * which we will then convert into a SmartPointer and take ownership over.
 * 
 * @tparam T The type of objects to keep
 */
template <typename T>
class Collection {

    private:

        /// A collection of objects to maintain
        std::vector<std::unique_ptr<T>> objs;

    public:

        Collection() =default;

        /**
         * @brief Adds the given object via unique pointer
         * 
         * We take the given unique pointer and add it to our collection.
         * It is critical that the given smart pointer is moved into our possession!
         * 
         * @param ptr Pointer to save
         */
        // void add_object(std::unique_ptr<T> ptr);

        /**
         * @brief Adds the given object via unique pointer
         * 
         * We take the unique pointer and add it to our collection.
         * We take the given pointer by reference, and move it into our ownership.
         * The passed unique pointer SHOULD NOT use be used after this operation!
         * 
         * @param ptr Pointer to save
         */
        void add_object(std::unique_ptr<T>& ptr);

        /**
         * @brief Gets the object at the given position
         * 
         * We retrieve the object at the given position
         * and return the raw pointer.
         * 
         * YOU SHOULD NO FREE OR ALTER THIS POINTER!
         * This class still has ownership of the pointer,
         * and manipulating it could lead to trouble.
         * 
         * @param index Index of object to retrieve
         * @return T* Raw pointer to retrieved object
         */
        T* get_object(int index) const { return this->objs.at(index).get(); }

        /**
         * @brief Gets the number of objects currently loaded
         * 
         * @return int Number of objects
         */
        int num_objects() const { return this->objs.size(); }

        /**
         * @brief Clears the objects loaded into this collection
         * 
         * This will free ALL given objects!
         * Please be careful when using this method.
         * 
         */
        void clear() { this->objs.clear(); }

        /**
         * @brief Releases the given object from collection
         * 
         * We release an object by getting the raw pointer from the 
         * unique pointer in our collection.
         * We then return the raw pointer to the caller that they can use as they see fit.
         * 
         * @param index Index of object to release
         * @return T* Pointer to released object
         */
        T* release_object(int index);

        /**
         * @brief Frees the object at the given index
         * 
         * @param index INdex of object to free
         */
        void free_object(int index);

};
