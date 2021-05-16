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



#ifndef CHAIN_H
#define CHAIN_H
#include <iostream>
#include <fstream>
#include <tuple>
#include <map>
#include <vector>
#include <Eigen/SparseCore> // Sparse Matrix Library
#include "aux_sys.h"
#include "Coefficients.h"
#include "BNAlgebra.h"

/// \file Chain.h
/// \brief declares the classes for loop-type complexes over the Bar-Natan algebra, see Complex<BNObj, BNMor, Coeff> 


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Clink
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
template<typename Coeff> class Chain;

/// a single link a Chain 

/// consists of an object (BNObj), a morphism (BNMor), and a boolean value \c rightarrow which is true if the morphism starts at the specified generator. The morphism should consist of a single label or no label (in which case \c rightarrow is undefined). 
///
template<typename Coeff> class Clink
{
private:
     BNObj object; // x
     BNMor<Coeff> morphism; // a
     bool rightarrow; //true x--(a)-> 
     // 'rightarrow' is undefined if morphism.is_0().

public:
     Clink (
          BNObj object,
          BNMor<Coeff> morphism,
          bool rightarrow );///< standard constructor
     friend Chain<Coeff>;///< friend (Chain)

     //                     //
     // getters and setters //
     //                     //
     void set_coeff ( const Coeff &coeff );///< assign a new coefficient to the Clink

     //                          //
     // output and sanity checks //
     //                          //
     
     /// string representation of a Clink
     
     /// examples:
     /// - without grading: \code{.sh} ⬯~~(-S)~>\endcode 
     /// - with grading: \code{.sh} h^-16 q^-47 δ^-15/2 ⬮——D—>\endcode 
     /// - \code{.sh} ⬯~>\endcode is the same as \code{.sh} ⬯~~(S)~>\endcode
     /// - \code{.sh} ⬮—>\endcode is the same as \code{.sh} ⬮——D—>\endcode
     ///
     /// If \c with_label is \c false, a label will still be included if it is necessary for specifying the morphism.
     /// If \c is_4ended is \c false, the morphism will be interpreted as an element of the free polynomial ring in the variable \f$H\f$, see BNAlgebra.h.
     std::string to_string (
          const max_gr_str &max_gr = {},
          const bool &with_grading = true,
          const bool &with_label = true,
          const bool &is_4ended = true ) const;
};


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Khr_curve
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// implements components of \f$\widetilde{\mathrm{Kh}}\f$

/// According to the geography result for components of \f$\widetilde{\mathrm{Kh}}\f$ from \cite KWZ_thin, all such components are either special or rational components, that are completely determined by their lenght, slope, and bigrading (and potentially local system for rationals).
/// \todo handle gradings more gracefully
/// \todo implement local systems (if necessary)
class Khr_curve
{
private:
    int length;
    // (length < 0) => rational curve of length '|length|'
    // (length > 0)  => special curve of length 'length'
    int_coeff slope_p;
    int_coeff slope_q;
    Q delta; // d| unless slope is ∞.
    Q q; // 
    
public:
    Khr_curve ( int length,
                int_coeff slope_p,
                int_coeff slope_q,
                Q delta = 0,
                Q q = 0);///< standard constructor
    std::string to_string () const;///< string representative of a Khr_curve
    
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Chain
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// list of clinks (Clink), representing a single indecomposable summand  of a loop-type complex. If the morphism of the last Clink is non-zero, the chain corresponds to a compact curve; otherwise it corresponds to a non-compact one. 
template<typename Coeff> class Chain
{
private:
     std::vector<Clink<Coeff>> clinks;

public:
     Chain ( std::vector<Clink<Coeff>> clinks );///< standard constructor

     //                     //
     // getters and setters //
     //                     //
     BNObj get_first_object() const;///< object of the first Clink

     //                          //
     // output and sanity checks //
     //                          //
     std::string to_string (
          const max_gr_str &max_gr = {},
          const bool &is_4ended = true
     ) const;///< string representative of chain; the options are the same as Clink::to_string

     //              //
     // main methods //
     //              //
     
     /// convert chain representation of short figure-8s into standard form
     
     /// Convert \code{.sh} ●——(D^n+S^2n)—>●\endcode into \code{.sh}●——(D^n)—>●<~(S^2n)~~\endcode.
     ///
     void fix_shorts();
     void sort();///< sort generators such that the first one has minimal quantum grading, and the first morphism is type S (if possible)
     void concentrate_local_system();///< move all non-trivial coefficients into the first Clink (if the corresponding curve is compact) or make all equal to 1 (if the corresponding curve is non-compact)
     Khr_curve to_Khr_curve () const; ///< attempt to recognize a chain as a component from the clasification of components of \f$\widetilde{\mathrm{Kh}}\f$

     //                      //
     // overloaded operators //
     //                      //
     
     /// standard comparison operator
     
     /// compares the following properties until they differ:
     /// 1. number of Clinks
     /// 2. idempotents
     /// 3. labels of morphism
     /// 4. δ-gradings 
     /// 5. quantum gradings
     bool operator< ( const Chain<Coeff> &chain2 ) const;
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Chains
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// list of chains (Chain)
template<typename Coeff> class Chains
{
private:
     std::vector<Chain<Coeff>> chains;

public:
     Chains ( std::vector<Chain<Coeff>> chains);///< standard constructor

     //                          //
     // output and sanity checks //
     //                          //
     std::string to_string (
          const bool &is_4ended = true,
          const bool &is_Khr = false ) const;///< string representative of chain; the options are the same as Clink::to_string

     //              //
     // main methods //
     //              //
     void fix_shorts();///< apply Chain::fix_shorts() to all components
     void sort();///< apply Chain::sort() to all components
     void concentrate_local_systems(); ///< apply Chain::concentrate_local_system() to all components
};



#endif
