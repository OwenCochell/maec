/**
 * @file mstream.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief MAEC stream components
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file defines the interface/components for MAEC streams.
 * MAEC streams are custom objects that support reading and writing
 * from various sources.
 * These mstreams are designed to abstract away the details of streaming data.
 * 
 * Please note, mstreams are not designed to intepret/format/decode the data they are retrieving!
 * They are designed to simply retrieve the data from a source.
 * You should use higher level components for using this data in a meaningful way.
 */

#pragma once

/**
 * @brief Base class for stream operations
 * 
 * This class defines methods to be implemented by stream classes.
 * We provide methods for reading and writing data,
 * and some methods for determining if we are in read or write mode.
 * This class also lays out a basic state framework for determining
 * and setting the state of the stream.
 * 
 * ALL mstream 
 */
class BaseMStream {

};