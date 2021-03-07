// 
// This file is part of kht++.
// 
// kht++ is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as 
// published by the Free Software Foundation, either version 3 of 
// the License, or (at your option) any later version.
// 
// kht++ is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General 
// Public License along with kht++.  If not, see 
// <https://www.gnu.org/licenses/>.
// 



#ifndef CONSTANTS_H
#define CONSTANTS_H

/// \file constants.h
/// \brief defines some constants used in the program

inline extern const int default_coeff {2};///< Specifies the default ring of coefficients, which is \f$\mathbb{Z}/\f$\c default_coeff; \c custom_coeff should be a prime number.  For details, see Coefficients.h
inline extern const int custom_coeff {11};///< Specifies the a custom ring of coefficients, which is \f$\mathbb{Z}/\f$\c custom_coeff. \c custom_coeff should be a prime not equal to \c 2, \c 3, \c 5, or \c 7.  For details, see Coefficients.h
inline extern const int max_clean_up_iter {100000};///< sets the maximum number of iterations that Complex::clean_up() will go through to put a complex into loop-type form. Currently this is set to 100000. 
inline extern const int wiggle_number {100};///< number of wiggle moves during tangle string simplification attempting a simplification move
inline extern const int wiggle_abort_after {10000};///< maximum number of iterations of unsuccessful attempts to perform a simplifacation move
inline extern const int default_number_of_lines {3};///< constant used in the interactive tangle definition; this constant should be eliminated
inline extern const int integer_simulant {default_coeff * 3 * 5 * 7 * custom_coeff};///< integer \f$N\f$ such that we can do the main computation for all finite fields \f$\mathbb{Z}/p\f$ for prime \f$p\f$ simultaneously, by first computing over \f$\mathbb{Z}/N\f$ and then doing a conversion. 

inline extern const std::string docs_mainpage {"docs/index.html"};///< relative path to the main page of the documentation; the default \c docs/index.html is a symbolic link and may not work under Windows/MacOS
#endif
