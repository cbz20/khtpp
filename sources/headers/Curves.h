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



#ifndef CURVES_H
#define CURVES_H
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <Eigen/SparseCore> // Sparse Matrix Library
#include "aux_sys.h"
#include "aux_svg.h"
#include "Coefficients.h"
#include "BNAlgebra.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  DoubleArrow
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef unsigned int Level;
typedef std::pair<int,int> Weight;
// (0,0)=infty; otherwise both integers are non-zero

std::string weight_to_string ( const Weight &w );

template<typename Coeff>
class Precurve;

template<typename Coeff>
class DoubleArrow
{
     // elementary matrix;
     // else if (to!=from): crossover arrow
     // else is (to==from): multiply by constant
private:
     Level from;
     Level to;
     Coeff coeff;
     //
public:
     DoubleArrow ( Level from, Level to,Coeff coeff );

     bool is_crossover() const;
     bool is_constant() const;
     Level get_from() const;
     Level get_to() const;
     std::string to_string() const;
     void operator* ( Coeff &x );

     template<typename Coeff_alt>
     friend class Precurve;
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Halfgen
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
typedef bool Side;
template<typename Coeff> class ArcN;

struct HalfgenID
// location of a halfgen in a precurve
{
     size_t arc;
     size_t index;
     Side side; // 0 (left side) or 1 (right side)
     HalfgenID (
          size_t arc=0,
          size_t index=0,
          Side side=0 );
     
     std::string to_string() const;
     bool operator== ( const HalfgenID &other ) const;
     bool operator!= ( const HalfgenID &other ) const;
};

class Halfgen
// generator on a side of an arc
{
private:
     HalfgenID halfgenID;
     int h;
     int q;
     HalfgenID next;
     // if joinlength = 0, next can be anything. Always use
     // joinlength to decide if joined
     int joinlength;
     //     positive = arrow ending at next halfgen
     //     negative = arrow startgen at next halfgen
     //            0 = joined to the puncture
public:
     Halfgen (
          HalfgenID halfgenID= {0,0,0},
          int h=0,
          int q=0,
          HalfgenID next= {0,0,0},
          int joinlength=0 );

     bool disjoinedQ() const;
     bool startQ() const;
     void set_join ( HalfgenID new_next, int new_joinlength );
     std::string to_string() const;

     friend int diverge ( const Halfgen &from,const Halfgen &to );
     // crossover arrow from--->to can be eliminated iff >0
     // crossover arrow from--->to can be moved to the next arc iff =0

     template<typename Coeff> friend class ArcN;
     template<typename Coeff> friend class Precurve;
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  ArcN
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
typedef std::vector<Halfgen> Halfgens;
template <typename Coeff>
using DoubleArrows = typename std::vector<DoubleArrow<Coeff>>;
typedef std::vector<size_t> Permutation;

template<typename Coeff>
class ArcN
{
     // arc neighbourhood
public:
     Halfgens left;
     Halfgens right;
     DoubleArrows<Coeff> mat_left;
     DoubleArrows<Coeff> mat_right;
     Permutation X;
     //permutations go from right to left

     ArcN (
          Halfgens left,
          Halfgens right,
          DoubleArrows<Coeff> mat_left,
          DoubleArrows<Coeff> mat_right,
          Permutation X );

     std::string to_string() const;
     int size();
     size_t other_side ( const Side start_side, const size_t &i ) const;
     // follow the matrices in mat

     void LPUdecompose ( const Side side ); // TODO
     // rearranges the crossover arrows such that they point
     // downwards in mat_left (decreasing index) and upwards in
     // mat_right (increasing index) when side=0(left) and 1
     // (right), respectively
     void sort ( const Side side ); //TODO
     // arrange crossover arrows on mat_left/mat_right such
     // that the ones closest to the face are the shortest
     void remove ( int complexity ); // TODO
     // remove all arrows of complexity 'complexity' (step 2 in
     // [HRW]), ie arrows that we can push off in the direction
     // of the face in 'complexity' steps. The 'complexity'th
     // face is the one where the two curve segments diverge,
     // so 'complexity'=1,2,3,...

     friend class Precurve<Coeff>;
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Precurves
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
template<typename Coeff>
using ArcNs = typename std::vector<ArcN<Coeff>>;

template<typename Coeff>
class Precurve
{
public://private:
     ArcNs<Coeff> arcNs;
     // arcNs[0]=b, arcNs[1]=c
     // b.left=D, c.left=D <-rotationally symmetrical

     Precurve ( ArcNs<Coeff> arcNs );

     std::string to_string() const;
     bool check() const;

     int size() const;
     Halfgen &halfgen ( const HalfgenID &halfgenID );
     const Halfgen &halfgen_copy ( const HalfgenID &halfgenID ) const;
     void add_join (
          const HalfgenID &from,
          const HalfgenID &to,
          const int &joinlength );
     bool compare (
          const HalfgenID &first,
          const HalfgenID &second,
          const Side &side,
          const size_t &depth ) const;
     int half_weight (
          const HalfgenID &first,
          const HalfgenID &second,
          const Side &side ) const;
     // compute \hat{w} and \hut{w}, depending on side
     Weight weight (
          const HalfgenID &first,
          const HalfgenID &second ) const;

     void sort ( const size_t &depth );
     // FIXME: probably want to start at 0.

     void LPUdecompose();
     void add_mat (
          const size_t &arc,
          Side side,
          size_t &start,
          size_t &end,
          const Coeff &coeff );

     // Use svg format to print Curves.
     void to_svg ( const std::string &name ) const;
};



#endif
