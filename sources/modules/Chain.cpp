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

template<typename Coeff>
Clink<Coeff>::Clink ( const std::string &s ){
     std::string str {s};
     // 
     // determine object:
     //
     Idem idem {true}; 
     Grading h {0};
     Grading q {0};
     std::string search_str {"h^"};
     auto i {str.find ( search_str )};
     if (i != std::string::npos ){
          try {
               // so there are gradings; we assume the format is:
               // h^\ *-?[0-9]* q^\ *-?[0-9]* δ^\ *-?[0-9]*
               str = str.substr(i+2);// cut after 'h^'
               search_str = "q^";
               i = str.find ( search_str );
               h = std::stoi (str.substr(0,i));// cut before 'q^'
               str = str.substr(i+2);// cut after 'q^'
               search_str = "δ^";
               i = str.find ( search_str );
               q = std::stoi (str.substr(0,i));// cut before 'δ^'
          } catch ( ... ) {
               throw std::invalid_argument( "Invalid grading format in string '" + s + "'.\n");
          };
     };
     // Idem: true = 1 = ● (= b = ⦁ = ⬮) and false = 0 = ○ (= c = ∘ = ⬯).
     search_str = "⬮";//3 bytes long
     i=str.find ( search_str );
     if (i == std::string::npos ){
          search_str = "⬯";//3 bytes long
          i = str.find ( search_str );
          idem = false;
     };
     if (i == std::string::npos ){
          throw std::invalid_argument( "Could not find any object ⬮ or ⬯ in the string '" + s + "'.\n");
     };
     object = BNObj(idem, h, q);
     str = str.substr(i+3);// ⬯ and ⬮ are 3 bytes long
//      std::cout << "1)" << str << " of size " << str.size() << "\n" << std::flush;
     //
     // determine rightarrow
     //
     search_str = ">";
     i = str.find ( search_str );
     rightarrow = true;
     if (i == std::string::npos ){
          search_str = "<";
          rightarrow = false;
          i = str.find ( search_str );
     };
     //
     // morphism
     //
     int type {1};
     Coeff coeff {1};
     try {
          // test if morphism is zero
          if ( str.empty() || i == std::string::npos ){
               morphism = BNMor<Coeff>(idem, idem, {});
          } else {
               // morphism is non-zero; now determine for sign of type
               if ( rightarrow ){
                    search_str = "~";// S-arrow
                    if ( str.substr(0,1) == search_str ){
                         type=-1;
                         str = str.substr(2);// remove leading '~~' and '~>'
                    } else {
                         str = str.substr(3);// remove leading '—'
                         search_str = ">";
                         if ( str.substr(0,1) == search_str ){
                              str = str.substr(1);// remove leading '>'
                         } else {
                              str = str.substr(3);// remove leading '—'
                         };
                    };
               } else {
                    str = str.substr(1);// remove leading '<'
                    search_str = "~";// S-arrow
                    if ( str.substr(0,1) == search_str ){
                         type=-1;
                         str = str.substr(1);// remove leading '~'
                    } else {
                         str = str.substr(3);// remove leading '—'
                    };
               };
               if ( !str.empty() ){
                    // there is a label on the arrowstring
                    // remove trailing '<—', '<~', '—>', '~>', '~~', and '——':
                    if ( type < 0 ){
                         search_str = "~";
                    } else {
                         search_str = "—";
                    };
                    i = str.find ( search_str );
                    str = str.substr(0,i);
//                     std::cout << "2)" << str << "\n" << std::flush;
                    // remove parentheses, if any exist
                    search_str = "(";
                    if ( str.substr(0,1) == search_str ){
                         str = str.substr(1);
                    };
                    search_str = ")";
                    i = str.find ( search_str );
                    str = str.substr(0,i);
//                     std::cout << "3)" << str << "\n" << std::flush;
                    // detect if morphism has negative coefficient
                    search_str = "-";
                    i = str.find ( search_str );
                    if (i != std::string::npos ){
                         coeff = -1;
                         str = str.substr( i+1 );
                    };
//                     std::cout << "4)" << str << "\n" << std::flush;
                    // detect if morphism has coeff ≠ ±1. 
                    search_str = ".";
                    i = str.find ( search_str );
                    if (i != std::string::npos ){
                         coeff *= std::stoi(str.substr(0,i));
                         str = str.substr( i+1 );
                    };
//                     std::cout << "5)" << str << "\n" << std::flush;
                    // determine type:
                    // if stringlabel is S or D, type remains the same
                    // if morphism is S^2, but there is no stringlabel, this needs to be fixed later in Chain::Chain( std::string).
                    search_str = "^";
                    i = str.find ( search_str );
                    if (i != std::string::npos ){
                         type *= std::stoi(str.substr(i+1));
                    };
               };
               auto front {idem};
               auto back {idem};
               if ( type < 0 && ((type-1) % 2 == 0)){
                    if ( rightarrow ){
                         back = !idem;
                    } else {
                         front = !idem;
                    };
               };
               //
               morphism = BNMor<Coeff>(front, back, {Label(type, coeff)});
          };
     } catch (...){
          throw std::invalid_argument( "Invalid arrow format in the string '" + s + "'.\n");
     };     
//      std::cout << this->to_string() << "\n" << std::flush;
//      std::cout << s << "\n\n" << std::flush;
}

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
               output += "<";//⇠
          };
          bool override_label = false;
          if ( ( mor > 1 )
                    || ( mor < -2 )
                    || ( is_id ( morphism.get_first_coeff() ) == false )
                    || ( morphism.get_labels().size()> 1 )
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
               output += ">";//⇢
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

template<typename Coeff>
Chain<Coeff>::Chain ( const std::string s )
     :
     clinks ( {} ) 
{
     std::string str {s};
     const std::string DotB {"⬮"};
     const std::string DotW {"⬯"};
     auto i { str.find( DotB ) };
     if ( i > str.find( DotW ) ) {
          i = str.find( DotW );
     };
     ++i;
     while ( true ){
//           std::cout << str << "\n" << std::flush;
          auto ii { str.find( DotB, i ) };
          if ( ii > str.find( DotW, i ) ) {
               ii = str.find( DotW, i );
          };
          i = 1;
          clinks.push_back( {str.substr( 0 , ii )} );
          if ( ii != std::string::npos ){
               str = str.substr( ii );
          } else {
               break;
          };
     };
     // check for zero morphisms that are not at the end
     for ( size_t i = 0; i < clinks.size()-1; ++i ){
          if ( clinks[i].morphism.is_0() ){
               std::cerr << "ERROR: Chain contains zero morphism in its interior.";
          };
     };
     // go through clinks to fix ambiguity S vs S²
     for ( size_t i = 0; i < clinks.size(); ++i ){
          // compare current object to next, but cyclically
          if (clinks[i].object==clinks[(i+1)% clinks.size()].object ){
               if (clinks[i].morphism.get_first_type()==-1){
                    clinks[i].morphism = BNMor<Coeff>(clinks[i].object.get_idem(), clinks[i].object.get_idem(), {Label(-2, clinks[i].morphism.get_coeff(false))});
               };
          };
     };
     // deduce gradings of objects from the first
     int sign_rightarrow {-1};
     int type {0};
     for ( size_t i = 0; i < clinks.size()-1; ++i ){
          if (clinks[i].rightarrow){
               sign_rightarrow = 1;
          } else {
               sign_rightarrow = -1;
          };
          clinks[i+1].object.set_h( clinks[i].object.get_h() + sign_rightarrow );
          //
          type = clinks[i].morphism.get_first_type();
          if ( type > 0 ){
               type *= 2;
          } else {
               type *= -1;
          };
          clinks[i+1].object.set_q( clinks[i].object.get_q() + sign_rightarrow*type );
     };
     // run some sanity checks
     // 1) S- and D-arrows should alternate.
     type = clinks.front().morphism.get_first_type();
     if ( !clinks.back().morphism.is_0() ){
          // Compare first and last only for compact chains
          if ( type*clinks.back().morphism.get_first_type() > 0 ){
               std::cerr << "ERROR: The first and last morphism in the chain are either both S-arrows or both D-arrows.";
          };
     };
     for ( size_t i = 1; i < clinks.size(); ++i ){
          if (type * clinks[i].morphism.get_first_type() > 0 ){
               std::cerr << "ERROR: Morphisms in chain do not alternate between S- and D-arrows.";
          };
          type = clinks[i].morphism.get_first_type();
     };
     // 2) Gradings of first and last objects should be consistent, if chain is compact
     if ( !clinks.back().morphism.is_0() ){
          if (clinks.back().rightarrow){
               sign_rightarrow = 1;
          } else {
               sign_rightarrow = -1;
          };
          if ( clinks.front().object.get_h() != clinks.back().object.get_h() + sign_rightarrow){
               std::cerr << "ERROR: The homological gradings of the first and last objects in the chain are inconsistent.";
          };
          //
          type = clinks.back().morphism.get_first_type();
          if ( type > 0 ){
               type *= 2;
          } else {
               type *= -1;
          };
          if (clinks.front().object.get_q() != clinks.back().object.get_q() + sign_rightarrow*type ){
               std::cerr << "ERROR: The homological gradings of the first and last objects in the chain are inconsistent.";
          };
     };
//      std::cout << "original: " << s << "\n" << std::flush;
//      std::cout << "understood: " << this->to_string() << "\n";
};

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

template<typename Coeff>
size_t Chain<Coeff>::size() const {
     return clinks.size();
};



//                     //
// getters and setters //
//                     //

template<typename Coeff>
bool Chain<Coeff>::is_compact() const
{
     return !clinks.back().morphism.is_0();
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
                    BNMor<Coeff> ( clinks.front().object.get_idem(),
                                   clinks.front().object.get_idem(),
                                   {} ),
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
     // Specials of slope < 0 contain a curve segment x=⬮—>⬮~>⬯—>⬯=y such that the curve segments starting at x and y are identical (same idempotents and directions of arrows) until they meet in a fourth curve segment similar to the first. For specials of slope > 0, swap ⬮ and ⬯. The first curve segment corresponds to wrapping around the special puncture, so this can tell all rationals and specials of slope ≠ 0,∞ apart. 
     //
     // first take care of 'short' curves; they are always rational:
     if ( N < 7 ) {
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

template<typename Coeff>
Chains<Coeff>::Chains ( const File &file )
     :
     chains ( {} )
{
     std::ifstream ifs ( file.fullname() );
     if ( ifs.good() != true ) {
          std::cerr << "I cannot read the file '" <<  file.fullname() << "'.\nPlease make sure it exists \n" ;
          exit ( 1 );
     };
     std::string content (
          ( std::istreambuf_iterator<char> ( ifs ) ),
          ( std::istreambuf_iterator<char>() ) );
     // remove commented lines (everything after '%')
     while ( true ) {
          size_t cPos = content.find ( "%" );
          if ( cPos + 1 ) {
               size_t lbPos = content.find ( '\n', cPos );
               content.erase ( cPos, lbPos - cPos );
          } else {
               break;
          };
     };
     //
     std::vector<std::string> vec {};
     while ( true ) {
          size_t cPos = content.find ( "\n" );
          vec.push_back(content.substr(0,cPos));
          if ( cPos == std::string::npos ){
               break;
          } else {
               content = content.substr( cPos + 1 );
          };
     };
     // try to read line by line and interpret it as a chain
     for ( auto &c : vec ){
          size_t cPos = c.find ( ")" );
          if ( cPos != std::string::npos ){
               c = c.substr( cPos + 1 );
          };   
     };
     std::cout << "Reading in chain from file '"
               << file.fullname()
               << "'...\n";
     std::string test_for_empty {};
     for ( const auto &c : vec ){
          test_for_empty = c;
          test_for_empty.erase(std::remove(test_for_empty.begin(), test_for_empty.end(), ' '), test_for_empty.end());
          if ( !test_for_empty.empty() ){
               try {
                    auto x {Chain<Coeff>( c )};
                    chains.push_back( x );
                    std::cout << "...understood:" 
                              << x.to_string() 
                              << "\n";
               } catch (std::invalid_argument const& ex){
                    std::cerr << "WARNING: Could not understand the following input line from file " 
                              << file.fullname()
                              << ":\n"
                              << c
                              << "\nError message: "
                              << ex.what()
                              << "I am going to ignore this input line.\n" 
                              << std::flush;
               };
          };
     };
};

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
          cur_gr = std::to_string ( chain.clinks.front().object.get_h() ).size();
          if ( cur_gr>max_h ) {
               max_h = cur_gr;
          };
          //
          cur_gr = std::to_string ( chain.clinks.front().object.get_q() ).size();
          if ( cur_gr>max_q ) {
               max_q = cur_gr;
          };
          //
          cur_gr = coeff_to_string ( chain.clinks.front().object.get_delta() ).size();
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

template<typename Coeff>
Chains<Coeff> Chains<Coeff>::non_compacts ()
{
     std::vector<Chain<Coeff>> vec;
     for ( auto &chain : this->chains ) {
          if ( !chain.is_compact() ){
               vec.push_back( chain );
          };
     };
     return vec;
};

template<typename Coeff>
Chains<Coeff> Chains<Coeff>::compacts ()
{
     std::vector<Chain<Coeff>> vec;
     for ( auto &chain : this->chains ) {
          if ( chain.is_compact() ){
               vec.push_back( chain );
          };
     };
     return vec;
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
