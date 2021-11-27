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



#ifndef COB_H
#define COB_H
#include <iostream>
#include <vector>
#include <algorithm> 
#include <list>
#include <Eigen/Core>
#include "aux_sys.h"
#include "Coefficients.h"
#include "BNAlgebra.h"


/// \file Cob.h
/// \brief declares the universal cobordism category from \cite BarNatanKhT
///
/// The universal cobordism category is implemented as a category in terms of objects (CobObj) and morphisms between them (CobMor, Deco).
///
/// \todo precompute cobordism algebra to speed up computation


class CobObj;
template<typename Coeff> class CobMor;

// Introduce new common types:
typedef unsigned int TE; ///< tangle end of a two-sided (top/bottom) tangle; tangle ends are enumerated, starting at 0, from the top left to the top right, then bottom left to bottom right. 
typedef std::vector<TE> Arcs; ///< arc-format for tangles; the \f$i^\text{th}\f$ entry is the tangle end connected to the tangle end \f$i\f$.
typedef std::vector<std::pair<TE,TE>> Pairs; ///< pair-format for tangles; consists of all pairs \f$(i,j)\f$ with \f$i<j\f$ for which the tangle connects the tangle ends \f$i\f$ to \f$j\f$. 



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  CobObj
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// a crossingless two-sided (top/bottom) tangle, ie object of the cobordism category

/// An object consists of the number of top tangle ends, the number of bottom tangle ends, arcs (Arcs), a homological grading, and a quantum grading.
///


class CobObj
{
private:
     TE top;
     TE bot;
     Arcs arcs;
     Grading h;
     Grading q;

     Pairs pairs() const;///< converts arc-format (Arcs) into pair-format (Pairs)

public:
     CobObj (
          TE top,
          TE bot,
          Arcs arcs,
          Grading h = 0,
          Grading q = 0 );///< standard constructor
     CobObj ( TE size );///< constructor of the identity tangle with 'size' strands

     //                     //
     // getters and setters //
     //                     //
     TE get_top() const;///< number of top tangle ends
     TE get_bot() const;///< number of bottom tangle ends
     TE ends() const;///< total number of tangle ends
     Arcs get_arcs() const;///< arc-format for object
     Grading get_h() const;///< homological grading of object
     bool h_is_even() const;///< true if the homological grading of object is even
     Grading get_q() const;///< quantum grading of object
     Grading get_delta2() const;///< twice the δ-grading of object
     void set_h ( const Grading &new_h );///< set homological grading of object
     void set_q ( const Grading &new_q );///< set quantum grading of object
     void shift_hq ( const Grading &h_shift,
                     const Grading &q_shift );///< shift gradings of object

     //                          //
     // output and sanity checks //
     //                          //
     std::string to_string() const;///< string representative of a cobordism object
     void print() const;///< print representative of a cobordism object in terminal
     bool check() const;///< true if the object is a well-defined crossingless tangle
     bool compatible_with ( const CobObj obj ) const;///< true if a cobordisms between these objects exist.

     //              //
     // main methods //
     //              //
     CobObj OneSided() const;///< crossingless tangle obtained by rotating the bottom half of the tangle to the top right such that the last top and last bot tangle end sit next to each other
     IndexLL components_to ( const CobObj &obj ) const;///< list of components from one crossingless tangle to another
     BNObj to_BNObj() const;///< corresponding object of the Bar-Natan algebra (BNObj); this only works for the basic crossingless tangles \ref c1, \ref b1, etc.
     void AddCap ( const TE &i );///< Crossingless tangle obtained by adding two extra ends before the \f$i^\text{th}\f$ tangle end (which should be at the tangle bottom) and connect them by an arc.
     void AddCup ( const TE &i );///< Crossingless tangle obtained by joining the \f$i^\text{th}\f$ tangle end (which should be at the tangle bottom) to the next and removing the new closed component if there is one.
     bool AddCupGivesClosedComponent ( const TE &i ) const; ///< true if the two ends were connected (=new closed component); false if the two ends belonged to different arcs

     //                      //
     // overloaded operators //
     //                      //
     bool operator== ( const CobObj &obj2 ) const;///< true if idempotents coincide (ignore gradings)
     bool operator!= ( const CobObj &obj2 ) const;///< true if idempotents differ (ignore gradings)
};

//               //
// basic objects //
//               //
const CobObj c1 {1,3,{1,0,3,2},0,0};///< basic 1-3-ended crossingless tangle: vertical tangle strands/hollow dot
const CobObj b1 {1,3,{3,2,1,0},0,0};///< basic 1-3-ended crossingless tangle: horizontal tangle strands/solid dot
const CobObj c2 {2,2,{2,3,0,1},0,0};///< basic 2-2-ended crossingless tangle: vertical tangle strands/hollow dot
const CobObj b2 {2,2,{1,0,3,2},0,0};///< basic 2-2-ended crossingless tangle: horizontal tangle strands/solid dot
const CobObj c3 {3,1,{3,2,1,0},0,0};///< basic 3-1-ended crossingless tangle: vertical tangle strands/hollow dot
const CobObj b3 {3,1,{1,0,3,2},0,0};///< basic 3-1-ended crossingless tangle: horizontal tangle strands/solid dot
const CobObj a0 {1,1,{1,0},0,0};///< basic 1-1-ended crossingless tangle: solid dot
const CobObj a1 {2,0,{1,0},0,0};///< basic 2-0-ended crossingless tangle: solid dot


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Deco
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Forward declare templates for Deco and friends of Deco:
template<typename Coeff>
class Deco;
template<typename Coeff>
bool deco_is_zero ( const Deco<Coeff> &deco );///< true if the coefficient is 0; this function is used in CobMor::simplify().


typedef std::vector<bool> Dots;///<  container type for dot decoration of cobordisms: the  \f$i^\text{th}\f$ entry is true iff the \f$i^\text{th}\f$ component of a cobordism carries a dot. 

/// auxillary class for the definition of CobMor

/// represents an elementary cobordism of the form \f$a\cdot H^n\cdot C\f$, where \f$a\f$ is a scalar, \f$n\f$ is a non-negative integer and \f$C\f$ is a single cobordism each of whose components carries at most one dot. Here, we treat all components equally, we do not pick a distinguished component in the sense of \cite KWZ.
///

template<typename Coeff> class Deco
{
private:
     int hpower;
     Dots dots;
     Coeff coeff;

public:
     Deco (
          int hpower,
          Dots dots,
          Coeff coeff );///< standard constructor
     friend CobMor<Coeff>;///< friend (CobMor)

     //              //
     // main methods //
     //              //

     /// reorder dots according to the new order of components
     
     /// new_order[old_index]=new_index;
     ///
     /// We assume new_order.size()==dots.size().
     void reorder_old2new ( const IndexL &new_order );

     /// reorder dots according to the new order of components
     
     /// new_order[new_index]=old_index;
     ///
     /// We assume new_order.size()==dots.size()
     void reorder_new2old ( const IndexL &new_order );
     
     /// grading of a decoration (\f$ \#\text{cobordism components}-2\cdot \text{hpower}-2\#\text{dots}\f$)
     Grading gr_q() const; 

     friend bool deco_is_zero <Coeff> ( const Deco<Coeff> &deco );///< check if zero; used in CobMor::simplify().

     void switch_dot ( const size_t &index );///< remove/add a single dot on a single component;
     void add_H();///< increase hpower by 1
     void sub_H();///< decrease hpower by 1

     //                      //
     // overloaded operators //
     //                      //
     bool operator== ( const Deco<Coeff> &deco2 ) const;///< true if decos agree, ignoring coefficients
     bool operator!= ( const Deco<Coeff> &deco2 ) const;///< true if decos differ, ignoring coefficients
     bool operator< ( const Deco<Coeff> &deco2 ) const;///< true if decos are ordered lexicographically (first dots, then hpower), ignoring coefficients. We assume, but do not check that the components are in the same order.
     Deco<Coeff> operator* ( const Deco<Coeff> &deco2 ) const;///< concatenate two (partial) decorations
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  CobMor
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Forward declare templates for friends of CobMor:
//template<typename Coeff> CobMor<Coeff> to_coeffs(const CobMor<int> cob);
// template <typename Coeff>
// using Decos = typename std::vector<Deco<Coeff>>;

/// morphism in the cobordism category 

/// A cobordism consists of two objects, namely front (CobObj) and back (CobObj), and a list of decorations (Deco) together with a list of components.
///
/// A component is a list of tangle ends (\ref TE), starting with the lowest, so that  components can be compared by their first entry. The components are ordered by their first element, which we will sometimes refer to as their 'name'; this allows us to add them together efficiently, without reordering dots.  A component always has an even number of tangle ends.  Adjacent entries are connected via arcs in the two tangles (alternatingly front and back); the second entry is the tangle end that is connected to the first via the front tangle.
template<typename Coeff> class CobMor
{
private:
     CobObj front;
     CobObj back;
     std::vector<Deco<Coeff>> decos;// using lists here instead makes the program marginally faster (219sec vs 223sec). 
     IndexLL comps;

public:
     CobMor (
          CobObj front,
          CobObj back,
          std::vector<Deco<Coeff>> decos,
          IndexLL comps );///< standard constructor, specifying the components; only to be used if the components are known to be in the correct order
     CobMor (
          CobObj front,
          CobObj back,
          std::vector<Deco<Coeff>> decos);///< constructor, which generates the list of components automatically; this is slower than the standard constructor
     CobMor ( int i );///< constructor for the zero-cobordism (needed by the Eigen Template Library)
     CobMor();///< constructor for the zero-cobordism (needed by the Eigen Template Library)

     //                     //
     // getters and setters //
     //                     //
     bool is_0() const;///< true iff morphism is zero. We assume that the cobordism is simplified.
     bool is_inv() const;///< true if the cobordism is invertible, ie an invertible multiple of the identity. We assume that the cobordism is simplified.
     Coeff get_first_coeff() const;///< coefficient of the first decoration of the morphism; this is used for extracting the coefficient of cancelled arrows
     
     //                          //
     // output and sanity checks //
     //                          //
     std::string to_string() const;///< string representation of a cobordism;
     void print() const;///< prints string representation of a cobordism in terminal;
     bool check() const;///< true if the cobordism passes some sanity checks
     bool check ( const CobObj &obj_from,
                  const CobObj &obj_to ) const;///< true if the cobordism is zero or goes between the two specified objects
     Grading gr_q() const; ///< grading of a morphism. We assume the cobordism is non-zero. A non-homogeneous morphism raises an exception.
     
     //              //
     // main methods //
     //              //
     CobMor<Coeff> simplify();///< add up all coefficients with same decoration and eliminate decorations with coefficient equal to 0; note that the list of labels might be empty after this operation. 
     BNMor<Coeff> to_BNMor ( TE special_end = 0 ) const;///< corresponding morphism in the Bar-Natan algebra \f$\mathcal{B}\f$ (for four-ended tangles) or in the free polynomial ring in the variable \f$H\f$ (for two-ended tangles); see BNAlgebra.h.
     template<typename New_Coeff>
     CobMor<New_Coeff> to_coeffs() const;///< converts a morphism over the integers \f$\mathbb{Z}\f$ to a morphism over the new ring of coefficients specified by 'New_Coeff'
     
     /// add a component [i,i+1] without any dot. 
     
     ///When applying this to entries in a complex (Complex.h), 'from' and 'to' should come from the already updated list of objects.
     ///
     CobMor<Coeff> AddCap (
          const CobObj &from,
          const CobObj &to,
          const TE &i ) const;
    
     /// Pre- and postcompose the capped-off morphism with the first delooping isomorphisms from Observation 4.18 in \cite KWZ, where the top generator in the middle corresponds to the first generator of the pair. 
    
     /// The output is not necessarily simplified.
     ///
     CobMor<Coeff> AddCup22 (
          const CobObj &start,
          const CobObj &end,
          const TE &i,
          const bool &from,
          const bool &to ) const;
          
     /// Precompose/Postcompose the capped-off morphism with the first  delooping isomorphism from Observation 4.18 in \cite KWZ, where the top generator in the middle corresponds to the first generator of the pair.
          
     /// For CASE 2->1, treat the RHS as 0; 
     ///
     /// For CASE 1->2, treat the LHS as 1. 
     ///     
     /// Note that 1--->0 is ambiguous, but it is treated identically in the two cases.) The output is not necessarily simplified.
     CobMor<Coeff> AddCupMixed (
          const CobObj &start,
          const CobObj &end,
          const TE &i,
          const bool &from,
          const bool &to ) const;
     /// Fourth case of AddCup, in which either two components are joined (and the dots just get added) or a component gets split into two (neck-cutting).  
          
     ///The output is not necessarily simplified.
     ///
     CobMor<Coeff> AddCup11 (
          const CobObj &start,
          const CobObj &end,
          const TE &i ) const;
     
     //                      //
     // overloaded operators //
     //                      //
     bool operator== ( const CobMor<Coeff> &cob2 ) const;///< true if the two morphisms are equal. We assume here that both are simplified (see simplify()).
     void operator*= ( const Coeff &coeff );///< multiplication assignment: with a scalar
     void operator+= ( const CobMor<Coeff> &cob2 );///< addition assignment with another morphism
     CobMor<Coeff> operator- () const;///< negative of a morphism
     CobMor<Coeff> operator* ( const Coeff &scalar ) const;///< multiply a morphism by a scalar
     CobMor<Coeff> operator* ( const CobMor<Coeff> &cob1 ) const;///< compose two morphisms as functions: \f$\mathrm{Mor}(b,c)\otimes\mathrm{Mor}(a,b)\rightarrow\mathrm{Mor}(a,c)\f$ (note the order!). The function raises an exception if the morphisms are incomposable.
     //cobordisms compose as CobMor(b,c)*CobMor(a,b)=CobMor(a,c)
     CobMor<Coeff> operator+ ( const CobMor<Coeff> &cob2 ) const;///< add two morphisms together; no sanity checks are performed. 
};

//                                            //
// non-member functions that relate to CobMor //
//                                            //
template<typename Coeff>
bool CobNonZero (
     const int& row,
     const int& col,
     const CobMor<Coeff> &value );///< true if morphism is zero; this function is needed by the Eigen Template Library

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  BoolVec
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// Wrapper class for some precomputed data needed for the multiplication of CobMor

struct BoolVec
{
     static std::vector<std::vector<Dots>> vec;///< the ith entry is a list of all possible instances of Dots of length i, except the instance with 1 in all entries.
     static std::vector<std::vector<int>> vec_sum;///< vec_sum[i][j] = number of 1s in vec[i][j]. 
     static int max; ///< always equal to vec.size()-1 (and vec_sum.size()-1)
};

/// compute new order for comp_names

/// the output 'new_order' can be used as a function
/// 
/// \code{.cpp}
///    new_order[new_index] = old_index
/// \endcode
///
/// to reference elements in comp_names by their new indices; primarily used for reordering components and deco.dots.
///
/// Assumptions:
///
/// - in AddCapMixed: (if the number of components is the same)
///
///   comp_names should be a permutation of the first entries of each new_comp in new_comps. 
///
/// - In AddCap11:
///
///     Case 1: One component is split into two. Then the larger comp_name cannot be found in comp_names, so the number of old components is returned in that index.
///
///     Case 2: Two component are merged. Then at the index of the merged component, the smaller of the two old_indices is returned.
///
IndexL new_order_new2old (
     const IndexLL &new_comps,
     const IndexL &comp_names );



std::vector<std::vector<TE>> CatalanTangles(const TE &n);

#endif

