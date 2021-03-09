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



#ifndef COB_TPP
#define COB_TPP
#ifndef COB_H
#include "../headers/Cob.h"
#include "../headers/constants.h"
#endif


/// \file Cob.cpp
/// \brief implements the universal cobordism category from \cite BarNatanKhT

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  CobObj
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

CobObj::CobObj ( TE top, TE bot, Arcs arcs, Grading h, Grading q ) :
     top ( top ),
     bot ( bot ),
     arcs ( arcs ),
     h ( h ),
     q ( q )
{
}

CobObj::CobObj ( TE size ) :
     top ( size ),
     bot ( size )
{
     arcs.reserve ( 2 * size );
     for ( size_t k = 0; k < size; ++k ) {
          arcs.push_back ( size+k );
     };
     for ( size_t k = 0; k < size; ++k ) {
          arcs.push_back ( k );
     };
     h = 0;
     q = 0;
}

Pairs CobObj::pairs() const
{
     // converts arc-format into pair-format
     Pairs pairs;
     for ( std::size_t i = 0; i < arcs.size(); ++i ) {
          if ( i < arcs[i] )
               pairs.push_back ( {i,arcs[i]} );
     };
     return pairs;
}


//                     //
// getters and setters //
//                     //
TE CobObj::get_top() const
{
     return top;
};
TE CobObj::get_bot() const
{
     return bot;
};
TE CobObj::ends() const
{
     return top+bot;
};
Arcs CobObj::get_arcs() const
{
     return arcs;
};
Grading CobObj::get_h() const
{
     return h;
};
bool CobObj::h_is_even() const
{
     return h % 2 == 0;
};
Grading CobObj::get_q() const
{
     return q;
};
Grading CobObj::get_delta2() const
{
     return q - 2 * h;
};
void CobObj::set_h ( const Grading &new_h )
{
     h = new_h;
};
void CobObj::set_q ( const Grading &new_q )
{
     q = new_q;
};
void CobObj::shift_hq ( const Grading &h_shift, const Grading &q_shift )
{
     h +=  h_shift;
     q +=  q_shift;
};

//                          //
// output and sanity checks //
//                          //
void CobObj::print() const
{
     std::cout << this->to_string();
}


std::string CobObj::to_string() const
{
     std::string output = "";
     //
     output +=  "tangle ends top:   ";
     for ( TE i = 0; i < top; ++i ) {
          output +=  std::to_string ( i );
          output +=  " ";
     };
     output +=  "\ntangle ends bot:   ";
     for ( TE i = 0; i < bot; ++i ) {
          output +=  std::to_string ( top+i );
          output +=  " ";
     };
     output +=  "\n           arcs:   ";
     Pairs pairs = this->pairs();
     for ( const auto &pair : pairs ) {
          output +=  std::to_string ( pair.first );
          output +=  "–";
          output +=  std::to_string ( pair.second );
          output +=  " ";
     };
     output +=  "\n homological gr:   ";
     output +=  std::to_string ( h );
     output +=  "\n     quantum gr:   ";
     output +=  std::to_string ( q );
     output +=  "\n";
     //
     return output;
}

bool CobObj::check() const
{
     for ( std::size_t i = 0; i < arcs.size(); ++i ) {
          // validate if CobObj defines a tangle
          // (ie is involution of {0,...,arcs.size()-1})
          if ( arcs[i] >=  arcs.size() ||
                    arcs[i] < 0 ||
                    i == arcs[i] ||
                    i !=  arcs[arcs[i]] ) {
               return false;
          };
     };
     // test if tangle is crossingless
     for ( const auto &pair1 : this->OneSided().pairs() ) {
          TE p1 {pair1.first};
          TE p2 {pair1.second};
          for ( const auto &pair2 : this->OneSided().pairs() ) {
               TE t1 {pair2.first};
               TE t2 {pair2.second};
               // we can assume that a<b for each pair {a,b}
               if ( ! ( ( p2 < t1 )
                         || ( t2 < p1 )
                         || ( t1 < p1 && p2 < t2 )
                         || ( p1 < t1 && t2 < p2 ) ) ) {
                    return false;
               };
          };
     };
     return true;
}

bool CobObj::compatible_with ( const CobObj obj ) const
{
     // check whether cobordisms between these objects exist.
     return ( top == obj.top && bot == obj.bot );
};



//              //
// main methods //
//              //
CobObj CobObj::OneSided() const
{
     // rotate bottom half of the tangle to the top right such that the
     // last top and last bot tangle end sit next to each other
     Arcs new_arcs {  top + bot };
     for ( std::size_t i = 0; i < arcs.size(); ++i ) {
          TE new_entry;
          if ( i < top ) {
               new_entry = arcs[i];
          } else {
               new_entry = arcs[top+bot-i];
          };
          if ( new_entry < top ) {
               new_arcs[i] = new_entry;
          } else {
               new_arcs[i] = top + bot - new_entry;
          };
     };
     return CobObj ( top + bot,0,new_arcs,h,q );
}

IndexLL CobObj::components_to ( const CobObj &obj ) const
{
     // components from this to obj; a component is a list of TEIs,
     // starting with the lowest TEI.  A component always has an even
     // number of TEIs.  The components are ordered by their lowest TEI
     // (ie the first entry of the component).
     IndexLL allcomponents;
     allcomponents.reserve ( ( top + bot ) /2 );
     // output list that will get populated by this method
     IndexL done;
     done.reserve ( top + bot );
     // list of TEIs which have already been sorted into components
     for ( TE i = 0; i < top + bot; ++i ) {
          //find component for ith tangle end
          if ( std::find ( done.begin(), done.end(), i ) == done.end() ) {
               //continue if we don't already have a component; otherwise move
               //on; equivalent to "i not in done"
               IndexL component;
               TE cur {i};
               do {
                    done.push_back ( cur );
                    component.push_back ( cur );
                    cur = arcs[cur];
                    done.push_back ( cur );
                    component.push_back ( cur );
                    cur = obj.arcs[cur];
               } while ( cur !=  i );
               allcomponents.push_back ( component );
          };
     };
     return allcomponents;
}

BNObj CobObj::to_BNObj() const
{
     // convert CobObj to BNObj; this only works for the CobObj b and
     // CobObj c
     bool type {false};
     if ( *this == a0 || *this == a1 || *this == b1 || *this == b2 || *this == b3 ) {
          // b
          type = true;
     } else if ( *this == c1 || *this == c2 || *this == c3 ) {
          // c
          type = false;
     } else {
          this->print();
          std::cerr << "Cannot convert this CobObj to BNObj!\n";
          abort();
          // sanity check;
     };
     return BNObj ( type,h,q );
}

void CobObj::AddCap ( const TE &i )
{
     // add two extra ends before the ith TEI, assumed to be at the
     // tangle bottom, and connect them by an arc.
     bot += 2;
     for ( auto &entry : arcs ) {
          if ( entry >= i ) {
               entry += 2;
          };
     };
     Arcs new_arcs {i+1,i};
     arcs.insert ( arcs.begin()+i,new_arcs.begin(),new_arcs.end() );
}

void CobObj::AddCup ( const TE &i )
{
     // join the ith TEI, assumed to be at the tangle bottom, to the
     // next and join the two arcs if neccessary.
     bot -=   2;
     arcs[arcs[i]] = arcs[i+1];
     arcs[arcs[i+1]] = arcs[i];
     // This join the arcs starting at i and i+1.  If (i)---(i+1), only
     // arcs[i] and arcs[i+1] change, but they are erased next anyway.
     arcs.erase ( arcs.begin() + i,arcs.begin() + i + 2 );
     for ( auto &entry : arcs ) {
          if ( entry > i ) {
               entry -=   2;
          };
     };
}

bool CobObj::AddCupGivesClosedComponent ( const TE &i ) const
{
     // 1 if the two ends were connected ( = new closed component)
     // 0 if the two ends belonged to different arcs
     if ( arcs[i] == i+1 ) {
          return true;
     } else {
          return false;
     };
}

//                      //
// overloaded operators //
//                      //
bool CobObj::operator  == ( const CobObj &obj2 ) const
{
     // ignore gradings
     return ( ( top == obj2.top ) &&
              ( bot == obj2.bot ) &&
              ( arcs == obj2.arcs ) );
};

bool CobObj::operator!= ( const CobObj &obj2 ) const
{
     // ignore gradings
     return ( ( top !=  obj2.top ) ||
              ( bot !=  obj2.bot ) ||
              ( arcs !=  obj2.arcs ) );
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//          Deco
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
Deco<Coeff>::Deco ( int hpower, Dots dots, Coeff coeff ) :
     hpower ( hpower ),
     dots ( dots ),
     coeff ( coeff )
{
}

//              //
// main methods //
//              //
template<typename Coeff>
void Deco<Coeff>::reorder_old2new ( const IndexL &new_order )
{
     // reorder dots according to the new order of components
     //
     // new_order[old_index] = new_index;
     //
     // assuming new_order.size() == dots.size()
     std::size_t n {dots.size() };
     auto temp {Dots ( n ) };
     for ( std::size_t i = 0; i < n; ++i ) {
          temp[new_order[i]] = dots[i];
     };
     dots = temp;
}

template<typename Coeff>
void Deco<Coeff>::reorder_new2old ( const IndexL &new_order )
{
     // reorder dots according to the new order of components
     //
     // new_order[new_index] = old_index;
     //
     // assuming new_order.size() == dots.size()
     Dots temp;
     temp.reserve ( dots.size() );
     for ( const auto &el : new_order ) {
          temp.push_back ( dots[el] );
     };
     dots = temp;
}

template<typename Coeff>
Grading Deco<Coeff>::gr_q() const
{
     // compute grading of a decoration, ie
     // #comps-2*hpower-2#dots
     Grading gr {-2*hpower};
     for ( const bool &dot : dots ) {
          // combine #comps with -2#dots
          if ( dot ) {
               --gr;
          } else {
               ++gr;
          };
     };
     return gr;
}

template<typename Coeff>
bool deco_is_zero ( const Deco<Coeff> &deco )
{
     return coeff_is_0 ( deco.coeff );
}

template<typename Coeff>
void Deco<Coeff>::switch_dot ( const size_t &index )
{
     dots[index] = !dots[index] ;
};

template<typename Coeff>
void Deco<Coeff>::add_H()
{
     hpower +=  1;
};

template<typename Coeff>
void Deco<Coeff>::sub_H()
{
     hpower -= 1;
};

//                      //
// overloaded operators //
//                      //

template<typename Coeff>
bool Deco<Coeff>::operator == ( const Deco<Coeff> &deco2 ) const
{
     return ( ( hpower == deco2.hpower ) && ( dots == deco2.dots ) );
};
template<typename Coeff>
bool Deco<Coeff>::operator!= ( const Deco<Coeff> &deco2 ) const
{
     return ( ( hpower !=  deco2.hpower ) || ( dots !=  deco2.dots ) );
};

template<typename Coeff>
bool Deco<Coeff>::operator< ( const Deco<Coeff> &deco2 ) const
{
     // lexicographic ordering (first dots, then hpower); we assume that
     // the components are in the same order!
     std::size_t i {0};
     for ( const auto &d : deco2.dots ) {
          if ( dots[i] != d ) {
               return d;
          };
          i++;
     }
     return hpower <= deco2.hpower;
}

template<typename Coeff>
Deco<Coeff> Deco<Coeff>::operator* (
     const Deco<Coeff> &deco2 ) const
{
     // used for merging partial decorations
     Dots new_dots {dots};
     // making a copy
     new_dots.insert ( new_dots.end(),deco2.dots.begin(),deco2.dots.end() );
     // concatenate dot-vectors (deco2 comes second)
     return Deco<Coeff> ( hpower+deco2.hpower,new_dots,coeff*deco2.coeff );
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//          CobMor
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
CobMor<Coeff>::CobMor ( int i ) :
     front ( a0 ),
     back ( a0 ),
     decos ( {} ),
      comps ( {} )
{
}

template<typename Coeff>
CobMor<Coeff>::CobMor() :
     front ( a0 ),
     back ( a0 ),
     decos ( {} ),
      comps ( {} )
{
}

template<typename Coeff>
CobMor<Coeff>::CobMor (
     CobObj front,
     CobObj back,
     std::list<Deco<Coeff>> decos,
     IndexLL comps )
     :
     front ( front ),
     back ( back ),
     decos ( decos ),
     comps ( comps )
{
}

template<typename Coeff>
CobMor<Coeff>::CobMor (
     CobObj front,
     CobObj back,
     std::list<Deco<Coeff>> decos )
     :
     front ( front ),
     back ( back ),
     decos ( decos ),
     comps ( front.components_to ( back ) )
{
}

//                     //
// getters and setters //
//                     //
template<typename Coeff>
bool CobMor<Coeff>::is_0() const
{
     return decos.empty();
};

template<typename Coeff>
bool CobMor<Coeff>::is_inv() const
{
     // we assume that the cobordism is simplified.
     if ( front != back ) {
          return false;
     };
     if ( decos.size() != 1 ) {
          return false;
     };
     if ( decos.front().hpower != 0 ) {
          return false;
     };
     for ( const auto &entry : decos.front().dots ) {
          if ( entry !=  0 ) {
               return false;
          };
     };
     return coeff_is_inv ( decos.front().coeff );
};

template<typename Coeff>
Coeff CobMor<Coeff>::get_first_coeff() const
{
     return decos.front().coeff;
};

//                          //
// output and sanity checks //
//                          //

template<typename Coeff>
void CobMor<Coeff>::print() const
{
     std::cout << this->to_string();
}

template<typename Coeff>
std::string CobMor<Coeff>::to_string() const
{
     std::string output;
     //
     if ( decos.empty() ) {
          output +=  "The zero-morphism.\n";
     } else {
          output +=  "                ***front***\n";
          output +=  front.to_string();
          output +=  "                ***back***\n";
          output +=  back.to_string();
          output +=  "                ***decos***\n           ";

          for ( const auto &deco : decos ) {
               output +=  std::to_string ( deco.hpower );
               output +=  "\t";
          };
          output +=  "H\n           ";
          std::size_t dotlength = decos.front().dots.size();
          //
          for ( std::size_t i = 0; i < dotlength; ++i ) {
               for ( const auto &deco : decos ) {
                    output +=  std::to_string ( deco.dots[i] );
                    output +=  "\t";
               };
               output +=  "-";
               for ( const auto &j : comps[i] ) {
                    output +=  std::to_string ( j );
                    output +=  "-";
               };
               output +=  "\n           ";
          };
          //
          for ( const auto &deco : decos ) {
               output +=  coeff_to_string ( deco.coeff );
               output +=  "\t";
          };
          output +=  "coeff \n";
     };
     //
     return output;
}

template<typename Coeff>
bool CobMor<Coeff>::check() const
{
     if ( decos.empty() ) {
          return true;
     };
     // We may now assume that decos.size()>0.
     if ( !front.compatible_with ( back ) ) {
          return false;
     };
     std::size_t dotlength {decos.front().dots.size() };
     for ( const auto &deco : decos ) {
          if ( deco.dots.size() !=  dotlength ) {
               return false;
          };
     };
     if ( comps.size() !=  dotlength ) {
          return false;
     };
     TE previous_TEI_0 {0};
     for ( const auto &comp : comps ) {
          // Check if our assumptions on components hold.
          // But we do not check that all integers occur only once.
          previous_TEI_0 = comp.front();
          for ( const auto &e : comp ) {
               if ( e < previous_TEI_0 ) {
                    return false;
               };
          };
     };
     return true;
}

template<typename Coeff>
Grading CobMor<Coeff>::gr_q() const
{
     // assuming decos is non-empty
     Grading gr {decos.front().gr_q() };
     for ( const auto &deco : decos ) {
          if ( deco.gr_q() != gr ) {
               this->print();
               std::cerr << "morphism is not homogeneous!\n";
               abort();
          };
     };
     return back.get_q()-front.get_q()+gr-front.ends() /2;
}

template<typename Coeff>
bool CobMor<Coeff>::check (
     const CobObj &obj_from,
     const CobObj &obj_to ) const
{
     if ( front !=  obj_from ) {
          return false;
     };
     if ( back !=  obj_to ) {
          return false;
     };
     return this->check();
};

//              //
// main methods //
//              //
template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::simplify()
{
     if ( decos.empty() ) {
          return CobMor<Coeff> ( 0 );
     };
     // We may now assume that the list of decos is non-empty.
     //  1) order decos using the overloaded operation< on decos
     //
     //std::sort ( decos.begin(),decos.end() );
     //old version with decos an instance of std::vector<Decos>.
     decos.sort();
     //  2) for each deco, except the first, check if it is equal to the
     //  predecessor except for the coefficient.
     std::list<Deco<Coeff>> new_decos {decos.front() };
     decos.erase ( decos.begin() );
     for ( const auto &deco : decos ) {
          if ( deco == new_decos.back() ) {
               new_decos.back().coeff +=  deco.coeff;
          } else {
               //eliminate decos that are zero.
               if ( coeff_is_0 ( new_decos.back().coeff ) ) {
                    new_decos.pop_back();
               };
               new_decos.push_back ( deco );
          };
     };
     if ( coeff_is_0 ( new_decos.back().coeff ) ) {
          new_decos.pop_back();
     };
     decos = new_decos;
     //std::cout << this->to_string() << std::flush;
     //CobMor<Coeff> cob =*this;
     //std::cout << "flush" << std::flush;
     return *this;
}

template<typename Coeff>
BNMor<Coeff> CobMor<Coeff>::to_BNMor ( TE special_end ) const
{
     BNObj obj_from = front.to_BNObj();
     BNObj obj_to = back.to_BNObj();
     std::list<Label<Coeff>> new_labels;
     if ( comps.empty() ) {
          return BNMor<Coeff> ( 0 );
     } else if ( comps.size() == 1 ) {
          // morphism between different idempotents
          for ( const auto &deco : decos ) {
               if ( deco.dots.front() == 0 ) {
                    if ( ( ( front.get_top() == 1 ) && ( front.get_bot() == 1 ) ) ||
                              ( ( front.get_top() == 2 ) && ( front.get_bot() == 0 ) ) ) {
                         // 1-1- or 2-0-tangle
                         new_labels.push_back ( {deco.hpower,deco.coeff} );
                    } else {
                         // 4-ended tangle, so power of S
                         if ( deco.hpower % 2 == 0 ) {
                              new_labels.push_back ( {-1-2*deco.hpower,deco.coeff} );
                         } else {
                              new_labels.push_back ( {-1-2*deco.hpower,-deco.coeff} );
                         };
                    };
               };
          };
     } else if ( comps.size()  == 2 ) {
          unsigned int special_comp_index;
          unsigned int non_special_comp_index;
          if ( comps[0][0] == special_end || comps[0][1] == special_end ) {
               special_comp_index = 0;
               non_special_comp_index = 1;
          } else {
               special_comp_index = 1;
               non_special_comp_index = 0;
          };
          //
          for ( const auto &deco : decos ) {
               int n {deco.hpower};
               // morphism between identical idempotents
               if ( deco.dots[special_comp_index] == 0 ) {
                    // making sure that the special component does not contain a
                    // dot.
                    if ( deco.dots[non_special_comp_index] == 0 ) {
                         // no dot on non-special component
                         if ( n == 0 ) {
                              // identity
                              new_labels.push_back ( {0,deco.coeff} );
                         } else {
                              // power of H^n = D^n+(-1)^n.S^2n
                              new_labels.push_back ( {n,deco.coeff} );
                              if ( n % 2 == 0 ) {
                                   new_labels.push_back ( {-2*n,deco.coeff} );
                              } else {
                                   new_labels.push_back ( {-2*n,-deco.coeff} );
                              };
                         };
                    } else {
                         // one dot on non-special component
                         new_labels.push_back ( {n+1,deco.coeff} );
                    };
               };
          };
     } else {
          this->print();
          std::cerr << "ERROR: there should be at most two components!\n";
          abort();
     };
     return BNMor<Coeff> ( obj_from, obj_to,new_labels ).simplify();
}

/// \brief implementation of to_coeffs()
template<>
template<typename New_Coeff>
CobMor<New_Coeff> CobMor<Z_mod<integer_simulant>>::to_coeffs() const
{
     //std::cout << << "Converting from Z to " << which_coeff ( New_Coeff ( 0 ) );
     std::list<Deco<New_Coeff>> new_decos;
     for ( const auto &deco : decos ) {
          New_Coeff new_coeff = deco.coeff.n;
          if ( coeff_is_0 ( new_coeff ) != true ) {
               new_decos.push_back ( Deco<New_Coeff> ( deco.hpower,deco.dots,new_coeff ) );
          };
     };
     return CobMor<New_Coeff> ( front,back,new_decos );
};

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::AddCap (
     const CobObj &from,
     const CobObj &to,
     const TE &i ) const
{
     // add a component [i,i+1] without any dot. We apply this to entries
     // in a complex, 'from' and 'to' should come from the already
     // updated list of objects.
     auto new_comps = comps;
     // shift the indices in the components:
     for ( auto &entryL : new_comps ) {
          for ( auto &entry : entryL ) {
               if ( entry>= i ) {
                    entry +=  2;
               };
          };
     };
     // correct index for the new component:
     size_t k = 0;
     for ( ; k!= comps.size(); k++ ) {
          if ( new_comps[k].front() >i ) {
               break;
          };
     };
     // insert the new component:
     new_comps.insert ( new_comps.begin()+k, {i,i+1} );
     // insert an entry '0' in each deco.dots
     auto new_decos = decos;
     for ( auto &deco : new_decos ) {
          deco.dots.insert ( deco.dots.begin()+k,0 );
     };
     return CobMor<Coeff> ( from,to,new_decos,new_comps );
}

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::AddCup22 (
     const CobObj &start,
     const CobObj &end,
     const TE &i,
     const bool &from,
     const bool &to ) const
{
     // Pre- and postcompose the capped-off morphism with the first
     // delooping isomorphisms from Observation 4.18 in [KWZ], where the
     // top generator in the middle corresponds to the first generator of
     // the pair.
     size_t comp_i{0};
     // finding the index of the component {i,i+1}:
     auto new_comps = comps;
     for ( const auto &comp : new_comps ) {
          if ( comp.front()  == i ) {
               break;
          };
          ++comp_i;
     };
     // erase the component {i,i+1}:
     new_comps.erase ( new_comps.begin()+comp_i );
     // shifting the indices of the components
     for ( auto &entryL : new_comps ) {
          for ( auto &entry : entryL ) {
               if ( entry>= i ) {
                    entry +=  -2;
               };
          };
     };
     //removing the corresponding entry of deco.dots in all decos and
     //updating the coefficient according to the following three cases:
     auto old_decos {decos};//make copy
     // 0--->0:
     std::list<Deco<Coeff>> new_decos;
     if ( from == 0 && to == 0 ) {
          for ( auto &deco : old_decos ) {
               // dot == 0  == > 1
               // dot == 1  == > -
               if ( deco.dots[comp_i] == 0 ) {
                    deco.dots.erase ( deco.dots.begin()+comp_i );
                    new_decos.push_back ( deco );
               };
          };
     }
     // 1--->0:
     else if ( from == 1 && to == 0 ) {
          for ( auto &deco : old_decos ) {
               // dot == 0  == > -
               // dot == 1  == > 1
               if ( deco.dots[comp_i] == 1 ) {
                    deco.dots.erase ( deco.dots.begin()+comp_i );
                    new_decos.push_back ( deco );
               };
          };
     }
     // 1--->1:
     else if ( from == 1 && to == 1 ) {
          for ( auto &deco : old_decos ) {
               // dot == 0  == > 1
               // dot == 1  == > H
               if ( deco.dots[comp_i] == 0 ) {
                    deco.dots.erase ( deco.dots.begin()+comp_i );
                    new_decos.push_back ( deco );
               } else {
                    deco.dots.erase ( deco.dots.begin()+comp_i );
                    deco.hpower +=  1;
                    new_decos.push_back ( deco );
               };
          };
     }
     // 0--->1:
     else {
          std::cout << "ERROR! 'CobMor<Coeff>::AddCap22(...)' should never be called with from == 0 and to == 1.";
     };
     return CobMor<Coeff> ( start, end, new_decos, new_comps );
}

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::AddCupMixed (
     const CobObj &start,
     const CobObj &end,
     const TE &i,
     const bool &from,
     const bool &to ) const
{
     // Precompose/Postcompose the capped-off morphism with the first
     // delooping isomorphism from Observation 4.18 in [KWZ], where the
     // top generator in the middle corresponds to the first generator of
     // the pair.  (For CASE 2->1, treat the RHS as 0; for CASE 1->2,
     // treat the LHS as 1. Note that 1--->0 is ambiguous, but it is
     // treated identically in the two cases.)  This involves:
     // finding the index of the component containing i (and i+1):
     size_t n {comps.size() };
     size_t comp_i {n};
     // larger than any entry in comp in comps; will become the index of
     // comp containing i and i+1
     IndexL comp_names;
     // first entry of each comp in comps
     comp_names.reserve ( n );
     {
          size_t k{0};
          // running index
          for ( const auto &comp : comps ) {
               if ( comp_i == n ) {
                    // if we have not found comp_i, go searching for i in comp:
                    if ( std::find ( comp.begin(),comp.end(),i ) != comp.end() ) {
                         // update once we have found i:
                         comp_i = k;
                    };
               };
               // populate comp_names:
               if ( comp.front() >i ) {
                    comp_names.push_back ( comp.front()-2 );
               } else {
                    comp_names.push_back ( comp.front() );
               };
               // increment running variable:
               ++k;
          };
     }
     // update the name of the component containing i if necessary:
     if ( comps[comp_i].front()  == i ) {
          // The smallest element of comp[comp_i] containing i is at most i
          // itself. If it is i, the new one needs to be found. Otherwise,
          // the smallest integer has not changed.
          auto smallest{comps[comp_i][2]};
          // smallest!=  i, i+1. This is because i is the 1st element and
          // i+1 is adjacent to i, so i+1 is either the 2nd element or at
          // the very end, the third element cannot be the last, since
          // comps has an even number of elements.
          for ( auto it = comps[comp_i].begin()+1; it!= comps[comp_i].end(); ++it ) {
               if ( *it<smallest && *it!= i+1 ) { // Note: *it>i
                    smallest = *it;
               };
          };
          comp_names[comp_i] = smallest-2;
          // i was the smallest integer in comp[comp_i], so the new one
          // is greater, so it needs to be shifted.
     };
     // compute new components:
     IndexLL new_comps = start.components_to ( end );
     // compute the new order for comps:
     IndexL new_order = new_order_new2old ( new_comps,comp_names );
     // update the index of the comp that previously contained i
     // and i+1:
     unsigned int comp_i_new = std::find ( new_order.begin(),new_order.end(),comp_i )-new_order.begin();
     // updating the corresponding entry of deco.dots in all decos and
     // updating the coefficient according to the following three cases:
     auto old_decos = decos;//make copy
     std::list<Deco<Coeff>> new_decos;
     // 0--->.( = 0):
     if ( from == 0 ) {
          for ( auto &deco : old_decos ) {
               // dot == 0  == > (add dot) - H
               // dot == 1  == > -
               if ( deco.dots[comp_i] == 0 ) {
                    // We use the old 'comp_i' here, since deco.dots has not been
                    // reordered; we do that now:
                    deco.reorder_new2old ( new_order );
                    // update decos
                    deco.dots[comp_i_new] = 1;
                    new_decos.push_back ( deco );
                    deco.dots[comp_i_new] = 0;
                    deco.hpower +=  1;
                    deco.coeff *=  -1;
                    new_decos.push_back ( deco );
               };
          };
     }
     // (1 = ).--->1: //
     else if ( to == 1 ) {
          for ( auto &deco : old_decos ) {
               // dot == 0  == > (add dot)
               // dot == 1  == > H
               //
               // reorder deco.dots:
               deco.reorder_new2old ( new_order );
               // update decos
               if ( deco.dots[comp_i_new] == 0 ) {
                    // using new 'comp_i', since 'deco.dots' has been reordered.
                    deco.dots[comp_i_new] = 1;
                    new_decos.push_back ( deco );
               } else {
                    deco.hpower +=  1;
                    new_decos.push_back ( deco );
               };
          };
     }
     // 1--->.( = 0) OR (1 = ).--->0:
     else {
          for ( auto &deco : old_decos ) {
               // dot == 0  == > 1
               // dot == 1  == > 1
               //
               // reorder deco.dots:
               deco.reorder_new2old ( new_order );
               // update decos
               new_decos.push_back ( deco );
          };
     };
     // 0--->1: This case does not exist.
     return CobMor<Coeff> ( start, end, new_decos, new_comps );
}

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::AddCup11 (
     const CobObj &start,
     const CobObj &end,
     const TE &i ) const
{
     // finding the index of the component containing i:
     size_t n {comps.size() } ;
     size_t comp_i0 {n};
     // will become the index of comp containing i
     size_t comp_i1 {n};
     // will become the index of comp containing i+1
     IndexL comp_names;
     // first entry of each comp in comps
     comp_names.reserve ( n );
     {
          size_t k{0};
          for ( const auto &comp : comps ) {
               if ( comp_i0 == n ) {
                    if ( std::find ( comp.begin(),comp.end(),i ) != comp.end() ) {
                         comp_i0 = k;
                    };
               };
               if ( comp_i1 == n ) {
                    if ( std::find ( comp.begin(),comp.end(),i+1 ) != comp.end() ) {
                         comp_i1 = k;
                    };
               };
               // populate comp_names:
               if ( comp.front() > i+1 ) {
                    comp_names.push_back ( comp.front()-2 );
               } else {
                    comp_names.push_back ( comp.front() );
               };
               // increment running variable:
               k++;
          };
     }
     // compute new components:
     IndexLL new_comps = start.components_to ( end );
     // Update the decos:
     auto old_decos = decos;//make copy
     std::list<Deco<Coeff>> new_decos;
     //
     if ( comp_i0 == comp_i1 ) {
          // Cupping this morphism adds another boundary component, so we
          // need to do neck-cutting; the old component is replaced by two
          // new components.

          // update the name of the component containing i if necessary:
          if ( comp_names[comp_i0] == i ) {
               // The smallest element of comp[comp_i0] containing i is at
               // most i itself. If it is i, the new one needs to be
               // found. Otherwise, the smallest integer has not changed.
               auto smallest{comps[comp_i0][2]};
               // smallest!=  i, i+1. This is because i is the 1st element and
               // i+1 is adjacent to i, so i+1 is either the 2nd element or at
               // the very end, the third element cannot be the last, since
               // comps has an even number of elements.
               for ( auto it = comps[comp_i0].begin()+1; it!= comps[comp_i0].end(); ++it ) {
                    if ( *it<smallest && *it!= i+1 ) { // Note: *it>i
                         smallest = *it;
                    };
               };
               comp_names[comp_i0] = smallest-2;
               // i was the smallest integer in comp[comp_i0], so the new one
               // is greater, so it needs to be shifted.
          };
          // compute the new order for comps:
          IndexL new_order = new_order_new2old ( new_comps,comp_names );
          // update the index of the comp that previously contained i
          // and i+1:
          unsigned int comp_i0_new = std::find ( new_order.begin(),new_order.end(),comp_i0 )-new_order.begin();
          // index of the first new component:
          // new_order[comp_i0_new] = comp_i0
          unsigned int comp_i1_new = std::find ( new_order.begin(),new_order.end(),n )-new_order.begin();
          // index of the second new component:
          // new_order[comp_i1_new] = n
          //
          for ( auto &deco : old_decos ) {
               // do neck-cutting
               if ( deco.dots[comp_i0] == 1 ) {
                    // the old component contained a dot
                    //  = > dots in both new components
                    //
                    // add a dot on the new component:
                    deco.dots.push_back ( 1 );
                    // reorder dots:
                    deco.reorder_new2old ( new_order );
                    //
                    new_decos.push_back ( deco );
               } else {
                    // add a dot on the new component:
                    deco.dots.push_back ( 1 );
                    // reorder dots:
                    deco.reorder_new2old ( new_order );
                    //
                    new_decos.push_back ( deco );
                    // dot on one side
                    deco.dots[comp_i0_new] = 1;
                    deco.dots[comp_i1_new] = 0;
                    new_decos.push_back ( deco );
                    //dot on the other side
                    deco.dots[comp_i0_new] = 0;
                    deco.hpower +=  1;
                    deco.coeff *=  -1;
                    new_decos.push_back ( deco );
                    // -H and no dots
               };
          };
     } else { // ie comp_i0!= comp_i1
          // Cupping this morphism joins two components; we just need to add
          // the dots together, and if there are 2, exchange one for H.
          //
          // update the name of the component containing i if necessary:
          if ( comp_names[comp_i0] == i ) {
               // The smallest element of comp[i_outer] containing i is at most i
               // itself. If it is i, the new one needs to be found. Otherwise,
               // the smallest integer has not changed.
               auto smallest{comps[comp_i0][1]};
               // smallest>i.
               for ( auto it = comps[comp_i0].begin()+1; it!= comps[comp_i0].end(); ++it ) {
                    if ( *it<smallest ) { // Note: *it>i
                         smallest = *it;
                    };
               };
               comp_names[comp_i0] = smallest-2;
               // i was the smallest integer in comp[comp_i0], so the new one
               // is greater, so it needs to be shifted.
          };
          // update the name of the component containing i+1 if necessary:
          if ( comp_names[comp_i1] == i+1 ) {
               // The smallest element of comp[i_outer] containing i+1 is at
               // most i+1 itself. If it is i+1, the new one needs to be
               // found. Otherwise, the smallest integer has not changed.
               auto smallest{comps[comp_i1][1]};
               // smallest>i+1.
               for ( auto it = comps[comp_i1].begin()+1; it!= comps[comp_i1].end(); ++it ) {
                    if ( *it<smallest ) { // Note: *it>i+1
                         smallest = *it;
                    };
               };
               comp_names[comp_i1] = smallest-2;
               // i+1 was the smallest integer in comp[comp_i1], so the new one
               // is greater, so it needs to be shifted.
          };
          // compute the new order for comps:
          IndexL new_order = new_order_new2old ( new_comps,comp_names );
          //updating the corresponding entry of deco.dots in all decos and
          //updating the coefficient according to the following two cases:

          for ( auto &deco : old_decos ) {
               if ( deco.dots[comp_i1] == 1 && deco.dots[comp_i0] == 1 ) {
                    deco.hpower +=  1;
               } else if ( deco.dots[comp_i1] == 1 || deco.dots[comp_i0] == 1 ) {
                    // at this point, not both can be 1.  The joined component
                    // carries a dot, no matter which of comp_i0 of comp_i1 is
                    // equal to new_order[joined component].
                    deco.dots[comp_i0] = 1;
                    deco.dots[comp_i1] = 1;
               };
               // If neither of the components carries a dot, the joined one
               // won't carry one either. Now reorder the dots:
               deco.reorder_new2old ( new_order );
               //
               new_decos.push_back ( deco );
          };
     };
     return CobMor<Coeff> ( start, end, new_decos, new_comps );
}



//                      //
// overloaded operators //
//                      //
template<typename Coeff>
bool CobMor<Coeff>::operator  == ( const CobMor<Coeff> &cob2 ) const
{
     if ( front!= cob2.front ) {
          return false;
     };
     if ( back!= cob2.back ) {
          return false;
     };
     if ( decos.size() != cob2.decos.size() ) {
          return false;
     };
     std::list<Deco<Coeff>> temp {cob2.decos};
     for ( const auto &deco : decos ) {
          // testing if the two decorations are equal as sets. We assume
          // that 'cob1' and 'cob2' are fully simplified.
          auto iter {std::find ( temp.begin(),temp.end(),deco ) };
          if ( iter == temp.end() ) {
               return false;
          } else {
               temp.erase ( iter );
          };
     };

     //return ( this->check() && cob2.check() );
     return true;
}

template<typename Coeff>
void CobMor<Coeff>::operator *= ( const Coeff &scalar )
{
     for ( auto &deco : decos ) {
          deco.coeff *=  scalar;
     };
}
template<typename Coeff>
void CobMor<Coeff>::operator+= ( const CobMor<Coeff> &cob2 )
{
     *this = ( *this+cob2 );
};

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::operator- () const
{
     // multiply the cobordism by -1;
     std::list<Deco<Coeff>> new_decos {decos};
     for ( auto &deco : new_decos ) {
          deco.coeff = -1*deco.coeff;
     };
     return CobMor<Coeff> ( front,back,new_decos,comps );
}

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::operator* ( const Coeff &scalar ) const
{
     auto new_cob = *this;
     for ( auto &deco : new_cob.decos ) {
          deco.coeff *=  scalar;
     };
     return new_cob;
};

/// preparation for CobMor<Coeff>::operator*( const CobMor<Coeff> &cob1 )
inline IndexLL partitionGenerator ( const IndexLL &new_comps, const Arcs &arcs )
{
     IndexLL partition;
     partition.reserve ( new_comps.size() );
     // the partition will be a list of list of indices of components of
     // the new cobordism; the following lines populates this list
     IndexL remaining;
     remaining.reserve ( new_comps.size() );
     // indices of the remaining components in our iteration process
     // populating 'partition'.
     for ( std::size_t i = 0; i < new_comps.size(); ++i ) {
          remaining.push_back ( i );
     };
     IndexL nucleus, nucleusL;
     TE arcend, x;
     bool found_arcend {false};
     //
     while ( remaining.size() >0 ) {
          // pick the first of the remaining components as the nucleus of a
          // new element of the partition and remove it from remaining:
          nucleus = { remaining.back() };
          remaining.pop_back();
          nucleusL = new_comps[nucleus.back()];
          // list of TEIs of nucleus
          arcend = 0;
          x = 0;
          found_arcend = false;
          // the first TEI of nucleus which connects via the middle clt to a
          // different component (whose index has to be irn 'remaining')
          for ( const auto &TEI : nucleusL ) {
               // find new 'arcend'
               x = arcs[TEI];
               if ( nucleusL.end()  == std::find ( nucleusL.begin(),nucleusL.end(),x ) ) {
                    arcend = x;
                    found_arcend = true;
                    break;
               };
          };
          while ( found_arcend ) {
               // find the component of 'arcend' and remove this component from
               // the remaining components (strictly reducing remaining.size())
               for ( std::size_t i = 0; i<remaining.size(); ++i ) {
                    IndexL comp = new_comps[remaining[i]];
                    if ( std::find ( comp.begin(),comp.end(),arcend ) != comp.end() ) {
                         // the following will execute exactly once in the for loop
                         nucleus.push_back ( remaining[i] );
                         remaining.erase ( remaining.begin()+i );
                         nucleusL.insert ( nucleusL.end(),comp.begin(),comp.end() );
                         break;
                    };
               };
               found_arcend = false;
               for ( const auto &TEI : nucleusL ) {
                    // find new 'arcend'
                    x = arcs[TEI];
                    if ( nucleusL.end()  == std::find ( nucleusL.begin(),nucleusL.end(),x ) ) {
                         arcend = x;
                         found_arcend = true;
                         break;
                    };
               };
          };
          partition.push_back ( nucleus );
     };
     return partition;
};


/// helper structure for the multiplication of cobordisms; this is in preparation for precomputing the algebra 

/// \todo precomputed algebra
struct CobMultHelper {
     /// partition of components of the first cobordism
     IndexL partition1;
     /// partition of components of the second cobordism
     IndexL partition2;
     /// genus of component
     unsigned int genus;
     /// \todo forgotten what this is about 
     unsigned int partitionLength;

     CobMultHelper ( IndexL partition1,
                     IndexL partition2,
                     unsigned int genus,
                     unsigned int partitionLength
                   );///< standard constructor
};

CobMultHelper::CobMultHelper ( IndexL partition1,
                               IndexL partition2,
                               unsigned int genus,
                               unsigned int partitionLength
                             ) :
     partition1 ( partition1 ),
     partition2 ( partition2 ),
     genus ( genus ),
     partitionLength ( partitionLength )
{
}

/// helper function for the multiplication of cobordisms; this is in preparation for precomputing the algebra 
inline std::tuple<std::vector<CobMultHelper>,IndexL> CobMultHelperFun ( const IndexLL &new_comps, const Arcs &arcs, const IndexLL &comps0, const IndexLL &comps1 )
{
     std::vector<CobMultHelper> output;
     IndexLL partition { partitionGenerator ( new_comps,arcs ) };
     //std::cout << "partition: " << stringLL ( partition ) << std::flush ;
     output.reserve ( partition.size() );
     IndexL new_order;
     IndexL part1;
     IndexL part2;
     IndexL comp; //'old_comp'

     // part1/2: indices of components of cob1/cob2 that are from
     // the intersection of cob1/cob2 with the old component
     // corresponding to the element of 'partition'
     for ( const auto &part : partition ) {
          comp.clear();
          for ( const auto &i : part ) {
               comp.insert ( comp.end(),new_comps[i].begin(),new_comps[i].end() );
          };
          part1.clear();
          for ( std::size_t i = 0; i<comps1.size(); ++i ) {
               if ( std::find ( comp.begin(),comp.end(),comps1[i][0] ) != comp.end() ) {
                    part1.push_back ( i );
               };
          };
          part2.clear();
          for ( std::size_t i = 0; i<comps0.size(); ++i ) {
               if ( std::find ( comp.begin(),comp.end(),comps0[i][0] ) != comp.end() ) {
                    part2.push_back ( i );
               };
          };
          output.push_back ( CobMultHelper ( part1,
                                             part2,
                                             1 - ( part1.size()
                                                       +part2.size()
                                                       -comp.size() / 2
                                                       +part.size() ) /2,
                                             part.size() ) );
          new_order.insert ( new_order.end(),part.begin(),part.end() );
     };
     return { output,new_order };
};

/// helper function for the multiplication of cobordisms; this is in preparation for precomputing the algebra 
template<unsigned int n>
std::vector<Dots> vec_l_Generator ( )
{
     std::vector<Dots> vec_l = {{0},{1}};
     while ( vec_l.front().size() < n ) {
          std::vector<Dots> temp = vec_l;
          for ( auto &vec:temp ) {
               vec.push_back ( 1 );
          };
          for ( auto &vec:vec_l ) {
               vec.push_back ( 0 );
          };
          vec_l.insert ( vec_l.end(),temp.begin(),temp.end() );
     };
     vec_l.pop_back(); // delete the constant 1 dot-vector
     return vec_l;
};

/// helper function for the multiplication of cobordisms; this is in preparation for precomputing the algebra 
inline std::vector<Dots> vec_l_default ( const unsigned int &n )
{
     std::vector<Dots> vec_l {{0},{1}};
     std::vector<Dots> temp;
     while ( vec_l.front().size() < n ) {
          temp = vec_l;
          for ( auto &vec:temp ) {
               vec.push_back ( 1 );
          };
          for ( auto &vec:vec_l ) {
               vec.push_back ( 0 );
          };
          vec_l.insert ( vec_l.end(),temp.begin(),temp.end() );
     };
     vec_l.pop_back(); // delete the constant 1 dot-vector
     return vec_l;
};

/// helper function for the multiplication of cobordisms; this is in preparation for precomputing the algebra 
inline std::vector<Dots> vec_l ( const int &m )
{
     // attempt to precompute this at compile time.
     switch ( m ) {
     case 1:
          return vec_l_Generator<1>();
          break;
     case 2:
          return vec_l_Generator<2>();
          break;
     case 3:
          return vec_l_Generator<3>();
          break;
     case 4:
          return vec_l_Generator<4>();
          break;
     case 5:
          return vec_l_Generator<5>();
          break;
     case 6:
          return vec_l_Generator<6>();
          break;
     case 7:
          return vec_l_Generator<7>();
          break;
     case 8:
          return vec_l_Generator<8>();
          break;
     case 9:
          return vec_l_Generator<9>();
          break;
     case 10:
          return vec_l_Generator<10>();
          break;
     case 11:
          return vec_l_Generator<11>();
          break;
     default:
          return vec_l_default ( m );
     };
};

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::operator* (
     const CobMor<Coeff> &cob1 ) const
{
     //multiplication of CobMor: cob*cob1
     if ( cob1.decos.empty() || decos.empty() ) {
          return CobMor<Coeff> ( 0 );
     };
     //multiplication with the zero-morphism.
     if ( front != cob1.back ) {
          // Not necessary, if we trust the program.
          std::cerr << "WARNING: You are trying to multiply two incomposable morphisms.\n";
          abort();
     };

     //cobordisms compose as CobMor(b,c)*CobMor(a,b) = CobMor(a,c)
     IndexLL new_comps { cob1.front.components_to ( back ) };
     auto X { CobMultHelperFun ( new_comps,cob1.back.get_arcs(),comps,cob1.comps ) };
     std::vector<CobMultHelper> cobMultVec { std::get<0> ( X ) };
     IndexL new_order { std::get<1> ( X ) };

     // temporary variables neded below
     std::list<Deco<Coeff>> new_decos {};
     std::vector<std::list<Deco<Coeff>>> part_decos {};
     std::list<Deco<Coeff>> new_part_decos {};
     std::list<Deco<Coeff>> more_decos {};
     std::list<Deco<Coeff>> temp {};
     unsigned int sum {0};
     unsigned int n {0};
     unsigned int g {0};
     int r {0};
     //auto vec_l_vec = vec_l_vec_Generator ( N );// TODO: this should be computed at compile time

     for ( const auto &deco1 : cob1.decos ) {
          for ( const auto &deco2 : decos ) {
               part_decos.clear();
               // for each element of the partition, compute the partial
               // decorations on the new components corresponding to that
               // element; a partial decoration is of type 'Deco', but the
               // length of the dot-vectors are equal to the length of the
               // partition element.
               for ( const auto &e : cobMultVec ) {
                    r = 0;// total #dots on this old component
                    for ( const auto &j : e.partition1 ) {
                         r +=  deco1.dots[j];
                    };
                    for ( const auto &j : e.partition2 ) {
                         r +=  deco2.dots[j];
                    };
                    n = e.partitionLength;
                    g = e.genus;
                    new_part_decos.clear();
                    if ( r > 0 ) {
                         new_part_decos.push_back ( Deco<Coeff> ( g+r-1,Dots ( n,1 ),1 ) );
                    } else {
                         // precompute this
                         for ( auto &vec : vec_l ( n ) ) {
                              sum = 0;
                              for ( const bool &v : vec ) {
                                   if ( v ) {
                                        sum++;
                                   };
                              };
                              // coefficients/signs for the deco of each dot_vector
                              if ( ( g+n-sum ) %2 == 0 ) {
                                   new_part_decos.push_back ( Deco<Coeff> ( g+n-sum-1,vec,-1 ) );
                              } else {
                                   new_part_decos.push_back ( Deco<Coeff> ( g+n-sum-1,vec,1 ) );
                              };
                         };
                         if ( g%2 == 1 ) { //genus odd
                              new_part_decos.push_back ( Deco<Coeff> ( g-1,Dots ( n,1 ),2 ) );
                         };
                    };
                    part_decos.push_back ( new_part_decos );
               };
               // finished computing partial decorations
               more_decos = { Deco<Coeff> ( 0,{},1 ) };
               for ( const auto &part_deco : part_decos ) {
                    temp.clear();
                    for ( const auto &deco : more_decos ) {
                         for ( const auto &p : part_deco ) {
                              temp.push_back ( deco*p );
                         };
                    };
                    more_decos = temp;
               };
               // compute more_decos from part_decos, by taking the cartesian
               // product of the lists of decos in part_decos and merging those
               // decos on the fly; we are assuming all the lists are
               // non-empty. The dot-vectors are still in the wrong order. We
               // fix this at the very end. Finally, we need to take the hpower
               // and coeff of the initial two decos into account; we do this
               // now:
               for ( auto &deco : more_decos ) {
                    deco = deco*Deco<Coeff> (
                                deco1.hpower+deco2.hpower,
                                {},
                                deco1.coeff*deco2.coeff );
               };
               new_decos.insert ( new_decos.end(),more_decos.begin(),more_decos.end() );
          };
     };
     for ( auto &deco : new_decos ) {
          deco.reorder_old2new ( new_order );
     };
     //reorder the dots according to the partition.
     return CobMor<Coeff> ( cob1.front,back,new_decos,new_comps ).simplify();
}

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::operator+ (
     const CobMor<Coeff> &cob2 ) const
{
     if ( decos.empty() ) {
          // cob1 = 0, so returning cob2
          return cob2;
     };
     if ( cob2.decos.empty() ) {
          //  cob2 = 0, so returning cob1
          return *this;
     };
     std::list<Deco<Coeff>> new_decos = decos;// concatenate decos1 and decos2
     new_decos.insert ( new_decos.end(), cob2.decos.begin(), cob2.decos.end() );
     return CobMor<Coeff> ( front, back, new_decos, comps ).simplify();
}

//                                            //
// non-member functions that relate to CobMor //
//                                            //
template<typename Coeff>
bool CobNonZero (
     const int& row,
     const int& col,
     const CobMor<Coeff> &value )
{
     return value.is_0()  == false;
};

IndexL new_order_new2old (
     const IndexLL &new_comps,
     const IndexL &comp_names )
{
     IndexL new_order;
     for ( const auto &new_comp : new_comps ) {
          int old_index = std::distance (
                               comp_names.begin(),
                               std::find ( comp_names.begin(),
                                           comp_names.end(),
                                           new_comp.front() ) );
          new_order.push_back ( old_index );
     };
//   std::cout << "There should be a most one element in 'new_order' = "
//             << stringL ( new_order )
//             << "which is equal to "
//             << comp_names.size()
//             << ". All the others should be strictly smaller.";
     return new_order;
}


/// generating function for the list of all crossingless 2\f$i\f$-0-tangles with \f$i<n\f$ 

/// All crossingless tangles are in arc format. 
/// The definition is recursive. We subdivide the tasks into the subtasks of finding those clts that connect 0 to (2x+1):
///
/// 0 . . . . . (2x+1) . . . (2n-1)
/// 0---(2x+1)
///
/// This is in preparation for precomputing the algebra
std::vector<std::vector<TE>> CatalanTangles ( const TE &n )
{
     if ( n == 0 || n > 15 ) {// failsafe; never call this with more than 15
          std::vector<TE> empty {};
          std::vector<std::vector<TE>> emptyempty {empty};
          return emptyempty;
     };
     std::vector<std::vector<TE>> output;
     std::vector<std::vector<TE>> first;
     std::vector<std::vector<TE>> second;
     for ( TE x = 0; x < n; ++x ) {

          first = CatalanTangles ( x );
          second = CatalanTangles ( n-1-x );
          for ( auto &v : first ) {
               for ( TE &e : v ) {
                    e += 1;
               };
               v.insert ( v.begin(),TE ( 2*x+1 ) );
               v.insert ( v.begin()+2*x+1,TE ( 0 ) );
          };
          for ( auto &v : second ) {
               for ( TE &e : v ) {
                    e += 2*x+2;
               };
          };
          for ( auto &v1 : first ) {
               for ( auto v2 : second ) {
                    v2.insert ( v2.begin(),v1.begin(),v1.end() );
                    output.push_back ( v2 );
               };
          };
     };
     return output;
};




// template specialization

#define VALUE Z_mod<2>
#include "../specialization/Cob.h"
#define VALUE Z_mod<3>
#include "../specialization/Cob.h"
#define VALUE Z_mod<5>
#include "../specialization/Cob.h"
#define VALUE Z_mod<7>
#include "../specialization/Cob.h"
#define VALUE Z_mod<custom_coeff>
#include "../specialization/Cob.h"
#define VALUE Z_mod<integer_simulant>
#include "../specialization/Cob.h"
#define nonconvertibles
#define VALUE Q
#include "../specialization/Cob.h"
#define VALUE int
#include "../specialization/Cob.h"
#undef nonconvertibles

#endif


