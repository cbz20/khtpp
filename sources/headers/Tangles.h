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



#ifndef TANGLES_H
#define TANGLES_H
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <list>
#include <random>
#include "aux_sys.h"
#include "aux_svg.h"
#include "Coefficients.h"
#include "Cob.h"
#include "Complex.h"


/// \file Tangles.h
/// \brief declares the class Tangle
///
/// \todo implement PD notation
/// \todo undo extra twisting after clean-up and add optimized output with extra parameter '-o'



/// a list of boolean values, representing oriented intersection points of a straight horizontal line with a generic oriented tangle diagram.

/// The boolean values are translated as follows:
/// - 1=↑
/// - 0=↓
typedef std::vector<bool> Cut;

/// represents an elementary tangle in a Morse presentation of a tangle

/// The first entry is either \c x, \c y, \c l, \c r, \c u. These characters are translated into elementary tangles as follows:
///
/// - \c x : ⤬ (crossing with over-strand starting at top left corner)
/// - \c y : ⤫ (crossing with over-strand starting at top right corner)
/// - \c l : ↶ (cap, oriented to the left)
/// - \c r : ↷ (cap, oriented to the right)
/// - \c u : ∪ (cup)
///
/// The second entry is the index of the left strand in a crossing/cap/cup, counted from the left, starting at 0.
typedef std::pair<char,int> Slice;

/// defines an oriented tangle

/// The first entry is a string which is a dot(.)-separated sequence of words of the form \c ci where the pair (\c c,\c i) defines a \ref Slice. The \f$n^\text{th}\f$ word represents the \f$n^\text{th}\f$ elementary tangle in a Morse presentation of the tangle, read from top to bottom.
/// For symmetric knots, the prefix "s." should be added to this string.
/// The second entry is a \ref Cut corresponding to the top of the tangle.
///
/// For example,
///
/// \code{.sh}tangle_input={"l1.x2.y0.x2.y0.x2.u1",{1,1}}\endcode
///
/// corresponds to the tangle
///
/// \image html "PT2m3.svg" width=3cm
///
//// \code{.sh}
//// l1	↑  ↶  ↑
//// x2	↑ ↓  ⤬
//// y0	 ⤫  ↑ ↑
//// x2	↓ ↑  ⤬
//// y0	 ⤫  ↑ ↑
//// x2	↑ ↓  ⤬
//// u1	↑   ͝  ↑
//// \endcode
///
/// and
///
/// \code{.sh}tangle_input={"s.l1.y0.y0.x1.u2",{1}}\endcode
///
/// corresponds to the symmetric knot
///
/// \image html "4_1-1.svg" width=3cm

typedef std::pair<std::string,Cut> tangle_input;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Tangle
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/// consists of a list of slices (\ref Slice) and a list of cuts (\ref Cut)

/// There is also a boolean value \c symmetry which is \c true iff the tangle represents a knot with strong inversion and a boolean value \c fixed_orientations which is \c true iff the orientation on the input tangle string was inconsistent and orientations had to be changed during initialization
class Tangle
{
private:

     std::vector<Slice> slices;
     std::vector<Cut> cuts;
     // orientations of the horizontal lines cutting the tangle
     // into slices; starting at the top.
     //
     //       ↑=+=1      ↓=-=0
     //
     // slices.size()+1==cuts.size()
     bool symmetry;
     bool fixed_orientations;

public:

     /// standard constructor

     /// The pair (\c input, \c top_orient) have the same format as \ref tangle_input. Tangles are built from top to bottom, hence all cuts are determined by this data. If \c auto_correct is \c true, the constructor will attempt to auto-correct the orientation of strands if the input is inconsistent
     Tangle ( std::string input,Cut top_orient, bool auto_correct = true);

     /// construct tangle from File \c file

     /// \todo explain the structure of a file representing a tangle
     Tangle (const File &file);

     Tangle();// constructor of empty tangle

     //                                 //
     // input, output and sanity checks //
     //                                 //
     void to_file( const File &file ) const;///< generates a file \c file.kht which specifies the tangle in the format \c tangle_input  and adds metadata
     bool is_symmetric() const;///< true iff \c symmetry is \c true
     void record_new_orientations( const File &file ) const;///< adds a comment to \c file which records the top \ref Cut; this is used if the input tangle was inconsistent and the constructor changed the top cut to fix orientations.
     std::string to_string ( const int &index,const bool &orient ) const;///< string representation of a single slice of the tangle. If \c orient is \c 1, all strands are oriented, otherwise only  the orientations of caps (↶/↷) are included.
     void print ( const bool &orient=1 ) const;///< prints a string represenation of a tangle into the terminal. \c orient is as in \ref to_string().
     std::string tanglestring() const;///< convert tangle back into input string, the first component of \ref tangle_input
     std::vector<bool> top_orient() const;///< top \ref Cut
     std::vector<bool> bot_orient() const;///< bottom \ref Cut
     std::string draw_svg_orient (
          const Cut &cut,
          const int &ycoord,
          const int &pref_orient=2 ) const;///< svg-string adding orientations according to \c cut at the y-coordinate \c ycoord ; \c pref_orient is the preferred orientation, which is 0 (down), 1 (up) or 2 (both).
     std::string draw_svg_slice_fill (
          const std::vector<Slice> &sliceL,
          const unsigned int &max,
          const unsigned int &level ) const;///< svg-string adding all vertical strands in a slice with a crossing
     std::string draw_svg_slice (
          const unsigned int &si,
          const unsigned int &level ) const;///< svg-string for a single slice
     void to_svg (
          const std::string &metadata,
          const std::string &name,
          const bool &orient = false ,
          const bool &transparent = false ) const;///< create svg file of the tangle diagram
     bool n_ended ( const size_t &n );///< true if tangle is n-ended and has at least one top tangle end (which is the reduction point) and one bottom tangle end
     size_t complexity () const;///< complexity of the tangle; equal to the sum of the cubes of the lengths of all cuts

     //              //
     // main methods //
     //              //

     /// computes the cobordism complex for a tangle \f$T\f$

     /// The first argument is a cobordism complex \f$C'\f$ that we tensor with the cobordim complex \f$C(T)\f$ for \f$T\f$, gluing the top of \f$C(T)\f$ to the bottom of \f$C'\f$. If this argument is empty, we replace it by a complex consisting of a single crossingless tangle, namely the identity tangle on the same number of strands as there are endpoints at the top of \f$T\f$.
     ///
     /// If the second argument is false, some feedback in the terminal concerning the status of the computation will be suppressed.
     template <typename Coeff>
     Complex<CobObj,CobMor,Coeff> CobComplex( Complex<CobObj,CobMor,Coeff> cx = {},const bool &with_feedback = true ) const;
     void flip_orient_at_index( int i );///< changes the orientation of the strand at the index \c i at the bottom of the tangle, counted from the left, starting with 0
     Tangle add( const std::string &twists ) const;///< add a tangle \c twists to the bottom of the tangle. The format of \c twists is as the first component of \ref tangle_input.
     Tangle quotient() const;///< computes the quotient of a symmetric knot under a strong inversion
     Tangle complete_symmetry() const;///< completes a symmetric knot to a full knot
     void doubled();///< double each strand of a tangle diagram. For example, this can be used to generate four-ended tangles from 1-1-knots. For those knots, the framing of the resulting cap-trivial tangle is chosen such that the linking number between the two strands is 0 and the \f$\infty\f$-filling is the unknot filling.
     void simplify_diagram();///< attempt to simplify a tangle diagram. This is currently only implemented for non-symmetric tangles. This function relies on simplify().
     int writhe() const;///< signed number of crossings
};

//                                             //
// non-member functions that relate to Tangles //
//                                             //
Tangle Sakuma ( const std::vector<int> &list1,const std::vector<int> &list2 );///< generates a symmetric knot corresponding to \f$I_1(a_1,\dots,a_n:c_1,\dots,c_n)\f$ from \cite Sakuma.
Tangle rational_tangle ( const int &p, const int &q);///< generates a rational tangle of slope p/q

Tangle Sakuma ( const std::vector<int> &list );///< generates a symmetric knot corresponding to \f$I_2(a_1,\dots,a_n)\f$ from \cite Sakuma.
void rational_quotient (const int &p,
                        const int &q,
                        const std::string &name,
                        const std::string &metadata,
                        std::vector<File> &files );///< generate diagrams for all strong inversions of the 2-bridge knot corresponding to the fraction p/q; we will assume |p|>q>0; see \cite Sakuma.
void rational_knot (const int &p,
                    const int &q,
                    const std::string &name,
                    const std::string &metadata,
                    std::vector<File> &files );///< generate a diagram for the 2-bridge knot corresponding to the fraction p/q; we will assume |p|>q>0.
File interactive_file ( const std::string &defaultpath = "");///< User dialogue for specifying a file (path)
void interactive ( const std::string &metadata,std::vector<File> &files );///< User dialogue for generating tangles and storing them for later use.

std::vector<Slice> to_slices( std::string tanglestring,int cutlength );///< list of slices generated from a tangle string (ame format as the first component of \ref tangle_input). This function will throw error messages if the input is inconsistent. \c cutlength is the number of top tangle ends
std::string slices_to_string( std::vector<Slice> slices );///< concatenates slices along dots (.)
std::string simplify( std::string tanglestring,int cutlength ); ///< attempts to simplify a tangle string. This function should not be called if the \c tanglestring defines a symmetric tangle.

/// attempts to apply Reidemeister I and II moves to reduce the number of crossings
bool simplification_move( std::vector<Slice> &slices );

/// pushes randomly selected slice randomly up or down as far as possible and then attempts to perform a Reidemeister III move or a 'wiggle move' (interchanging x- and y crossings above cups or below caps)
void wiggle_move( std::vector<Slice> &slices );

/// pushes cups up as far as possible
void cleanup_move( std::vector<Slice> &slices );

/// follows a strand at a given level and index one step up
///
/// returns 'r' or 'l' if there is a cap at the next level and index,
/// or 'a' otherwise. 'l' is returned iff the cap is 'r'.
/// Level and index are updated
/// input level must be > 0
char follow_strand_up( const std::vector<Slice> &slices,
					   const bool &symmetry,
					   size_t &level,
					   int &index);

/// follows a strand at a given level and index one step down
///
/// returns 'u' if there is a cup at the next level and index,
///         'U' or 'O' if the strand is involved in an under/over
///             crossing, respectively, and
///         'a' otherwise.
/// level and index are updated
/// input level must be at most slices.size()-1
char follow_strand_down( const std::vector<Slice> &slices,
					     const bool &symmetry,
					     size_t &level,
					     int &index);

/// helper function to keep track of over/under strands
///
/// if c1 in {O,U}, true if and only if c2=c1 or c1 not in {O,U}.
/// if c1 not in {O,U}, c1=c2
bool ou_helper(char &c1,const char &c2);


/// returns level and index of the strand coming out of the
/// first cap that we meet along the upwards path from the point
/// a given level and index, if the strand remains always above or
/// always below all other strands; returns (0,0) otherwise.
/// This function is only used in global_move;
std::pair<size_t,int> find_cap( const std::vector<Slice> &slices,
								size_t level,
								int index,
								char &ou_char,
								const size_t &uptolevel );


/// returns level and index of the strand coming out of the
/// first cup that we meet along the downwards path from the point
/// a given level and index, if the strand remains always above or
/// always below all other strands; returns (0,0) otherwise.
/// This function is only used in global_move;
std::pair<size_t,int> find_cup( const std::vector<Slice> &slices,
								size_t level,
								int index,
								char &ou_char,
								const size_t &uptolevel );

/// erases the strands at indices i and j at a slice x or y.

/// Returns true if slice was erased and updates all inputs;
/// The relation i<j is assumed as for input and guaranteed for output.
/// This function does not check if the move preserves the tangle type.
/// It is only used in push_cup and push_cap;
bool erase_x_or_y ( std::vector<Slice> &slices,
                  size_t &level,
                  int &i,
                  int &j );

/// erases the strands at indices i and j at the top of a cup/bottom of a cap.

/// Returns true if slice was erased and updates all inputs.
/// The relation i<j is assumed as for input and guaranteed for output.
/// This function does not check if the move preserves the tangle type.
/// It is only used in push_cup and push_cap;
bool erase_cup_down_or_cap_up ( std::vector<Slice> &slices,
                  size_t &level,
                  int &i,
                  int &j );

/// erases the strands at indices i and j at the bottom of a cup/top of a cap.

/// This function never erases a slice.
/// The relation i<j is assumed as for input and guaranteed for output.
/// This function does not check if the move preserves the tangle type.
/// It is only used in push_cup and push_cap;
void erase_cup_up_or_cap_down  ( std::vector<Slice> &slices,
                  size_t &level,
                  int &i,
                  int &j );

/// eliminate cup-cap pair by pushing cup upwards through the tangle.

/// If cup-cap pair gives closed component, we add a single cap and cup
/// below the whole tangle afterwards.
/// This function does not check if the move preserves the tangle type.
/// It is only used in global_move.
void push_cup ( std::vector<Slice> &slices,
				size_t level );

/// eliminate cup-cap pair by pushing cap downwards through the tangle.

/// If cup-cap pair gives closed component, we add a single cap and cup
/// below the whole tangle afterwards.
/// This function does not check if the move preserves the tangle type.
/// It is only used in global_move.
void push_cap ( std::vector<Slice> &slices,
				size_t level );

/// attempts to remove a cap-cup pair
bool global_move( std::vector<Slice> &slices );

/// interchange two slices
///
/// This function does not check if the move preserves the tangle type.
/// It is only used in wiggle_move().

void flip_slices(std::vector<Slice> &slices, const int &level );
#endif
