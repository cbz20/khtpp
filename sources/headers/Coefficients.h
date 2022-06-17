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



#ifndef COEFFICIENTS_H
#define COEFFICIENTS_H
#include <iostream>
#include <vector>

/// \file Coefficients.h
/// \brief declares all classes that represent rings of coefficients
///
/// At present, finite fields of the form \f$\mathbb{Z}/p\f$ are implemented for prime integers \f$p\f$, as well as the ring of integers \f$\mathbb{Z}\f$ and the field \f$\mathbb{Q}\f$ of rational numbers. 
/// 
/// \todo add rational coefficients via library (boost, mpir, etc)
/// \todo add integer coefficients with certificates for integer overflow free computations

typedef int_fast64_t int_coeff;///< element of the ring of integers \f$\mathbf{Z}\f$
typedef int prime;///< prime number

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Z
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool coeff_is_inv ( const int_coeff &n );///< true iff \c n is invertible, ie either +1 or -1
bool is_id ( const int_coeff &n );///< true iff \c n is equal to +1
bool coeff_is_0 ( const int_coeff &n );///< true iff \c n is equal to 0
std::string which_coeff ( const int_coeff &i );///< returns the string \c Z
std::string coeff_to_string ( const int_coeff &i );///< converts integer to string



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Z_mod<P>
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// the field \f$\mathbb{Z}/p\f$ for prime integers \f$p\f$. We do not check if P is a prime number.
template<prime P> struct Z_mod {
     /// integer between -P and P
     int n;
     
     /// standard constructor
     Z_mod ( int n ) :n ( n%P ) {};

};

template<prime P>
bool coeff_is_inv ( const Z_mod<P> &n );///< true iff \c n is invertible, ie non-zero

template<prime P>
bool is_id ( const Z_mod<P> &n );///< true iff \c n is equal to +1

template<prime P>
bool coeff_is_0 ( const Z_mod<P> &n );///< true iff \c n is equal to 0

template<prime P>
std::string coeff_to_string ( const Z_mod<P> &i );///< converts the coefficient to string

template<prime P>
prime which_P ( const Z_mod<P> &n );///< returns the prime \f$p\f$

template<prime P>
std::string which_coeff ( const Z_mod<P> &n );///< returns the string \c Z_mod<p>

template<prime P>
std::ostream& operator<< (
     std::ostream &out,
     const Z_mod<P> &n );///< stream ouput format for \c Z_mod<P>

template<prime P>
Z_mod<P> operator+ ( const Z_mod<P> &n1,
                     const Z_mod<P> &n2 );///< addition

template<prime P>
void operator+= ( Z_mod<P> &n1,
                  const Z_mod<P> &n2 );///< addition assignment

template<prime P>
Z_mod<P> operator- ( const Z_mod<P> &n );///< negative

template<prime P>
Z_mod<P> operator* ( const Z_mod<P> &n1,
                     const Z_mod<P> &n2 );///< multiplication

template<prime P>
Z_mod<P> operator* ( const int_coeff &n1,
                     const Z_mod<P> &n2 );///< left multiplication with integer

template<prime P>
Z_mod<P> operator* ( const Z_mod<P> &n1,
                     const int_coeff &n2 );///< right multiplication with integer

template<prime P>
void operator*= ( Z_mod<P> &n,
                  const int_coeff &i );///< multiplication assignment with integer

template<prime P>
void operator*= ( Z_mod<P> &n1,
                  const Z_mod<P> &n2 );///< multiplication assignment

template<prime P>
Z_mod<P> operator/ ( const int_coeff &n1,
                     const Z_mod<P> &n2 );///< division; gives error if \c n2 is 0. 



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Q (Rationals)
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// the field \f$\mathbb{Q}\f$ of rational numbers
struct Q {
     /// numerator
     int_coeff p; 
     /// denominator
     int_coeff q; 
     /// standard constructor
     Q ( int_coeff p, int_coeff q ) :p ( p ),q ( q ) {};
     /// constructor from integers
     Q ( int_coeff p ) :p ( p ),q ( 1 ) {};

     Q reduce();///< divide numerator and denominator by their gcd
};

int_coeff gcd ( int_coeff p, int_coeff q );///< gcd of \f$p\f$ and \f$q\f$

std::vector<int> continued_fraction ( int p, int q );///< computes the continued fraction of \f$p/q\f$ such that all coefficients have the same sign

bool coeff_is_inv ( const Q &r );///< true iff \c n is invertible, ie non-zero
bool is_id ( const Q &r );///< true iff \c n is equal to +1
bool coeff_is_0 ( const Q &r );///< true iff \c n is equal to 0
std::string coeff_to_string ( Q r );///< converts the coefficient to string
std::string which_coeff ( const Q &r );///< returns the string \c Q
std::ostream& operator<< ( std::ostream &out,
                           const Q &r );///< stream ouput format for \c Q
bool operator< ( const Q &r1,
                 const Q &r2 );///< comparison operator
bool operator== ( const Q &r1,
                  const Q &r2 );///< true iff both sides define the same rational number 
bool operator!= ( const Q &r1,
                  const Q &r2 );///< true iff the two sides define the distinct rational numbers
Q operator+ ( const Q &r1,
              const Q &r2 );///< addition
void operator+= ( Q &r1,
                  const Q &r2 );///< addition assignment
Q operator- ( const Q &r );///< negative
Q operator* ( const Q &r1,
              const Q &r2 );///< multiplication
Q operator* ( const int_coeff &n1,
              const Q &r2 );///< left multiplication with integer
Q operator* ( const Q &r1,
              const int_coeff &n2 );///< right multiplication with integer
void operator*= ( Q &r,
                  const int_coeff &n );///< multiplication assignment with scalar
void operator*= ( Q &r1,
                  const Q &r2 );///< multiplication assignment
Q operator/ ( const int_coeff &n1,
              const Q &r2 );///< division; gives error if \c n2 is 0. 


#endif
