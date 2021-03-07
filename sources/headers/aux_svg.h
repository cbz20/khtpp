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



#ifndef AUX_SVG_H
#define AUX_SVG_H
#include <string>
#include <vector>


/// \file aux_svg.h
/// \brief declares drawing commands to generate svg-files

constexpr int stringwidth {8};///< width of a tangle string
constexpr int stringwhite {24};///< width of the break of the understrand in a crossing



typedef int Coord;///< single coordinate 
typedef int Length;///< length 
typedef int Degrees;///< angle in degrees
// typedef std::vector<Length> Lengths;///< list o lengths
typedef std::pair<Coord,Coord> Coords;///< coordinates of a point in the plane



Coords size_t2Coords ( std::pair<size_t,size_t> &input );///< convert coordinate of type \c size_t to type \ref Coord. 

std::string svg_bezier (
    const Coords &x,
    const Coords &y,
    const Coords &z,
    const Coords &w,
    const std::string &colour="black",
    const Length &width=stringwidth ,
    const std::string &options = "" );///< svg bezier curve

std::string svg_line (
    const Coords &x,
    const Coords &y,
    const std::string &colour="black",
    const Length &width=stringwidth );///< svg line

std::string svg_text (
    const Coords &x,
    const std::string &text,
    const std::string &colour="black",
    const Length &pixel=10 ); ///< svg text

std::string svg_circle (
    const Coords &x,
    const Length &r,
    const std::string &linecolour="black",
    const std::string &fillcolour="none",
    const Length &width=stringwidth );///< svg circle

std::string svg_arrowhead (
    const Coords &x,
    const Degrees &angle,
    const std::string &colour="black",
    const Length &width=stringwidth );///< svg arrowhead

// std::string svg_small_arrowhead (
//     const Coords &x,
//     const Degrees &angle,
//     const std::string &colour="black",
//     const Length &width=stringwidth );///< svg arrowhead

std::string svg_mask( 
    const std::string &inner, 
    const std::string &mask_id);///< svg mask for under-strands of crossings
#endif
