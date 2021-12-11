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
//                  Precomputed Algebra
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// int PCA::max_strands {1};
std::vector<std::vector<Arcs>> PCA::gens {};
std::vector<std::vector<std::vector<size_t>>> PCA::addCup;
std::vector<std::vector<std::vector<bool>>> PCA::addCupGCC;
std::vector<std::vector<std::vector<size_t>>> PCA::addCap;

/// computes more entries for PCA::gens if PCA::max_strands < n  
void expand_gens_if_needed ( const int &n ){
     while ( PCA::gens.size() < n+1 ) {
          if ( PCA::gens.size() < 2 ){
               PCA::gens = {{Arcs({})},{Arcs({1,0})}};
               PCA::addCup = {{},{{0}}};
               PCA::addCupGCC = {{},{{true}}};
               PCA::addCap = {{{0}}};
          } else {
               std:: cout << "\nExpanding generators to "
                         << PCA::gens.size()
                         << "="
                         << PCA::addCup.size()
                         << "="
                         << PCA::addCupGCC.size()
                         << "="
                         << (PCA::addCap.size()+1)
                         << " strands.\n\n";
               if ( n > maximum_number_of_tangle_strands ){
                    std::cerr << "There are "
                              << n
                              << " tangle strands, which is too many."
                              << " Try inceasing the value of 'maximum_number_of_tangle_strands' in 'sources/headers/constants.h'. ";
               };
               std::vector<Arcs> new_gens;
               std::vector<Arcs> first;
               std::vector<Arcs> second;
               for ( TE x = 0; x < PCA::gens.size(); ++x ) {                    
                    first = PCA::gens[x]; 
                    second = PCA::gens[PCA::gens.size()-1-x];
                    for ( auto &v : first ) {
                         for ( TE &e : v.arcs ) {
                              e += 1;
                         };
                         v.arcs.insert ( v.arcs.begin(),TE ( 2*x+1 ) );
                         v.arcs.insert ( v.arcs.begin()+2*x+1,TE ( 0 ) );
                    };
                    for ( auto &v : second ) {
                         for ( TE &e : v.arcs ) {
                              e += 2*x+2;
                         };
                    };
                    for ( auto &v1 : first ) {
                         for ( auto v2 : second ) {
                              v2.arcs.insert ( v2.arcs.begin(),v1.arcs.begin(),v1.arcs.end() );
                              new_gens.push_back ( v2.arcs );
                         };
                    };
               };
               //
               std::vector<std::vector<size_t>> new_addCupCap;
               std::vector<std::vector<bool>> new_addCupGCC;
               std::vector<size_t> temp {};
               std::vector<bool> temp_bool {};
               for ( auto gen : new_gens){
                    auto temp_gen {gen};
                    for ( size_t k=0; k < 2*PCA::gens.size()-1; ++k ){
                         temp_gen = gen;
                         temp_bool.push_back(temp_gen.addCupGCC( k ));
                         temp_gen.addCup( k );
                         temp.push_back( std::find (
                                             PCA::gens.back().begin(),
                                             PCA::gens.back().end(),
                                             temp_gen ) -
                                        PCA::gens.back().begin() );
                    };
                    new_addCupCap.push_back( temp );
                    temp.clear();
                    new_addCupGCC.push_back( temp_bool );
                    temp_bool.clear();
               };
               PCA::addCup.push_back( new_addCupCap );
               PCA::addCupGCC.push_back( new_addCupGCC );
               //
               new_addCupCap.clear();// recycle old container
//                std::cout << PCA::gens.back().size() << " length gens back.\n" ;
               for ( auto gen : PCA::gens.back()){
                    auto temp_gen {gen};
                    for ( size_t k=0; k < 2*PCA::gens.size()-1   ; ++k ){
//                          std::cout << " addCap: " << k << std::flush;
                         temp_gen = gen;
                         temp_gen.addCap( k );
                         temp.push_back( std::find ( 
                                             new_gens.begin(),
                                             new_gens.end(),
                                             temp_gen ) - 
                                        new_gens.begin() );
                    };
//                     std::cout << "\n" << std::flush;
                    new_addCupCap.push_back( temp );
                    temp.clear();
               };
               PCA::addCap.push_back( new_addCupCap );
               //
               PCA::gens.push_back( new_gens );
          };
     };
};


std::vector<std::vector<Dots>> PCA::vec;
std::vector<std::vector<int>> PCA::vec_sum;
// the first entry of vec will not be used, but it is needed to get the indexing right. The same applies to vec_sum.

/// computes more entries for PCA::vec if PCA::max < n
inline void expand_vec_if_needed ( const int &n ){
     while ( PCA::vec.size() < n+1 ) {
          if ( PCA::vec.size() < 2 ){
               PCA::vec = {{},{{0}}};
               PCA::vec_sum = {{},{0}};
          } else {
               // Create new vectors of length 'PCA::vec.size()'
               if ( n > maximum_number_of_tangle_strands ){
                    std::cerr << "There are "
                              << n
                              << " tangle strands, which is too many."
                              << " Try inceasing the value of 'maximum_number_of_tangle_strands' in 'sources/headers/constants.h'. ";
               };
               // create temporary copy of last entry of vec to append 1s
               std::vector<Dots> temp {PCA::vec.back()};
               std::vector<int> temp_sum {PCA::vec_sum.back()};
               for ( auto &v : temp ) {
                    v.push_back ( 1 );
               };
               for ( auto &v : temp_sum ) {
                    ++v;
               };
               // copy the last entry of vec and append dots_all_1 to it 
               Dots dots_all_1 {};
               for ( int j = 0; j < PCA::vec.size()-1; ++j ){
                    dots_all_1.push_back(1);
               };
               PCA::vec.push_back( PCA::vec.back() );
               PCA::vec_sum.push_back( PCA::vec_sum.back() );
               // append 0s to all Dots in new last entry of vec
               PCA::vec.back().push_back( dots_all_1 );
               PCA::vec_sum.back().push_back( PCA::vec.size()-2 );
               for ( auto &v : PCA::vec.back() ) {
                    v.push_back ( 0 );
               };
               // append temporary vector to last entry of vec
               PCA::vec.back().insert ( PCA::vec.back().end(),
                                        temp.begin(),
                                        temp.end() );
               PCA::vec_sum.back().insert ( PCA::vec_sum.back().end(),
                                             temp_sum.begin(),
                                             temp_sum.end() );
          };
     };
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Arcs
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Arcs::Arcs ( std::vector<TE> arcs ) :
     arcs ( arcs )
{
}

Arcs::Arcs ( TE size ){
     arcs.reserve ( 2 * size );
     for ( size_t k = 0; k < 2 * size; ++k ) {
          arcs.push_back ( 2 * size - 1 - k );
     };
}

//                          //
// output and sanity checks //
//                          //

Pairs Arcs::pairs() const
{
     // converts arc-format into pair-format
     Pairs pairs;
     for ( std::size_t i = 0; i < arcs.size(); ++i ) {
          if ( i < arcs[i] ){
               pairs.push_back ( {i,arcs[i]} );
          };
     };
     return pairs;
}

void Arcs::print() const
{
     std::cout << this->to_string();
}


std::string Arcs::to_string() const
{
     std::string output {};
     //
//      output +=  "arcs: ";
//      for ( auto e : arcs ){
//           output += std::to_string( e );
//           output += ",";
//      };
//      output.pop_back();
     for ( const auto &pair : this->pairs() ) {
          output +=  std::to_string ( pair.first );
          output +=  "–";
          output +=  std::to_string ( pair.second );
          output +=  " ";
     };
     //
     return output;
}

bool Arcs::check() const
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
     return true;
}



//              //
// main methods //
//              //

void Arcs::rotate ( const TE &top ){
     TE temp;
     for ( std::size_t i = 0; 2*i+1 < top ; ++i ) {
          // reverse the order of the first 'top' entries
          // eg if top=3, [1,4,3,2,5,0] -> [3,4,1,2,5,0]
          // and if top=4, [1,4,3,2,5,0] -> [2,3,4,1,5,0].
          temp = arcs[i];
          arcs[i] = arcs[top-1-i];
          arcs[top-1-i] = temp;
     };
     for (auto &a : arcs ){
          // relabel the entries if they are less than 'top'
          if ( a < top ) {
               a = top - 1 - a;
          };
     };
};

void Arcs::addCap( const TE &i ){
     for ( auto &entry : arcs ) {
          if ( entry >= i ) {
               entry += 2;
          };
     };
     Arcs new_arcs ({static_cast<TE>(i+1),i});
     arcs.insert ( arcs.begin()+i,new_arcs.arcs.begin(),new_arcs.arcs.end() );
};

void Arcs::addCup( const TE &i ){
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
};

bool Arcs::addCupGCC( const TE &i ) const {
     // 1 if the two ends were connected ( = new closed component)
     // 0 if the two ends belonged to different arcs
     return (arcs[i] == i+1);
};

IndexLL Arcs::components_to ( const Arcs &a2 ) const
{
     // components from this to obj; a component is a list of TEIs,
     // starting with the lowest TEI.  A component always has an even
     // number of TEIs.  The components are ordered by their lowest TEI
     // (ie the first entry of the component).
     IndexLL allcomponents;
     auto arcs2 = a2.arcs;
     TE strands {static_cast<TE>(arcs.size()/2)};
     // output list that will get populated by this method
     IndexL done;
     done.reserve ( 2 * strands );
     // list of TEIs which have already been sorted into components
     for ( TE i = 0; i < 2 * strands; ++i ) {
          //find component for ith tangle end
          if ( std::find ( done.begin(), done.end(), i ) == done.end() ) {
               //continue if we don't already have a component; otherwise move
               //on; equivalent to "i not in done"
               IndexL component;
               TE cur {i};
               int overflow {0};
               do {
                    if (overflow>100){
                         std::cerr << "ERROR overflow in components_to" << std::flush;
                         abort();
                    };
                    ++overflow;
                    done.push_back ( cur );
                    component.push_back ( cur );
                    cur = arcs[cur];
                    done.push_back ( cur );
                    component.push_back ( cur );
                    cur = arcs2[cur];
               } while ( cur !=  i );
               allcomponents.push_back ( component );
          };
     };
     return allcomponents;
}



//                      //
// overloaded operators //
//                      //
bool Arcs::operator== ( const Arcs &arcs2 ) const
{
     // ignore gradings
     return ( arcs == arcs2.arcs );
};

bool Arcs::operator!= ( const Arcs &arcs2 ) const
{
     // ignore gradings
     return ( arcs != arcs2.arcs );
};




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  CobObj
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

CobObj::CobObj ( TE strands, TE top, size_t index, Grading h, Grading q ) :
     strands ( strands ),
     top ( top ),
     index ( index ),
     h ( h ),
     q ( q )
{
     expand_gens_if_needed( strands );
}

CobObj::CobObj ( TE strands, TE top, Arcs arcs, Grading h, Grading q ) :
     strands ( strands ),
     top ( top ),
     h ( h ),
     q ( q )
{
     expand_gens_if_needed( strands );
     arcs.rotate( top );
     index = std::find( PCA::gens[strands].begin(),
                      PCA::gens[strands].end(),
                      arcs ) - PCA::gens[strands].begin();
}

CobObj::CobObj ( TE strands ) :
     strands ( strands ),
     top ( strands )
{
     expand_gens_if_needed( strands );
     Arcs arcs ( strands );
     index = std::find( PCA::gens[strands].begin(),
                      PCA::gens[strands].end(),
                      arcs ) - PCA::gens[strands].begin();
     h = 0;
     q = 0;
}


//                     //
// getters and setters //
//                     //
TE CobObj::get_top() const
{
     return top;
};
size_t CobObj::get_index() const
{
     return index;
};
TE CobObj::get_strands() const
{
     return strands;
};
TE CobObj::ends() const
{
     return 2 * strands;
};
std::vector<TE> CobObj::arcs() const
{
//      expand_gens_if_needed( strands );
     Arcs arcs {PCA::gens[strands][index]};
     return arcs.arcs;
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
     std::string output = "CobObj (h=";
     output += std::to_string( h );
     output += ", q=";
     output += std::to_string( q );
     output += ", index=";
     output += std::to_string( index );
     output += "):\n";
     //
     output +=  "    top:   ";
     for ( TE i = 0; i < top; ++i ) {
          output +=  std::to_string ( i );
          output +=  " ";
     };
     output +=  "\n    bot:   ";
     for ( TE i = 0; i < 2 * strands - top; ++i ) {
          output +=  std::to_string ( top+i );
          output +=  " ";
     };
     output +=  "\n    arcs:  ";
     Arcs arcs {PCA::gens[strands][index]};
     arcs.rotate( top );     
     for ( const auto &pair : arcs.pairs() ) {
          output +=  std::to_string ( pair.first );
          output +=  "–";
          output +=  std::to_string ( pair.second );
          output +=  " ";
     };
     output +=  "\n";
     //
     return output;
}

bool CobObj::check() const
{
     auto arcs {this->arcs()};
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
     for ( const auto &pair1 : Arcs(this->OneSided().arcs()).pairs() ) {
          TE p1 {pair1.first};
          TE p2 {pair1.second};
          for ( const auto &pair2 : Arcs(this->OneSided().arcs()).pairs() ) {
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
     return ( top == obj.top && strands == obj.strands );
};



//              //
// main methods //
//              //
CobObj CobObj::OneSided() const
{
     // rotate bottom half of the tangle to the top right such that the
     // last top and last bot tangle end sit next to each other
     return CobObj ( strands,0,PCA::gens[strands][index],h,q );
}

bool to_BNObj( const TE &strands, const TE &top, const size_t &index ){
     // convert CobObj to BNObj; this only works for the CobObj b and
     bool type {false};// vertical
     if ( strands == 1){
          type = true;
     } else if ( strands == 2) {
          // index 0 = [1,0,3,2], index 1 = [3,2,1,0]
          if ( ( index == 0 && top == 2 ) || ( index == 1 && top != 2 ) ){
               // horizontal
               type = true;
          };
     } else {
          PCA::gens[strands][index].print();
          std::cerr << "Cannot convert this CobObj (with "
                    << strands 
                    << " strands, index "
                    << index
                    << ", and "
                    << top
                    << " tangle ends at the top) to BNObj!\n";
          abort();
          // sanity check;
     };
     return type;
}

BNObj CobObj::to_BNObj() const
{
     // convert CobObj to BNObj; this only works for the CobObj b and
     return BNObj ( ::to_BNObj( strands,top,index ),h,q );
}

void CobObj::AddCap ( const TE &i )
{
     // add two extra ends before the ith TEI, assumed to be at the
     // tangle bottom, and connect them by an arc.
     expand_gens_if_needed( strands+1 );
     index = PCA::addCap[strands][index][ i ];
     ++strands;
}

void CobObj::AddCup ( const TE &i )
{
     // join the ith TEI, assumed to be at the tangle bottom, to the
     // next and join the two arcs if neccessary.
     index = PCA::addCup[strands][index][ i ];
     --strands;
}


bool CobObj::AddCupGivesClosedComponent ( const TE &i ) const
{
     // 1 if the two ends were connected ( = new closed component)
     // 0 if the two ends belonged to different arcs
     return PCA::addCupGCC[strands][index][ i ];
}

IndexLL CobObj::components_to ( const CobObj &obj2 ) const 
{
     return PCA::gens[strands][index].components_to( PCA::gens[strands][obj2.index] );
}

//                      //
// overloaded operators //
//                      //
bool CobObj::operator  == ( const CobObj &obj2 ) const
{
     // ignore gradings
     return ( ( strands == obj2.strands ) &&
              ( top == obj2.top ) &&
              ( index == obj2.index ) );
};

bool CobObj::operator!= ( const CobObj &obj2 ) const
{
     // ignore gradings
     return ( ( strands !=  obj2.strands ) ||
              ( top !=  obj2.top ) ||
              ( index !=  obj2.index ) );
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
     return hpower < deco2.hpower;
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
     return Deco<Coeff> ( hpower+deco2.hpower,new_dots,coeff*(deco2.coeff) );
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//          CobMor
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
CobMor<Coeff>::CobMor ( int i ) :
     strands ( 1 ),
     top ( 1 ),
     front ( 0 ),
     back ( 0 ),
     decos ( {} ),
     comps ( {} )
{
}

template<typename Coeff>
CobMor<Coeff>::CobMor() :
     strands ( 1 ),
     top ( 1 ),
     front ( 0 ),
     back ( 0 ),
     decos ( {} ),
     comps ( {} )
{
}

template<typename Coeff>
CobMor<Coeff>::CobMor (
     TE strands,
     TE top,
     size_t front,
     size_t back,
     std::vector<Deco<Coeff>> decos,
     IndexLL comps )
     :
     strands ( strands ),
     top ( top ),
     front ( front ),
     back ( back ),
     decos ( decos ),
     comps ( comps )
{
}

template<typename Coeff>
CobMor<Coeff>::CobMor (
     TE strands,
     TE top,
     size_t front,
     size_t back,
     std::vector<Deco<Coeff>> decos )
     :
     strands ( strands ),
     top ( top ),
     front ( front ),
     back ( back ),
     decos ( decos ),
     comps ( PCA::gens[strands][front].components_to ( PCA::gens[strands][back] ) )
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
          output += "The zero-morphism.\n";
     } else {
          output += "CobMor (strands=";
          output += std::to_string(strands);
          output += ", top=";
          output += std::to_string(top);
          output += ");\n";
          output += "    from:  ";
          auto arcs {PCA::gens[strands][front]};
          arcs.rotate(top);
          output += arcs.to_string();
          output += "  (index=";
          output += std::to_string(front);
          output += ")\n    to:    ";
          arcs = PCA::gens[strands][back];
          arcs.rotate(top);
          output += arcs.to_string();
          output += "  (index=";
          output += std::to_string(back);
          output += ")\n    decos: ";
          //
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
                    if (j < top){
                         output +=  std::to_string ( top-1-j );
                    } else {
                         output +=  std::to_string ( j );
                    };
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

// template<typename Coeff>
// Grading CobMor<Coeff>::gr_q() const
// {
//      // assuming decos is non-empty
//      Grading gr {decos.front().gr_q() };
//      for ( const auto &deco : decos ) {
//           if ( deco.gr_q() != gr ) {
//                this->print();
//                std::cerr << "morphism is not homogeneous!\n";
//                abort();
//           };
//      };
//      return back.get_q()-front.get_q()+gr-front.ends() /2;
// }

template<typename Coeff>
bool CobMor<Coeff>::check (
     const CobObj &obj_from,
     const CobObj &obj_to ) const
{
     if ( (front !=  obj_from.get_index()) || (strands != obj_from.get_strands()) || (top != obj_from.get_top()) ) {
          return false;
     };
     if ( (back !=  obj_to.get_index()) || (strands != obj_to.get_strands()) || (top != obj_to.get_top()) ) {
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
     std::sort(decos.begin(),decos.end());
     // alternative with decos = std::list<Deco>: 
     // decos.sort() // Could be marginally faster
     //  2) for each deco, except the first, check if it is equal to the
     //  predecessor except for the coefficient.
     std::vector<Deco<Coeff>> new_decos {decos.front() };
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
     return *this;
}

template<typename Coeff>
BNMor<Coeff> CobMor<Coeff>::to_BNMor ( TE special_end ) const
{
     BNObj obj_from = BNObj( to_BNObj( strands,top,front ) );
     BNObj obj_to = BNObj( to_BNObj( strands,top,back ) );
     std::list<Label<Coeff>> new_labels;
     if ( comps.empty() ) {
          return BNMor<Coeff> ( 0 );
     } else if ( comps.size() == 1 ) {
          // morphism between different idempotents
          for ( const auto &deco : decos ) {
               if ( deco.dots.front() == 0 ) {
                    if ( strands == 1 ) {
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
     std::vector<Deco<New_Coeff>> new_decos;
     for ( const auto &deco : decos ) {
          New_Coeff new_coeff = deco.coeff.n;
          if ( coeff_is_0 ( new_coeff ) != true ) {
               new_decos.push_back ( Deco<New_Coeff> ( deco.hpower,deco.dots,new_coeff ) );
          };
     };
     return CobMor<New_Coeff> ( strands,top,front,back,new_decos );
};

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::AddCap (
     const size_t &start,
     const size_t &end,
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
     new_comps.insert ( new_comps.begin()+k, {i,static_cast<TE>(i+1)} );
     // insert an entry '0' in each deco.dots
     auto new_decos = decos;
     for ( auto &deco : new_decos ) {
          deco.dots.insert ( deco.dots.begin()+k,0 );
     };
     return CobMor<Coeff> ( strands+1,top,start,end,new_decos,new_comps );
}

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::AddCup22 (
     const size_t &start,
     const size_t &end,
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
     std::vector<Deco<Coeff>> new_decos;
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
     return CobMor<Coeff> ( strands-1,top,start, end, new_decos, new_comps );
}

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::AddCupMixed (
     const size_t &start,
     const size_t &end,
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
     IndexLL new_comps = PCA::gens[strands-1][start].components_to ( PCA::gens[strands-1][end] );
     // compute the new order for comps:
     IndexL new_order = new_order_new2old ( new_comps,comp_names );
     // update the index of the comp that previously contained i
     // and i+1:
     unsigned int comp_i_new = std::find ( new_order.begin(),new_order.end(),comp_i )-new_order.begin();
     // updating the corresponding entry of deco.dots in all decos and
     // updating the coefficient according to the following three cases:
     auto old_decos = decos;//make copy
     std::vector<Deco<Coeff>> new_decos;
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
     return CobMor<Coeff> ( strands-1,top,start, end, new_decos, new_comps );
}

template<typename Coeff>
CobMor<Coeff> CobMor<Coeff>::AddCup11 (
     const size_t &start,
     const size_t &end,
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
     IndexLL new_comps = PCA::gens[strands-1][start].components_to ( PCA::gens[strands-1][end] );
     // Update the decos:
     auto old_decos = decos;//make copy
     std::vector<Deco<Coeff>> new_decos;
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
     return CobMor<Coeff> ( strands-1,top,start, end, new_decos, new_comps );
}



//                      //
// overloaded operators //
//                      //
template<typename Coeff>
bool CobMor<Coeff>::operator  == ( const CobMor<Coeff> &cob2 ) const
{
     if ( strands != cob2.strands ){
          return false;
     };
     if ( top != cob2.top ){
          return false;
     };   
     if ( front!= cob2.front ) {
          return false;
     };
     if ( back!= cob2.back ) {
          return false;
     };
     if ( decos.size() != cob2.decos.size() ) {
          return false;
     };
     std::vector<Deco<Coeff>> temp {cob2.decos};
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
     std::vector<Deco<Coeff>> new_decos {decos};
     for ( auto &deco : new_decos ) {
          deco.coeff = -deco.coeff;
     };
     return CobMor<Coeff> ( strands,top,front,back,new_decos,comps );
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
               x = arcs.arcs[TEI];
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
                    x = arcs.arcs[TEI];
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
     /// Each instance of this structure corresponds to a component C of the joined up cobordisms cob2*cob1 and records the following data:
     
     /// components of the first cobordism that lie on C
     IndexL partition1;
     /// components of the second cobordism that lie on C
     IndexL partition2;
     /// genus of C
     unsigned int genus;
     /// number of (open) components of new basic cobordism that are obtained by maximally neck-cutting C
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
     std::vector<CobMultHelper> output {};
     IndexLL partition { partitionGenerator ( new_comps,arcs ) };
//      std::cout << "partition: " << stringLL ( partition ) << std::flush ;
     output.reserve ( partition.size() );
     IndexL new_order {};
     IndexL part1 {};
     IndexL part2 {};
     IndexL comp {}; //'old_comp'

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


/// generating function for the list of all crossingless 0-2\f$i\f$-tangles with \f$i<n\f$ 

/// All crossingless tangles are in arc format. 
/// The definition is recursive. We subdivide the tasks into the subtasks of finding those clts that connect 0 to (2x+1):
///
/// 0 . . . . . (2x+1) . . . (2n-1)
/// 0---(2x+1)
///
/// This is in preparation for precomputing the algebra


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
     IndexLL new_comps { PCA::gens[strands][cob1.front].components_to ( PCA::gens[strands][back] ) };
     auto X { CobMultHelperFun ( new_comps,PCA::gens[strands][cob1.back],comps,cob1.comps ) };
     std::vector<CobMultHelper> cobMultVec { std::get<0> ( X ) };
     IndexL new_order { std::get<1> ( X ) };

     // temporary variables neded below
     std::vector<Deco<Coeff>> new_decos {};
     std::vector<std::vector<Deco<Coeff>>> part_decos {};
     std::vector<Deco<Coeff>> new_part_decos {};
     std::vector<Deco<Coeff>> more_decos {};
     std::vector<Deco<Coeff>> temp {};
     unsigned int n {0};
     unsigned int g {0};
     int r {0};
     
//      std::vector<int> r1 {};
//      r1.reserve( cobMultVec.size() );
//      Eigen::Matrix<int,Eigen::Dynamic,Eigen::Dynamic> r2( decos.size(),
//                                                           cobMultVec.size() );
//      for ( int j = 0; j < decos.size(); ++j ) {
//           for ( int k = 0; k < cobMultVec.size(); ++k ) {
//                r = 0;// total #dots on this old component
//                for ( const auto &l : cobMultVec[k].partition2 ) {
//                     r +=  decos[j].dots[l];
//                };
//                r2.coeffRef(j,k) = r;
//           };
//      };
     for ( int i=0; i < cob1.decos.size(); ++i ) {
//           r1.clear();
//           for ( const auto &c : cobMultVec ) {
//                r = 0;// total #dots on this old component
//                for ( const auto &l : c.partition1 ) {
//                     r +=  cob1.decos[i].dots[l];
//                };
//                r1.push_back( r );
//           };
          for ( int j=0; j < decos.size(); ++j ) {
               part_decos.clear();
               // for each element of the partition, compute the partial
               // decorations on the new components corresponding to that
               // element; a partial decoration is of type 'Deco', but the
               // length of the dot-vectors are equal to the length of the
               // partition element.
               for ( int k = 0; k < cobMultVec.size(); ++k ) {
//                     r = r1[k]+r2.coeff(j,k);// total #dots on this old component
                    r = 0;
                    for ( const auto &l : cobMultVec[k].partition1 ) {
                         r +=  cob1.decos[i].dots[l];
                    };
                    for ( const auto &l : cobMultVec[k].partition2 ) {
                         r +=  decos[j].dots[l];
                    };
                    n = cobMultVec[k].partitionLength;
                    g = cobMultVec[k].genus;
                    new_part_decos.clear();
                    if ( r > 0 ) {
                         new_part_decos.push_back ( Deco<Coeff> ( g+r-1,Dots ( n,1 ),1 ) );
                    } else {
                         // make sure, CobMor::vec[n] is already computed.
                         expand_vec_if_needed( n );
                         for ( int v=0; v < PCA::vec[n].size(); ++v ) {
                              // coefficients/signs for the deco of each dot_vector
                              if ( ( g+n-PCA::vec_sum[n][v] ) %2 == 0 ) {
                                   new_part_decos.push_back ( Deco<Coeff> ( g+n-PCA::vec_sum[n][v]-1,PCA::vec[n][v],-1 ) );
                              } else {
                                   new_part_decos.push_back ( Deco<Coeff> ( g+n-PCA::vec_sum[n][v]-1,PCA::vec[n][v],1 ) );
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
                                cob1.decos[i].hpower+decos[j].hpower,
                                {},
                                cob1.decos[i].coeff*decos[j].coeff );
               };
               new_decos.insert ( new_decos.end(),more_decos.begin(),more_decos.end() );
          };
     };
     for ( auto &deco : new_decos ) {
          deco.reorder_old2new ( new_order );
     };
     //reorder the dots according to the partition.
     return CobMor<Coeff> ( strands,top,cob1.front,back,new_decos,new_comps ).simplify();
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
     std::vector<Deco<Coeff>> new_decos = decos;// concatenate decos1 and decos2
     new_decos.insert ( new_decos.end(), cob2.decos.begin(), cob2.decos.end() );
     return CobMor<Coeff> ( strands,top,front, back, new_decos, comps ).simplify();
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


