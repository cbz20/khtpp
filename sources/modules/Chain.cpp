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



#ifndef CHAIN_TPP
#define CHAIN_TPP
#ifndef CHAIN_H
#include "../headers/Chain.h"
#include "../headers/constants.h"
#endif


/// \file Chain.cpp
/// \brief implements the classes for loop-type complexes over the Bar-Natan algebra, see Complex<BNObj, BNMor, Coeff>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Clink
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
Clink<Coeff>::Clink ( BNObj object, BNMor<Coeff> morphism, bool rightarrow )
     :
     object ( object ),
     morphism ( morphism ),
     rightarrow ( rightarrow )
{
}

//                     //
// getters and setters //
//                     //

template<typename Coeff>
void Clink<Coeff>::set_coeff ( const Coeff &coeff )
{
     morphism.set_coeff ( coeff );
};

//                          //
// output and sanity checks //
//                          //

template<typename Coeff>
std::string Clink<Coeff>::to_string (
     const max_gr_str &max_gr,
     const bool &with_grading,
     const bool &with_label,
     const bool &is_4ended ) const
{
     // the output should look like one of these:
     //h^-1q^3 ●——(-1.D)—>●
     //h^-1q^3 ●<~(-1.S)~~○
     std::string output {};
     std::string mor_str {""};
     if ( with_grading ) {
          output += object.gr_string ( max_gr );
          output+=" ";
     };
     output+= object.to_string ( 0 );
     // true  = 1 = ● = b
     // false = 0 = ○ = c
     if ( morphism.is_0() == false ) {
          int mor = morphism.get_first_type();
          std::string line = "—";// face D
          if ( mor<0 ) {
               // face S
               line = "~";//┄
          };
          if ( rightarrow ) {
               output += line;
          } else {
               if ( mor > 0 ) {
                    output += "<";//←
               } else {
                    output += "<";//⇠
               };
          };
          bool override_label = false;
          if ( ( mor > 1 )
                    || ( mor < -2 )
                    || ( is_id ( morphism.get_first_coeff() ) == false )
                    || ( morphism.get_labels().size() > 1 )
             ) {
               // the last check is just for debugging purposes; there should only be one label anyway
               override_label = true;
          };
          if ( with_label || override_label ) {
               output += line;
               mor_str = morphism.to_string ( 1,is_4ended );
               if ( mor_str.front() == '-' ) {
                    output += "(";
                    output += mor_str;
                    output += ")";
               } else {
                    output += mor_str;
               };
               output += line;
          };
          if ( rightarrow ) {
               if ( mor > 0 ) {
                    output += ">";//→
               } else {
                    output += ">";//⇢
               };
          } else {
               output += line;
          };
     };
     return output;
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Khr_curve
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Khr_curve::Khr_curve ( int length,int_coeff slope_p,int_coeff slope_q, Q delta, Q q ) :
     length ( length ),
     slope_p ( slope_p ),
     slope_q ( slope_q ),
     delta ( delta ),
     q ( q )
{
}

std::string Khr_curve::to_string () const
{
     std::string output;
     //
     if ( length == -1 ) {
          output += "r";
     } else if ( length < 0 ) {
          output += "r" + std::to_string ( -length );
     } else if ( length > 0 ) {
          output += "s" + std::to_string ( length );
     } else {
          std::cerr << "ERROR: There are no Khr_curves of length 0.\n";
          exit ( 1 );
     };
     output += "(";
     if ( slope_q == 0 ) {
          output += "∞";
     } else {
          output += coeff_to_string ( Q ( slope_p,slope_q ) );
     };
     output += ")";
     //
     output += " q^";
     output += coeff_to_string ( q );
     //
     if ( slope_q == 0 ) {
          output += " δ_^";
     } else {
          output += " δ|^";
     };
     output += coeff_to_string ( delta );
     //
     return output;
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Chain
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
Chain<Coeff>::Chain ( std::vector<Clink<Coeff>> clinks )
     :
     clinks ( clinks )
{
}

//                          //
// output and sanity checks //
//                          //

template<typename Coeff>
std::string Chain<Coeff>::to_string (
     const max_gr_str &max_gr,
     const bool &is_4ended
) const
{
     //! converts a single chain to a string

     //! detailed test documentation
     //!

     // the output should look like
     //h^-1q^3 ●—(-1)—>○<~●~>●->●~>
     std::string output {};
     bool first = true;
     for ( const Clink<Coeff> &clink : this->clinks ) {
          if ( first ) {
               output += clink.to_string ( max_gr,1,1,is_4ended );
               first = false;
          } else {
               output += clink.to_string ( max_gr,0,0,is_4ended );
          };
     };
     return output;
};



//                     //
// getters and setters //
//                     //

template<typename Coeff>
BNObj Chain<Coeff>::get_first_object() const
{
     return clinks.front().object;
};



//              //
// main methods //
//              //

template<typename Coeff>
void Chain<Coeff>::fix_shorts ()
{
     if ( clinks.size() == 2 ) {
          // if the chain consists of just two clinks after the conversion from Complexes over the BNAlgebra, both clinks carry the same label. There are two cases:
          std::list<Label<Coeff>> labels {clinks.front().morphism.get_labels() };
          if ( labels.size() > 1 ) {
               // the label is a.D^n+b.S^2n in BNAlgebra.
               // We relabel the first clink by b.S^2n, the second by a.D^n.
               std::list<BNMor<Coeff>> mors {clinks.front().morphism.split() };
               *this = Chain<Coeff> ( {
                    {
                         clinks.front().object,
                         mors.front(),
                         clinks.front().rightarrow
                    },
                    {
                         clinks.back().object,
                         mors.back(),
                         clinks.front().rightarrow==false
                    }
               } );
          } else {
               // there is just one label, namely either a power of D, or S in the BNAlgebra, or of H in k[H].
               // We delete the arrow in the second clink:
               *this = Chain<Coeff> ( {
                    {
                         clinks.front().object,
                         clinks.front().morphism,
                         clinks.front().rightarrow
                    },
                    {
                         clinks.back().object,
                         0,
                         true
                    }
               } );
          };
     };
};


template<typename Coeff>
void Chain<Coeff>::sort ()
{
     if ( clinks.back().morphism.is_0() ) {
          // non-compact curve = arc
          if ( clinks.back().object.get_q() < clinks.front().object.get_q() ) {
               // reverse chain
               std::vector<Clink<Coeff>> new_clinks {};
               for ( auto it = end ( clinks ); it != begin ( clinks )+1; --it ) {
                    new_clinks.push_back ( {
                         // loop backwards through all but the first clink in clinks
                         // ---a--->x becomes x<---a---
                         ( * ( it-1 ) ).object,
                         ( * ( it-2 ) ).morphism,
                         ( * ( it-2 ) ).rightarrow == false
                    } );
               };
               // add the final clink from the old start
               new_clinks.push_back ( {
                    clinks.front().object,
                    BNMor<Coeff> ( clinks.front().object,clinks.front().object,{} ),
                    0
               } );
               clinks = new_clinks;
          };
     } else {
          // compact curve = loop
          int lowest_gr {INT_MAX};
          int lowest_index {0};
          int current_index {0};
          for ( const Clink<Coeff> &clink : clinks ) {
               if ( lowest_gr > clink.object.get_q() ) {
                    lowest_gr = clink.object.get_q();
                    lowest_index = current_index;
               };
               current_index++;
          };
          //
          std::rotate ( clinks.begin(), clinks.begin() + lowest_index, clinks.end() );
          //
          if ( clinks.front().morphism.get_first_type() >0 ) {
               // starts with a D
               // reverse chain
               std::vector<Clink<Coeff>> new_clinks {};
               new_clinks.push_back ( {
                    clinks.front().object,
                    clinks.back().morphism,
                    clinks.back().rightarrow == false
               } );
               for ( auto it = end ( clinks ); it != begin ( clinks )+1; --it ) {
                    new_clinks.push_back ( {
                         // loop backwards through all but the first clink in clinks
                         // ---a--->x becomes x<---a---
                         ( * ( it-1 ) ).object,
                         ( * ( it-2 ) ).morphism,
                         ( * ( it-2 ) ).rightarrow == false
                    } );
               };
               clinks = new_clinks;
          };
     };
};

template<typename Coeff>
void Chain<Coeff>::concentrate_local_system ()
{
     if ( clinks.back().morphism.is_0() ) {
          // non-compact chains = arcs
          for ( auto it = begin ( clinks ); it != end ( clinks )-1; ++it ) {
               ( *it ).morphism.set_coeff ( 1 );
          };
     } else {
          // compact chains = loops
          Coeff new_coeff {1};
          for ( Clink<Coeff> &clink : clinks ) {
               if ( clink.rightarrow ) {
                    new_coeff *= clink.morphism.get_first_coeff();
               } else {
                    new_coeff *= 1/clink.morphism.get_first_coeff();
               };
               clink.morphism.set_coeff ( 1 );
          };
          if ( clinks.front().rightarrow ) {
               clinks.front().morphism.set_coeff ( new_coeff );
          } else {
               clinks.front().morphism.set_coeff ( 1/new_coeff );
          };
     };
};

template<typename Coeff>
Khr_curve Chain<Coeff>::to_Khr_curve () const
{
     // we do not check if the chain actually agrees with
     // a rational or special curve, we just check those
     // features that are sufficient to check based on the
     // geography result for Khr.
     //
     // experimental quantum grading on curves: just take the average
     Q q {0};
     for ( auto clink : clinks ) {
          q += clink.object.get_q();
     };
     size_t N {clinks.size() };
     q *= Q ( 1,N );
     //
     if ( clinks.back().morphism.is_0() ) {
          std::cerr << "ERROR: Failed to convert chain into Khr_curve, because the chain is non-compact.\n";
          exit ( 1 );
     } else if ( N % 2 != 0 ) {
          std::cerr << "ERROR: Failed to convert chain into Khr_curve, because the chain has odd length. This should never happen, unless the chain is non-compact, but it is not.\n";
          exit ( 1 );
     };
     // 1) Test for specials of slope 0 or ∞
     if ( N > 7 ) {
          // test if special of slope 0, eg ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
          // test if special of slope ∞, eg ⬮~~S~>⬯—>⬯~>⬯—>⬯~>⬮—>⬮<~⬯<—⬯<~⬯<—⬯<~⬮<—
          bool first_idem {clinks.front().object.get_idem() };
          // true  = 1 = ● = b
          // false = 0 = ○ = c
          if ( clinks.back().object.get_idem() == first_idem
                    && clinks[N/2-1].object.get_idem() == first_idem
                    && clinks[N/2].object.get_idem() == first_idem ) {
               bool recognized {true};
               // test objects:
               for ( size_t iter = 0 ; iter < N; ++iter ) {
                    if ( iter != 0 && iter != N/2-1 && iter != N/2 && iter != N-1 ) {
                         if ( clinks[iter].object.get_idem() == first_idem ) {
                              recognized = false;
                              break;
                         };
                    };
               };
               // test directions of arrows:
               for ( size_t iter = 0 ; iter < N/2; ++iter ) {
                    if ( !clinks[iter].rightarrow || clinks[N/2 + iter].rightarrow ) {
                         recognized = false;
                         break;
                    };
               };
               if ( recognized && first_idem ) {
                    return Khr_curve ( N/2 - 2,1,0,clinks[1].object.get_delta(),q );
               } else if ( recognized && !first_idem ) {
                    // slope 0
                    return Khr_curve ( N/2 - 2,0,1,clinks[1].object.get_delta(),q );
               };
          };
     };
     // 2) Determine the absolute value of the slope of the curve
     Q delta {0};
     int_coeff slope_p {0};// slope_p=#⬯
     int_coeff slope_q {0};// slope_q=#⬮
     for ( const auto &clink : clinks ) {
          if ( clink.object.get_idem() ) {
               // true  = 1 = ● = b
               ++slope_q;
               delta = clink.object.get_delta();
          } else {
               // false = 0 = ○ = c
               ++slope_p;
          };
     };
     // 3) Compute the length of the curve
     auto length = gcd ( slope_p, slope_q );// this should be a positive even integer.
     if ( length % 2 != 0 ) {
          std::cerr << "ERROR: Failed to convert chain into Khr_curve.\n"
                    << "       The length should be an integer.\n";
          exit ( 1 );
     };
     slope_p /= length;
     slope_q /= length;
     length /= 2;// This is the actual length of the curve.
     // 4) Test for rationals of slope 0 or ∞:
     if ( slope_q == 0 || slope_p == 0) {
          return Khr_curve ( -length,slope_p,slope_q,clinks.front().object.get_delta(),q );
     };
     // 5) determine sign of slope: (∃ ⬮~>⬯ or ⬯<~⬮) => -
     for ( const auto &clink : clinks ) {
          if ( clink.morphism.get_first_type() == -1 ) {
               if ( ( clink.object.get_idem() && clink.rightarrow )
                         || ( !clink.object.get_idem() && !clink.rightarrow ) ) {
                    slope_p *= -1;
                    break;
               };
          };
     };
     // 6) Work out if the curve is special or rational. 
     // Specials of slope > 0 contain a curve segment x=⬮—>⬮~>⬯—>⬯=y such that the curve segments starting at x and y are identical (same idempotents and directions of arrows) until they meet in a fourth curve segment similar to the first. For specials of slope <0, swap ⬮ and ⬯. The first curve segment corresponds to wrapping around the special puncture, so this can tell all rationals and specials of slope ≠ 0,∞ apart. 
     //
     // first take care of 'short' curves; they are always rational:
     if ( N < 9 ) {
          return Khr_curve ( -length,slope_p,slope_q,clinks.front().object.get_delta(),q );
     };
     //
     int length_sign = -1;// default rational
     for ( size_t iter = 0 ; iter < N/2; ++iter ) {
          // find first curve segment it is one of the following two, depending on the slope: 
          // slope < 0: x=⬮—>⬮~>⬯—>⬯=y or x=⬯<—⬯<~⬮<—⬮=y
          // slope > 0: x=⬯—>⬯~>⬮—>⬮=y or x=⬮<—⬮<~⬯<—⬯=y
          // The fourth curve segment is the other one. 
          // iter corresponds to x, iter+3 (cyclically) to y
          bool first_idem {clinks[iter].object.get_idem() };
          bool rightarrow {clinks[iter].rightarrow };
          if ( first_idem == clinks[(iter + 1) % N].object.get_idem() &&
               first_idem != clinks[(iter + 2) % N].object.get_idem() &&
               first_idem != clinks[(iter + 3) % N].object.get_idem() &&
               rightarrow == clinks[(iter + 1) % N].rightarrow &&
               rightarrow == clinks[(iter + 2) % N].rightarrow &&
               first_idem != clinks[(N/2 + iter + 0) % N].object.get_idem() &&
               first_idem != clinks[(N/2 + iter + 1) % N].object.get_idem() &&
               first_idem == clinks[(N/2 + iter + 2) % N].object.get_idem() &&
               first_idem == clinks[(N/2 + iter + 3) % N].object.get_idem() &&
               rightarrow != clinks[(N/2 + iter + 0) % N].rightarrow &&
               rightarrow != clinks[(N/2 + iter + 1) % N].rightarrow &&
               rightarrow != clinks[(N/2 + iter + 2) % N].rightarrow){
               // found first and fourth segment. Now check if the other two curve segments stay parallel:
               bool is_special {true};
               for ( size_t run = 0 ; run < N/2-4; ++run ) {
                    // compare idempotents of generators; there are N-8 of these, so N/2-4 pairs to check.
                    if ( clinks[(N + iter - run -1) % N].object.get_idem() !=
                         clinks[(iter + 4 + run) % N].object.get_idem() ){
                         is_special = false;
                         break;
                    };
               };
               if ( is_special ){
                    for ( size_t run = 0 ; run < N/2-3; ++run ) {
                         // compare arrow; there is one more pair of arrows than pairs of generators.
                         if ( clinks[(N + iter - run -1) % N].rightarrow ==
                              clinks[(iter + 3 + run) % N].rightarrow){
                              is_special = false;
                              break;
                         };
                    };
               };
               if ( is_special ){
                    length_sign = 1;
                    break;
               };
          };
     };
     return Khr_curve ( length_sign*length,slope_p,slope_q,delta,q );
};



//                      //
// overloaded operators //
//                      //
template<typename Coeff>
bool Chain<Coeff>::operator< ( const Chain<Coeff> &chain2 ) const
{
     if ( clinks.size() != chain2.clinks.size() ) {
          return clinks.size() < chain2.clinks.size();
     }
     if ( clinks.front().object.get_idem() != chain2.clinks.front().object.get_idem() ) {
          return clinks.front().object.get_idem();
     }
     /// \todo also compare direction of arrows
     for ( size_t iter = 0; iter < clinks.size(); ++iter ) {
          if ( clinks[iter].morphism.get_first_type() != chain2.clinks[iter].morphism.get_first_type() ) {
               return clinks[iter].morphism.get_first_type() < chain2.clinks[iter].morphism.get_first_type();
          };
     };
     if ( clinks.front().object.get_delta() != chain2.clinks.front().object.get_delta() ) {
          return clinks.front().object.get_delta() < chain2.clinks.front().object.get_delta();
     };
     if ( clinks.front().object.get_q() != chain2.clinks.front().object.get_q() ) {
          return clinks.front().object.get_q() < chain2.clinks.front().object.get_q();
     };
     return false;
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Chains
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
Chains<Coeff>::Chains ( std::vector<Chain<Coeff>> chains )
     :
     chains ( chains )
{
}

//                          //
// output and sanity checks //
//                          //

template<typename Coeff>
std::string Chains<Coeff>::to_string ( const bool &is_4ended, const bool &is_Khr ) const
{
     std::string output {};
     size_t max_h {0};
     size_t max_q {0};
     size_t max_delta {0};
     size_t cur_gr {0};
     for ( const Chain<Coeff> &chain : chains ) {
          cur_gr = std::to_string ( chain.get_first_object().get_h() ).size();
          if ( cur_gr>max_h ) {
               max_h = cur_gr;
          };
          //
          cur_gr = std::to_string ( chain.get_first_object().get_q() ).size();
          if ( cur_gr>max_q ) {
               max_q = cur_gr;
          };
          //
          cur_gr = coeff_to_string ( chain.get_first_object().get_delta() ).size();
          if ( cur_gr>max_delta ) {
               max_delta = cur_gr;
          };
     };
     max_gr_str max_gr { max_h,max_q,max_delta };
     //
     size_t counter {0};
     size_t max_counter {std::to_string ( chains.size() ).size() };
     for ( const Chain<Coeff> &chain : this->chains ) {
          counter++;
          std::string counter_str {std::to_string ( counter ) };
          for ( size_t i = counter_str.size(); i<max_counter; ++i ) {
               output+= " ";
          };
          output += counter_str;
          output += ") ";
          output += chain.to_string ( max_gr,is_4ended );
          if ( is_Khr ) {
               output +=  "     " + chain.to_Khr_curve().to_string() ;
          };
          output += "\n";
     };
     return output;
};

//              //
// main methods //
//              //

template<typename Coeff>
void Chains<Coeff>::fix_shorts ()
{
     for ( Chain<Coeff> &chain : this->chains ) {
          chain.fix_shorts ();
     };
};

template<typename Coeff>
void Chains<Coeff>::sort ()
{
     for ( Chain<Coeff> &chain : this->chains ) {
          chain.sort ();
     };
     std::sort ( chains.begin(), chains.end() );
};

template<typename Coeff>
void Chains<Coeff>::concentrate_local_systems ()
{
     for ( Chain<Coeff> &chain : this->chains ) {
          chain.concentrate_local_system ();
     };
};



// template specialization

#define VALUE int
#include "../specialization/Chain.h"
#define VALUE Z_mod<2>
#include "../specialization/Chain.h"
#define VALUE Z_mod<3>
#include "../specialization/Chain.h"
#define VALUE Z_mod<5>
#include "../specialization/Chain.h"
#define VALUE Z_mod<7>
#include "../specialization/Chain.h"
#define VALUE Z_mod<custom_coeff>
#include "../specialization/Chain.h"
#define VALUE Z_mod<integer_simulant>
#include "../specialization/Chain.h"
#define VALUE Q
#include "../specialization/Chain.h"



#endif
