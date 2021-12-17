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



#ifndef COMPLEX_H
#define COMPLEX_H
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cassert>
#include <Eigen/Core>
#include <Eigen/SparseCore> // Sparse Matrix Library
#include "aux_sys.h"
#include "Coefficients.h"
#include "Cob.h"
#include "BNAlgebra.h"
// #include "Curves.h"
#include "Chain.h"
#include "constants.h"

/// \file Complex.h
/// \brief declares the category of complexes over a category, such as the Bar-Natan algebra \f$\mathcal{B}\f$ from \cite KWZ or the universal cobordism category from \cite BarNatanKhT
///
/// \todo read/write complexes into backup files to be reused for quick computations like closures or conversion from Z to other coefficients
/// \todo optimize complex and add graphical output for complexes over Z

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Complex_Base
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// base class for complexes

/// A complex consists of a list \ref objects of objects, a matrix \ref diffs of morphisms that defines a differential and a list \ref cancelled_indices of cancelled indices. 
/// 
template<typename Obj, template <typename> typename Mor, typename Coeff> class Complex_Base
{
protected:
     /// list of objects of the complex (corresponds to the underlying 'chain module')
     std::vector<Obj> objects;
     /// matrix defining the differential of the complex
     Eigen::SparseMatrix<Mor<Coeff>> diffs;
     /// list of cancelled indices, which is empty by default; cancel() adds the indices of the cancelled generators, but keeps objects and diffs intact; resize() then actually deletes the objects and resizes diffs according to this list.
     std::list<unsigned int> cancelled_indices;

public:
     Complex_Base (
          std::vector<Obj> objects,
          Eigen::SparseMatrix<Mor<Coeff>> diffs );///< standard constructor
     Complex_Base ();///< constructor of zero object in the category of complexes
     
     //                     //
     // getters and setters //
     //                     //
     std::vector<Obj> get_objects() const;///< list of objects in the complex
     Eigen::SparseMatrix<Mor<Coeff>> get_diffs() const;///< Eigen matrix defining the differential
     std::list<unsigned int> get_cancelled_indices() const;///< list of cancelled indices

     //                          //
     // output and sanity checks //
     //                          //
     std::string to_string () const;///< string representative of a complex
     void print (
          const ssize_t &index_from,
          const ssize_t &index_to ) const;///< print a component of the differential of a complex in terminal
     bool check() const;///< true if the components of the differential d are well-defined and d²=0. Otherwise, this function will give a warning.

     //              //
     // main methods //
     //              //
     void cancel();///< computes fully reduced complex obtained by repeated application of the Cancellation Lemma from \cite KWZ
     void cancel ( const ssize_t &sourceindex,
                   const ssize_t &targetindex );///< implements the Cancellation Lemma from \cite KWZ for a single component of the differential
     void resize(); ///< resizes the list of objects and matrix of differentials according to the list of cancelled indices
     std::pair<int,int> find_invertible() const;///< component of the differential  to which one can apply the Cancellation Lemma from \cite KWZ; if there is no such component, this returns (n,n), where n is the number of objects in the complex
     void isotopy ( const ssize_t &start,
                    const ssize_t &end,
                    const Mor<Coeff> &mor ); ///< implements the Clean-Up Lemma from \cite KWZ, ie performs a basic isotopy along start---(mor)--->end
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//   Complex (Generic Child of Complex_Base)
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// generic child of Complex_Base
template <typename Obj,
          template <typename> typename Mor,
          typename Coeff>
class Complex: public Complex_Base<Obj,Mor,Coeff>
{
public:
     using Complex_Base<Obj,Mor,Coeff>::Complex_Base;
     //  Complex(std::vector<Obj> objects, Eigen::SparseMatrix<Mor> diffs);
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//     Specialization of Complex to Cob
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// Specialization of Complex to the cobordism category (Cob.h)
template <typename Coeff>
class Complex<CobObj,CobMor,Coeff>:
     public Complex_Base<CobObj,CobMor,Coeff>
{
public:
     using Complex_Base<CobObj,CobMor,Coeff>::Complex_Base;

     void print ( const std::string &name = "",
                  const std::string &metadata = "" ) const;///< if name is the empty, print string representative of a complex over the cobordism category into the terminal; if name is non-empty, this prints this string representative into the file specified by 'name'; metadata must be provided.

     void AddCap ( const unsigned int &i );///< computes the cobordism complex  obtained by tensoring with the complex consisting of a single object, which is the identity plus a cap at the tangle end i, counted from the bottom left starting at 0.
     void AddCup ( const unsigned int &i );///< computes the cobordism complex  obtained by tensoring with the complex consisting of a single object, which is the identity plus a cup at the tangle end i, counted from the bottom left starting at 0.
     
     /// computes the cobordism complex  obtained by tensoring with the complex corresponding to a crossing at the tangle end i, counted from the bottom left starting at 0.
     
     /// The complexes for the single crossing look as follows (compare Section 4 in \cite KWZ)
     /// 
     /// for crossing Y, ie ⤫ (XorY == 0):
     /// - if positive crossing (sign==1): \f$h^0  q^1 \parallel \rightarrow h^1 q^2 =\f$
     /// - if negative crossing (sign==0): \f$h^{-1} q^{-2} \parallel \rightarrow h^0 q^{-1}  =\f$
     ///
     /// for crossing X, ie ⤬ (XorY == 1):
     /// - if positive crossing (sign==1): \f$h^0  q^1   = \rightarrow h^1 q^2  \parallel\f$
     /// - if negative crossing (sign==0): \f$h^{-1} q^{-2} = \rightarrow h^0 q^{-1} \parallel\f$
     ///
     void AddCrossing (
          const unsigned int &i,
          const bool &sign,
          const bool &XorY );
     
     Complex<BNObj,BNMor,Coeff> to_BN() const;///< Corresponding complex over the Bar-Natan algebra (or the free polynomial ring in the variable \f$H\f$); this only works if such a conversion is defined for crossingless atangles and cobordisms, ie for four-ended (two-ended) tangles; see also CobMor::to_BNMor() and CobObj::to_BNObj().
     std::string optimize (
          const std::vector<bool> &bot_orient,
          std::string tanglestring = "" );///< add crossings such that the number of generators is minimized, or add tangle according to tanglestring. The returned string represents the crossings that were added.
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//   Specialization of Complex to BNAlgebra
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// Specialization of Complex to the Bar-Natan algebra \f$\mathcal{B}\f$ (BNAlgebra.h)

/// \todo add graphical output (tikz and/or svg)
template <typename Coeff>
class Complex<BNObj,BNMor,Coeff>:
     public Complex_Base<BNObj,BNMor,Coeff>
{
public:
     using Complex_Base<BNObj,BNMor,Coeff>::Complex_Base;

//      Precurve<Coeff> to_precurve() const;
     Complex (const Chains<Coeff> chains);///< constructor from Chains
     
     
     Chains<Coeff> to_chains() const;///< Chains corresponding to the complex over the Bar-Natan algebra, see Chains.h for details.
     Complex<BNObj,BNMor,Coeff> cone ( const int &n ) const;///< cone of the endomorphism on the complex defined by \f$H^n\cdot\mathrm{id}\f$. For example, \f$\widetilde{\mathrm{Kh}}(T)\f$ is the cone of \f$H\cdot\mathrm{id}\f$ on the complex \f$\widetilde{\mathrm{BN}}(T)\f$. If \f$n = 0\f$, this acts like the identity; \f$n<0\f$ raises an exception.
     Complex<BNObj,BNMor,Coeff> operator+ ( const Complex<BNObj,BNMor,Coeff> &cx2) const;///< Take the tensor product between two complexes corresponding to taking the (vertical) tangle sum, ie stack two 2-2-tangles. cx2 is the lower one. 
     void print (
          const std::string &name = "",
          const std::string &metadata = "",
          const bool is_4ended = true, 
          const bool &is_Khr = false ) const;///< if name is empty, print string representation of complex into terminal; if name is non-empty, prints the string representation into the file specified by 'name'. Metadata must be provided. If the complex is loop-type (see is_loop_type()), the complex is first converted into chains (see Chains.h); 'is_4ended' should be false if algebra elements should be interpreted as elements of the free polynomial ring in the variable \f$H\f$ (ie for four-ended tangles). 'is_Khr' should be true if components are expected to be special and rational see \cite KWZ_thin.

     // Basic homotopy along start---(mor)--->end:
//      void isotopySide (
//           const int &start,
//           const int &end,
//           const BNMor<Coeff> &mor,
//           const Side &side );

     bool is_loop_type ( const bool &faceD ) const;///< true if each generator of the complex is connected to at most one other generator along a differential labelled by a power of \f$D\f$ if 'faceD'==true and \f$S\f$ if 'faceD'==false. We assume the complex is fully reduced. 
     bool is_loop_type () const;///< true if each generator of the complex is connected to at most one other generator along a differential labelled by a power of \f$D\f$, and the same for powers of \f$S\f$. We assume the complex is fully reduced. 
     void clean_up_once ( const bool &faceD );///< makes the complex correspond to a precurve in the sense of \cite KWZ which is simply-faced with respect to \f$D\f$ if 'faceD'==true and \f$S\f$ if 'faceD'==false.
     void clean_up ( ssize_t max_iter = max_clean_up_iter );///< attempts to turn a complex into a loop-type complex in the sense of \cite HRW, so it can be converted to chains (Chains)
     void isolate_arrow (
          const size_t &start,
          const size_t &end,
          const bool &faceD );///< implements the main step of the proof of Proposition 5.10 in \cite KWZ, compare also Proposition 4.16 \cite pqMod. After this step, the component of the differential from 'start' to 'end' in the face corresponding to the algebra element \f$D\f$ if 'faceD==true (\f$S\f$ if 'faceD==false) is the only one connecting these two generators in this face. 
};

//                                             //
// non-member functions that relate to Complex //
//                                             //

template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
Complex<Obj,Mor,Coeff> to_coeff ( const Complex<Obj,Mor,Z_mod<integer_simulant>> &cx );///< converts a complex over the integers \f$\mathbb{Z}\f$ to a complex over the new ring of coefficients specified by 'New_Coeff'

template<typename Mor>
bool NonZero ( const Eigen::Index &row, 
               const Eigen::Index &col, 
               const Mor &value );///< true if the component of the Eigen matrix is non-zero; needed by the Eigen Template Library



#endif
