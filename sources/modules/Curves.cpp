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



#ifndef CURVES_TPP
#define CURVES_TPP
#ifndef CURVES_H
#include "../headers/Curves.h"
#include "../headers/constants.h"
#endif

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  DoubleArrow
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

std::string weight_to_string ( const Weight &w )
{
     if ( w.first==0 ) {
          return "∞";
     };
     return std::to_string ( w.first ) + "|" + std::to_string ( w.second );
};

template<typename Coeff>
DoubleArrow<Coeff>::DoubleArrow (
     Level from,
     Level to,
     Coeff coeff )
     :
     from ( from ),
     to ( to ),
     coeff ( coeff )
{
}

template<typename Coeff>
std::string DoubleArrow<Coeff>::to_string() const
{
     std::string output;
     output+=std::to_string ( from );
     output+="---(";
     output+=coeff_to_string ( coeff );
     output+=")--->";
     output+=std::to_string ( to );
     output+="\n";
     return output;
};

template<typename Coeff>
bool DoubleArrow<Coeff>::is_crossover() const
{
     return ( coeff_is_0 ( coeff ) ==0 && from!=to );
};

template<typename Coeff>
bool DoubleArrow<Coeff>::is_constant() const
{
     return ( coeff_is_0 ( coeff ) ==0 && from==to );
};

template<typename Coeff>
Level DoubleArrow<Coeff>::get_from() const
{
     return from;
};

template<typename Coeff>
Level DoubleArrow<Coeff>::get_to() const
{
     return to;
};

template<typename Coeff>
void DoubleArrow<Coeff>::operator* ( Coeff &x )
{
     coeff*=x;
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Halfgen
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

HalfgenID::HalfgenID (
     size_t arc,
     size_t index,
     Side side )
     :
     arc ( arc ),
     index ( index ),
     side ( side )
{
}

std::string HalfgenID::to_string() const
{
     std::string output;
     output+= ( side ) ? "R" : "L";
     output+=std::to_string ( arc );
     output+="[";
     output+=std::to_string ( index );
     output+="]";
     return output;
};

Halfgen::Halfgen (
     HalfgenID halfgenID,
     Grading h,
     Grading q,
     HalfgenID next,
     int joinlength )
     :
     halfgenID ( halfgenID ),
     h ( h ),
     q ( q ),
     next ( next ),
     joinlength ( joinlength )
{
}

std::string Halfgen::to_string() const
{
     std::string output;
     output+=halfgenID.to_string();
     output+="{h=";
     output+=std::to_string ( h );
     output+=",q=";
     output+=std::to_string ( q );
     output+="}";
     if ( joinlength==0 )
          output+="---o";
     else {
          if ( joinlength>0 ) {
               output+= "---(";
               output+= std::to_string ( joinlength );
               output+= ")-->";
          } else {
               output+= "<--(";
               output+= std::to_string ( -joinlength );
               output+= ")---";
          };
          output+=next.to_string();
     };
     output+="\n";
     return output;
};

bool HalfgenID::operator== ( const HalfgenID &other ) const
{
     if ( arc!=other.arc ) {
          return false;
     };
     if ( index!=other.index ) {
          return false;
     };
     if ( side!=other.side ) {
          return false;
     };
     return true;
};

bool HalfgenID::operator!= ( const HalfgenID &other ) const
{
     return ! ( this->operator== ( other ) );
};


bool Halfgen::disjoinedQ() const
{
     return joinlength==0;
};
bool Halfgen::startQ() const
{
     return joinlength>0;
};
void Halfgen::set_join ( HalfgenID new_next, int new_joinlength )
{
     next=new_next;
     joinlength=new_joinlength;
};


int diverge ( const Halfgen &from,const Halfgen &to )
{
     return from.joinlength-to.joinlength;
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  ArcN
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
ArcN<Coeff>::ArcN (
     Halfgens left,
     Halfgens right,
     DoubleArrows<Coeff> mat_left,
     DoubleArrows<Coeff> mat_right,
     Permutation X )
     :
     left ( left ),
     right ( right ),
     mat_left ( mat_left ),
     mat_right ( mat_right ),
     X ( X )
{
}

template<typename Coeff>
std::string ArcN<Coeff>::to_string() const
{
     std::string output;
     output+="right:\n";
     for ( size_t i=0; i<right.size(); ++i ) {
          output+=right[i].to_string();
     };
     for ( const auto &el : mat_right ) {
          output+=el.to_string();
     };
     output+="permutation: ";
     output+=stringL ( X );
     output+="\n";
     output+="left:\n";
     for ( size_t i=0; i<left.size(); ++i ) {
          output+=left[i].to_string();
     };
     for ( const auto &el : mat_left ) {
          output+=el.to_string();
     };
     return output;
};

template<typename Coeff>
void ArcN<Coeff>::LPUdecompose ( const Side side )
{
     // rearranges the crossover arrows such that they point
     // downwards in mat_left (decreasing index) and upwards in
     // mat_right (increasing index) when side=0(left) and 1
     // (right), respectively

     //TODO
};

template<typename Coeff>
void ArcN<Coeff>::sort ( const Side side )
{
     // arrange crossover arrows on mat_left/mat_right such
     // that the ones closest to the face are the shortest

     //TODO
};

template<typename Coeff>
int ArcN<Coeff>::size()
{
     return left.size();
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Precurve
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<typename Coeff>
Precurve<Coeff>::Precurve ( ArcNs<Coeff> arcNs ) :
     arcNs ( arcNs )
{
}

template<typename Coeff>
std::string Precurve<Coeff>::to_string() const
{
     std::string output;
     for ( size_t i=0; i<arcNs.size(); ++i ) {
          output+="\nArc #";
          output+=std::to_string ( i );
          output+="\n";
          output+=arcNs[i].to_string();
     };
     return output;
};

template<typename Coeff>
bool Precurve<Coeff>::check() const
{
     for ( size_t arc_index=0; arc_index<arcNs.size(); ++arc_index ) {
          for ( size_t i=0; i<arcNs[arc_index].left.size(); ++i ) {
               auto gen{arcNs[arc_index].left[i]};
               if ( gen.halfgenID.arc!=arc_index ) return false;
               if ( gen.halfgenID.index!=i ) return false;
               if ( gen.halfgenID.side!=0 ) return false;
               if ( gen.disjoinedQ() ==false ) {
                    if ( this->halfgen_copy ( gen.next ).next != gen.halfgenID )
                         return false;
               };
          };
     };
     return true;
};

template<typename Coeff>
size_t ArcN<Coeff>::other_side (
     const Side start_side,
     const size_t &i ) const
{
     // map the index i through the permutation
     size_t output{i};
     if ( start_side ) {
          // right -> left
          output=this->X[i];
     } else {
          // left -> right
          output=std::distance (
                      X.begin(),std::find ( X.begin(), X.end(), i ) );
     };
     return output;
};

template<typename Coeff>
bool Precurve<Coeff>::compare (
     const HalfgenID &first,
     const HalfgenID &second,
     const Side &side,
     const size_t &depth ) const
{
     // FIXME: side is superfluous: it should agree with first.side and second.side
     //
     HalfgenID halfgenID1=first;
     HalfgenID halfgenID2=second;
     // satisfies Compare requirements (weak order <) a<a is false.
     size_t counter{0};
     auto gen1=this->halfgen_copy ( halfgenID1 );
     auto gen2=this->halfgen_copy ( halfgenID2 );
     //
     while ( gen1.joinlength==gen2.joinlength && counter<depth ) {
          if ( gen1.joinlength==0 ) { // both joins end at the puncture
               return 0;
          };
          // follow joins:
          halfgenID1=this->halfgen_copy ( halfgenID1 ).next;
          halfgenID2=this->halfgen_copy ( halfgenID2 ).next;
          // map through the arc neighbourhood:
          halfgenID1.index =
               arcNs[halfgenID1.arc].other_side (
                    halfgenID1.side,halfgenID1.index );
          halfgenID1.side = !halfgenID1.side;
          halfgenID2.index =
               arcNs[halfgenID2.arc].other_side (
                    halfgenID2.side,halfgenID2.index );
          halfgenID2.side = !halfgenID2.side;
          //
          gen1=this->halfgen_copy ( halfgenID1 );
          gen2=this->halfgen_copy ( halfgenID2 );
          ++counter;
     };
     if ( gen1.joinlength == gen2.joinlength ) {
          return false;
     }
     if ( side ) {
          //right
          if ( gen1.joinlength*gen2.joinlength>0 ) {
               //same sign and non-zero
               return gen1.joinlength<gen2.joinlength;
          };
          return gen1.joinlength>gen2.joinlength;
     };
     //left
     if ( gen1.joinlength*gen2.joinlength>0 ) {
          //same sign and non-zero
          return gen1.joinlength>gen2.joinlength;
     };
     return gen1.joinlength<gen2.joinlength;
};

template<typename Coeff>
int Precurve<Coeff>::half_weight (
     const HalfgenID &first,
     const HalfgenID &second,
     const Side &side ) const
{
     Side thisside{first.side};
     if ( second.side!=thisside ) {
          std::cerr << "INPUT ERROR: sides do not agree.";
     };
     HalfgenID halfgenID1=first;
     HalfgenID halfgenID2=second;
     // satisfies Compare requirements (weak order <) a<a is false.
     size_t counter{0};
     auto gen1=this->halfgen_copy ( halfgenID1 );
     auto gen2=this->halfgen_copy ( halfgenID2 );
     //
     while ( gen1.joinlength==gen2.joinlength && counter<this->size() ) {
          if ( gen1.joinlength==0 ) { // both joins end at the puncture
               return counter+1;
          };
          // follow joins:
          halfgenID1=this->halfgen_copy ( halfgenID1 ).next;
          halfgenID2=this->halfgen_copy ( halfgenID2 ).next;
          // map through the arc neighbourhood:
          halfgenID1.index =
               arcNs[halfgenID1.arc].other_side (
                    halfgenID1.side,halfgenID1.index );
          halfgenID1.side = !halfgenID1.side;
          halfgenID2.index =
               arcNs[halfgenID2.arc].other_side (
                    halfgenID2.side,halfgenID2.index );
          halfgenID2.side = !halfgenID2.side;
          //
          gen1=this->halfgen_copy ( halfgenID1 );
          gen2=this->halfgen_copy ( halfgenID2 );
          ++counter;
     };
     if ( gen1.joinlength==gen2.joinlength ) {
          return 0;
     }
     if ( side==thisside ) {
          //right
          if ( gen1.joinlength*gen2.joinlength>0 ) {
               //same sign and non-zero
               return counter; //FIXME
          };
          return -counter; //FIXME
     };
     //left
     if ( gen1.joinlength*gen2.joinlength>0 ) {
          //same sign and non-zero
          return counter; //FIXME gen1.joinlength>gen2.joinlength;
     };
     return -counter; //FIXME gen1.joinlength<gen2.joinlength;
};

template<typename Coeff>
Weight Precurve<Coeff>::weight (
     const HalfgenID &first,
     const HalfgenID &second ) const
{
     return {this->half_weight ( first,second,first.side ),
             this->half_weight ( first,second,first.side!=true ) };
};

template<typename Coeff>
void Precurve<Coeff>::sort ( const size_t &depth )
{
     for ( size_t arc_index=0; arc_index<this->arcNs.size(); ++arc_index ) {
          for ( Side side : {
                         0,1
                    } ) {
               // we sort the Halfgens using only halfgenID, and we
               // use std::list instead of std::vector, so sorting is
               // easier.
               std::list<HalfgenID> new_order;
               // list of old 'halfgenID's in the new order
               if ( side ) {
                    // right
                    for ( auto halfgen : this->arcNs[arc_index].right ) {
                         new_order.push_back ( halfgen.halfgenID );
                    };
               } else {
                    // left
                    for ( auto halfgen : this->arcNs[arc_index].left ) {
                         new_order.push_back ( halfgen.halfgenID );
                    };
               };
               //
               new_order.sort (
                    [this,side,depth] (
                         HalfgenID first,
               HalfgenID second ) {
                    return compare ( first, second, side, depth );
               } );
               //reorder halfgens on this side:
               Halfgens new_halfgens;
               Permutation old_indices;
               // list of old indices in the new order
               {
                    size_t it{0};
                    for ( auto el : new_order ) {
                         // find the halfgen we want to append
                         Halfgen gen{this->halfgen ( el ) };
                         // update its index and append
                         old_indices.push_back ( gen.halfgenID.index );
                         gen.halfgenID.index=it;
                         new_halfgens.push_back ( gen );
                         ++it;
                    };
               };
               //
               Permutation new_indices;
               for ( size_t it=0; it<old_indices.size(); ++it ) {
                    size_t new_index =
                         std::distance (
                              old_indices.begin(),
                              std::find ( old_indices.begin(),
                                          old_indices.end(),
                                          it ) );
                    new_indices.push_back ( new_index );
               };
               //
               if ( side ) {
                    // right
                    this->arcNs[arc_index].right=new_halfgens;
                    // update joins on the adjacent face
                    for ( auto &gen : this->arcNs[arc_index].right ) {
                         if ( gen.disjoinedQ() ==false ) {
                              // only update joined generators to avoid undefined
                              // behaviour
                              HalfgenID next{gen.next};
                              if ( next.arc==arc_index && next.side==side ) {
                                   // join returns to the same arc and side
                                   gen.next.index=new_indices[gen.next.index];
                              } else {
                                   ( this->halfgen ( gen.next ) ).next=gen.halfgenID;
                              };
                         };
                    };
               } else {
                    // left
                    this->arcNs[arc_index].left=new_halfgens;
                    // update joins on the adjacent face
                    for ( auto &gen : this->arcNs[arc_index].left ) {
                         if ( gen.disjoinedQ() ==false ) {
                              // only update joined generators to avoid undefined
                              // behaviour
                              HalfgenID next{gen.next};
                              if ( next.arc==arc_index && next.side==side ) {
                                   // join returns to the same arc and side
                                   gen.next.index=new_indices[gen.next.index];
                              } else {
                                   ( this->halfgen ( gen.next ) ).next=gen.halfgenID;
                              };
                         };
                    };
               };
               // change permutation and update matrices
               if ( side ) {
                    // right side
                    Permutation new_X;
                    for ( auto &el : old_indices ) {
                         new_X.push_back ( arcNs[arc_index].X[el] );
                    };
                    arcNs[arc_index].X=new_X;
                    // update all entries in mat_left
                    for ( auto &el : arcNs[arc_index].mat_right ) {
                         el.from=new_indices[el.from];
                         el.to=new_indices[el.to];
                    };
               } else {
                    // left side
                    for ( auto &el : arcNs[arc_index].X ) {
                         el=new_indices[el];
                    };
                    // update all entries in mat_left
                    for ( auto &el : arcNs[arc_index].mat_left ) {
                         el.from=new_indices[el.from];
                         el.to=new_indices[el.to];
                    };
               };
          };
     };
};


template<typename Coeff>
void Precurve<Coeff>::to_svg ( const std::string &name ) const
{
     //FIXME: check that this is a 3-punctured disk
     //
     // create svg file of the precurve
     std::ofstream file;
     file.open ( name+".svg" );
     // define basic lengths:
     int mat_S{7};
     Lengths ST= {
          size_t2int ( arcNs[0].left.size() ),
          size_t2int ( arcNs[1].left.size() )
     };
     Lengths SL= {
          size_t2int ( mat_S*arcNs[0].mat_left.size() ),
          size_t2int ( mat_S*arcNs[1].mat_left.size() )
     };
     Lengths SR= {
          size_t2int ( mat_S*arcNs[0].mat_right.size() ),
          size_t2int ( mat_S*arcNs[1].mat_right.size() )
     };
     int S=ST[0];
     if ( ST[0]<ST[1] ) {
          //take the maximum
          S=ST[1];
     };
     S*=10;
     // svg file header
     file << "<svg "
          << "height=\"" << 4*S+12 << "\" "
          << "width =\"" << 8*S+SL[0]+SR[0]+SL[1]+SR[1]+10 << "\"> "
          << "<rect x=\"0\" y=\"0\" width=\"100%\" height=\"100%\" style=\"fill:white\"/>\n";
     file << "<g transform=\"translate("
          << std::to_string ( 6+4*S+SL[0]+SR[0] ) << ","
          << std::to_string ( 6+2*S ) << ")\">";
     // surface:
     // straight line left
     file << svg_line (
               Coords ( -2*S-SL[0]-SR[0],-2*S ),
               Coords ( -2*S-SL[0]-SR[0],+2*S ),"red",3 );
     file << svg_small_arrowhead (
               Coords ( -2*S-SL[0]-SR[0],-2*S+4 ),0,"red",3 );
     // straight line midleft
     file << svg_line (
               Coords ( -S,-2*S ),
               Coords ( -S,+2*S ),"red",3 );
     file << svg_small_arrowhead (
               Coords ( -S,-2*S+4 ),0,"red",3 );
     // straight line mid-right
     file << svg_line (
               Coords ( S,-2*S ),
               Coords ( S,+2*S ),"red",3 );
     file << svg_small_arrowhead (
               Coords ( S,2*S-4 ),180,"red",3 );
     // straight line right
     file << svg_line (
               Coords ( 2*S+SL[1]+SR[1],-2*S ),
               Coords ( 2*S+SL[1]+SR[1],+2*S ),"red",3 );
     file << svg_small_arrowhead (
               Coords ( 2*S+SL[1]+SR[1],2*S-4 ),180,"red",3 );
     // corner left top
     file << svg_bezier (
               Coords ( -2*S-SL[0]-SR[0],-2*S ),
               Coords ( -3.5*S-SL[0]-SR[0],-2*S ),
               Coords ( -4*S-SL[0]-SR[0],-1.5*S ),
               Coords ( -4*S-SL[0]-SR[0],0 ),"green",4 );
     // corner right top
     file << svg_bezier (
               Coords ( 2*S+SL[1]+SR[1],-2*S ),
               Coords ( 3.5*S+SL[1]+SR[1],-2*S ),
               Coords ( 4*S+SL[1]+SR[1],-1.5*S ),
               Coords ( 4*S+SL[1]+SR[1],0 ),"green",4 );
     // corner left bottom
     file << svg_bezier (
               Coords ( -2*S-SL[0]-SR[0],2*S ),
               Coords ( -3.5*S-SL[0]-SR[0],2*S ),
               Coords ( -4*S-SL[0]-SR[0],1.5*S ),
               Coords ( -4*S-SL[0]-SR[0],0 ),"green",4 );
     // corner right bottom
     file << svg_bezier (
               Coords ( 2*S+SL[1]+SR[1],2*S ),
               Coords ( 3.5*S+SL[1]+SR[1],2*S ),
               Coords ( 4*S+SL[1]+SR[1],1.5*S ),
               Coords ( 4*S+SL[1]+SR[1],0 ),"green",4 );
     // straight line top
     file << svg_line (
               Coords ( -2*S-SL[0]-SR[0],-2*S ),
               Coords ( +2*S+SL[1]+SR[1],-2*S ),"green",4 );
     // straight line bottom
     file << svg_line (
               Coords ( -2*S-SL[0]-SR[0],2*S ),
               Coords ( +2*S+SL[1]+SR[1],2*S ),"green",4 );
     // arrows for algebra
     file << svg_small_arrowhead (
               Coords ( -2*S-SL[0]-SR[0]-4,2*S ),90,"green",3 );
     file << svg_small_arrowhead (
               Coords ( 2*S+SL[1]+SR[1]+4,-2*S ),-90,"green",3 );
     file << svg_small_arrowhead (
               Coords ( -S+5,-2*S ),-90,"green",3 );
     file << svg_small_arrowhead (
               Coords ( S-5,2*S ),90,"green",3 );
     //
     for ( int i : {
                    0,1
               } ) {
          if ( i==0 ) {
               file << "<g>";
          } else {
               file << "<g transform=\"rotate(180,0,0)\">";
          };
          // generators
          for ( int j=0; j<ST[i]; ++j ) {
               int y{2*S- ( j+1 ) *4*S/ ( ST[i]+1 ) };
               int pcoor{2*S- ( static_cast<int> ( arcNs[i].X[j] )+1 ) *4*S/ ( ST[i]+1 ) };
               file << svg_circle (
                         Coords ( -2*S-SL[i]-SR[i],y ),1,"black","black",1 );
               file << svg_circle (
                         Coords ( -S,y ),1,"black","black",1 );
               //
               file << svg_line (
                         Coords ( -S-SR[i],y ),
                         Coords ( -S,y ),"black",3 );
               file << svg_bezier (
                         Coords ( -1*S-SR[i],y ),
                         Coords ( -1.5*S-SR[i],y ),
                         Coords ( -1.5*S-SR[i],pcoor ),
                         Coords ( -2*S-SR[i],pcoor ),"black",3 );
               file << svg_line (
                         Coords ( -2*S-SR[i],y ),
                         Coords ( -2*S-SR[i]-SL[i],y ),"black",3 );
          };
          file << "</g>";
          // draw decorations of arcs next to D-faces
          if ( i==0 ) {
               file << "<g>";
          } else {
               file << "<g transform=\"rotate(180,0,0)\">";
          };
          int max=static_cast<int> ( arcNs[i].mat_left.size() );
          for ( int j=0; j<max; ++j ) {
               auto el=arcNs[i].mat_left[j];
               int to=static_cast<int> ( el.to );
               int from=static_cast<int> ( el.from );
               int xcoor= {-2*S-SR[i]-j*mat_S};
               int yfrom{2*S- ( from+1 ) *4*S/ ( ST[i]+1 ) };
               int yto{2*S- ( to+1 ) *4*S/ ( ST[i]+1 ) };
               if ( to==from ) {
                    // passing loop
                    file << svg_circle (
                              Coords ( xcoor,yfrom ),2,"black","white",2 );
                    if ( i==0 ) {
                         file << svg_text (
                                   Coords ( xcoor,yfrom+12 ),coeff_to_string ( el.coeff ) );
                    } else {
                         file << "<g transform=\"rotate(180,"+std::to_string ( xcoor )+","+std::to_string ( yfrom )+")\">";
                         file << svg_text ( Coords ( xcoor,yfrom+12 ),coeff_to_string ( el.coeff ) );
                         file << "</g>";
                    };
               } else {
                    // crossover arrow
                    if ( yfrom<yto ) {
                         // arrow going down
                         file << svg_line (
                                   Coords ( xcoor,yfrom ),
                                   Coords ( xcoor,yto-23 ),"black",2 );
                         file << svg_line (
                                   Coords ( xcoor,yto-10 ),
                                   Coords ( xcoor,yto ),"black",2 );
                         file << svg_small_arrowhead (
                                   Coords ( xcoor,yto-3 ),180,"black",2 );
                         if ( i==0 ) {
                              file << svg_text (
                                        Coords ( xcoor,yto-13 ),coeff_to_string ( el.coeff ) );
                         } else {
                              file << "<g transform=\"rotate(180,"+std::to_string ( xcoor )+","+std::to_string ( yto-20 )+")\">";
                              file << svg_text (
                                        Coords ( xcoor,yto-20 ),coeff_to_string ( el.coeff ) );
                              file << "</g>";
                         };
                    } else {
                         // arrow going up
                         file << svg_line (
                                   Coords ( xcoor,yfrom ),
                                   Coords ( xcoor,yto+23 ),"black",2 );
                         file << svg_line (
                                   Coords ( xcoor,yto+10 ),
                                   Coords ( xcoor,yto ),"black",2 );
                         file << svg_small_arrowhead (
                                   Coords ( xcoor,yto+3 ),0,"black",2 );
                         if ( i==0 ) {
                              file << svg_text (
                                        Coords ( xcoor,yto+20 ),coeff_to_string ( el.coeff ) );
                         } else {
                              file << "<g transform=\"rotate(180,"+std::to_string ( xcoor )+","+std::to_string ( yto+13 )+")\">";
                              file << svg_text (
                                        Coords ( xcoor,yto+13 ),coeff_to_string ( el.coeff ) );
                              file << "</g>";
                         };
                    };
               };
          };
          file << "</g>";
          // draw decorations of arcs next to S-faces
          if ( i==0 ) {
               file << "<g>";
          } else {
               file << "<g transform=\"rotate(180,0,0)\">";
          };
          max=static_cast<int> ( arcNs[i].mat_right.size() );
          for ( int j=0; j<max; ++j ) {
               auto el=arcNs[i].mat_right[j];
               int to=static_cast<int> ( el.to );
               int from=static_cast<int> ( el.from );
               int xcoor= {-S- ( 1+j )*mat_S};
               int yfrom{2*S- ( from+1 ) *4*S/ ( ST[i]+1 ) };
               int yto{2*S- ( to+1 ) *4*S/ ( ST[i]+1 ) };
               if ( to==from ) {
                    // passing loop
                    file << svg_circle (
                              Coords ( xcoor,yfrom ),2,"black","white",2 );
                    if ( i==0 ) {
                         file << svg_text (
                                   Coords ( xcoor,yfrom+12 ),coeff_to_string ( el.coeff ) );
                    } else {
                         file << "<g transform=\"rotate(180,"+std::to_string ( xcoor )+","+std::to_string ( yfrom )+")\">";
                         file << svg_text (
                                   Coords ( xcoor,yfrom+12 ),coeff_to_string ( el.coeff ) );
                         file << "</g>";
                    };
               } else {
                    // crossover arrow
                    if ( yfrom<yto ) {
                         // arrow going down
                         file << svg_line (
                                   Coords ( xcoor,yfrom ),
                                   Coords ( xcoor,yto-23 ),"black",2 );
                         file << svg_line (
                                   Coords ( xcoor,yto-10 ),
                                   Coords ( xcoor,yto ),"black",2 );
                         file << svg_small_arrowhead (
                                   Coords ( xcoor,yto-3 ),180,"black",2 );
                         if ( i==0 ) {
                              file << svg_text (
                                        Coords ( xcoor,yto-13 ),coeff_to_string ( el.coeff ) );
                         } else {
                              file << "<g transform=\"rotate(180,"+std::to_string ( xcoor )+","+std::to_string ( yto-20 )+")\">";
                              file << svg_text (
                                        Coords ( xcoor,yto-20 ),coeff_to_string ( el.coeff ) );
                              file << "</g>";
                         };
                    } else {
                         // arrow going up
                         file << svg_line (
                                   Coords ( xcoor,yfrom ),
                                   Coords ( xcoor,yto+23 ),"black",2 );
                         file << svg_line (
                                   Coords ( xcoor,yto+10 ),
                                   Coords ( xcoor,yto ),"black",2 );
                         file << svg_small_arrowhead (
                                   Coords ( xcoor,yto+3 ),0,"black",2 );
                         if ( i==0 ) {
                              file << svg_text (
                                        Coords ( xcoor,yto+20 ),coeff_to_string ( el.coeff ) );
                         } else {
                              file << "<g transform=\"rotate(180,"+std::to_string ( xcoor )+","+std::to_string ( yto+13 )+")\">";
                              file << svg_text (
                                        Coords ( xcoor,yto+13 ),coeff_to_string ( el.coeff ) );
                              file << "</g>";
                         };
                    };
               };
          };
          file << "</g>";
          //
          // D-joins
          if ( i==0 ) {
               file << "<g transform=\"translate(" << std::to_string ( -SL[0]-SR[0] ) <<" 0)\">";
          } else {
               file << "<g transform=\"rotate(180,0,0) translate(" << std::to_string ( -SL[1]-SR[1] ) <<" 0)\">";
          };
          for ( int j=0; j<ST[i]; ++j ) {
               int next=static_cast<int> ( arcNs[i].left[j].next.index );
               int joinlength{arcNs[i].left[j].joinlength};
               int y{ ( j+1 ) *4*S/ ( ST[i]+1 ) };
               int ny{ ( next+1 ) *4*S/ ( ST[i]+1 ) };
               if ( joinlength==0 ) {
                    // connect to puncture
                    file << svg_bezier (
                              Coords ( -2*S,2*S-y ),
                              Coords ( -2.5*S,2*S-y ),
                              Coords ( -2.5*S,0 ),
                              Coords ( -3*S,0 ),"black",3 );
               } else if ( joinlength>0 ) {
                    // only draw join once
                    file << svg_bezier (
                              Coords ( -2*S,2*S-y ),
                              Coords ( -2.5*S,2*S-y ),
                              Coords ( -2.5*S,-y/2 ),
                              Coords ( -3*S,-y/2 ),"black",3 );
                    file << svg_bezier (
                              Coords ( -3*S,-y/2 ),
                              Coords ( -3*S- ( y/8 ),-y/2 ),
                              Coords ( -3*S- ( y/4 ),-y/4 ),
                              Coords ( -3*S- ( y/4 ),0 ),"black",3 );
                    //
                    file << svg_bezier (
                              Coords ( -2*S,2*S-ny ),
                              Coords ( -2.5*S,2*S-ny ),
                              Coords ( -2.5*S,2*S- ( ny/2 ) ),
                              Coords ( -3*S,2*S- ( ny/2 ) ),"black",3 );
                    file << svg_bezier (
                              Coords ( -3*S,2*S- ( ny/2 ) ),
                              Coords ( -3*S- ( y/8 ),2*S- ( ny/2 ) ),
                              Coords ( -3*S- ( y/4 ),1*S- ( ny/4 ) ),
                              Coords ( -3*S- ( y/4 ),0 ),"black",3 );
               };
          };
          file << "</g>";
          // S-joins
          if ( i==0 ) {
               file << "<g>";
          } else {
               file << "<g transform=\"rotate(180,0,0)\">";
          };
          for ( int j=0; j<ST[i]; ++j ) {
               int next=static_cast<int> ( arcNs[i].right[j].next.index );
               int joinlength{arcNs[i].right[j].joinlength};
               int y{ ( j+1 ) *4*S/ ( ST[i]+1 ) };
               int ny{ ( next+1 ) *4*S/ ( ST[i]+1 ) };
               if ( joinlength==0 ) {
                    // connect to puncture
                    file << svg_bezier (
                              Coords ( -1*S,2*S-y ),
                              Coords ( -0.5*S,2*S-y ),
                              Coords ( -0.5*S,0 ),
                              Coords ( 0,0 ),"black",3 );
               } else if ( joinlength>0&& joinlength%2==0 ) {
                    // only draw join once
                    file << svg_bezier (
                              Coords ( -1*S,2*S-y ),
                              Coords ( -0.5*S,2*S-y ),
                              Coords ( -0.5*S,2*S- ( y/2 ) ),
                              Coords ( 0,2*S- ( y/2 ) ),"black",3 );
                    file << svg_bezier (
                              Coords ( 0,2*S- ( y/2 ) ),
                              Coords ( S-3* ( y/8 ),2*S- ( y/2 ) ),
                              Coords ( S- ( y/4 ),2*S-3* ( y/4 ) ),
                              Coords ( S- ( y/4 ),     0 ),"black",3 );
                    //
                    file << svg_bezier (
                              Coords ( -1*S,2*S-ny ),
                              Coords ( -0.5*S,2*S-ny ),
                              Coords ( -0.5*S,- ( ny/2 ) ),
                              Coords ( 0,- ( ny/2 ) ),"black",3 );
                    file << svg_bezier (
                              Coords ( 0,- ( ny/2 ) ),
                              Coords ( S-3* ( y/8 ),- ( ny/2 ) ),
                              Coords ( S- ( y/4 ),- ( ny/4 ) ),
                              Coords ( S- ( y/4 ),0 ),"black",3 );
               } else if ( joinlength>0&& joinlength%2==1 ) {
                    if ( i==0 ) {
                         ny= ( next+1 ) *4*S/ ( ST[1]+1 );
                    } else {
                         ny= ( next+1 ) *4*S/ ( ST[0]+1 );
                    };
                    // only draw join once
                    file << svg_bezier (
                              Coords ( -1*S,2*S-y ),
                              Coords ( -0.5*S,2*S-y ),
                              Coords ( -0.5*S,2*S- ( y/2 ) ),
                              Coords ( 0,2*S- ( y/2 ) ),"black",3 );
                    file << svg_bezier (
                              Coords ( 1*S,-2*S+ny ),
                              Coords ( 0.5*S,-2*S+ny ),
                              Coords ( 0.5*S,2*S- ( y/2 ) ),
                              Coords ( 0,2*S- ( y/2 ) ),"black",3 );
               };
          };
          file << "</g>";
     };
     // punctures
     file << svg_circle (
               Coords ( -3*S-SL[0]-SR[0],0 ),5,"black","white",4 );
     file << svg_circle (
               Coords ( 0,0 ),5,"black","white",4 );
     file << svg_circle (
               Coords ( 3*S+SR[1]+SL[1],0 ),5,"black","white",4 );
     //
     // end of file
     file << "</g></svg>";
};


template<typename Coeff>
int Precurve<Coeff>::size() const
{
     int output;
     for ( auto arcN : arcNs ) {
          output += arcN.size();
     };
     return output;
};

template<typename Coeff>
Halfgen & Precurve<Coeff>::halfgen ( const HalfgenID &halfgenID )
{
     // returns a reference to the halfgen specified at the
     // location halfgenID; use this if we want to change the
     // precurve
     return ( halfgenID.side ) ? arcNs[halfgenID.arc].right[halfgenID.index] : arcNs[halfgenID.arc].left[halfgenID.index];
};

template<typename Coeff>
const Halfgen & Precurve<Coeff>::halfgen_copy ( const HalfgenID &halfgenID ) const
{
     // returns a copy of the halfgen specified at the
     // location halfgenID; use this if we do not want to
     // change the precurve
     return ( halfgenID.side ) ?
            arcNs[halfgenID.arc].right[halfgenID.index] :
            arcNs[halfgenID.arc].left[halfgenID.index];
};

template<typename Coeff>
void Precurve<Coeff>::add_join (
     const HalfgenID &from,
     const HalfgenID &to,
     const int &joinlength )
{
     // add joins symmetrically; joinlength(assumed to be
     // non-zero) may have positive and negative values
     this->halfgen ( from ).set_join ( to,joinlength );
     this->halfgen ( to ).set_join ( from,-joinlength );
};


template<typename Coeff>
void Precurve<Coeff>::LPUdecompose()
{
     for ( auto &arc : arcNs ) {
          arc.LPUdecompose();
     };
};


template<typename Coeff>
void Precurve<Coeff>::add_mat (
     const size_t &arc,
     Side side,
     size_t &start,
     size_t &end,
     const Coeff &coeff )
{
     if ( side ) { // 1 = add on the right
          arcNs[arc].mat_right.push_back (
               DoubleArrow<Coeff> ( start,end,coeff ) );
     } else { // 0 = add on the left
          arcNs[arc].mat_left.insert (
               arcNs[arc].mat_left.begin(),
               DoubleArrow<Coeff> ( start,end,coeff ) );
     };
};



#endif
