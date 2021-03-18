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



#ifndef TANGLES_TPP
#define TANGLES_TPP
#ifndef TANGLES_H
#include "../headers/Tangles.h"
#include "../headers/constants.h"
#endif


/// \file Tangles.cpp
/// \brief implements the class Tangles

Tangle::Tangle() : slices(), cuts(), symmetry ( false ) {}

Tangle::Tangle ( std::string input,std::vector<bool> top_orient,bool auto_correct )
{
     fixed_orientations = false;
     symmetry = false;
     if ( input.front() == 's' ) {
          symmetry = true;
          if ( top_orient.size() == 1 ) {
               top_orient.push_back ( top_orient.back() );
          } else {
               top_orient = { 1,1 };
          };
          input = input.substr ( input.find_first_of ( '.' ) + 1 );
     };
     slices = to_slices ( input,top_orient.size() );
     //
     // define cuts
     std::string error_msg;
     cuts.push_back ( top_orient );
     std::vector<bool> temp {};
     bool go_on {true};
     while ( go_on ) {
          go_on = false;
          std::string tanglestring {};
          for ( auto slice : slices ) {
               cuts.push_back ( cuts.back() );
               switch ( slice.first ) {
               case 'x':
               case 'y':
                    if ( symmetry && slice.second == 0 ) {
                         cuts.back() [1] = !cuts.back() [1] ;
                    } else {
                         std::swap ( cuts.back() [slice.second],cuts.back() [slice.second+1] );
                    }
                    break;
               case 'u':
                    if ( symmetry && slice.second == 0 ) {
                         error_msg="ERROR: This is a symmetric knot. Only crossings are allowed at position 0!\n";
                         throw std::invalid_argument ( error_msg );
                    } else if ( cuts.back() [slice.second]!=cuts.back() [slice.second+1] ) {
                         cuts.back().erase ( cuts.back().begin()+slice.second,cuts.back().begin()+slice.second+2 );
                    } else if ( auto_correct ) {
                         tanglestring.pop_back();
                         Tangle TempT {};
                         if ( symmetry ) {
                              TempT = Tangle ( "s."+tanglestring,cuts.front() );
                         } else {
                              TempT = Tangle ( tanglestring,cuts.front() );
                         };
                         TempT.flip_orient_at_index ( slice.second );
                         fixed_orientations = true;
                         // update slices
                         for ( size_t i = 0; i < TempT.slices.size(); i++ ) {
                              slices[i] = TempT.slices[i];
                         };
                         // redefine cuts
                         cuts = {TempT.top_orient() };
                         go_on = true;
                    } else {
                         error_msg="ERROR: orientation do not match up!\n";
                         throw std::invalid_argument ( error_msg );
                    };
                    break;
               case 'l':
                    if ( symmetry && slice.second == 0 ) {
                         error_msg="ERROR: This is a symmetric knot. Only crossings are allowed at position 0!\n";
                         throw std::invalid_argument ( error_msg );
                    };
                    temp = {0,1};
                    cuts.back().insert ( cuts.back().begin()+slice.second,temp.begin(),temp.end() );
                    break;
               case 'r':
                    if ( symmetry && slice.second == 0 ) {
                         error_msg="ERROR: This is a symmetric knot. Only crossings are allowed at position 0!\n";
                         throw std::invalid_argument ( error_msg );
                    };
                    temp = {1,0};
                    cuts.back().insert ( cuts.back().begin()+slice.second,temp.begin(),temp.end() );
               };
               if ( go_on ) {
                    break;
               };
               tanglestring += slice.first;
               tanglestring += std::to_string ( slice.second );
               tanglestring += ".";
          };
     };
};



Tangle::Tangle ( const File &file )
{
     std::ifstream ifs ( file.fullname() + ".kht" );
     if ( ifs.good() != true ) {
          std::cerr << "I cannot read the file " <<  file.fullname() << ".kht.\nPlease make sure it exists, has the correct file ending 'kht',\nand has the right permissions. \n" ;
          exit ( 1 );
     };
     // create/check subdirectory, but only after verifying that the file itself exists
     file.create_directories();
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
     // remove any characters that do not look like a tangle (in particular linebreaks \n and \r)
     std::string content_new = "";
     std::string allowed_chars = "srluxy.1234567890,";
     for ( char const &c : content ) {
          if ( allowed_chars.find ( c ) != std::string::npos ) {
               content_new.push_back ( c );
          };
     };
     content = content_new;
     // split off top_orient
     content+=",";
     std::vector<bool> top_orient {};
     std::size_t posFcomma = content.find_first_of ( "," );
     std::string tangle = content.substr ( 0,posFcomma );
     std::string top_orient_str = content.substr ( posFcomma );
     for ( char const &c : top_orient_str ) {
          if ( c == '0' ) {
               top_orient.push_back ( 0 );
          } else if ( c == '1' ) {
               top_orient.push_back ( 1 );
          };
     };
     // default if no top is specified
     if ( top_orient.size() == 0 ) {
          top_orient = {1};
     };
     *this = Tangle ( tangle, top_orient );
};

//                                 //
// input, output and sanity checks //
//                                 //

void Tangle::to_file ( const File &file ) const
{
     std::ofstream out ( file.fullname() + ".kht" );
     out << "% " << file.name() << "\n"
         << this->tanglestring() << "\n";
     if ( this->is_symmetric() ) {
          out << "," << this->top_orient() [1];
     } else {
          for ( const bool e : this->top_orient() ) {
               out << "," << e;
          };
     };
     out << "\n";
     out.close();
};

void Tangle::record_new_orientations ( const File &file ) const
{
     if ( fixed_orientations ) {
          std::string tangle {};
          tangle += this->tanglestring();
          if ( symmetry ) {
               tangle += "," + std::to_string ( this->top_orient().back() );
          } else {
               for ( const bool e : this->top_orient() ) {
                    tangle += "," + std::to_string ( e );
               };
          };
          conditional_append (
               file.fullname() + ".kht",
               "% Fixed orientations: " +  tangle );
     };
};

bool Tangle::is_symmetric() const
{
     return symmetry;
};

std::string Tangle::to_string ( const int &index,const bool &orient ) const
{
     // print only the kth slice of the tangle
     // 1= with maximal orientation data (all string have orientations)
     // 0= with minimal orientation data (only caps have orientations)
     std::string output;
     Slice slice=slices[index];
     auto top=cuts[index];
     auto bot=cuts[index+1];
     // strands before the index:
     size_t start_index {0};
     if ( symmetry && ( slice.second != 0 ) ) {
          start_index = 1;
          output += "  ";
     };
     for ( int j = start_index; j<slice.second; ++j ) {
          if ( orient==1 ) {
               output+= ( top[j]==0 ) ? "↓ " : "↑ ";
          } else {
               output+=               "| ";
          };
     };
     // two strands at the index:
     switch ( slice.first ) {
     case 'x':
          output+=" ⤬  ";
          break;
     case 'y':
          output+=" ⤫  ";
          break;
     case 'l':
          output+=" ↶ ";
          break;
     case 'r':
          output+=" ↷ ";
          break;
     case 'u':
          output+="  ͝";
          break;
     };
     // strands after the index:
     if ( slice.first=='x' || slice.first=='y' ) {
          for ( size_t j=slice.second+2; j<top.size(); ++j ) {
               if ( orient==1 ) {
                    output+= ( top[j]==0 ) ? "↓ " : "↑ ";
               } else {
                    output+=               "| ";
               };
          };
     } else if ( slice.first=='r' || slice.first=='l' ) {
          for ( size_t j=slice.second+2; j<bot.size(); ++j ) {
               output+=" ⟍";
          };
     } else {
          for ( size_t j=slice.second+2; j<top.size(); ++j ) {
               output+="⟋ ";
          };
     };
     return output;
}

void Tangle::print ( const bool &orient ) const
{
     // 1= with maximal orientation data (all string have orientations)
     // 0= with minimal orientation data (only caps have orientations)
     std::string s{""};
     size_t N {slices.size() };
     if ( symmetry ) {
          std::cout << fill_front ( "|",std::to_string ( N ) )
                    << "    \t";
          if ( cuts.front().front() == 0 ) {
               std::cout << " ↷\n";
          } else {
               std::cout << " ↶\n";
          };
     };
     //
     for ( size_t i=0; i<N; ++i ) {
          if ( ( ( i+1 ) %5==0 ) || ( i==0 ) || ( i==N-1 ) ) {
               s = std::to_string ( i+1 );
          } else {
               s = "|";
          };
          std::cout << fill_front ( s,std::to_string ( N ) )
                    << " "
                    << slices[i].first
                    << slices[i].second
                    << "\t"
                    << this->to_string ( i,orient )
                    << "\n";
          //<< "\t\tcut above:" << cuts[i].size() << "\n";
     };
     if ( symmetry ) {
          std::cout << fill_front ( "|",std::to_string ( N ) )
                    << "    \t"
                    << "  ͝\n";
     };
}

std::string Tangle::tanglestring() const
{
     if ( symmetry ) {
          return "s." + slices_to_string ( slices );
     };
     return slices_to_string ( slices );
}

std::vector<bool> Tangle::top_orient() const
{
     //
     return this->cuts.front();
}

std::vector<bool> Tangle::bot_orient() const
{
     //
     return this->cuts.back();
}

std::string Tangle::draw_svg_orient ( const Cut &cut, const int &ycoord, const int &pref_orient ) const
{
     std::string output;
     int j {0};
     for ( const auto &orient : cut ) {
          if ( !symmetry || ( j != 0 ) ) {
               if ( orient && pref_orient!=0 ) {
                    // up
                    output += svg_arrowhead ( Coords ( j*100+50,ycoord*100 -50 ),0 );
               } else if ( orient==false && pref_orient!=1 ) {
                    //down
                    output += svg_arrowhead ( Coords ( j*100+50,ycoord*100 -50 ),180 );
               };
          };
          ++j;
     };
     return output;
};

std::string Tangle::draw_svg_slice_fill ( const std::vector<Slice> &sliceL,const unsigned int &max, const unsigned int &level ) const
{
     int y = level*100-50;
     std::string output;
     std::list<unsigned int> remaining;
     unsigned int start_index {0};
     if ( symmetry ) {
          start_index = 1;
     };
     switch ( sliceL.front().first ) {
     case 'x':
     case 'y':
          // list of tangle indices of points in the cut:
          for ( unsigned int i = start_index; i<max; ++i ) {
               remaining.push_back ( i );
          };
          // remove those indices that are occupied by crossings:
          for ( const auto &slice : sliceL ) {
               remaining.remove ( slice.second );
               remaining.remove ( slice.second+1 );
          };
          // or each remaining index, draw a straight line:
          for ( const auto &i : remaining ) {
               output+=svg_bezier ( Coords ( 100*i+50,y ),
                                    Coords ( 100*i+50,y+50 ),
                                    Coords ( 100*i+50,y+50 ),
                                    Coords ( 100*i+50,y+100 ) );
          };
          return output;
          break;
     case 'l':
     case 'r':
     case 'u':
          return "";
     };
     return "ERROR"; //PROGRAM SHOULD NEVER GET TO THIS LINE.
}

std::string Tangle::draw_svg_slice ( const unsigned int &si,const unsigned int &level ) const
{
     Slice slice=slices[si];
     //top left coordinate:
     int x= slice.second*100+50;
     int y= level*100-50;
     unsigned int start_index {0};
     if ( symmetry ) {
          start_index = 1;
     };
     //
     std::string output;
     output += "<!-- level " + std::to_string ( level ) + " ";
     switch ( slice.first ) {
     case 'x':
          output+= "x" + std::to_string ( slice.second ) + " -->\n"
                   //over strand white:
                   +svg_mask ( svg_bezier ( Coords ( x,y ),
                                            Coords ( x,y+50 ),
                                            Coords ( x+100,y+50 ),
                                            Coords ( x+100,y+100 ),"black",stringwhite ),
                               std::to_string ( slice.second ) + "-" + std::to_string ( level ) )
                   //under strand:
                   +svg_bezier ( Coords ( x+100,y ),
                                 Coords ( x+100,y+50 ),
                                 Coords ( x,y+50 ),
                                 Coords ( x,y+100 ),
                                 "black",
                                 stringwidth,
                                 "mask='url(#mask-" + std::to_string ( slice.second ) + "-" + std::to_string ( level ) + ")'" )
                   //over strand:
                   +svg_bezier ( Coords ( x,y ),
                                 Coords ( x,y+50 ),
                                 Coords ( x+100,y+50 ),
                                 Coords ( x+100,y+100 ) );
          break;
     case 'y':
          output+= "y" + std::to_string ( slice.second ) + " -->\n"
                   //over strand white:
                   +svg_mask ( svg_bezier ( Coords ( x,y+100 ),
                                            Coords ( x,y+50 ),
                                            Coords ( x+100,y+50 ),
                                            Coords ( x+100,y ),"black",stringwhite ),
                               std::to_string ( slice.second ) + "-" + std::to_string ( level ) )
                   //under strand:
                   +svg_bezier ( Coords ( x+100,y+100 ),
                                 Coords ( x+100,y+50 ),
                                 Coords ( x,y+50 ),
                                 Coords ( x,y ),
                                 "black",
                                 stringwidth,
                                 "mask='url(#mask-" + std::to_string ( slice.second ) + "-" + std::to_string ( level ) + ")'" )
                   //over strand:
                   +svg_bezier ( Coords ( x,y+100 ),
                                 Coords ( x,y+50 ),
                                 Coords ( x+100,y+50 ),
                                 Coords ( x+100,y ) );
          break;
     case 'l':
     case 'r':
          output+= "l/r" + std::to_string ( slice.second ) + " -->\n"
                   +svg_bezier ( Coords ( x,y+100 ),
                                 Coords ( x,y+50 ),
                                 Coords ( x+100,y+50 ),
                                 Coords ( x+100,y+100 ) );
          for ( int i = start_index; i!=slice.second; ++i ) {
               output+= svg_bezier ( Coords ( 100*i+50,y ),
                                     Coords ( 100*i+50,y+50 ),
                                     Coords ( 100*i+50,y+50 ),
                                     Coords ( 100*i+50,y+100 ) );
          };
          for ( size_t i=slice.second; i!=cuts[si].size(); ++i ) {
               output+= svg_bezier ( Coords ( 100*i+50,y ),
                                     Coords ( 100*i+50,y+50 ),
                                     Coords ( 100*i+250,y+50 ),
                                     Coords ( 100*i+250,y+100 ) );
          };
          break;
     case 'u':
          output+= "u" + std::to_string ( slice.second ) + " -->\n"
                   +svg_bezier ( Coords ( x,y ),
                                 Coords ( x,y+50 ),
                                 Coords ( x+100,y+50 ),
                                 Coords ( x+100,y ) );
          for ( int i = start_index; i!=slice.second; ++i ) {
               output+= svg_bezier ( Coords ( 100*i+50,y ),
                                     Coords ( 100*i+50,y+50 ),
                                     Coords ( 100*i+50,y+50 ),
                                     Coords ( 100*i+50,y+100 ) );
          };
          for ( size_t i=slice.second+2; i!=cuts[si].size(); ++i ) {
               output+= svg_bezier ( Coords ( 100*i+50,y ),
                                     Coords ( 100*i+50,y+50 ),
                                     Coords ( 100*i-150,y+50 ),
                                     Coords ( 100*i-150,y+100 ) );
          };
          break;
     };
     return output;
}

void Tangle::to_svg ( const std::string &metadata,
                      const std::string &name,
                      const bool &orient,
                      const bool &transparent ) const
{
     // create svg file of the tangle (compactified) diagram
     std::ofstream file;
     file.open ( name+".svg" );
     // draw multiple crossings on the same level:
     std::vector<std::vector<Slice>> compact_slices;
     std::vector<Slice> compact_slice;
     std::vector<int> occ;// occupied indices
     for ( const auto &slice : slices ) {
          if ( ( slice.first!='x' && slice.first!='y' ) ) {
               // slice is either cap or cup:
               if ( compact_slice.empty() ==false ) {
                    compact_slices.push_back ( compact_slice );
               }
               compact_slices.push_back ( {slice} );
               compact_slice= {};
               occ= {};
          } else if ( std::find ( occ.begin(),occ.end(),slice.second ) ==occ.end() &&
                      std::find ( occ.begin(),occ.end(),slice.second+1 ) ==occ.end() ) {
               // slice is a crossing and does not interfer with the other
               // crossings in compact_slice so far:
               compact_slice.push_back ( slice );
               occ.push_back ( slice.second );
               occ.push_back ( slice.second+1 );
          } else {
               // slice is a crossing and interfers with the other crossings:
               compact_slices.push_back ( compact_slice );
               compact_slice= {{slice}};
               occ = {slice.second,slice.second+1};
          };
     };
     if ( compact_slice.empty() ==false ) {
          compact_slices.push_back ( compact_slice );
     }
     //
     // define basic lengths:
     unsigned int height=50+compact_slices.size() *100+50;
     if ( symmetry ) {
          height +=200;
     };
     // top sep 100, bottom sep 100
     unsigned int width{0};
     for ( auto &cut : cuts ) {
          if ( cut.size() >width ) {
               width=cut.size();
          };
     };
     width*=100;
     // svg file header
     file << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
          << "height=\"" << height << "\" "
          << "width =\"" << width << "\"> \n";
     if ( transparent == false ) {
          file << "<rect x=\"0\" y=\"0\" width=\"100%\" height=\"100%\" style=\"fill:white\"/>\n";
     };
     file << "<!-- file '"
          << name
          << ".svg', "
          << metadata
          << " -->\n";
     //
//      file << "<text textLength='80' x='50%' y='50' style='font-weight:bold;font-size:40px;line-height:1.25;' font-family='monospace' text-anchor='middle'>"
//           << name
//           << "</text>";
     //
     if ( symmetry ) {
          file << svg_line ( {100,50}, {100,height-50},"grey' stroke-dasharray='0,12.5" );
          file << svg_bezier ( {100,100},{130,100},{150,120},{150,150});
          file << svg_bezier ( {100,height-100},{130,height-100},{150,height-120},{150,height-150});
          file << svg_circle ( {100,100}, 7, "black", "solid");
          file << svg_circle ( {100,height-100}, 7, "black", "solid");
          file << "<g transform=\"translate(0 100)\">\n";
     };
     //
     // Crossings, Caps and Cups
     {
          int i{0};// cut index
          int ycoord{1};// y-coord
          file << "\n<!-- crossings, caps and cups -->\n";
          for ( const auto &sliceL : compact_slices ) {
               for ( unsigned int si=i; si!=i+sliceL.size(); ++si ) {
                    file << this->draw_svg_slice ( si,ycoord );
               };
               file << this->draw_svg_slice_fill ( sliceL,cuts[i].size(),ycoord );
               i+=sliceL.size();
               ++ycoord;
          };
     }
     // orientations
     file << "\n<!-- orientations -->\n";
     if ( orient==1 ) {
          int i{0};// cut index
          int ycoord{1};// y-coordinate
          for ( const auto &sliceL : compact_slices ) {
               file << this->draw_svg_orient ( cuts[i],ycoord );
               i+=sliceL.size();
               ++ycoord;
          };
          file << this->draw_svg_orient ( cuts[i],ycoord );
     } else {
          int i{0};// cut index
          int ycoord{1};// y-coordinate
          file << this->draw_svg_orient ( cuts.front(),ycoord,1 );
          // only draw orientations up
          for ( unsigned int index=0; index!=compact_slices.size(); ++index ) {
               int j = compact_slices[index].front().second;
               if ( compact_slices[index].front().first=='l' ) {
                    file << svg_arrowhead ( Coords ( j*100+50,ycoord*100+50 ),180 );
               } else if ( compact_slices[index].front().first=='r' ) {
                    file << svg_arrowhead ( Coords ( j*100+150,ycoord*100+50 ),180 );
               };
               i+=compact_slices[index].size();
               ++ycoord;
          };
          file << this->draw_svg_orient ( cuts.back(),ycoord,0 );
          // only draw orientations down
     };
     if ( symmetry ) {
          file << "</g>\n";
     };
     //
     file << "</svg>";
     file.close();
}

bool Tangle::n_ended ( const size_t &n )
{
     if ( cuts.front().size() == 0 ) {
          return false;
     };
     return ( cuts.front().size() + cuts.back().size() == n );
};



//              //
// main methods //
//              //
template <typename Coeff>
Complex<CobObj,CobMor,Coeff> Tangle::CobComplex ( Complex<CobObj,CobMor,Coeff> cx,const bool &with_feedback ) const
{
     TE top = cuts.front().size();
     if ( cx.get_objects().empty() ) {
          std::vector<CobObj> objects{CobObj ( top ) };
          Eigen::SparseMatrix<CobMor<Coeff>> diffs ( 1,1 );
          cx = { objects,diffs };
     } else if ( top != cx.get_objects().front().get_bot() ) {
          // sanity checks
          std::cerr << "unable to add tangle to complex, since the number of tangle ends do not match.";
          exit ( 1 );
     } else {
          top = cx.get_objects().front().get_top();
     };
     std::string feedback="";
     auto start_t {std::chrono::high_resolution_clock::now() };
     auto current_t {start_t};
     auto previous_t {start_t};
     //
     for ( size_t k=0; k<slices.size(); ++k ) {
          // start feedback
          previous_t = current_t;
          current_t = std::chrono::high_resolution_clock::now();
          feedback = "\33[2K\r";
          // erase current line and bring cursor back to start
          feedback += to_percentage ( k,slices.size() ) + " ";
          feedback += spin ( k );
          feedback += " Computed slice "
                      + std::to_string ( k )
                      + "/"
                      + std::to_string ( slices.size() )
                      + " with "
                      + std::to_string ( cx.get_objects().size() )
                      + " generators in "
                      + time_to_string (
                           std::chrono::duration_cast<std::chrono::milliseconds> ( current_t-previous_t ) )
                      + ". ";
          if ( with_feedback ) {
               std::cout << feedback << std::flush;
          };
          int i=slices[k].second;
          // index of the tangle strand where we add crossing/cap/cup, where
          // our count excludes the tangle ends at the top
          if ( slices[k].first=='x' ) {
               // CASE x: ⤬ (XorY==1)
               // X positive crossing (sign==1): h^0  q^1   = ---> h^1 q^2  )(
               // X negative crossing (sign==0): h^-1 q^-2  = ---> h^0 q^-1 )(
               if ( cuts[k][i]!=cuts[k][i+1] ) {
                    // positive crossing (strands pointing in opposite directions)
                    cx.AddCrossing ( i+top,1,1 );
               } else {
                    // negative crossing
                    cx.AddCrossing ( i+top,0,1 );
               };
          } else if ( slices[k].first=='y' ) {
               // CASE y: ⤫ (XorY==0)
               // Y positive crossing (sign==1): h^0  q^1  )( ---> h^1 q^2   =
               // Y negative crossing (sign==0): h^-1 q^-2 )( ---> h^0 q^-1  =
               if ( cuts[k][i]==cuts[k][i+1] ) {
                    // positive crossing (strands pointing in the same directions)
                    cx.AddCrossing ( i+top,1,0 );
               } else {
                    // negative crossing
                    cx.AddCrossing ( i+top,0,0 );
               };
          } else if ( slices[k].first=='u' ) {
               // CASE u: ∪
               cx.AddCup ( i+top );
          } else {
               // Case l or r : ↶ or ↷
               cx.AddCap ( i+top );
          };
//           if (cx.check()==false){
//                std::cerr << "Failed d²=0 before cancellation.";
//                exit(1);
//           };
          cx.cancel();
//           if (cx.check()==false){
//                std::cerr << "Failed d²=0 after cancellation.";
//                exit(1);
//           };
     };
     if ( with_feedback ) {
          std::cout << "\33[2K\rComputed cobordism complex with "
                    << cx.get_objects().size()
                    << " generators in "
                    << time_to_string ( std::chrono::duration_cast<std::chrono::milliseconds> ( current_t-start_t ) )
                    << ".\n"
                    << std::flush;
     };
     return cx;
};


void Tangle::flip_orient_at_index ( int i )
{
     size_t level { slices.size() };
     cuts[level][i] = !cuts[level][i];
     //
     bool go_up {true};
     while ( true ) {
//           if ( go_up ) {
//                std::cout << "go_up = true, ";
//           } else {
//                std::cout << "go_up = false, ";
//           };
//           std::cout << "level = " << level
//                     << ", index = " << i
//                     << "\n" << std::flush;
          if ( go_up ) {
               switch ( slices[level-1].first ) {
               case 'x':
               case 'y':
                    if ( slices[level-1].second == i ) {
                         i++ ;
                         level--;
                         // still go up
                    } else if ( slices[level-1].second == i-1 ) {
                         if ( symmetry && i == 1 ) {
                              // index unchanged
                              level--;
                              // still go up
                         } else {
                              i-- ;
                              level--;
                              // still go up
                         };
                    } else {
                         // index unchanged
                         level--;
                         // still go up
                    };
                    break;
               case 'l':
                    if ( slices[level-1].second == i ) {
                         slices[level-1].first = 'r';
                         //
                         i++ ;
                         // level unchanged
                         go_up = false;
                    } else if ( slices[level-1].second == i-1 ) {
                         slices[level-1].first = 'r';
                         //
                         i-- ;
                         // level unchanged
                         go_up = false;
                    } else if ( slices[level-1].second < i-1 ) {
                         i--;
                         i--;
                         level--;
                         // still go up
                    } else {
                         // index unchanged
                         level--;
                         // still go up
                    };
                    break;
               case 'r':
                    if ( slices[level-1].second == i ) {
                         slices[level-1].first = 'l';
                         //
                         i++ ;
                         // level unchanged
                         go_up = false;
                    } else if ( slices[level-1].second == i-1 ) {
                         slices[level-1].first = 'l';
                         //
                         i-- ;
                         // level unchanged
                         go_up = false;
                    } else if ( slices[level-1].second < i-1 ) {
                         i--;
                         i--;
                         level--;
                         // still go up
                    } else {
                         // index unchanged
                         level--;
                         // still go up
                    };
                    break;
               case 'u':
                    if ( slices[level-1].second < i+1 ) {
                         i++;
                         i++;
                         level--;
                         // still go up
                    } else {
                         // index unchanged
                         level--;
                         // still go up
                    };
                    break;
               };
          } else {
               switch ( slices[level].first ) {
               case 'x':
               case 'y':
                    if ( slices[level].second == i ) {
                         i++ ;
                         level++;
                         // still go down
                    } else if ( slices[level].second == i-1 ) {
                         if ( symmetry && i == 1 ) {
                              // index unchanged
                              level--;
                              // still go down
                         } else {
                              i-- ;
                              level++;
                              // still go down
                         };
                    } else {
                         // index unchanged
                         level++;
                         // still go down
                    };
                    break;
               case 'l':
               case 'r':
                    if ( slices[level].second < i+1 ) {
                         i++;
                         i++;
                         level++;
                         // still go down
                    } else {
                         // index unchanged
                         level++;
                         // still go down
                    };
                    break;
               case 'u':
                    if ( slices[level].second == i ) {
                         i++ ;
                         // level unchanged
                         go_up = true;
                    } else if ( slices[level].second == i-1 ) {
                         i-- ;
                         // level unchanged
                         go_up = true;
                    } else if ( slices[level].second < i-1 ) {
                         i--;
                         i--;
                         level++;
                         // still go down
                    } else {
                         // index unchanged
                         level++;
                         // still go down
                    };
                    break;
               };
          };
          cuts[level][i] = !cuts[level][i];
          if ( level == slices.size() || level == 0 ) {
               break;
          };
     };
};


Tangle Tangle::add ( const std::string &twists ) const
{
     std::string tanglestring {this->tanglestring() };
     if ( twists.empty() == false ) {
          tanglestring += ".";
          tanglestring += twists;
     };
     return Tangle ( tanglestring,this->top_orient() );
};

Tangle Tangle::quotient ( ) const
{
     if ( !symmetry ) {
          std::cerr << "ERROR: Unable to take the quotient. The input needs to be a strongly invertible knot. (The tangle string needs to start with 's.'.)";
          abort();
     };
     std::string tanglestring;
     Cut new_top;
     bool orient = cuts.front().back();
     new_top.push_back ( !orient );
     new_top.push_back ( !orient );
     if ( orient ) {
          tanglestring += "r2.r3.";
     } else {
          tanglestring += "l2.l3.";
     };
     //
     for ( const Slice &slice : slices ) {
          switch ( slice.first ) {
          case 'x':
               if ( slice.second == 0 ) {
                    tanglestring += "x1.x2.x2.x1.";
                    break;
               }
          case 'y':
               if ( slice.second == 0 ) {
                    tanglestring += "y1.y2.y2.y1.";
                    break;
               } else {
                    tanglestring += slice.first +
                                    std::to_string ( 2*slice.second+1 ) + ".";
                    tanglestring += slice.first +
                                    std::to_string ( 2*slice.second ) + ".";
                    tanglestring += slice.first +
                                    std::to_string ( 2*slice.second+2 ) + ".";
                    tanglestring += slice.first +
                                    std::to_string ( 2*slice.second+1 ) + ".";
                    break;
               }
          case 'l':
          case 'r':
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second ) + ".";
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second+1 ) + ".";
               break;
          case 'u':
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second+1 ) + ".";
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second ) + ".";
               break;
          };
     };
     tanglestring += "u1.u0.";
     // fix framing
     int twists {this->writhe() };
     std::string twiststring;
     if ( twists > 0 ) {
          twiststring = "x0.x0.";
     } else {
          twiststring = "y0.y0.";
          twists *= -1;
     };
     for ( int i = 0; i<twists ; ++i ) {
          tanglestring += twiststring;
     };
     tanglestring.pop_back();
     //
     Tangle TempT {tanglestring, new_top};
     if ( TempT.bot_orient().front() != orient ) {
          TempT.flip_orient_at_index ( 0 );
     };
     if ( TempT.bot_orient().back() != orient ) {
          TempT.flip_orient_at_index ( 1 );
     };
     //
     return TempT;
};

Tangle Tangle::complete_symmetry ( ) const
{
     if ( !symmetry ) {
          std::cerr << "ERROR: Unable to do conversion to a complete knot diagram. The input needs to be a strongly invertible knot. (The tangle string needs to start with 's.'.)";
          abort();
     };
     std::string tanglestring;
     Cut new_top;
     new_top.push_back ( !cuts.front().back() );
     new_top.push_back ( cuts.front().back() );
     int halfwidth {1};
     for ( const Slice &slice : slices ) {
          if ( slice.second == 0 ) {
               tanglestring = tanglestring + slice.first +
                              std::to_string ( halfwidth - 1 ) + ".";
          } else {
               switch ( slice.first ) {
               case 'x':
               case 'y':
                    tanglestring = tanglestring + slice.first +
                                   std::to_string ( halfwidth + slice.second - 1 ) + ".";
                    tanglestring = tanglestring + slice.first +
                                   std::to_string ( halfwidth - slice.second - 1 ) + ".";
                    break;
               case 'l':
                    tanglestring = tanglestring + "l" +
                                   std::to_string ( halfwidth + slice.second - 1 ) + ".";
                    tanglestring = tanglestring + "r" +
                                   std::to_string ( halfwidth - slice.second + 1 ) + ".";
                    halfwidth += 2;
                    break;
               case 'r':
                    tanglestring = tanglestring + "r" +
                                   std::to_string ( halfwidth + slice.second - 1 ) + ".";
                    tanglestring = tanglestring + "l" +
                                   std::to_string ( halfwidth - slice.second + 1 ) + ".";
                    halfwidth += 2;
                    break;
               case 'u':
                    tanglestring = tanglestring + "u" +
                                   std::to_string ( halfwidth + slice.second - 1 ) + ".";
                    tanglestring = tanglestring + "u" +
                                   std::to_string ( halfwidth - slice.second - 1 ) + ".";
                    halfwidth -= 2;
                    break;
               };
          };
     };
     tanglestring += "u0";
     //
     return Tangle ( tanglestring, new_top );
};

void Tangle::doubled ()
{
     std::string tanglestring;
     Cut new_top;
     for ( const bool &orient : cuts.front() ) {
          new_top.push_back ( orient );
          new_top.push_back ( orient );
     };
     for ( const Slice &slice : slices ) {
          switch ( slice.first ) {
          case 'x':
          case 'y':
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second+1 ) + ".";
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second ) + ".";
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second+2 ) + ".";
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second+1 ) + ".";
               break;
          case 'l':
          case 'r':
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second ) + ".";
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second+1 ) + ".";
               break;
          case 'u':
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second+1 ) + ".";
               tanglestring += slice.first +
                               std::to_string ( 2*slice.second ) + ".";
               break;
          }
     };
     if ( tanglestring.empty() == false ) {
          tanglestring.pop_back();
     };
     //
     *this = Tangle ( tanglestring, new_top );
};

void Tangle::simplify_diagram ()
{
     if ( !symmetry ) {
          *this = Tangle ( simplify ( this->tanglestring(), cuts.front().size() ),cuts.front() );
     };
};

int Tangle::writhe () const
{
     // signed number of crossings
     int writhe {0};
     int changer {1};
     for ( size_t i=0; i<this->slices.size(); ++i ) {
          changer = 1;
          switch ( this->slices[i].first ) {
          case 'x':
               changer = -1;
          case 'y':
               if ( this->cuts[i][slices[i].second+1] ==
                         this->cuts[i+1][slices[i].second+1] ) {
                    writhe += changer;
               } else {
                    writhe -= changer;
               };
          };
     };
     return writhe;
};

Tangle Sakuma ( const std::vector<int> &list1,const std::vector<int> &list2 )
{

     std::string tanglestring {"s.l1"};
     int temp {0};
     for ( size_t i = 0; i<list1.size(); ++i ) {
          std::string add {".y0"};
          temp = list1[i];
          if ( temp < 0 ) {
               add = ".x0";
               temp = -temp;
          };
          for ( int it = 0; it<temp; ++it ) {
               tanglestring += add;
          };
          add = ".x1";
          temp = list2[i];
          if ( temp < 0 ) {
               add = ".y1";
               temp = -temp;
          };
          for ( int it = 0; it<temp; ++it ) {
               tanglestring += add;
          };
     };
     tanglestring += ".u2";
     Tangle T0 = Tangle ( tanglestring, {1} );
     // fix orientation: it should always point upwards
     if ( T0.top_orient() [1] == false ) {
          tanglestring[2] = 'r';
          T0 = Tangle ( tanglestring, {1} );
     };
     return T0;
};

Tangle Sakuma ( const std::vector<int> &list )
{
     std::string tanglestring {"s.l1"};
     int temp {0};
     int counter {1};
     for ( const auto &e : list ) {
          if ( counter % 2 != 0 ) {
               tanglestring += ".y0.x2";
          } else {
               tanglestring += ".x0.y2";
          };
          std::string add {".x1"};
          temp = e;
          if ( temp < 0 ) {
               if ( counter % 2 == 0 ) {
                    add = ".y1";
               };
               temp = -temp;
          } else if ( counter % 2 != 0 ) {
               add = ".y1";
          };
          for ( int it = 0; it<temp; ++it ) {
               tanglestring += add;
          };
          ++counter;
     };
     tanglestring += ".u2";
     Tangle T0 = Tangle ( tanglestring, {1} );
     // fix orientation: it should always point upwards
     if ( T0.top_orient() [1] == false ) {
          tanglestring[2] = 'r';
          T0 = Tangle ( tanglestring, {1} );
     };
     return T0;
};

//                                             //
// non-member functions that relate to Tangles //
//                                             //
void rational_quotient ( const int &p,
                         const int &q,
                         const std::string &name,
                         const std::string &metadata,
                         std::vector<File> &files )
{
     /// \todo add safety checks for input
     File file {"examples/strong_inversions/2-bridge_knots"};
     file.create_directories();
     file /= name;
     std::vector<int> continued_frac {continued_fraction ( p,q ) };
     std::string message {"2-bridge knot corresponding to the fraction " + std::to_string ( p ) + "/" + std::to_string ( q ) + " = " + stringL ( continued_frac ) };
     std::vector<int> temp1 {};
     std::vector<int> temp2 {};
     // Symmetry 1
     int counter = 1;
     for ( const auto &e : continued_frac ) {
          if ( counter % 2 != 0 ) {
               temp1.push_back ( e );
          } else {
               temp2.push_back ( e/2 );
          };
          ++counter;
     };
     Tangle T0 = Sakuma ( temp1,temp2 );
     if ( ( q-1 ) % p == 0 || ( q+1 ) % p == 0 ) {
          T0.to_file ( file );
          files.push_back ( file );
          conditional_append ( file.fullname()+".kht",
                               "% Symmetry of a "+message+" (Sakuma, Appendix I)." );
     } else {
          T0.to_file ( file+"-1" );
          files.push_back ( file+"-1" );
          conditional_append ( file.fullname()+"-1.kht",
                               "% Symmetry 1 of a "+message+" (Sakuma, Prop. 3.6, symmetry 1)." );
          if ( ( q*q-1 ) % p != 0 ) {
               // Sakuma, Prop. 3.6, Case (1), symmetry 2
               counter = 1;
               temp1.clear();
               temp2.clear();
               for ( const auto &e : continued_frac ) {
                    if ( counter % 2 != 0 ) {
                         temp2.push_back ( -e/2 );
                    } else {
                         temp1.push_back ( -e );
                    };
                    ++counter;
               };
               std::reverse ( temp1.begin(),temp1.end() );
               std::reverse ( temp2.begin(),temp2.end() );
               T0 = Sakuma ( temp1,temp2 );
               T0.to_file ( file+"-2" );
               conditional_append ( file.fullname()+"-2.kht","% Symmetry 2 of a "+message+" (Sakuma, Prop. 3.6, Case (1), symmetry 2)." );
               files.push_back ( file+"-2" );
          } else {
               // Sakuma, Prop. 3.6, Case (2), symmetry 2
               temp1 = continued_frac;
               temp1.resize ( temp1.size() /2 );
               T0 = Sakuma ( temp1 );
               T0.to_file ( file+"-2" );
               conditional_append ( file.fullname()+"-2.kht","% Symmetry 2 of a "+message+" (Sakuma, Prop. 3.6, Case (2), symmetry 2)." );
               files.push_back ( file+"-2" );
          };
     };
};

File interactive_file ( const std::string &defaultpath )
{
     File file {};
     while ( true ) {
          if ( defaultpath.empty() ) {
               while ( true ) {
                    std::cout << "Please specify a path for this project: \n"
                              << "(default 'examples" + file_sep + "tests') \n";
                    std::string fpath {};
                    std::getline ( std::cin, fpath );
                    file = File ( fpath );
                    if ( file.is_valid() ) {
                         if ( file.is_empty() ) {
                              file /= ( "examples" + file_sep + "tests" );
                         };
                         break;
                    };
               };
          } else {
               file = File ( defaultpath );
          };
          while ( true ) {
               std::string fname {};
               std::cout << "Please specify a name for this project: \n"
                         << "(default 'test'; only use alphanumeric characters, '-', and '_')\n";
               std::getline ( std::cin, fname );
               if ( my_alpha_num ( fname ) ) {
                    if ( fname.empty() ) {
                         fname = "test";
                    };
                    file/= fname ;
                    break;
               };
          };
          std::ifstream input_test ( ( file.fullname() +".kht" ).c_str() );
          if ( input_test.good() ) {
               std::cout << "There already exists a file of the name " << file.fullname() << ".kht. ";
               if ( yes_no_dialogue ( "Do you want to overwrite?" ) ) {
                    break;
               };
          } else {
               break;
          };
     };
     return file;
};

void interactive ( const std::string &metadata,std::vector<File> &files )
{
     /// \todo make this function cope with symmetric tangles and behave more gracefully in edge cases; in particular, remove the necessity of \ref default_number_of_lines. 
     bool aborted = false;
     File file {interactive_file() };
     file.create_directories();
     // Interface:
     std::string svg_output {file.fullname() + "/" + file.name() };
     std::string tangle_string="";
     std::string tangle_string_test="";
     std::string slice="";
     bool first_it=1;
     std::cout << "Specify the number of tangle ends at the top:\n";
     int ends;
     std::vector<bool> top_orient;
     std::cin >> ends;
     while ( std::cin.fail() || ends < 1) {
          std::cin.clear();
          std::cin.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );
          std::cout << "Input error: Please enter a positive integer.\n";
          std::cin >> ends;
     };
     std::cout << "Specify the orientation of each strand at the top: (1=↑,0=↓)\n";
     for ( int i=0; i!=ends; ++i ) {
          bool orient;
          std::cout << i << ": ";
          std::cin >> orient;
          if ( std::cin.fail() ) {
               std::cin.clear();
               std::cin.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );
               std::cout << "Input error: The orientation should be 1=↑ or 0=↓.\n";
               --i;
          } else {
               top_orient.push_back ( orient );
          };
     };
     std::cout << "===tangle slices===\n";
     int number_of_lines {default_number_of_lines-1};
     int number_of_lines_extra {0};
     while ( slice.compare ( "." ) !=0 ) {
          std::cout << "Define a tangle slice: (type h+ENTER for help)\n";
          std::cin >> slice;
          // erase previous picture
          std::cout << "\33[2K\r";
          for ( int i=0; i<number_of_lines+number_of_lines_extra; ++i ) {
               std::cout << "\033[F\33[2K";
          };
          number_of_lines_extra=0;
          //
          if ( slice.compare ( "." ) ==0 ) {
               Tangle T0=Tangle ( tangle_string,top_orient );
               // update svg file and output:
               T0.print ( 1 );
               T0.to_svg ( metadata,svg_output,0 );
               break;
          } else if ( slice.compare ( "k" ) ==0 ) {
               size_t new_size=tangle_string.find_last_of ( "." );
               if ( new_size==std::string::npos ) {
                    new_size=0;
                    first_it=1;
               };
               tangle_string.resize ( new_size );
               Tangle T0=Tangle ( tangle_string,top_orient );
               // update svg file and output:
               T0.print ( 1 );
               T0.to_svg ( metadata,svg_output,0 );
               if ( number_of_lines>default_number_of_lines ) {
                    --number_of_lines;
               };
          } else if ( slice.compare ( "q" ) ==0 ) {
               std::cout << "Quitting interactive mode, computing trivial tangle\n";
               Tangle ( "", {1} ).to_svg ( metadata,svg_output,0 );
               aborted = true;
               break;
          } else if ( slice.compare ( "h" ) ==0 ) {
               // repeat svg file and output:
               Tangle T0 = Tangle ( tangle_string,top_orient );
               T0.print ( 1 );
               T0.to_svg ( metadata,svg_output,0 );
               std::cout << "Type ai, where a=x,y,l,r,u and i is an index.\n"
                         << "Type .+ENTER to finish, k+ENTER to undo the last step, q+ENTER to quit.\n";
               number_of_lines_extra = 2;
          } else {
               try {
                    tangle_string_test = tangle_string;
                    tangle_string_test+= ( first_it ) ? slice : ( "."+slice );
                    Tangle T0=Tangle ( tangle_string_test,top_orient );
                    // if tangle is well-defined, update tanglestring permanently:
                    tangle_string=tangle_string_test;
                    first_it=0;
                    // update svg file and output:
                    T0.print ( 1 );
                    T0.to_svg ( metadata,svg_output,0 );
                    for ( auto &c : ( "."+slice ) ) {
                         if ( c == '.' ) {
                              ++number_of_lines;
                         };
                    };
               } catch ( std::exception &e ) {
                    // repeat svg file and output:
                    Tangle T0=Tangle ( tangle_string,top_orient );
                    T0.print ( 1 );
                    T0.to_svg ( metadata,svg_output,0 );
                    //
                    std::cout << e.what();
                    std::cout << "Try again!\n";
                    number_of_lines_extra = 3;
               };
          };
     };
     Tangle T0 { tangle_string,top_orient };
     if ( aborted ) {
          T0 = Tangle ( "", {1} );
     };
     T0.to_file ( file );
     files.push_back ( file );
};


std::vector<Slice> to_slices (
     std::string tanglestring,
     int cutlength )
{
     std::vector<Slice> slices {};
     std::vector<std::string> pre_slices;
     std::string error_msg;
     // slice the tanglestring along "." into 'pre_slices':
     if ( tanglestring.empty() == false ) {
          tanglestring += ".";
          // so the last pre_slice gets added
     };
     {
          std::string temp;
          for ( char const &str : tanglestring ) {
               if ( str=='.' ) {
                    pre_slices.push_back ( temp );
                    temp="";
               } else {
                    temp.push_back ( str );
               };
          };
     }
     // Turn preslices into actual slices:
     Slice temp;
     slices.reserve ( pre_slices.size() );
     for ( std::string &pre_slice : pre_slices ) {
          //
          // test if first letter of slice is legit:
          if ( pre_slice.size() !=0 ) {
               if ( pre_slice.front() !='x'&&
                         pre_slice.front() !='y'&&
                         pre_slice.front() !='l'&&
                         pre_slice.front() !='r'&&
                         pre_slice.front() !='u'
                  ) {
                    error_msg="tangle input wrong: the first letter of a string representing a slice should be 'x', 'y', 'l', 'r' or 'u', but it is ";
                    error_msg+=pre_slice.front();
                    error_msg+=".\n";
                    throw std::invalid_argument ( error_msg );
               };
          } else {
               error_msg="tangle input wrong: the first letter of a string representing a slice should be 'x', 'y', 'l', 'r' or 'u', but it is empty.\n" ;
               throw std::invalid_argument ( error_msg );
          };
          //
          // set slice:
          temp.first=pre_slice.front();
          pre_slice.erase ( pre_slice.begin() );
          try {
               temp.second=std::stoi ( pre_slice );
          } catch ( ... ) {
               error_msg="tangle input wrong: the second part of a string representing a slice should be an integer, but it is ";
               error_msg+=pre_slice;
               error_msg+=".\n";
               throw std::invalid_argument ( error_msg );
          };
          //
          // test if second component of slice is legit:
          switch ( temp.first ) {
          case 'l':
          case 'r':
               if ( temp.second>cutlength ) {
                    error_msg="tangle input wrong: the second part of a string representing a slice should be an integer in [0,current cutlength]=[0,";
                    error_msg+= std::to_string ( cutlength );
                    error_msg+="], but it is ";
                    error_msg+= std::to_string ( temp.second );
                    error_msg+=".\n";
                    throw std::invalid_argument ( error_msg );
               };
               // adapt cutlength:
               cutlength+=2;
               break;
          case 'u':
               if ( temp.second>cutlength-2 ) {
                    error_msg="tangle input wrong: the second part of a string representing a slice should be an integer in [0,current cutlength-2]=[0,";
                    error_msg+= std::to_string ( cutlength-2 );
                    error_msg+="], but it is ";
                    error_msg+= std::to_string ( temp.second );
                    error_msg+=".\n";
                    throw std::invalid_argument ( error_msg );
               };
               // adapt cutlength:
               cutlength-=2;
               break;
          case 'x':
          case 'y':
               if ( temp.second>cutlength-2 ) {
                    error_msg="tangle input wrong: the second part of a string representing a slice should be an integer in [0,current cutlength-2]=[0,";
                    error_msg+= std::to_string ( cutlength-2 );
                    error_msg+="], but it is ";
                    error_msg+= std::to_string ( temp.second );
                    error_msg+=".\n";
                    throw std::invalid_argument ( error_msg );
               };
               break;
          };
          //
          slices.push_back ( temp );
     };
     return slices;
};

std::string slices_to_string ( std::vector<Slice> slices )
{
     std::string tanglestring {};
     if ( slices.empty() ){
          return "";
     };
     tanglestring += slices.front().first;
     tanglestring += std::to_string ( slices.front().second );
     for ( size_t i=1; i<slices.size(); ++i ) {
          tanglestring += ".";
          tanglestring += slices[i].first;
          tanglestring += std::to_string ( slices[i].second );
     };
     return tanglestring;
};

std::string simplify ( std::string tanglestring,
                       int cutlength )
{
     std::vector<Slice> slices {to_slices ( tanglestring,cutlength ) };
     size_t counter {0};
//      Tangle T0;
     std::string old_string;
     while ( counter < wiggle_abort_after ) {
          ++counter;
          if ( counter%100   == 0 ) {
               std::cout << "\33[2K" << to_percentage ( counter,wiggle_abort_after ) << " wiggling\r" << std::flush;
          };
          for ( size_t iter = 0; iter < wiggle_number; ++iter ) {
               wiggle_move ( slices );
               //std::cout << slices_to_string ( slices ) << "\n";
//                T0=Tangle ( slices_to_string ( slices ), {1,1} );
//                if( T0.bot_orient()[0]==0 || T0.bot_orient()[1]==0){
//                     std::cout << "old: " << old_string << "\n"
// //                               << "new: " << slices_to_string ( slices ) << "\n";
//                };
//                old_string = slices_to_string ( slices );
          };
          while ( simplification_move ( slices ) ) {
               counter = 0;// reset
               std::cout << slices_to_string ( slices ) << "\n" << std::flush;
               Tangle ( slices_to_string ( slices ), {1,1} ).print();
//                T0=Tangle ( slices_to_string ( slices ), {1,1} );
//                if( T0.bot_orient()[0]==0 || T0.bot_orient()[1]==0){
//                     std::cout << "old: " << old_string << "\n"
//                               << "new: " << slices_to_string ( slices );
//                };
//                old_string = slices_to_string ( slices );
          };
          // to be effective for complicated examples, need to add some move that looks for long strands that remain below or above others for a long time.
     };
     std::cout << "\33[2KSimplified tangle to "
               << slices_to_string ( slices ) << ".\n";
     return slices_to_string ( slices );
};

bool simplification_move ( std::vector<Slice> &slices )
{
     bool simplified {false};
     for ( size_t iter = 0; iter < slices.size(); ++iter ) {
          switch ( slices[iter].first ) {
          case 'x':
               if ( iter+1 < slices.size() ) {
                    switch ( slices[iter+1].first ) {
                    case 'y':
                         if ( slices[iter].second == slices[iter+1].second ) {
                              simplified = true;
                              // simplify: (x<i>.y<i>: Reidemeister-II)
                              slices.erase ( slices.begin() + iter, slices.begin() + iter + 2 );
                         };
                         break;
                    case 'u':
                         if ( slices[iter].second == slices[iter+1].second ) {
                              simplified = true;
                              // simplify: (x<i>.u<i>: Reidemeister-I)
                              slices.erase ( slices.begin() + iter, slices.begin() + iter + 1 );
                         };
                         break;
                    };
               };
               break;
          case 'y':
               if ( iter+1 < slices.size() ) {
                    switch ( slices[iter+1].first ) {
                    case 'x':
                         if ( slices[iter].second == slices[iter+1].second ) {
                              simplified = true;
                              // simplify: (y<i>.x<i>: Reidemeister-II)
                              slices.erase ( slices.begin()+iter, slices.begin()+iter+2 );
                         };
                         break;
                    case 'u':
                         if ( slices[iter].second == slices[iter+1].second ) {
                              simplified = true;
                              // simplify: (y<i>.u<i>: Reidemeister-I)
                              slices.erase ( slices.begin()+iter, slices.begin()+iter+1 );
                         };
                         break;
                    };
               };
               break;
          case 'l':
          case 'r':
               if ( iter+1 < slices.size() ) {
                    switch ( slices[iter+1].first ) {
                    case 'x':
                    case 'y':
                         if ( slices[iter].second == slices[iter+1].second ) {
                              simplified = true;
                              // simplify: (cap<i>.crossing<i>: Reidemeister-I)
                              slices.erase ( slices.begin()+iter+1, slices.begin()+iter+2 );
                              if ( slices[iter].first == 'r' ) {
                                   slices[iter].first = 'l';
                              } else {
                                   slices[iter].first = 'r';
                              };
                         };
                         break;
                    case 'u':
                         if ( ( slices[iter].second+1 == slices[iter+1].second ) || ( slices[iter].second == slices[iter+1].second+1 ) ) {
                              simplified = true;
                              // simplify: (cap<i>.u<i+-1> elimination)
                              slices.erase ( slices.begin() + iter, slices.begin() + iter + 2 );
                         };
                         break;
                    };
               };
               break;
          };
          if ( simplified ) {
               std::cout << "simplified tangle!\n";
               break;
          };
     };
     return simplified;
};

void wiggle_move ( std::vector<Slice> &slices )
{
     std::random_device random_device;
     std::mt19937 engine { random_device() };
     const int N { int ( slices.size() ) - 1 };
     std::uniform_int_distribution<int> level_dist ( 0, N );
     std::uniform_int_distribution<int> bool_dist ( 0,1 );
     int level {level_dist ( engine ) };
     bool push_up {bool ( bool_dist ( engine ) ) };
     bool changed {true};
//      std::cout << " level = " << level << "\n";
     // push one step at a time and as far as possible past caps and cups
     if ( push_up ) {
          // push up, but only if it is neither l nor r
          if ( slices[level].first!='l' && slices[level].first!='r' ) {
               while ( level != 0 && changed ) {
                    changed = false;
                    if ( ( ( slices[level-1].first == 'u' )
                              && ( slices[level-1].second != slices[level].second + 1 ) )
                              || ( slices[level-1].second > slices[level].second + 1 )
                              || ( slices[level-1].second + 1 < slices[level].second ) ) {
                         flip_slices ( slices,level );
                         --level;
                         changed = true;
//                          std::cout << "changed\n";
                    };
               };
          };
     } else {
          // push down, but only if it is not a cup
          if ( slices[level].first != 'u' ) {
               while ( level != N && changed ) {
                    changed = false;
                    if ( ( ( ( slices[level+1].first == 'l' )
                              || ( slices[level+1].first == 'r' ) )
                              && ( slices[level+1].second != slices[level].second + 1 ) )
                              || ( slices[level+1].second > slices[level].second + 1 )
                              || ( slices[level+1].second + 1 < slices[level].second ) ) {
                         flip_slices ( slices,level+1 );
                         ++level;
                         changed = true;
//                          std::cout << "changed\n";
                    };
               };
          };
     };
//      std::cout << "push_move: " << slices_to_string ( slices ) << "\n";
// do wiggle moves (cap.crossing or crossing.cup)
     char xy_opp {'x'}; // only needed for wiggle move
     int l1 {-1};
     int l2 {-1};
     int l3 {-1};
     if ( push_up ) {
          l1 = level - 1;
          l2 = level;
//           std::cout << "push up: level=" << level<< "\n";
     } else if ( level < N ) {
          l1 = level;
          l2 = level + 1;
//           std::cout << "push down: level=" << level<< "\n";
     };
     // now try to perform some wiggle move above or below
     if ( l2 > 0 ) {
          switch ( slices[l2].first ) {
          case 'x':
               xy_opp = 'y';
          case 'y':
               switch ( slices[l1].first ) {
               case 'l':
               case 'r':
                    if ( slices[l1].second + 1 == slices[l2].second ) {
                         // cap<i>.x<i+1> -> cap<i+1>.y<i>
                         slices[l1].second += 1;
                         slices[l2].first = xy_opp;
                         slices[l2].second -= 1;
//                          std::cout << "wiggle move a at " << level << ": " << slices_to_string ( slices ) << "\n";
                    } else if ( slices[l1].second == slices[l2].second + 1 ) {
                         // cap<i+1>.x<i> -> cap<i>.y<i+1>
                         slices[l1].second -= 1;
                         slices[l2].first = xy_opp;
                         slices[l2].second += 1;
//                          std::cout << "wiggle move b at " << level << ": " << slices_to_string ( slices ) << "\n";
                    };
                    break;
               };
               break;
          case 'u':
               switch ( slices[l1].first ) {
               case 'x':
                    xy_opp = 'y';
               case 'y':
                    if ( slices[l1].second + 1 == slices[l2].second ) {
                         // x<i-1>.cup<i> -> y<i>.cup<i-1>
                         slices[l1].second += 1;
                         slices[l1].first = xy_opp;
                         slices[l2].second -= 1;
//                          std::cout << "wiggle move c at " << level << ": " << slices_to_string ( slices ) << "\n";
                    } else if ( slices[l1].second == slices[l2].second + 1 ) {
                         // x<i+1>.cup<i> -> y<i>.cup<i+1>
                         slices[l1].second -= 1;
                         slices[l1].first = xy_opp;
                         slices[l2].second += 1;
//                          std::cout << "wiggle move d at " << level << ": " << slices_to_string ( slices ) << "\n";
                    };
                    break;
               };
               break;
          };
     };
// next try Reidemeister III
     if ( push_up ) {
          --l1;
          --l2;
          l3 = level;
     } else if ( level < N-1 ) {
          l3 = level + 2;
     };
     if ( l3 > 2 ) {
          if ( ( slices[l1].first == 'x' || slices[l1].first == 'y' )
                    && ( slices[l2].first == 'x' || slices[l2].first == 'y' )
                    && ( slices[l3].first == 'x' || slices[l3].first == 'y' )
             ) {
               if ( slices[l3].second == slices[l1].second ) {
                    if ( slices[l2].second == slices[l3].second + 1 ) {
                         //   X  |
                         //  |  X
                         //   X  |
                         if ( slices[l3].first == slices[l2].first ) {
                              // move first slice
                              flip_slices ( slices,l2 );
                              flip_slices ( slices,l3 );
                              slices[l3].second += 1;
//                               std::cout << "RMIIIa at " << level << ": " << slices_to_string ( slices ) << "\n";
                         } else if ( slices[l1].first == slices[l2].first ) {
                              // move third slice
                              flip_slices ( slices,l3 );
                              flip_slices ( slices,l2 );
                              slices[l1].second += 1;
//                               std::cout << "RMIIIb at " << level << ": " << slices_to_string ( slices ) << "\n";
                         };
                    } else if ( slices[l2].second == slices[l3].second - 1 ) {
                         //  |  X
                         //   X  |
                         //  |  X
                         if ( slices[l3].first == slices[l2].first ) {
                              // move first slice
                              flip_slices ( slices,l2 );
                              flip_slices ( slices,l3 );
                              slices[l3].second -= 1;
//                               std::cout << "RMIIIc at " << level << ": " << slices_to_string ( slices ) << "\n";
                         } else if ( slices[l1].first == slices[l2].first ) {
                              // move third slice
                              flip_slices ( slices,l3 );
                              flip_slices ( slices,l2 );
                              slices[l1].second -= 1;
//                               std::cout << "RMIIId at " << level << ": " << slices_to_string ( slices ) << "\n";
                         };
                    };
               };
          };
     };
};

void flip_slices ( std::vector<Slice> &slices, const int &level )
{
     // adjust indices of slices
     const int l1 {level - 1};
     const int l2 {level};
     int change1 {0};
     int change2 {0};
     if ( ( slices[l1].first == 'u' )
               && ( slices[l1].second < slices[l2].second + 1 ) ) {
          change2 = 2;
     } else if ( ( slices[l1].first == 'l' || slices[l1].first == 'r' )
                 && ( slices[l1].second + 1 < slices[l2].second ) ) {
          change2 = -2;
     };
     if ( ( slices[l2].first == 'u' )
               && ( slices[l1].second > slices[l2].second + 1 ) ) {
          change1 = -2;
     } else if ( ( slices[l2].first == 'l' || slices[l2].first == 'r' )
                 && ( slices[l1].second + 1 > slices[l2].second ) ) {
          change1 = 2;
     };
     slices[l1].second += change1;
     slices[l2].second += change2;
     std::swap ( slices[level - 1], slices[level] );
};



// template specialization

#define VALUE int
#include "../specialization/Tangles.h"
#define VALUE Z_mod<default_coeff>
#include "../specialization/Tangles.h"
#define VALUE Z_mod<3>
#include "../specialization/Tangles.h"
#define VALUE Z_mod<5>
#include "../specialization/Tangles.h"
#define VALUE Z_mod<7>
#include "../specialization/Tangles.h"
#define VALUE Z_mod<11>
#include "../specialization/Tangles.h"
#define VALUE Z_mod<integer_simulant>
#include "../specialization/Tangles.h"
#define VALUE Q
#include "../specialization/Tangles.h"

#endif
