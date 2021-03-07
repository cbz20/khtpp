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



#ifndef AUX_SVG_CPP
#define AUX_SVG_CPP
#ifndef AUX_SVG_H
#include "../headers/aux_svg.h"
#endif



/// \file aux_svg.cpp
/// \brief implements drawing commands to generate svg-files

Coords size_t2Coords ( std::pair<size_t,size_t> &input )
{
     return Coords (
                 static_cast<int> ( input.first ),
                 static_cast<int> ( input.second ) );
};

std::string svg_bezier (
     const Coords &x,
     const Coords &y,
     const Coords &z,
     const Coords &w,
     const std::string &colour,
     const Length &width,
     const std::string &options )
{
     std::string output {""};
     output += "<path d = 'M ";
     output += std::to_string ( x.first ) + " ";
     output += std::to_string ( x.second ) + " ";
     output += "C ";
     output += std::to_string ( y.first ) + " ";
     output += std::to_string ( y.second ) + ", ";
     output += std::to_string ( z.first ) + " ";
     output += std::to_string ( z.second ) + " ";
     output += std::to_string ( w.first ) + ", ";
     output += std::to_string ( w.second ) + "' ";
     output += "stroke='"+colour+"' fill='none' ";
     output += "stroke-width='"+std::to_string ( width ) +"' ";
     if ( width == stringwidth ) {
          // do not use round caps for white covers in crossings
          output += "stroke-linecap='round' ";
     };
     output += options;
     output += "/>\n";
     return output;
}

std::string svg_line (
     const Coords &x,
     const Coords &y,
     const std::string &colour,
     const Length &width )
{
     return "<line x1='"
            + std::to_string ( x.first ) + "' "
            + "y1='" + std::to_string ( x.second ) + "' "
            + "x2='" + std::to_string ( y.first ) + "' "
            + "y2='" + std::to_string ( y.second ) + "' "
            + "stroke='"+colour+"' fill='none' "
            + "stroke-linecap='round' "
            + "stroke-width='"+std::to_string ( width )+"'/>\n";
}

std::string svg_text (
     const Coords &x,
     const std::string &text,
     const std::string &colour,
     const Length &pixel )
{
     return "<text x='"
            + std::to_string ( x.first ) + "' "
            + "y='" + std::to_string ( x.second ) + "' "
            + "text-anchor='middle' "
            + "font-family='Arial' "
            //    + "font-family='sans-serif' "
            + "font-size='" + std::to_string ( pixel ) + "px' "
            + "fill='" + colour + "'>"+text+"</text>";
}

std::string svg_circle (
     const Coords &x,
     const Length &r,
     const std::string &linecolour,
     const std::string &fillcolour,
     const Length &width )
{
     return "<circle cx='"
            + std::to_string ( x.first ) + "' "
            + "cy='" + std::to_string ( x.second ) + "' "
            + "r='" + std::to_string ( r ) + "' "
            + "stroke='"+linecolour+"' fill='"+fillcolour+"' "
            + "stroke-width='"+std::to_string ( width )+"'/>\n";
};

std::string svg_arrowhead (
     const Coords &x,
     const Degrees &angle,
     const std::string &colour,
     const Length &width )
{
     return "<path d = '"
            "M 0 -5 c 5 7, 10 10, 20 15 "
            "M 0 -5 c -5 7, -10 10, -20 15' "
            "stroke='"+colour+"' "
            "fill='none' "
            "stroke-width='"+std::to_string ( width )+"' "
            "stroke-linecap='round' "
            "transform='translate("
            + std::to_string ( x.first ) + " "
            + std::to_string ( x.second ) + ") rotate("
            + std::to_string ( angle ) + " 0 0) '/>\n";
}

// std::string svg_small_arrowhead (
//      const Coords &x,
//      const Degrees &angle,
//      const std::string &colour,
//      const Length &width )
// {
//      return "<path d = '"
//             "M 0 -1 c 1 1.4, 2 2, 4 3 "
//             "M 0 -1 c -1 1.4, -2 2, -4 3' "
//             "stroke='"+colour+"' "
//             "fill='none' "
//             "stroke-width='"+std::to_string ( width )+"' "
//             "stroke-linecap='round' "
//             "transform='translate("
//             + std::to_string ( x.first ) + " "
//             + std::to_string ( x.second ) + ") rotate("
//             + std::to_string ( angle ) + " 0 0) '/>\n";
// }


std::string svg_mask (
     const std::string &inner,
     const std::string &mask_id )
{
     return "<mask id='mask-" 
          + mask_id
          + "'>\n"
          + "<rect height='100%' width='100%' fill='white'/>\n"
          + inner
          + "</mask>\n";
};
#endif
