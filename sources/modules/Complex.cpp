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



#ifndef COMPLEX_TPP
#define COMPLEX_TPP
#ifndef COMPLEX_H
#include "../headers/Complex.h"
#include "../headers/Tangles.h"
#include "../headers/constants.h"
#endif


/// \file Complex.cpp
/// \brief implements the category of complexes over a category, such as the Bar-Natan algebra \f$\mathcal{B}\f$ from \cite KWZ or the universal cobordism category from \cite BarNatanKhT

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Complex_Base
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Obj,
         template <typename> class Mor,
         typename Coeff>
Complex_Base<Obj,Mor,Coeff>::Complex_Base (
     std::vector<Obj> objects,
     Eigen::SparseMatrix<Mor<Coeff>> diffs )
     :
     objects ( objects ),
     diffs ( diffs )
{
     cancelled_indices= {};
}

template<typename Obj,
         template <typename> class Mor,
         typename Coeff>
Complex_Base<Obj,Mor,Coeff>::Complex_Base ()
     :
     objects ( {} ),
        diffs ( {} )
{
     cancelled_indices = {};
}

//                     //
// getters and setters //
//                     //
template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
std::vector<Obj> Complex_Base<Obj,Mor,Coeff>::get_objects() const
{
     return objects;
};

template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
Eigen::SparseMatrix<Mor<Coeff>> Complex_Base<Obj,Mor,Coeff>::get_diffs() const
{
     return diffs;
};

template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
std::list<unsigned int> Complex_Base<Obj,Mor,Coeff>::get_cancelled_indices() const
{
     return cancelled_indices;
};

//                          //
// output and sanity checks //
//                          //
template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
std::string Complex_Base<Obj,Mor,Coeff>::to_string () const
{
     std::string output = "";
     //
     if ( cancelled_indices.size() !=0 ) {
          output += "\n\nWARNING\n";
          output += "This complex has not been resized after cancellation!\n";
          output += "cancelled_indices=";
          output +=  stringL ( cancelled_indices );
     };
     //
     output += "\n    === objects ===\n";
     for ( std::size_t i=0; i<objects.size(); i++ ) {
          output += "object ";
          output += std::to_string ( i );
          output += ":";
          output += objects[i].to_string();
          output += "\n";
     };
     //
     output += "    === diffs ===";
     for ( int k=0; k<diffs.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<Mor<Coeff>>::InnerIterator it ( diffs,k ); it; ++it ) {
               output += "\n∂:";
               output += std::to_string ( it.col() );
               output += "–––>";
               output += std::to_string ( it.row() );
               output += ": ";
               output += it.value().to_string();
          };
     };
     output += "\n\n";
     return output;
}

template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
void Complex_Base<Obj,Mor,Coeff>::print (
     const ssize_t &index_from,
     const ssize_t &index_to ) const
{
     // print only one component of the differential.
     std::cout << "\n   %%% differential "
               << index_from << "–––>" << index_to << " %%%\n";
     this->diffs.coeff ( index_to,index_from ).print();
}


template<typename Coeff>
void Complex<CobObj,CobMor,Coeff>::print ( const std::string &name, const std::string &metadata ) const
{
     if ( name.empty() ) {
          std::cout << this->to_string();
     } else if ( metadata.empty() ) {
          // Providing metadata should be strictly enforced
          std::cerr << "No metadata was specified.";
          abort();
     } else {
          std::ofstream file ( name );
          file << "% file '";
          file << name;
          file << "', ";
          file << metadata;
          file << "\n";
          file << this->to_string();
          file.close();
     };
}

template<typename Coeff>
void Complex<CobObj,CobMor,Coeff>::AddCap ( const unsigned int &i )
{
     // We are assuming that the TEI i is at the bottom of the tangle;
     //
     for ( auto &obj : this->objects ) {
          obj.AddCap ( i );
     };
     for ( int k=0; k<this->diffs.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<CobMor<Coeff>>::InnerIterator it ( this->diffs,k ); it; ++it ) {
               it.valueRef() = it.value().AddCap ( this->objects[it.col()],this->objects[it.row()],i );
          };
     };
     //  return Complex_Base<CobObj,CobMor<Coeff>,Coeff>(objects,diffs);
};

template<typename Coeff>
void Complex<CobObj,CobMor,Coeff>::AddCup ( const unsigned int &i )
{
     // We are assuming that the TEI i is at the bottom of the tangle;
     //
     std::vector<CobObj> new_objects;
     std::vector<bool> index_pusher;
     IndexL new_indices;
     new_objects.reserve ( 2*this->objects.size() ); // max needed capacity
     index_pusher.reserve ( this->objects.size() );
     new_indices.reserve ( this->objects.size() );
     int new_index{0};

     // Update objects and create list to keep track of which indices get doubled (1) and which do not (0);
     for ( auto &obj : this->objects ) {
          bool cc=obj.AddCupGivesClosedComponent ( i );
          index_pusher.push_back ( cc );
          new_indices.push_back ( new_index );
          new_index+=1+cc;
          obj.AddCup ( i );
          if ( cc ) {
               obj.shift_hq ( 0,-1 );
               // shift first object
               new_objects.push_back ( obj );
               obj.shift_hq ( 0,2 );
               // shift second object
          };
          new_objects.push_back ( obj );
     };
     new_objects.shrink_to_fit();
     // Update differentials
     //
     // Note: we are using the first delooping isomorphism from
     // Observation 4.18 in [KWZ], with the top generator in the middle
     // corresponds to the first generator of the pair.
     std::vector<Eigen::Triplet<CobMor<Coeff>>> triplets {};
     for ( int k=0; k<this->diffs.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<CobMor<Coeff>>::InnerIterator it ( this->diffs,k ); it; ++it ) {
               auto start=new_indices[it.col()];
               auto end=new_indices[it.row()];
               if ( index_pusher[it.col()] ) {
                    if ( index_pusher[it.row()] ) {
                         //CASE 2->2//
                         auto mor00=it.value().AddCup22 ( new_objects[start],new_objects[end],i,0,0 );
                         auto mor10=it.value().AddCup22 ( new_objects[start+1],new_objects[end],i,1,0 );
                         auto mor11=it.value().AddCup22 ( new_objects[start+1],new_objects[end+1],i,1,1 );
                         //
                         if ( mor00.is_0() ==false ) {
                              if ( mor00.gr_q() !=0 ) {
                                   std::cerr << "mor00: wrong q-grading";
                              };
                              triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( end,start,mor00 ) );
                         };
                         if ( mor10.is_0() ==false ) {
                              if ( mor10.gr_q() !=0 ) {
                                   std::cerr << "mor10: wrong q-grading";
                              };
                              triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( end,start+1,mor10 ) );
                         };
                         if ( mor11.is_0() ==false ) {
                              if ( mor11.gr_q() !=0 ) {
                                   std::cerr << "mor11: wrong q-grading";
                              };
                              triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( end+1,start+1,mor11 ) );
                         };
                    } else {
                         //CASE 2->1//
                         auto mor0=it.value().AddCupMixed ( new_objects[start],new_objects[end],i,0,0 );
                         auto mor1=it.value().AddCupMixed ( new_objects[start+1],new_objects[end],i,1,0 );
                         if ( mor0.is_0() ==false ) {
                              if ( mor0.gr_q() !=0 ) {
                                   std::cerr << "mor0: wrong q-grading";
                              };
                              triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( end,start,mor0 ) );
                         };
                         if ( mor1.is_0() ==false ) {
                              if ( mor1.gr_q() !=0 ) {
                                   std::cerr << "mor1: wrong q-grading";
                              };
                              triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( end,start+1,mor1 ) );
                         };
                    };
               } else {
                    if ( index_pusher[it.row()] ) {
                         //CASE 1->2//
                         auto mor0=it.value().AddCupMixed ( new_objects[start],new_objects[end],i,1,0 );
                         auto mor1=it.value().AddCupMixed ( new_objects[start],new_objects[end+1],i,1,1 );
                         if ( mor0.is_0() ==false ) {
                              if ( mor0.gr_q() !=0 ) {
                                   std::cerr << "mor0: wrong q-grading";
                              };
                              triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( end,start,mor0 ) );
                         };
                         if ( mor1.is_0() ==false ) {
                              if ( mor1.gr_q() !=0 ) {
                                   std::cerr << "mor1: wrong q-grading";
                              };
                              triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( end+1,start,mor1 ) );
                         };
                    } else {
                         //CASE 1->1//
                         auto mor=it.value().AddCup11 ( new_objects[start],new_objects[end],i );
                         if ( mor.is_0() ==false ) {
                              if ( mor.gr_q() !=0 ) {
                                   std::cerr << "mor: wrong q-grading";
                              };
                              triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( end,start,mor ) );
                         };
                    };
               };
          };
     };
     Eigen::SparseMatrix<CobMor<Coeff>> new_diffs ( new_index,new_index );
     new_diffs.setFromTriplets ( triplets.begin(),triplets.end() );
     // update complex
     this->objects=new_objects;
     this->diffs=new_diffs;
};

template<typename Coeff>
void Complex<CobObj,CobMor,Coeff>::AddCrossing (
     const unsigned int &i,
     const bool &sign,
     const bool &XorY )
{
//      std::cout << " at i = " << i
//                << ", sign = " << to_string ( sign )
//                << ", XorY = " << to_string ( XorY );
     // compute subcomplex for =:
     Complex<CobObj,CobMor,Coeff> AddCupCap = *this;
     AddCupCap.AddCup ( i );
     AddCupCap.AddCap ( i );
     // objects left:
     std::vector<CobObj> new_objects =
          ( XorY ) ? AddCupCap.objects : this->objects;
     size_t dim1 {new_objects.size() };
     // objects right:
     std::vector<CobObj> new_objects2 =
          ( XorY ) ? this->objects : AddCupCap.objects;
     // grading objects left:
     if ( sign ) {
          for ( auto &obj : new_objects ) {
               obj.shift_hq ( 0,1 );
          };
     } else {
          for ( auto &obj : new_objects ) {
               obj.shift_hq ( -1,-2 );
          };
     };
     // grading objects right:
     if ( sign ) {
          for ( auto &obj : new_objects2 ) {
               obj.shift_hq ( 1,2 );
          };
     } else {
          for ( auto &obj : new_objects2 ) {
               obj.shift_hq ( 0,-1 );
          };
     };
     // concatenate the two lists of objects:
     new_objects.insert ( new_objects.end(),new_objects2.begin(),new_objects2.end() );
     //
     // Compute differentials
     Eigen::SparseMatrix<CobMor<Coeff>> new_diffs1 =
                                          ( XorY ) ? AddCupCap.diffs : this->diffs;
     Eigen::SparseMatrix<CobMor<Coeff>> new_diffs2 =
                                          ( XorY ) ? this->diffs : AddCupCap.diffs;
     //
     // first diagonal block:
     std::vector<Eigen::Triplet<CobMor<Coeff>>> triplets;
     for ( int k=0; k<new_diffs1.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<CobMor<Coeff>>::InnerIterator it ( new_diffs1,k ); it; ++it ) {
               triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( it.row(),it.col(),it.value() ) );
          };
     };
     //
     // second diagonal block:
     for ( int k=0; k<new_diffs2.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<CobMor<Coeff>>::InnerIterator it ( new_diffs2,k ); it; ++it ) {
               triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( it.row()+dim1,it.col()+dim1,it.value() ) );
          };
     };
     //
     // quasi-diagonal matrix connecting the two blocks:
     size_t n_arcs=new_objects.front().get_arcs().size() /2;
     // number of components
     Deco<Coeff> deco_no_closed_comp=Deco<Coeff> ( 0,std::vector<bool> ( n_arcs-1 ),Coeff ( 1 ) );
     Deco<Coeff> deco_closed_comp=Deco<Coeff> ( 0,std::vector<bool> ( n_arcs ),Coeff ( 1 ) );
     IndexLL comps;
     //
     //Computing the third block.
     if ( XorY ) {
          // CASE X:
          int from{0};// <-/-> CASE Y
          for ( size_t to=dim1; to<new_objects.size(); ++to ) { // <-/-> CASE Y
               // Adjust sign in the chain complex:
               Coeff diff_sign{1};
               if ( new_objects[from].h_is_even() ) {
                    diff_sign*=-1;
               };
               // both from-objects in this iteration have the same homological
               // grading, but we could also use the single to-object here.
               if ( new_objects[to].AddCupGivesClosedComponent ( i ) ) {
                    // <-/-> CASE Y
                    // differential splits in two:
                    comps=new_objects[from].components_to ( new_objects[to] );
                    // find the index of the component {i,i+1}:
                    size_t k{0};
                    for ( auto comp : comps ) {
                         if ( comp.front() ==i ) {
                              break;
                         };
                         ++k;
                    };
                    //
                    // 0--->.: dot - H  // <-/-> CASE Y
                    // (Case X 0--->.)
                    // add the dot to the right component:
                    deco_closed_comp.switch_dot ( k );
                    // dot cobordism:
                    auto mor=CobMor<Coeff> ( new_objects[from],new_objects[to], {deco_closed_comp},comps ) *diff_sign;
                    // remove the dot again:
                    deco_closed_comp.switch_dot ( k );
                    deco_closed_comp.add_H();
                    // -H cobordism:
                    mor+= ( CobMor<Coeff> ( new_objects[from],new_objects[to], {deco_closed_comp},comps ) * ( -diff_sign ) );
                    // lower Hpower again, so we may recycle 'deco_closed_comp':
                    deco_closed_comp.sub_H();
                    // Sanity check:
                    if ( mor.gr_q() !=0 ) {
                         std::cerr << "Case X 0--->.: Wrong quantum grading!";
                    };
                    // Add this component to the differential:
                    triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( to,from,mor ) );
                    // 1--->.: no dot // <-/-> CASE Y
                    ++from; // <-/-> CASE Y
                    //
                    // (Case X 1--->.)
                    //
                    mor=CobMor<Coeff> ( new_objects[from],new_objects[to], {deco_closed_comp},comps ) *diff_sign;
                    // Sanity check:
                    if ( mor.gr_q() !=0 ) {
                         std::cerr << "Case X 1--->.: Wrong quantum grading!";
                    };
                    // Add this component to the differential:
                    triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( to,from,mor ) );
               } else {
                    // differential is a simple saddle (Case X .--->.)
                    auto mor=CobMor<Coeff> ( new_objects[from],new_objects[to], {deco_no_closed_comp} ) *diff_sign;
                    // Sanity check:
                    if ( mor.gr_q() !=0 ) {
                         std::cerr << "Case X .--->.: Wrong quantum grading!";
                    };
                    // Add this component to the differential:
                    triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( to,from,mor ) );
               };
               ++from;
          };
     } else {
          // CASE Y:
          size_t to{dim1};// <-/-> CASE X
          for ( size_t from=0; from<dim1; ++from ) { // <-/-> CASE X
               // Adjust sign in the chain complex:
               Coeff diff_sign{1};
               if ( new_objects[from].h_is_even() ) {
                    diff_sign*=-1;
               };
               //
               //
               if ( new_objects[from].AddCupGivesClosedComponent ( i ) ) {
                    // <-/-> CASE X
                    // differential splits in two:
                    comps=new_objects[from].components_to ( new_objects[to] );
                    // find the index of the component {i,i+1}:
                    size_t k{0};
                    for ( auto comp : comps ) {
                         if ( comp.front() ==i ) {
                              break;
                         };
                         ++k;
                    };
                    //
                    // .--->0: no dots // <-/-> CASE X
                    // (Case Y .--->0)
                    auto mor=CobMor<Coeff> ( new_objects[from],new_objects[to], {deco_closed_comp},comps ) *diff_sign;
                    // Sanity check:
                    if ( mor.gr_q() !=0 ) {
                         std::cerr << "Case Y .--->0: Wrong quantum grading!";
                    };
                    // Add this component to the differential:
                    triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( to,from,mor ) );
                    // .--->1: single dot // <-/-> CASE X
                    ++to;// <-/-> CASE X
                    // (Case Y .--->1)
                    // add the dot to the right component:
                    deco_closed_comp.switch_dot ( k );
                    mor=CobMor<Coeff> ( new_objects[from],new_objects[to], {deco_closed_comp},comps ) *diff_sign;
                    // Sanity check:
                    if ( mor.gr_q() !=0 ) {
                         std::cerr << "Case Y .--->1: Wrong quantum grading!";
                    };
                    // Add this component to the differential:
                    triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( to,from,mor ) );
                    // delete the dot again, so 'deco_closed_comp' can be recycled:
                    deco_closed_comp.switch_dot ( k );
               } else {
                    // differential is a simple saddle (Case .--->.)
                    auto mor=CobMor<Coeff> ( new_objects[from],new_objects[to], {deco_no_closed_comp} ) *diff_sign;
                    // Sanity check:
                    if ( mor.gr_q() !=0 ) {
                         std::cerr << "Case Y .--->.: Wrong quantum grading!";
                    };
                    // Add this component to the differential:
                    triplets.push_back ( Eigen::Triplet<CobMor<Coeff>> ( to,from,mor ) );
               };
               ++to;
          };
     }
     Eigen::SparseMatrix<CobMor<Coeff>> new_diffs ( new_objects.size(),new_objects.size() );
     new_diffs.setFromTriplets ( triplets.begin(),triplets.end() );
     // update complex
     this->objects=new_objects;
     this->diffs=new_diffs;
}

template<typename Coeff>
Complex<BNObj,BNMor,Coeff>
Complex<CobObj,CobMor,Coeff>::to_BN() const
{
     std::vector<BNObj> new_objects;
     new_objects.reserve ( this->objects.size() );
     for ( auto object : this->objects ) {
          new_objects.push_back ( object.to_BNObj() );
     };
     std::vector<Eigen::Triplet<BNMor<Coeff>>> triplets;
     triplets.reserve ( this->diffs.nonZeros() );
     for ( int k=0; k<this->diffs.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<CobMor<Coeff>>::InnerIterator it ( this->diffs,k ); it; ++it ) {
               // Converting differential it.col()–––>it.row()
               triplets.push_back ( Eigen::Triplet<BNMor<Coeff>> ( it.row(),it.col(),it.value().to_BNMor() ) );
          };
     };
     Eigen::SparseMatrix<BNMor<Coeff>> new_diffs ( this->diffs.rows(),this->diffs.cols() );
     new_diffs.setFromTriplets ( triplets.begin(),triplets.end() );
     new_diffs.prune ( NonZero<BNMor<Coeff>> );
     return Complex<BNObj,BNMor,Coeff> ( new_objects,new_diffs );
};

template<typename Coeff>
std::string Complex<CobObj,CobMor,Coeff>::optimize ( const std::vector<bool> &bot_orient, std::string tanglestring )
{
     if ( bot_orient.size() < 2 ) {
          return "";
     };
     if ( tanglestring.empty() ) {
          bool go_on {true};
          auto cx = *this;
          std::string twist {""};
          TE N = cx.objects.front().get_bot()-1;
          std::list<std::string> twists {};
          for ( TE i = 0; i<N; ++i ) {
               twists.push_back ( "x" + std::to_string ( i ) );
               twists.push_back ( "y" + std::to_string ( i ) );
          };
          size_t iter = 0;
          std::string feedback {""};
          while ( go_on ) {
               go_on = false;
               for ( const auto &twist : twists ) {
                    ++iter;
                    std:: cout << "\33[2K\r"
                               << spin ( iter )
                               << " Trying to reduce number of generators by adding twists. "
                               << feedback
                               << std::flush;
                    cx = Tangle ( twist,bot_orient ).CobComplex<Coeff> ( *this,false );
                    if ( cx.get_objects().size() < this->objects.size() ) {
                         *this = cx;
                         go_on = true;
                         tanglestring = tanglestring + twist;
                         feedback = "Added '" + tanglestring + "'. ";
                         tanglestring = tanglestring + ".";
                         break;
                    };
               };
          };
          if ( tanglestring.empty() == false ) {
               tanglestring.pop_back();
          };
          std::cout << "\33[2K\r";
          if ( tanglestring.empty() == false ) {
               std::cout << "Added twists '"
                         << tanglestring
                         << "' to complex to reduce the total number of generators.\n";
          };
          std::cout << std::flush;
     } else {
          std::cout << "Adding specified tangle '"
                    << tanglestring
                    << "' to complex "
                    << std::flush;
          *this = Tangle ( tanglestring ).CobComplex<Coeff> ( *this,false );
          std::cout << "\33[2K\rAdded specified tangle '"
                    << tanglestring
                    << "' to complex.\n"
                    << std::flush;
     };
     return tanglestring;
};


template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
bool Complex_Base<Obj,Mor,Coeff>::check() const
{
     // testing if front and back of components of diffs are compatible
     // with the objects.
     for ( int k=0; k<this->diffs.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<Mor<Coeff>>::InnerIterator it ( this->diffs,k ); it; ++it ) {
               if ( it.value().check ( this->objects[it.col()],
                                       this->objects[it.row()] ) !=true ) {
                    return false;
               };
          };
     };
     // check if d²=0.
     Eigen::SparseMatrix<Mor<Coeff>> d_squared = ( this->diffs ) * ( this->diffs );
     d_squared.prune ( NonZero<Mor<Coeff>> );
     if ( d_squared.nonZeros() !=0 ) {
          std::cerr << "Warning: the differential does not satisfy d²=0."
                    << "\nERROR: d²is not 0.\n\n"
                    << Complex_Base<Obj,Mor,Coeff> ( objects,d_squared ).to_string();
          return false;
     }
     return true;
}

//              //
// main methods //
//              //
template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
void Complex_Base<Obj,Mor,Coeff>::cancel()
{
     std::pair<int,int> id_coor=this->find_invertible();
     while ( id_coor.first!=diffs.outerSize() ) {
          this->cancel ( id_coor.second,id_coor.first );
          id_coor=this->find_invertible();
     }
     this->resize();
};

template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
void Complex_Base<Obj,Mor,Coeff>::cancel (
     const ssize_t &sourceindex,
     const ssize_t &targetindex )
{
     // we only add the indices of the cancelled generators to
     // 'cancelled_indices', erase all entries in the corresponding rows
     // and columns of 'diffs' and modify the differential. Make sure
     // that resize() is called at the end of all cancellations.
     cancelled_indices.push_back ( targetindex );
     cancelled_indices.push_back ( sourceindex );
     int Max=std::max ( targetindex,sourceindex );
     int Min=std::min ( targetindex,sourceindex );
     //
     auto label = diffs.coeffRef ( targetindex,sourceindex ).get_first_coeff();
     label= ( -1 ) /label;
     //
     Eigen::SparseMatrix<Mor<Coeff>> target_in=diffs.row ( targetindex );
     // auto gives: class Eigen::Block<Eigen::SparseMatrix<CobMor<Coeff>, 0, int>, 1, -1, false>
     Eigen::SparseMatrix<Mor<Coeff>> source_out=diffs.col ( sourceindex );
     // auto gives: class Eigen::Block<Eigen::SparseMatrix<CobMor<Coeff>, 0, int>, -1, 1, true>
     target_in.coeffRef ( 0,sourceindex ) =Mor<Coeff> ( 0 ); // not necessary
     source_out.coeffRef ( targetindex,0 ) =Mor<Coeff> ( 0 ); // not necessary
     // multiply by the negative of the inverse of the label coeff:
     for ( typename Eigen::SparseMatrix<Mor<Coeff>>::InnerIterator it ( source_out,0 ); it; ++it ) {
          it.valueRef() *=label;
     };
     //std::cout << "sparse_ratio: " << diffs.nonZeros()
     //	    << " / total: " << diffs.rows()*diffs.rows() << "\n";
     //std::cout << ((diffs.col(sourceindex))*(diffs.row(targetindex))).cols() << " rows\n";
     diffs+= ( source_out*target_in );
     //
     auto zeromor=Mor<Coeff> ( 0 );
     // empty the two columns and rows of the cancelled generators
     for ( int k=0; k<diffs.outerSize(); ++k ) {
          if ( ( k==Min ) || ( k==Max ) ) {
               for ( typename Eigen::SparseMatrix<Mor<Coeff>>::InnerIterator it ( diffs,k ); it; ++it ) {
                    it.valueRef() =zeromor;
               };
          } else {
               for ( typename Eigen::SparseMatrix<Mor<Coeff>>::InnerIterator it ( diffs,k ); it; ++it ) {
                    if ( ( it.index() ==Min ) || ( it.index() ==Max ) ) {
                         it.valueRef() =zeromor;
                    };
               };
          };
     };
     diffs.prune ( NonZero<Mor<Coeff>> );
};

template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
void Complex_Base<Obj,Mor,Coeff>::resize()
{
     if ( cancelled_indices.size() !=0 ) {
          cancelled_indices.sort();
          std::vector<int> new_indices;
          int counter=0;
          // resize objects and generate a list of new_indices
          std::vector<Obj> new_objects;
          auto it=cancelled_indices.begin();
          for ( unsigned int k=0; k<objects.size(); ++k ) {
               new_indices.push_back ( counter );
               if ( k==*it && it!=cancelled_indices.end() ) {
                    // Without the second condition, this runs over unless
                    // cancelled_indices contains the last object.
                    it++;
               } else {
                    new_objects.push_back ( objects[k] );
                    counter++;
               };
          };
          objects=new_objects;
          //
          // resize differential
          std::vector<Eigen::Triplet<Mor<Coeff>>> triplets;
          triplets.reserve ( diffs.nonZeros() );
          for ( int k=0; k<diffs.outerSize(); ++k ) {
               for ( typename Eigen::SparseMatrix<Mor<Coeff>>::InnerIterator it ( diffs,k ); it; ++it ) {
                    int new_row=new_indices[it.row()];
                    int new_col=new_indices[it.col()];
                    triplets.push_back ( Eigen::Triplet<Mor<Coeff>> ( new_row,new_col,it.value() ) );
               };
          };
          int n=cancelled_indices.size();
          Eigen::SparseMatrix<Mor<Coeff>> new_diffs ( diffs.rows()-n,diffs.cols()-n );
          new_diffs.setFromTriplets ( triplets.begin(),triplets.end() );
          diffs=new_diffs;
          cancelled_indices= {};
     };
};

template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
std::pair<int,int> Complex_Base<Obj,Mor,Coeff>::find_invertible() const
{
     for ( int k=0; k<diffs.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<Mor<Coeff>>::InnerIterator it ( diffs,k ); it; ++it ) {
               // Checking for an invertible component from it.col() to it.row()
               if ( it.value().is_inv() ) {
                    return std::pair<int,int> ( it.row(),it.col() );
               };
          };
     };
     return std::pair<int,int> ( diffs.outerSize(),diffs.outerSize() );
}

template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
void Complex_Base<Obj,Mor,Coeff>::isotopy (
     const ssize_t &start,
     const ssize_t &end,
     const Mor<Coeff> &mor )
{
     if ( diffs.coeff ( start,end ).is_0() == false ) {
          std::cerr <<"Error: This isotopy might not be legit and may change the homotopy class of the complex.";
     } else {
          // performing isotopy via start ---mor.to_string()---> end
          // add all start------>end------>x
          Eigen::SparseVector<Mor<Coeff>> post_comp=diffs.col ( end );
          // auto class Eigen::Block<Eigen::SparseMatrix<CobMor, 0, int>, -1, 1, true>
          for ( typename Eigen::SparseVector<Mor<Coeff>>::InnerIterator
                    it ( post_comp ); it; ++it ) {
               it.valueRef() =it.valueRef() *mor;
          };
          diffs.col ( start )+=post_comp;

          // subtract all x------>start------>end; this operation is more
          // costly, since diffs is a column-major matrix
          Mor<Coeff> mor1 {-mor};
          std::vector<std::pair<int,Mor<Coeff>>> pre_comp;
          for ( Eigen::Index k = 0; k<diffs.outerSize(); ++k ) {
               for ( typename Eigen::SparseMatrix<Mor<Coeff>>::InnerIterator
                         it ( diffs,k ); it; ++it ) {
                    if ( it.row() == start ) {
                         pre_comp.push_back (
                              std::pair<int,Mor<Coeff>> ( it.col(),mor1*it.value() ) );
                    };
               };
          };

          for ( auto e : pre_comp ) {
               diffs.coeffRef ( end,e.first )+=e.second;
          };
          diffs.makeCompressed();
          diffs.prune ( NonZero<Mor<Coeff>> );
     };
     if ( this->check() == false ) {
          std::cerr << "d² no longer satisfied. "
                    << "Component end_isotopy ->start_isotopy"
                    << this->diffs.coeff ( start,end ).to_string();
          exit ( 1 );
     };
};

template<typename Coeff>
Chains<Coeff> Complex<BNObj,BNMor,Coeff>::to_chains () const
{
     std::vector<Chain<Coeff>> chains {};
     //
     if ( this->is_loop_type() ==false ) {
          std::cerr << "complex is not loop-type, so I cannot compute the corresponding chains. Returning empty Chains.\n";
          return Chains<Coeff> ( {} );
     };
     //
     std::vector<long int> remaining {};
     long int N {this->diffs.outerSize() };
     remaining.reserve ( N );
     for ( long int i=0; i<N; ++i ) {
          remaining.push_back ( i );
     };
     //
     while ( remaining.empty() == false ) {
          // loop constructing all chains
          std::vector<Clink<Coeff>> chain {};
          long int start {remaining.back() };
          remaining.pop_back();
          long int current {start};
          long int previous {start};
          BNObj object;
          BNMor<Coeff> morphism;
          bool rightarrow {true};
          bool second_loop {true}; // do we need to run in the other direction from start?
          //
          bool go_on {true};
          while ( go_on ) {
               // loop constructing the first part of a single new chain starting at start.
               // We already know the object of the next clink
               object = this->objects[current];
               remaining.erase ( std::remove ( remaining.begin(), remaining.end(), current ), remaining.end() );
               previous = current;
               // but we need to search for the morphism of the next clink
               for ( const auto &k : remaining ) {
                    if ( this->diffs.coeff ( k,current ).is_0() == false ) {
                         // found morphism
                         morphism = this->diffs.coeff ( k,current );
                         current = k;
                         rightarrow = true;
                         break;
                    } else if ( this->diffs.coeff ( current,k ).is_0() == false ) {
                         // found morphism
                         morphism = this->diffs.coeff ( current,k );
                         current = k;
                         rightarrow = false;
                         break;
                    };
               };
               if ( current != previous ) {
                    // we found a morphism, so we need to continue
                    go_on = true;
               } else if ( this->diffs.coeff ( start,current ).is_0() == false && chain.size() > 1 ) {
                    // There a morphism  start <--- current, and this is not the whole chain.
                    morphism = this->diffs.coeff ( start,current );
                    rightarrow = true;
                    second_loop = false;
                    go_on = false;
               } else if ( this->diffs.coeff ( current,start ).is_0() == false && chain.size() > 1 ) {
                    // There a morphism start ---> current, and this is not the whole chain.
                    morphism = this->diffs.coeff ( current,start );
                    rightarrow = false;
                    second_loop = false;
                    go_on = false;
               } else {
                    morphism = BNMor<Coeff> ( object,object, {{}} );
                    go_on = false;
               };
               chain.push_back ( {object, morphism, rightarrow} );
          };
          //
          go_on = true;
          current = start;
          current = start;
          while ( go_on && second_loop ) {
               // find the second part of chain if it is an arc, ie not a loop
               previous = current;
               // now we do not know the object nor the morphism; let us search:
               for ( const auto &k : remaining ) {
                    if ( this->diffs.coeff ( k,current ).is_0() == false ) {
                         // found morphism
                         morphism = this->diffs.coeff ( k,current );
                         current = k;
                         rightarrow = false;
                         break;
                    } else if ( this->diffs.coeff ( current,k ).is_0() == false ) {
                         // found morphism
                         morphism = this->diffs.coeff ( current,k );
                         current = k;
                         rightarrow = true;
                         break;
                    };
               };
               if ( current != previous ) {
                    // we found a morphism, so we need to continue
                    object = this->objects[current];
                    remaining.erase ( std::remove ( remaining.begin(), remaining.end(), current ), remaining.end() );
                    chain.insert ( chain.begin(), {object, morphism, rightarrow} );
                    go_on = true;
               } else {
                    go_on = false;
               };
          };
          chains.push_back ( chain );
     };
     Chains<Coeff> output {Chains ( chains ) };
     output.fix_shorts();
     output.sort();
     output.concentrate_local_systems();
     return output;
};

template<typename Coeff>
Complex<BNObj,BNMor,Coeff> Complex<BNObj,BNMor,Coeff>::cone ( const int &n ) const
{
     if ( n < 0 ) {
          std::cerr << "the method 'cone' should be called with a non-negative argument.";
          return *this;
     } else if ( n==0 ) {
          return *this;
     };
     // cone : q^-n h^-1 X --H^n-> q^n h^0 X
     //
     // H = D -S^2, so
     // H^n = D^n (-1)^n S^2n
     std::list<Label<Coeff>> labels {{n,1}};
     if ( n % 2 == 0 ) {
          labels.push_back ( {-2*n,1} );
     } else {
          labels.push_back ( {-2*n,-1} );
     };
     // objects left:
     std::vector<BNObj> new_objects = this->objects;
     for ( auto &obj : new_objects ) {
          obj.shift_hq ( -1,-n );
     };
     size_t dim1 {new_objects.size() };
     // objects right:
     std::vector<BNObj> new_objects2 = this->objects;
     for ( auto &obj : new_objects2 ) {
          obj.shift_hq ( 0,n );
     };
     // concatenate the two lists of objects:
     new_objects.insert ( new_objects.end(),new_objects2.begin(),new_objects2.end() );
     //
     // Compute differentials
     Eigen::SparseMatrix<BNMor<Coeff>> new_diffs1 = this->diffs;
     Eigen::SparseMatrix<BNMor<Coeff>> new_diffs2 = this->diffs;
     //
     // first diagonal block:
     std::vector<Eigen::Triplet<BNMor<Coeff>>> triplets;
     for ( int k=0; k<new_diffs1.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<BNMor<Coeff>>::InnerIterator it ( new_diffs1,k ); it; ++it ) {
               triplets.push_back ( Eigen::Triplet<BNMor<Coeff>> ( it.row(),it.col(),-it.value() ) );
          };
     };
     //
     // second diagonal block:
     for ( int k=0; k<new_diffs2.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<BNMor<Coeff>>::InnerIterator it ( new_diffs2,k ); it; ++it ) {
               triplets.push_back ( Eigen::Triplet<BNMor<Coeff>> ( it.row()+dim1,it.col()+dim1,it.value() ) );
          };
     };
     //
     for ( size_t iter = 0; iter < new_objects2.size(); ++iter ) {
          triplets.push_back (
               Eigen::Triplet<BNMor<Coeff>> (
                    iter+dim1,
                    iter,
                    BNMor<Coeff> (
                         new_objects[iter],
                         new_objects2[iter],
                         labels ) ) );
     };
     //
     Eigen::SparseMatrix<BNMor<Coeff>> new_diffs ( new_objects.size(),new_objects.size() );
     new_diffs.setFromTriplets ( triplets.begin(),triplets.end() );
     return Complex<BNObj,BNMor,Coeff> ( new_objects,new_diffs );
};


template<typename Coeff>
void Complex<BNObj,BNMor,Coeff>::print (
     const std::string &name,
     const std::string &metadata,
     const bool is_4ended,
     const bool &is_Khr ) const
{
     if ( name.empty() ) {
          if ( this->is_loop_type() ) {
               std::cout << this->to_chains().to_string ( true, is_Khr );
          } else {
               std::cout << this->to_string();
          };
     } else if ( metadata.empty() ) {
          // Providing metadata should be strictly enforced
          std::cerr << "No metadata was specified.";
          abort();
     } else {
          std::ofstream file ( name );
          file << "% file '"
               << name
               << "', "
               << metadata
               << "\n";
          if ( this->is_loop_type() ) {
               file << this->to_chains().to_string ( is_4ended, is_Khr );
               std::cout << this->to_chains().to_string ( is_4ended, is_Khr );
          } else {
               file << this->to_string();
          };
          file.close();
     };
}

// template<typename Coeff>
// void Complex<BNObj,BNMor,Coeff>::isotopySide (
//      const int &start,
//      const int &end,
//      const BNMor<Coeff> &mor,
//      const bool &side )
// {
//      // this is identical to 'isotopy', except that we use a different
//      // multiplication and that Mor and Obj have been specialized to
//      // BNMor and BNObj and 'diffs' is replaced by 'this->diffs'
//      std::cout << "Performing an isotopy from "
//                << start << " to "  << end
//                << " via " << mor.to_string()
//                << " on side " << side << ".";
//      //  this->print();
//
//      if ( this->diffs.coeff ( start,end ).is_0() ==false ) {
//           std::cerr <<"Error: This isotopy might not be legit and may change the homotopy class of the complex.";
//      } else {
//           // subtract all start------>end------>x
//           Eigen::SparseVector<BNMor<Coeff>> post_comp=this->diffs.col ( end );
//           // auto class Eigen::Block<Eigen::SparseMatrix<CobMor, 0, int>, -1, 1, true>
//           for ( typename Eigen::SparseVector<BNMor<Coeff>>::InnerIterator it ( post_comp ); it; ++it ) {
//                it.valueRef() =it.valueRef().productSide ( mor,side );
//                // it.valueRef()=it.valueRef()*mor;
//                // ADAPTED FOR VERSION INCLUDING SIDE
//           };
//           this->diffs.col ( start )+=post_comp;
//           //
//           // subtract all x------>start------>end; this operation is more
//           // costly, since diffs is a column-major matrix
//           BNMor<Coeff> mor1 {-mor};
//           std::vector<std::pair<int,BNMor<Coeff>>> pre_comp;
//           for ( int k=0; k<this->diffs.outerSize(); ++k ) {
//                for ( typename Eigen::SparseMatrix<BNMor<Coeff>>::InnerIterator it ( this->diffs,k ); it; ++it ) {
//                     if ( it.row() ==start ) {
//                          pre_comp.push_back ( std::pair<int,BNMor<Coeff>> ( it.col(),mor1.productSide ( it.value(),side ) ) );
//                          // pre_comp.push_back(std::pair<int,Mor<Coeff>>(it.col(),mor*it.value()));
//                          // ADAPTED FOR VERSION INCLUDING SIDE
//                     };
//                };
//           };
//           //
//           for ( auto e : pre_comp ) {
//                this->diffs.coeffRef ( end,e.first )+=e.second;
//           };
//           //
//           this->diffs.makeCompressed();
//           this->diffs.prune ( NonZero<BNMor<Coeff>> );
//      };
// };

template<typename Coeff>
bool Complex<BNObj,BNMor,Coeff>::is_loop_type ( const bool &faceD ) const
{
     long int N{this->diffs.outerSize() };
     std::vector<bool> rows;
     rows.reserve ( N );
     for ( long int i=0; i<N; ++i ) {
          rows.push_back ( false );
     };
     for ( long int k=0; k<N; ++k ) {
          // go through column k
          bool found_one_entry{false};
          for ( typename Eigen::SparseMatrix<BNMor<Coeff>>::InnerIterator it ( this->diffs,k ); it; ++it ) {
               for ( const auto &l : it.value().get_labels() ) {
                    if ( l.is_type ( faceD ) ) {
                         if ( found_one_entry ) {
                              return false;
                         }
                         found_one_entry = true;
                         if ( rows[it.row()] ) {
                              return false;
                         };
                         rows[it.row()] = true;
                    };
               };
          };
     };
     //
     return true;
}


template<typename Coeff>
bool Complex<BNObj,BNMor,Coeff>::is_loop_type () const
{
     return this->is_loop_type ( 0 ) && this->is_loop_type ( 1 );
};


template<typename Coeff>
void Complex<BNObj,BNMor,Coeff>::clean_up_once ( const bool &faceD )
{
     std::vector<size_t> remaining;
     size_t N {this->objects.size() };
     remaining.reserve ( N );
     for ( size_t i=0; i<N; i++ ) {
          remaining.push_back ( i );
     };
     //
     while ( remaining.empty() == false ) {
          std::random_device random_device;
          std::mt19937 engine{random_device() };
          std::uniform_int_distribution<size_t> dist ( 0, remaining.size() - 1 );
          size_t start {remaining[dist ( engine )]};
          size_t end {start};
          bool arrow_found = false;
          bool arrow_changed = true;
          int x_type;
          int type = INT_MAX;// face D
          if ( faceD == false ) {
               type = INT_MIN; // face S
          };
          //
          while ( arrow_changed ) {
               arrow_changed = false;
               // find shortest arrow of the given face starting at start
               for ( const size_t &x : remaining ) {
                    x_type = this->diffs.coeff ( x,start ).get_type ( faceD );
                    if ( arrow_is_shorter ( x_type,type ) ) {
                         end = x;
                         type = x_type;
                         arrow_found = true;
                         arrow_changed = true;
                    };
               };
               // try to find shorter arrow of the given face ending at end
               for ( const size_t &x : remaining ) {
                    x_type = this->diffs.coeff ( end,x ).get_type ( faceD );
                    if ( arrow_is_shorter ( x_type,type ) ) {
                         start = x;
                         type = x_type;
                         arrow_found = true;
                         arrow_changed = true;
                    };
               };
          };
          //
          if ( arrow_found ) {
               this->isolate_arrow ( start,end,faceD );
               remaining.erase ( std::remove ( remaining.begin(), remaining.end(), end ), remaining.end() );
               remaining.erase ( std::remove ( remaining.begin(), remaining.end(), start ), remaining.end() );
          } else {
               remaining.erase ( std::remove ( remaining.begin(), remaining.end(), start ), remaining.end() );
          };
     };
}

template<typename Coeff>
void Complex<BNObj,BNMor,Coeff>::clean_up ( ssize_t max_iter )
{
     max_iter *= 10;
     if ( this->is_loop_type () ) {
          std::cout << "The complex is already loop-type.\n";
     } else {
          bool go_on {true};
          ssize_t iter {0};
          std::string feedback {""};
          auto start_t {std::chrono::high_resolution_clock::now() };
          auto current_t {start_t};
          while ( go_on ) {
               go_on = false;
               while ( iter < max_iter ) {
                    ++iter;
                    this->clean_up_once ( false );
                    this->clean_up_once ( true );
                    if ( ( iter % 10 ) == 0 ) {
                         if ( this->is_loop_type ( false ) ) {
                              // the complex is already cleaned up wrt 1
                              iter-=1;
                              break;
                         };
                         current_t = std::chrono::high_resolution_clock::now();
                         feedback = " Clean-up iteration "
                                    + std::to_string ( iter )
                                    + " of "
                                    + std::to_string ( max_iter )
                                    + " (max); time elapsed: "
                                    + time_to_string (
                                         std::chrono::duration_cast<std::chrono::milliseconds> ( current_t-start_t ) );
                    };
                    std::cout << ( "\33[2K\r" + spin ( iter ) + feedback ) << std::flush;
               };
               current_t = std::chrono::high_resolution_clock::now();
               if ( iter == max_iter ) {
                    std::cout << "\33[2K\rFAILED to clean-up the complex in "
                              << max_iter
                              << " iterations and "
                              << time_to_string (
                                   std::chrono::duration_cast<std::chrono::milliseconds> ( current_t-start_t ) )
                              <<".\n";
                    if ( yes_no_dialogue ( "Do you want to continue cleaning up?" ) ) {
                         max_iter *= 2;
                         go_on = true;
                    };
               } else {
                    std::cout << "\33[2K\rCleaned-up complex in "
                              << iter + 1
                              << " iterations and "
                              << time_to_string (
                                   std::chrono::duration_cast<std::chrono::milliseconds> ( current_t-start_t ) )
                              << ".\n";
               };
          };
     };
};

template<typename Coeff>
void Complex<BNObj,BNMor,Coeff>::isolate_arrow (
     const size_t &start,
     const size_t &end,
     const bool &faceD )
{
     BNMor<Coeff> mor=this->diffs.coeff ( end,start );
     // assuming that any arrow has at most two labels, one for each face type
     Coeff inverse_coeff {mor.get_coeff ( faceD ) };
     if ( coeff_is_0 ( inverse_coeff ) ) {
          std::cerr << "Did not find correct label. morphism = " << mor.to_string();
          exit ( 1 );
     } else {
          inverse_coeff = 1/inverse_coeff;
     };
     int type {mor.get_type ( faceD ) };
     //
     int current_type;
     BNMor<Coeff> alg;
     BNMor<Coeff> isotopy;
     Coeff coeff {0};
     // remove arrows (start------>x!=end)
     for ( size_t x = 0; x<this->objects.size(); ++x ) {
          if ( x!=end ) {
               alg = this->diffs.coeff ( x,start );
               current_type = alg.get_type ( faceD );
               if ( current_type !=0 ) {
                    if ( this->diffs.coeff ( end,x ).is_0() == false ) {
                         // do not attempt an isotopy end------>x if there is an arrow x------>end
                         std::cerr << "Attempted illegal isotopy whilst trying to isolate start:"
                                   << " x--->end: "
                                   << this->diffs.coeff ( end,x ).to_string()
                                   << ", start--->end: "
                                   << this->diffs.coeff ( end,start ).to_string()
                                   << ", start--->x: "
                                   << this->diffs.coeff ( x,start ).to_string();
                         exit ( 1 );
                    };
                    coeff = alg.get_coeff ( faceD ) * inverse_coeff;
                    isotopy = BNMor<Coeff> ( this->objects[end],this->objects[x], {{current_type-type,coeff}} );
                    this->isotopy ( end,x,isotopy );
               };
               if ( this->diffs.coeff ( x,start ).get_type ( faceD ) !=0 ) {
                    std::cerr << "CLEANING UP START DID NOT WORK! alg = "
                              << alg.to_string()
                              << ",  coeff:"
                              << coeff_to_string ( coeff )
                              << " new algebra element: "
                              << this->diffs.coeff ( x,start ).to_string();
                    exit ( 1 );
               };
          };
     };
     for ( size_t x = 0; x<this->objects.size(); ++x ) {
          if ( x!=start ) {
               alg = this->diffs.coeff ( end,x );
               current_type = alg.get_type ( faceD );
               if ( current_type !=0 ) {
                    if ( this->diffs.coeff ( x,start ).is_0() == false ) {
                         // do not attempt an isotopy x------>start if there is an arrow start------>x
                         std::cerr << "Attempted illegal isotopy whilst trying to isolate end:"
                                   << " x--->end: "
                                   << this->diffs.coeff ( end,x ).to_string()
                                   << ", start--->end: "
                                   << this->diffs.coeff ( end,start ).to_string()
                                   << ", start--->x: "
                                   << this->diffs.coeff ( x,start ).to_string();
                         exit ( 1 );
                    };
                    coeff = ( -1 ) * alg.get_coeff ( faceD ) * inverse_coeff;
                    isotopy = BNMor<Coeff> ( this->objects[x],this->objects[start], {{current_type-type,coeff}} );
                    this->isotopy ( x,start,isotopy );
               };
               if ( this->diffs.coeff ( end,x ).get_type ( faceD ) !=0 ) {
                    std::cerr << "CLEANING UP END DID NOT WORK! alg = "
                              << alg.to_string()
                              << ",  coeff:"
                              << coeff_to_string ( coeff )
                              << " new algebra element: "
                              << this->diffs.coeff ( end,x ).to_string();
                    exit ( 1 );
               };
          };
     };
};
/*
template<typename Coeff>
Precurve<Coeff> Complex<BNObj,BNMor,Coeff>::to_precurve() const
{
     auto cx=*this;// make copy of complex
     std::vector<size_t> obj2arc;
     std::vector<size_t> obj2index;
     obj2arc.reserve ( cx.objects.size() );
     obj2index.reserve ( cx.objects.size() );
     std::vector<Halfgen> leftB;
     std::vector<Halfgen> rightB;
     std::vector<Halfgen> leftC;
     std::vector<Halfgen> rightC;
     size_t counterB{0};
     size_t counterC{0};
     for ( auto obj : cx.objects ) {
          if ( obj.get_idem() ) {
               leftB.push_back ( obj.to_halfgen ( counterB,0 ) );
               rightB.push_back ( obj.to_halfgen ( counterB,1 ) );
               obj2arc.push_back ( 0 );
               obj2index.push_back ( counterB );
               ++counterB;
          } else {
               leftC.push_back ( obj.to_halfgen ( counterC,0 ) );
               rightC.push_back ( obj.to_halfgen ( counterC,1 ) );
               obj2arc.push_back ( 1 );
               obj2index.push_back ( counterC );
               ++counterC;
          };
     };
     std::vector<size_t> permutationsB;
     for ( size_t it=0; it<leftB.size(); ++it ) {
          permutationsB.push_back ( it );
     };
     std::vector<size_t> permutationsC;
     for ( size_t it=0; it<leftC.size(); ++it ) {
          permutationsC.push_back ( it );
     };
     auto arcnB=ArcN<Coeff> ( leftB,rightB, {}, {},permutationsB );
     auto arcnC=ArcN<Coeff> ( leftC,rightC, {}, {},permutationsC );
     auto precurve=Precurve<Coeff> ( {arcnB,arcnC} );

     // check if Coeff is the ring of integer; all other template
     // specializations for Coeff are assumed to be fields:
     if ( which_coeff ( Coeff ( 0 ) ) =="Z" ) {
          std::cerr << "USER ERROR: You should be working over field coefficients when converting a complex over BNAlgebra to a precurve.";
          return precurve;
     };
     // debugging
     precurve.to_svg ( "debugging/to_precurve_generators" );
     //
     int safety_counter{0};// in case something goes wrong
     bool diff_nonzero{1};
     while ( diff_nonzero && safety_counter<100 ) {
          // find a generator pair among the remaining generators which is
          // connected via a shortest join via a face f;
          int join_start;
          int join_end;
          int join_type{0};
          Coeff join_coeff{0};
          // pick the first non-zero differential, and see if we can find a
          // shorter one of the same type.
          for ( int k=0; k<cx.diffs.outerSize(); ++k ) {
               for ( typename Eigen::SparseMatrix<BNMor<Coeff>>::InnerIterator it ( cx.diffs,k ); it; ++it ) {
                    for ( auto label : it.value().get_labels() ) {
                         if ( ( label.get_type() *join_type>0 &&
                                   label.get_type() *label.get_type() <join_type*join_type )
                                   || ( join_type==0 ) ) {
                              // only change the current candidate for join if it has
                              // the same type (D) and is strictly shorter. This might
                              // change the face of the current candidate, but that is
                              // fine; also change the candidate if there is non yet;
                              // assuming full cancelled input here;
                              join_type=label.get_type();
                              join_start=it.col();
                              join_end=it.row();
                              join_coeff=label.get_coeff();
                         };
                    };
               };
          };
          if ( join_type==0 ) {
               // we are done; exit while loop.
               diff_nonzero=0;
          } else {
               // add the join to the joins; side=0(left)=D
               bool side = ( join_type>0 ) ? 0 : 1;
               int joinlength = ( join_type>0 ) ? join_type : -join_type;
               HalfgenID halfgenID_start =
                    HalfgenID ( obj2arc[join_start],obj2index[join_start],side );
               HalfgenID halfgenID_end =
                    HalfgenID ( obj2arc[join_end],obj2index[join_end],side );
               std::cout << "Found new join for iteration "
                         << safety_counter
                         << ": "
                         << join_start
                         << "--(side=" << side << ")-->"
                         << join_end
                         << " ("
                         << halfgenID_start.to_string()
                         << "--(joinlength=" << joinlength << ")-->"
                         << halfgenID_end.to_string()
                         << ")";
               precurve.add_join ( halfgenID_start,halfgenID_end,joinlength );
               // homotope away all strictly longer arrows starting at the same
               // object as join and push all arrows: those that would need to
               // be homotoped via the identity are pushed into the arc nbhd.
               bool proceed{true};
               int current;
               Label<Coeff> label{0,0};
               while ( proceed ) {
                    proceed=false;
                    for ( typename Eigen::SparseMatrix<BNMor<Coeff>>::InnerIterator it ( cx.diffs,join_start ); it; ++it ) {
                         // We are iterating over a matrix that might change in the
                         // iteration. So we need to restart the iteration after
                         // each update of the matrix
                         //
                         // loop over column
                         current=it.row();
                         if ( current!=join_end ) {
                              // ignore the actual join
                              for ( auto l : it.value().get_labels() ) {
                                   if ( l.get_type() *join_type>0 ) {
                                        label=l;
                                        proceed=true;
                                        break;
                                   };
                              };
                         };
                         if ( proceed ) {
                              //label has same type as join, so proceed...
                              Coeff h_coeff = ( label.get_coeff() * ( 1/join_coeff ) );
                              // no sign above, because we eliminate arrows by
                              //multiplying by the homotopy as the second morphism;
                              //the cancelling sign is added in the isotopy
                              auto mor=BNMor<Coeff> ( cx.objects[join_end],
                              cx.objects[current], {
                                   {
                                        label.get_type()-join_type,
                                        h_coeff
                                   }
                              } );
                              // Do a special homotopy that only changes one face,
                              // namely the one specified by 'side'
                              cx.isotopySide ( join_end,current,mor,side );
                              //face corresponding to join_type
                              if ( label.get_type() ==join_type ) {
                                   // push into arc nbh as crossover arrow
                                   if ( side==0 ) { //0 = add on the left
                                        h_coeff*=-1;
                                   };
                                   precurve.add_mat ( obj2arc[join_end],
                                                      side,
                                                      obj2index[join_end],
                                                      obj2index[current],
                                                      h_coeff );
                              };
                              break;
                         };
                    };
               };
               // homotope away all strictly longer arrows ending at the same
               // object as join and push all arrows: those that would need to
               // be homotoped via the identity are pushed into the arc nbhd.
               proceed=true;
               while ( proceed ) {
                    proceed=false;
                    for ( int k=0; k<cx.diffs.outerSize(); ++k ) {
                         // We are iterating over a matrix that might change in the
                         // iteration. So we need to restart the iteration after
                         // each update of the matrix
                         //
                         for ( typename Eigen::SparseMatrix<BNMor<Coeff>>::InnerIterator it ( cx.diffs,k ); it; ++it ) {
                              if ( it.row() ==join_start ) {
                                   // loop over row
                                   current=it.col();
                                   if ( current!=join_start ) {
                                        // ignore the actual join
                                        for ( auto l : it.value().get_labels() ) {
                                             if ( l.get_type() *join_type>0 ) {
                                                  label=l;
                                                  proceed=true;
                                                  break;
                                             };
                                        };
                                   };
                              };
                              if ( proceed ) {
                                   break;
                              };
                         };
                         if ( proceed ) {
                              //label has same type as join, so proceed...
                              Coeff h_coeff= - ( label.get_coeff() * ( 1/join_coeff ) );
                              // we add the sign here, because we eliminate arrows
                              //by multiplying by the homotopy as the first
                              //morphism; no cancelling sign is added in the
                              //isotopy
                              auto mor=BNMor<Coeff> ( cx.objects[current],
                              cx.objects[join_start], {
                                   {
                                        label.get_type()-join_type,
                                        h_coeff
                                   }
                              } );
                              // Do a special homotopy that only changes one face,
                              // namely the one specified by 'side'
                              cx.isotopySide ( current,join_start,mor,side );
                              //face corresponding to join_type
                              if ( label.get_type() ==join_type ) {
                                   // push into arc nbh as crossover arrow
                                   if ( side ) { //1 = add on the right
                                        h_coeff*=-1;
                                   };
                                   precurve.add_mat ( obj2arc[join_start],
                                                      side,
                                                      obj2index[current],
                                                      obj2index[join_start],
                                                      h_coeff );
                              };
                              break;
                         };
                    };
               };
               // push coefficient into the start arc neighbourhood:
               auto passing_loop_coeff{join_coeff};
               if ( side ) { // 1 = add on the right
                    passing_loop_coeff=1/join_coeff;
               };
               precurve.add_mat ( obj2arc[join_start],
                                  side,
                                  obj2index[join_start],
                                  obj2index[join_start],
                                  passing_loop_coeff );
               // remove component of the differential from complex
               cx.diffs.coeffRef ( join_end,join_start ) +=
                    BNMor<Coeff> ( cx.objects[join_start],
                                   cx.objects[join_end],
               {Label<Coeff> ( join_type,-join_coeff ) } );
               // clean up differentials
               cx.diffs.makeCompressed();
               cx.diffs.prune ( NonZero<BNMor<Coeff>> );
               // std::cout << "\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
               //           << "iteration " << safety_counter << ":\n";
               // cx.print();
          };
          // debugging
          precurve.to_svg ( "debugging/to_precurve_diffs"+std::to_string ( safety_counter ) );
          ++safety_counter;
     };
     if ( safety_counter==100 ) {
          std::cout << "Aborted conversion to precurve as a precaution, because it took too long. It that is to be expected, change the maximum number of loops. Otherwise, the might be a bug in the code.";
     };
     return precurve;
};*/



//                                             //
// non-member functions that relate to Complex //
//                                             //

template<typename Obj,
         template <typename> typename Mor,
         typename Coeff>
Complex<Obj,Mor,Coeff> to_coeff ( const Complex<Obj,Mor,Z_mod<integer_simulant>> &cx )
{
     if ( cx.get_cancelled_indices().size() !=0 ) {
          std::cerr << " Please resize the complex first. Good practise, etc,...";
     };
     // Convert differential by iterating through all non-zero entries and selecting only those that are non-zero in the new coefficients.
     Eigen::SparseMatrix<Mor<Z_mod<integer_simulant>>> old_diffs{cx.get_diffs() };
     std::vector<Eigen::Triplet<Mor<Coeff>>> triplets;
     for ( int k=0; k<old_diffs.outerSize(); ++k ) {
          for ( typename Eigen::SparseMatrix<Mor<Z_mod<integer_simulant>>>::InnerIterator it ( old_diffs,k ); it; ++it ) {
               Mor<Coeff> new_coeff=it.value().template to_coeffs<Coeff>();
               if ( new_coeff.is_0() ==false ) {
                    triplets.push_back ( Eigen::Triplet<Mor<Coeff>> ( it.row(),it.col(),new_coeff ) );
               };
          };
     };
     Eigen::SparseMatrix<Mor<Coeff>> new_diffs ( old_diffs.rows(),old_diffs.cols() );
     new_diffs.setFromTriplets ( triplets.begin(),triplets.end() );
     // output new complex
     return Complex<Obj,Mor,Coeff> ( cx.get_objects(),new_diffs );
}

template<typename Mor>
bool NonZero ( const Eigen::Index &row,
               const Eigen::Index &col,
               const Mor& value )
{
     return value.is_0() == false;
}



// template specialization

#define VALUE Z_mod<2>
#include "../specialization/Complex.h"
#define VALUE Z_mod<3>
#include "../specialization/Complex.h"
#define VALUE Z_mod<5>
#include "../specialization/Complex.h"
#define VALUE Z_mod<7>
#include "../specialization/Complex.h"
#define VALUE Z_mod<custom_coeff>
#include "../specialization/Complex.h"
#define VALUE Z_mod<integer_simulant>
#include "../specialization/Complex.h"
#define nonconvertibles
#define VALUE int
#include "../specialization/Complex.h"
#define VALUE Q
#include "../specialization/Complex.h"
#undef nonconvertibles

#endif
