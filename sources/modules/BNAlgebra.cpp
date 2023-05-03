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



#ifndef BNALGEBRA_TPP
#define BNALGEBRA_TPP
#ifndef BNALGEBRA_H
#include "../headers/BNAlgebra.h"
#include "../headers/constants.h"
#include "../headers/aux_svg.h"
#endif


/// \file BNAlgebra.cpp
/// \brief implements the Bar-Natan algebra \f$\mathcal{B}\f$ from \cite KWZ

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  BNObj
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

max_gr_str::max_gr_str ( size_t h,size_t q,size_t delta ) :
     h ( h ),
     q ( q ),
     delta ( delta )
{
}

BNObj::BNObj ( Idem idem,Grading h, Grading q ) :
     idem ( idem ),
     h ( h ),
     q ( q )
{
}

//                     //
// getters and setters //
//                     //

Grading BNObj::get_h() const
{
     return h;
};

Grading BNObj::get_q() const
{
     return q;
};
Q BNObj::get_delta() const
{
     return -h+Q ( q,2 );
};
Idem BNObj::get_idem() const
{
     return idem;
};
void BNObj::set_h ( const Grading &new_h )
{
     h = new_h;
};
void BNObj::set_q ( const Grading &new_q )
{
     q = new_q;
};
void BNObj::shift_hq (
     const Grading &h_shift,
     const Grading &q_shift )
{
     h+=h_shift;
     q+=q_shift;
};

//                          //
// output and sanity checks //
//                          //
void BNObj::print() const
{
     std::cout << this->to_string() <<"\n";
}

std::string BNObj::to_svg ( const bool &with_grading ) const 
{
     std::string output;
     if ( idem ) {
          output+=svg_text({0,0},"⬮");//●⦁▮█
     } else {
          output+=svg_text({0,0},"⬯");//○∘▯Oo
     };
     if ( with_grading ) {
          output += svg_text({0,20},this->gr_string());
     };
     return output;
}

std::string BNObj::gr_string ( const max_gr_str &max_gr ) const
{
     std::string output;
     std::string str_gr;
     //
     output+= "h^";
     str_gr = std::to_string ( h );
     for ( size_t i = str_gr.size(); i<max_gr.h; ++i ) {
          output+= " ";
     };
     output+= str_gr;
     //
     output+= " q^";
     str_gr = std::to_string ( q );
     for ( size_t i = str_gr.size(); i<max_gr.q; ++i ) {
          output+= " ";
     };
     output+= str_gr;
     //
     output+= " δ^";
     str_gr = coeff_to_string ( this->get_delta() );
     for ( size_t i = str_gr.size(); i<max_gr.delta; ++i ) {
          output+= " ";
     };
     output+= str_gr;
     //
     return output;
}

std::string BNObj::to_string ( const bool &with_grading ) const
{
     std::string output;
     if ( idem ) {
          output+="⬮";//●⦁▮█
     } else {
          output+="⬯";//○∘▯Oo
     };
     if ( with_grading ) {
          output += " ";
          output += this->gr_string();
     };
     return output;
}

std::string BNObj::to_TeX() const
{
     std::string output;
     if ( idem ) {
          output+="\\DotB";
     } else {
          output+="\\DotC";
     };
     output+= ( "h^{"+std::to_string ( h )+"}" );
     output+= ( "q^{"+std::to_string ( q )+"}" );
     output+= ( "\\delta^{"+coeff_to_string ( this->get_delta() )+"}" );
     return output;
}

//                      //
// overloaded operators //
//                      //
bool BNObj::operator== ( const BNObj &obj2 ) const
{
     return ( idem==obj2.idem );
};
// ignore gradings
bool BNObj::operator!= ( const BNObj &obj2 ) const
{
     return ( idem!=obj2.idem );
};
// ignore gradings

//              //
// main methods //
//              //
// Halfgen BNObj::to_halfgen ( const size_t &index, const Side &side )
// {
//      return ( idem )
//             ? Halfgen ( {0,index,side},h,q )
//             : Halfgen ( {1,index,side},h,q );
// };



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//          Label
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
Label<Coeff>::Label ( int type, Coeff coeff ) :
     type ( type ),
     coeff ( coeff )
{
};

//                     //
// getters and setters //
//                     //
template<typename Coeff>
int Label<Coeff>::get_type() const
{
     return type;
};
template<typename Coeff>
bool Label<Coeff>::is_type ( const bool &faceD ) const
{
     // undefined behaviour for identity morphism
     if ( faceD ) {
          // face D
          return type>0;
     };
     // face S
     return type<0;
};
template<typename Coeff>
Coeff Label<Coeff>::get_coeff() const
{
     return coeff;
};
template<typename Coeff>
void Label<Coeff>::set_coeff ( const Coeff & set_coeff_to_1 )
{
     coeff = set_coeff_to_1;
};

//        //
// output //
//        //
template<typename Coeff>
std::string Label<Coeff>::to_string (
     const bool &with_coeff,
     const bool &is_4ended ) const
{
     std::string output;
     if ( with_coeff ) {
          if ( is_id ( coeff ) ) {
               output+="";
          } else if ( is_id ( -coeff ) ) {
               output+="-";
          } else {
               output+=coeff_to_string ( coeff );
               output+=".";
          };
     };
     if ( type>0 ) {
          if ( is_4ended ) {
               if ( type == 1 ) {
                    output+= "D";
               } else {
                    output+= ( "D^"+std::to_string ( type ) );
               };
          } else {
               if ( type == 1 ) {
                    output+= "H";
               } else {
                    output+= ( "H^"+std::to_string ( type ) );
               };
          };
     } else if ( type==0 ) {
          output+= ( "id" );
     } else {
          if ( is_4ended == false ) {
               std::cerr << "Encountered label of a morphism of k[H] of negative type." << std::endl;
               abort();
          };
          if ( type == -1 ) {
               output+= "S";
          } else {
               output+= ( "S^"+std::to_string ( -type ) );
          };
     };
     return output;
};



//                      //
// overloaded operators //
//                      //
template<typename Coeff>
bool Label<Coeff>::operator== ( const Label<Coeff> &label2 ) const
{
     return ( type == label2.type );
}
//coefficient is ignored
template<typename Coeff>
bool Label<Coeff>::operator!= ( const Label<Coeff> &label2 ) const
{
     return ! ( *this==label2 );
};

template<typename Coeff>
bool Label<Coeff>::operator< ( const Label<Coeff> &label2 ) const
{
     return ( type < label2.type );
};

//              //
// main methods //
//              //
template<typename Coeff>
bool label_is_zero ( const Label<Coeff> &label )
{
     // this function is used in BNMor::simplify().
     return coeff_is_0 ( label.coeff );
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//          BNMor
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
BNMor<Coeff>::BNMor ( int i ) :
     labels ( {} )
{
}

template<typename Coeff>
BNMor<Coeff>::BNMor() :
     labels ( {} )
{
}

template<typename Coeff>
BNMor<Coeff>::BNMor (
     Idem front,
     Idem back,
     std::list<Label<Coeff>> labels )
     :
     front ( front ),
     back ( back ),
     labels ( labels )
{
}

//                     //
// getters and setters //
//                     //
template<typename Coeff>
Idem BNMor<Coeff>::get_front ( ) const
{
     return this->front;
};

template<typename Coeff>
Idem BNMor<Coeff>::get_back ( ) const
{
     return this->back;
};

template<typename Coeff>
int BNMor<Coeff>::get_type ( const bool &faceD ) const
{
     for ( const Label<Coeff> &label : labels ) {
          if ( label.is_type ( faceD ) ) {
               return label.type;
          };
     };
     return 0;
};

template<typename Coeff>
Coeff BNMor<Coeff>::get_coeff ( const bool &faceD ) const
{
     for ( const Label<Coeff> &label : labels ) {
          if ( label.is_type ( faceD ) ) {
               return label.coeff;
          };
     };
     return 0;
};

template<typename Coeff>
int BNMor<Coeff>::get_first_type() const
{
     return labels.front().type;
};

template<typename Coeff>
Coeff BNMor<Coeff>::get_first_coeff() const
{
     return labels.front().coeff;
};

template<typename Coeff>
std::list<Label<Coeff>> BNMor<Coeff>::get_labels() const
{
     return labels;
};

template<typename Coeff>
void BNMor<Coeff>::set_coeff ( const Coeff &coeff )
{
     for ( Label<Coeff> &l : labels ) {
          l.coeff = coeff;
     };
}

template<typename Coeff>
bool BNMor<Coeff>::is_0() const
{
     return labels.empty();
};

template<typename Coeff>
bool BNMor<Coeff>::is_inv() const
{
     // we assume that the morphism is simplified.
     if ( labels.size() !=1 ) {
          return false;
     };
     if ( labels.front().type!=0 ) {
          return false;
     };
     if ( coeff_is_inv ( labels.front().coeff ) !=true ) {
          return false;
     };
     return true;
};




//                          //
// output and sanity checks //
//                          //
template<typename Coeff>
std::string BNMor<Coeff>::to_string (
     bool with_coeff,
     bool is_4ended ) const
{
     std::string s;
     if ( labels.empty() ) {
          return "0";
     } else {
          for ( auto label: labels ) {
               s+=label.to_string ( with_coeff, is_4ended );
               s+="+";
          };
          s.resize ( s.size()-1 );
     };
     return s;
}

template<typename Coeff>
void BNMor<Coeff>::print ( const bool &is_4ended ) const
{
     if ( labels.empty() ) {
          std::cout << "The zero-morphism.\n";
     } else {
          std::cout << BNObj( front ).to_string ( false )
                    << "---("
                    << this->to_string ( 1, is_4ended )
                    << ")--->"
                    << BNObj( back ).to_string ( false );
     };
}

template<typename Coeff>
bool BNMor<Coeff>::check (
     const BNObj &obj_from,
     const BNObj &obj_to ) const
{
     if ( labels.empty() ) {
          return true;
     };
     // We may now assume that the morphism is non-zero.
     if ( front!=obj_from.get_idem() ) {
          return false;
     };
     if ( back!=obj_to.get_idem() ) {
          return false;
     };
     return true;
};

//              //
// main methods //
//              //
template<typename Coeff>
std::list<BNMor<Coeff>> BNMor<Coeff>::split()
{
     std::list<BNMor<Coeff>> output {};
     for ( const Label<Coeff> &label : labels ) {
          output.push_back ( {front, back, {label}} );
     };
     return output;
};

template<typename Coeff>
BNMor<Coeff> BNMor<Coeff>::simplify()
{
     if ( labels.empty() ) {
          return BNMor<Coeff> ( 0 );
     };
     // We may now assume that the list of labels is non-empty.
     //  1) order labels using the overloaded operation< on labels
     labels.sort();
     //  2) for each label, except the first, check if it is equal to the
     //  predecessor except for the coefficient.
     std::list<Label<Coeff>> new_labels= {labels.front() };
     labels.erase ( labels.begin() );
     for ( const Label<Coeff> &label : labels ) {
          if ( label==new_labels.back() ) {
               new_labels.back().coeff+=label.coeff;
          } else {
               new_labels.push_back ( label );
          };
     };
     //  3) eliminate labels that are zero.
     new_labels.remove_if ( label_is_zero<Coeff> );
     labels=new_labels;
     return *this;
}

/// \brief implementation of to_coeffs()
template<>
template<typename New_Coeff>
BNMor<New_Coeff> BNMor<Z_mod<integer_simulant>>::to_coeffs() const
{
//      std::cout << "Converting from Z to "
//            << which_coeff ( New_Coeff ( 0 ) );
     std::list<Label<New_Coeff>> new_labels;
     for ( auto label : labels ) {
          New_Coeff new_coeff=label.coeff.n;
          if ( coeff_is_0 ( new_coeff ) !=true ) {
               new_labels.push_back ( Label<New_Coeff> ( label.type,new_coeff ) );
          };
     };
     return BNMor<New_Coeff> ( front,back,new_labels );
};

//                      //
// overloaded operators //
//                      //
template<typename Coeff>
bool BNMor<Coeff>::operator== ( const BNMor<Coeff> &mor2 ) const
{
     // test if both morphisms are zero
     if ( ( labels.empty() ) && ( mor2.labels.empty() ) ) {
          return true;
     };
     // test if front+back are identical
     if ( front!=mor2.front ) {
          return false;
     };
     if ( back!=mor2.back ) {
          return false;
     };
     // testing if labels have same length
     if ( labels.size() !=mor2.labels.size() ) {
          return false;
     };
     std::list<Label<Coeff>> temp=mor2.labels;
     for ( auto label : labels ) {
          // testing if the two lists of labels are equal as sets. We assume
          // that 'mor1' and 'mor2' are fully simplified.
          auto iter=std::find ( temp.begin(),temp.end(),label );
          if ( iter==temp.end() ) {
               return false;
          } else {
               temp.erase ( iter );
          };
     };
     return true;
}

template<typename Coeff>
void BNMor<Coeff>::operator*= ( const Coeff &scalar )
{
     for ( auto &label : labels ) {
          label.coeff*=scalar;
     };
}

template<typename Coeff>
void BNMor<Coeff>::operator+= ( const BNMor<Coeff> &mor2 )
{
     *this = ( *this+mor2 );
}

template<typename Coeff>
BNMor<Coeff> BNMor<Coeff>::operator- () const
{
     // multiply the morphism by -1;
     std::list<Label<Coeff>> new_labels=labels;
     for ( Label<Coeff> &label : new_labels ) {
          label.coeff=-1*label.coeff;
     };
     return BNMor<Coeff> ( front,back,new_labels );
}

template<typename Coeff>
BNMor<Coeff> BNMor<Coeff>::operator* ( const Coeff &scalar ) const
{
     auto new_mor=*this;
     for ( auto &label : new_mor.labels ) {
          label.coeff*=scalar;
     };
     return new_mor;
};

template<typename Coeff>
BNMor<Coeff> BNMor<Coeff>::operator* ( const BNMor<Coeff> &mor1 ) const
{
     // multiplication of BNMor<Coeff>: mor2*mor1
     if ( mor1.labels.empty() || labels.empty() ) {
          return BNMor<Coeff> ( 0 );
     };
     //multiplication with the zero-morphism.
     if ( front!=mor1.back ) {
          // Not necessary, if we trust the program.
          std::cerr << "WARNING: You are trying to compose two incomposable morphisms.\n";
          abort();
     };

     //morphisms compose as BNMor(b,c)*BNMor(a,b)=BNMor(a,c)
     std::list<Label<Coeff>> new_labels= {};
     for ( const Label<Coeff> &label1 : mor1.labels ) {
          for ( const Label<Coeff> &label2 : labels ) {
               // now multiply each pair of labels
               if ( label1.type*label2.type>=0 ) {
                    new_labels.push_back ( Label<Coeff> ( label1.type+label2.type,label1.coeff*label2.coeff ) );
               };
          };
     };
     return BNMor<Coeff> ( mor1.front,back,new_labels ).simplify();
}

// template<typename Coeff>
// BNMor<Coeff> BNMor<Coeff>::productSide (
//      const BNMor<Coeff> &mor1,
//      const Side &side )
// {
//      // only those labels that lie on the side 'side' are multiplied,
//      // even if 'mor1' is the identity; we are assuming that label does
//      // not contain any identity component. This is identical to
//      // 'operation*', except for the single highlighted line below:
//      //multiplication of BNMor<Coeff>: mor2*mor1";
// 
//      if ( mor1.labels.empty() || labels.empty() ) {
//           return BNMor<Coeff> ( 0 );
//      };
//      //multiplication with the zero-morphism.
//      if ( front!=mor1.back ) {
//           // Not necessary, if we trust the program.
//           std::cerr << "WARNING: You are trying to compose two incomposable morphisms.\n";
//           abort();
//      };
// 
//      //morphisms compose as BNMor(b,c)*BNMor(a,b)=BNMor(a,c)
//      std::list<Label<Coeff>> new_labels= {};
//      for ( const Label<Coeff> &label1 : mor1.labels ) {
//           for ( const Label<Coeff> &label2 : labels ) {
//                // now multiply each pair of labels
//                if ( ( ( label1.type>=0 ) && ( label2.type>=0 ) && ( side==0 ) )
//                          || ( ( label1.type<=0 ) && ( label2.type<=0 ) && ( side==1 ) ) ) {
//                     // PREVIOUS LINE ADAPTED FOR VERSION INCLUDING SIDE
//                     new_labels.push_back ( Label<Coeff> ( label1.type+label2.type,label1.coeff*label2.coeff ) );
//                };
//           };
//      };
//      return BNMor<Coeff> ( mor1.front,back,new_labels ).simplify();
// };

template<typename Coeff>
BNMor<Coeff> BNMor<Coeff>::operator+ (
     const BNMor<Coeff> &mor2 ) const
{
     // addition mor1+mor2
     if ( labels.empty() ) {
          // mor1=0, so returning mor2
          return mor2;
     };
     if ( mor2.labels.empty() ) {
          // mor2=0, so returning mor1.
          //BNMor<Coeff> output=*this; //making copy
          return *this;
     };
     std::list<Label<Coeff>> new_labels=labels;
     new_labels.insert ( new_labels.end(), mor2.labels.begin(), mor2.labels.end() );
     // concatenate labels1 and labels2
     return BNMor<Coeff> ( front, back, new_labels ).simplify();
}

//                                            //
// non-member functions that relate to CobMor //
//                                            //

template<typename Coeff>
bool BNMorNonZero ( const int &row,
                    const int &col,
                    const BNMor<Coeff> &value )
{
     return value.is_0() == false;
};

bool arrow_is_shorter ( const int &a, const int &b )
{
     if ( a == 0 ) {
          // no arrow at all
          return false;
     } else if ( a > 0 ) {
          return b>a;
     };
     return b<a;
};



// template specialization
#define VALUE Z_mod<2>
#include "../specialization/BNAlgebra.h"
#define VALUE Z_mod<3>
#include "../specialization/BNAlgebra.h"
#define VALUE Z_mod<5>
#include "../specialization/BNAlgebra.h"
#define VALUE Z_mod<7>
#include "../specialization/BNAlgebra.h"
#define VALUE Z_mod<custom_coeff>
#include "../specialization/BNAlgebra.h"
#define VALUE Z_mod<integer_simulant>
#include "../specialization/BNAlgebra.h"
#define nonconvertibles
#define VALUE int
#include "../specialization/BNAlgebra.h"
#define VALUE Q
#include "../specialization/BNAlgebra.h"
#undef nonconvertibles

#endif
