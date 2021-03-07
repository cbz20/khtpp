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



#ifndef BNALGEBRA_H
#define BNALGEBRA_H
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "aux_sys.h"
#include "Coefficients.h"

/// \file BNAlgebra.h
/// \brief declares the Bar-Natan algebra \f$\mathcal{B}\f$ from \cite KWZ
///
/// The Bar-Natan algebra \f$\mathcal{B}\f$ is implemented as a category in terms of objects (BNObj) and morphisms between them (BNMor, Label).
/// We also use the classes defined here to implement the free polynomial ring \f$k[H]\f$, namely we regard it as the subalgebra \f$\langle \mathrm{id}_\bullet,D_\bullet,D_\bullet^2, D_\bullet^3,\dots\rangle\f$ of \f$\mathcal{B}\f$ based at the idempotent ●. 

class BNObj;
template<typename Coeff> class BNMor;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  BNObj
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef bool Idem;///< idempotent of the Bar-Natan algebra \f$\mathcal{B}\f$: true = 1 = ● (= b = ⦁ = ⬮) and false = 0 = ○ (= c = ∘ = ⬯).



/// object of the Bar-Natan algebra \f$\mathcal{B}\f$

/// An object consists of an idempotent, a homological grading, and a quantum grading.
///
class BNObj
{
private:
     Idem idem;//!< idempotent
     Grading h;
     Grading q;

public:
     BNObj ( Idem idem=0,
             Grading h=0,
             Grading q=0 );///< standard constructor

     //                     //
     // getters and setters //
     //                     //
     
     Grading get_h() const;///< homological grading
     Grading get_q() const;///< quantum grading
     Q get_delta() const;///< δ-grading
     Idem get_idem() const;///< idempotent
     void set_h ( const Grading &new_h );///< set homological grading
     void set_q ( const Grading &new_q );///< set quantum grading
     void shift_hq (
          const Grading &h_shift,
          const Grading &q_shift );///< shift gradings

     //                          //
     // output and sanity checks //
     //                          //
     void print() const;///< print string representation of generator in terminal
     std::string gr_string ( const max_gr_str &max_gr = {} ) const;///< string representation of grading
     std::string to_string ( const bool &with_grading = true ) const;///< string representation of generator
     std::string to_TeX() const;///< TeX representation of generator

     
     
     //                      //
     // overloaded operators //
     //                      //
     bool operator== ( const BNObj &obj2 ) const;///< true if idempotents coincide (ignore gradings)
     bool operator!= ( const BNObj &obj2 ) const;///< true if idempotents differ (ignore gradings)

     //              //
     // main methods //
     //              //
//      Halfgen to_halfgen (
//           const size_t &index,
//           const Side &side );
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Label
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Forward declare templates for Label and friends of Label:
template<typename Coeff>
class Label;
template<typename Coeff>
bool label_is_zero ( const Label<Coeff> &label );///< true if the coefficient of the label is zero. This function is used in BNMor::simplify().


/// auxillary class for the definition of BNMor

/// represents an elementary morphism of the form \f$a\cdot D^n\f$, \f$a\cdot S^n\f$, or \f$a\cdot \mathrm{id}\f$ for some coefficient \f$a\f$ and positive integer \f$n\f$. We sometimes also interpret elements in the subalgebra \f$\langle \mathrm{id}_\bullet,D_\bullet,D_\bullet^2, D_\bullet^3,\dots\rangle\f$ of \f$\mathcal{B}\f$ as elements of the free polynomial ring \f$k[H]\f$. This is the case if the boolean argument 'is_4ended' is false. 
///
template<typename Coeff> class Label
{
private:
     int type;
     // negative: power of S
     // 0: identity
     // positive: power of D
     Coeff coeff;

public:
     Label ( int type, Coeff coeff );///< type is an integer whose absolute value \f$n\f$ is the exponent of \f$S\f$ (if \f$n\leq0\f$) or \f$D\f$ (if \f$n\geq0\f$); coeff is the coefficient \f$a\f$ of the label
     friend BNMor<Coeff>;///< friend (BNMor)

     //                     //
     // getters and setters //
     //                     //
     int get_type() const;///< type of label (\f$n\f$)
     bool is_type ( const bool &faceD ) const; ///< true if \f$n>0\f$, false if \f$n<0\f$, undefined behaviour if \f$n=0\f$.
     Coeff get_coeff() const;///< coefficient of label (\f$a\f$)
     void set_coeff ( const Coeff &set_coeff_to_1 );///< replace coefficient of label (\f$a\f$) by a new one
     //        //
     // output //
     //        //
     std::string to_string (
          const bool &with_coeff = true,
          const bool &is_4ended = true ) const;///< string representation of label; coefficients are included, iff the first argument is true; if second argument is false, non-negative label types (\f$n\f$) are interpreted as powers of the varible \f$H\in k[H]\f$ and negative label types (\f$n\f$) raise exceptions.

     //                      //
     // overloaded operators //
     //                      //
     bool operator== ( const Label<Coeff> &label2 ) const;///< true if label types (\f$n\f$) agree (ignores coefficients)
     bool operator!= ( const Label<Coeff> &label2 ) const;///< true if label types (\f$n\f$) differ (ignores coefficients)
     bool operator< ( const Label<Coeff> &label2 ) const;///< true if label types (\f$n\f$) are ordered (ignores coefficients)

     //              //
     // main methods //
     //              //
     friend bool label_is_zero <Coeff> ( const Label<Coeff> &label );
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  BNMor
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// template<typename Coeff> class BNMor;
//template<typename Coeff> BNMor<Coeff> operator- (const BNMor<Coeff> mor);


/// morphism of the Bar-Natan algebra \f$\mathcal{B}\f$

/// A morphism consists of two objects, namely domain (BNObj) and codomain (BNObj), and a list of labels (Label).
///
template<typename Coeff> class BNMor
{
private:
     BNObj front;
     BNObj back;
     std::list<Label<Coeff>> labels;

public:
     BNMor ( BNObj front,
             BNObj back,
             std::list<Label<Coeff>> labels );///< standard constructor
     BNMor ( int i );///< constructor for the zero-morphism (needed by the Eigen Template Library)
     BNMor();///< constructor for the zero-morphism (needed by the Eigen Template Library)

     //                     //
     // getters and setters //
     //                     //
     
     /// type of same type as faceD and 0 if there is none.
     
     /// results in undefined behaviour unless the morphism is homogeneous of non-zero quantum grading, so that there is:
     /// - no identity component
     /// - at most one type \f$n < 0\f$
     /// - at most one type \f$n > 0\f$
     int get_type ( const bool &faceD ) const;
     
     Coeff get_coeff ( const bool &faceD ) const;///< coefficient of morphism, assuming get_type() returned a non-zero value
     int get_first_type() const;///< type \f$n\f$ of the first label of the morphism; results in undefined behaviour if the morphism is zero.
     Coeff get_first_coeff() const;///< coefficient \f$a\f$ of the first label of the morphism; results in undefined behaviour if the morphism is zero.
     std::list<Label<Coeff>> get_labels() const;///< all labels of the morphism
     void set_coeff ( const Coeff &coeff );///< replace the coefficients of all labels of the morphism
     bool is_0() const;///< true if the morphism is 0. We assume that the morphism is simplified.
     bool is_inv() const;///< true if the morphism is invertible, ie an invertible multiple of the identity. We assume that the morphism is simplified.

     //                          //
     // output and sanity checks //
     //                          //
     std::string to_string (
          bool with_coeff = 1,
          bool is_4ended = true ) const;///< string representation of a morphism; coefficients are included, iff the first argument is true; if second argument is false, non-negative label types (\f$n\f$) are interpreted as powers of the varible \f$H\in k[H]\f$ and negative label types (\f$n\f$) raise exceptions.
     void print ( const bool &is_4ended = true ) const;///< print string representation of morphism in terminal. If the argument is false, non-negative label types (\f$n\f$) are interpreted as powers of the varible \f$H\in k[H]\f$ and negative label types (\f$n\f$) raise exceptions.
     bool check ( const BNObj &obj_from,
                  const BNObj &obj_to ) const;///< true if the morphism is zero or goes between the two specified objects

     //              //
     // main methods //
     //              //
     std::list<BNMor<Coeff>> split();///< split morphism into multiple morphisms with exactly one label; primarily used for morphisms of the form \f$D^m+S^{2m}\f$.
     BNMor simplify();///< add up all coefficients with same label and eliminate labels with coefficient equal to 0; note that the list of labels might be empty after this operation. 
     
     /// converts a morphism over the integers \f$\mathbb{Z}\f$ to a morphism over the new ring of coefficients specified by 'New_Coeff'
     template<typename New_Coeff>
     BNMor<New_Coeff> to_coeffs() const;

     //                      //
     // overloaded operators //
     //                      //
     bool operator== ( const BNMor<Coeff> &mor2 ) const;///< true if the two morphisms are equal. We assume here that both are simplified (see simplify()).
//      BNMor<Coeff> productSide (
//           const BNMor<Coeff> &mor,
//           const Side &side );
     void operator*= ( const Coeff &coeff );///< multiplication assignment: with a scalar
     void operator+= ( const BNMor<Coeff> &mor2 );///< addition assignment with another morphism
     BNMor<Coeff> operator- () const;///< negative of a morphism
     BNMor<Coeff> operator* ( const Coeff &scalar ) const;///< multiply a morphism by a scalar
     BNMor<Coeff> operator* ( const BNMor<Coeff> &mor1 ) const;///< compose two morphisms as functions: \f$\mathcal{B}(b,c)\otimes\mathcal{B}(a,b)\rightarrow\mathcal{B}(a,c)\f$ (note the order!). The function raises an exception if the morphisms are incomposable.
     BNMor<Coeff> operator+ ( const BNMor<Coeff> &mor2 ) const;///< add two morphisms together; no sanity checks are performed. 
};

//                                           //
// non-member functions that relate to BNMor //
//                                           //
template<typename Coeff>
bool BNMorNonZero ( const int &row,
                    const int &col,
                    const BNMor<Coeff> &value );///< true if morphism is zero; this function is needed by the Eigen Template Library
bool arrow_is_shorter ( const int &a, 
                       const int &b );///<  false if a=0; otherwise true if |a|<|b|, assuming both have the same sign. This function is needed in the arrow pushing algorithm.




#endif
