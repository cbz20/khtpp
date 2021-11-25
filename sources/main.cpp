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



#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <string>
#include <getopt.h>
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include "headers/aux_sys.h"
#include "headers/Complex.h"
#include "headers/Tangles.h"
#include "headers/constants.h"
#include "headers/Tests.h"
//#include "headers/Curves.h"


/// \file main.cpp
/// \brief processes arguments and runs main computation
///
/// \todo clean-up code basis following guidelines https://github.com/isocpp/CppCoreGuidelines

/// main function processing arguments 

/// Running \code{.sh} ./khtpp --help \endcode displays the following help file
/// \verbinclude Help.md

int main ( int argc, char **argv )
{
     //
     // define meta data
     //
     std::time_t timestamp_t = std::time ( nullptr );
     std::stringstream timestamp_ss;
     timestamp_ss << std::put_time ( std::gmtime ( &timestamp_t ), "%c %Z" );
     const std::string timestamp {timestamp_ss.str() };
     const std::string metadata {
          "generated by kht++ " +
          std::string ( VERSION ) +
          " on " +
          timestamp
     };
     //
     // Options Interpreter
     //
     bool a_flag {false};// allcoeff flag
     bool b_flag {false};// braid input
     std::string b_opt_name; // braid name
     std::vector<int> b_opt_braid; // braid input (-=y,+=x)
     std::vector<bool> b_opt_top_orient; // number of strands at the top
     std::string b_opt_comment; // comment
     bool d_flag {false};// double up
     bool h_flag {false};// help flag
     bool o_flag {false};// optimize flag
     bool prefix_flag {false};// prepend flag
     bool q_flag {false};// take quotient (of a strongly invertible knot)
     bool r_flag {false};// rationalquotient
     bool s_flag {false};// simplify flag
     static int sum_flag {false};// tangle sum flag
     static int sum_non_compact_flag {false};// tangle sum flag for ignoring compact input curves
     std::string summand1 {};// relative file name for first summand
     std::string summand2 {};// relative file name for second summand
     bool t_flag {false};// test flag
     bool v_flag {false};// version flag
     bool w_flag {false};// web/html flag
     bool w_opened {false};// set true if website was opened
     std::vector<int> coeffs_all {2,3,5,7,custom_coeff};
     std::string p_opt_prefix; // prefix to append to tangle string for prefix_flag
     Cut p_opt_new_top;
     std::vector<std::string> p_opt_suffix {}; // list of suffixes to append to tangle string
     int r_opt_p; // rational quotient numerator
     int r_opt_q; // rational quotient denominator
     std::string r_opt_n; // rational quotient name of knot
     int c_opt;// coefficient option
     std::vector<int> coeffs {};// coefficients argument list
     int optresult;
     opterr = 0;
     //
     while ( true ) {
          // There are three types of flags:
          //    1) Exceptional = causes the program to ignore any further arguments
          //    2) Input = affects the input (before the main calculation)
          //    3) Output = affects the output (after the main calculation)

          static struct option long_options[] = {
               {"allcoeff",  no_argument, 0, 'a'},      // Input
               {"braid",     required_argument, 0, 'b'},// Input
               {"coeff",     required_argument, 0, 'c'},// Input
               {"double",    no_argument, 0, 'd'},      // Input
               {"help",      no_argument, 0, 'h'},      // Exceptional
               {"optimize",  no_argument, 0, 'o'},      // Output
               {"prepend",   required_argument, 0, 'P'},// Input
               {"append",    required_argument, 0, 'p'},// Input
               {"quotient",  no_argument, 0, 'q'},      // Input
               {"rational",  required_argument, 0, 'r'},// Input
               {"simplify",  no_argument, 0, 's'},      // Input
               {"sum",       required_argument, 0,  2 },// Exceptional
               {"sumnc",     required_argument, 0,  1 },// Exceptional
               {"test",      no_argument, 0, 't'},      // Exceptional
               {"version",   no_argument, 0, 'v'},      // Exceptional
               {"web",       no_argument, 0, 'w'},      // Output
               {0, 0, 0, 0}
          };
          int option_index = 0;
          //
          optresult = getopt_long ( argc, argv, "ab:c:dhoP:p:qr:stvw",
                                    long_options, &option_index );
          if ( optresult == -1 ) {
               break;
          };
          //
          switch ( optresult ) {
          case 1: 
               sum_non_compact_flag = true;
          case 2: 
               sum_flag = true;
               try {
                    std::string input = optarg;
                    auto i {input.find_first_of ( ':' ) };
                    summand1 = input.substr ( 0,i );
                    summand2 = input.substr ( i+1 );
               } catch ( ... ) {
                    std::cerr << "INPUT ERROR: The option '--sum'/'--sumnc' needs to be followed by an.\n"
                              << "             expression of the form '<summand1>:<summand2>.\n"
                              << "             Type 'kht++ --help' for more info.\n";
                    return 0;
               };
               break;
          case 'a':
               a_flag = true;
               break;
          case 'b':
               try {
                    b_flag = true;
                    std::string input = optarg;
                    auto i {input.find_first_of ( ',' ) };
                    b_opt_name = input.substr ( 0,i );
                    input = input.substr ( i+1 );
                    i = input.find_first_of ( ']' );
                    auto j {input.find_first_of ( '[' ) }; 
                    if ( i<j ){
                         throw "input error";
                    };
                    auto b_opt_top_orient_str { input.substr ( j+1,i-j-1 ) };
                    input = input.substr ( i+1 );
                    i = input.find_first_of ( ']' );
                    j = input.find_first_of ( '[' );
                    if ( i<j ){
                         throw "input error";
                    };
                    auto b_opt_braid_str { input.substr ( j+1,i-j-1 ) };
                    input = input.substr ( i+1 );
                    i = input.find_first_of ( ',' );
                    b_opt_comment = input.substr ( i+1 );
                    // now deal with the lists
                    bool cont {true};
                    while ( cont ){
                         i = b_opt_top_orient_str.find_first_of ( ',' );
                         int entry {0};
                         if ( i == std::string::npos ){
                              entry = std::stoi(b_opt_top_orient_str);
                              cont = false;
                         } else {
                              entry = std::stoi(b_opt_top_orient_str.substr(0,i));
                              b_opt_top_orient_str = b_opt_top_orient_str.substr ( i+1 );
                         };
                         if ( entry == 0 ){
                              b_opt_top_orient.push_back (0);
                         } else {
                              b_opt_top_orient.push_back (1);
                         };
                    };
                    cont = true;
                    while ( cont ){
                         i = b_opt_braid_str.find_first_of ( ',' );
                         int entry {0};
                         if ( i == std::string::npos ){
                              entry = std::stoi(b_opt_braid_str);
                              cont = false;
                         } else {
                              entry = std::stoi(b_opt_braid_str.substr(0,i));
                              b_opt_braid_str = b_opt_braid_str.substr ( i+1 );
                         };
                         b_opt_braid.push_back (entry);
                    };
               } catch ( ... ) {
                    std::cerr << "INPUT ERROR: The option '-b'/'--braid' needs to be followed by an.\n"
                              << "             expression of the form '<name>,[<b_opt_top_orient>],[<b_opt_braid>],<comment>'.\n"
                              << "             Type 'kht++ --help' for more info.\n";
                    return 0;
               };
               break;
          case 'd':
               d_flag = true;
               break;
          case 'h':
               h_flag = true;
               break;
          case 'o':
               o_flag = true;
               break;
          case 'P':
               if (prefix_flag){
                    std::cerr << "WARNING: The option '-P' can only be used once. \n";
               } else {
                    try {
                         prefix_flag = true;
                         std::string input = optarg;
                         auto i {input.find_first_of( ',' )};                    p_opt_prefix = input.substr(0,i);
                         while ( true ) {
                              input = input.substr(i+1);
                              i = input.find_first_of( ',' );
                              p_opt_new_top.push_back( std::stoi (input.substr(0,i)) );
                              if (  i == std::string::npos ){
                                   break;
                              };
                         };
                    } catch ( ... ) {
                         std::cerr << "INPUT ERROR: The option '-P' needs to be followed by a\n"
                                   << "             valid tangle input string with orientation info.\n"
                                   << "             Type 'kht++ --help' for more info.\n";
                         return 0;
                    };
               };
               break;
          case 'p':
               try {
                    p_opt_suffix.push_back( optarg );
               } catch ( ... ) {
                    std::cerr << "INPUT ERROR: The option '-p' needs to be followed by a\n"
                              << "             valid tangle input string.\n"
                              << "             Type 'kht++ --help' for more info.\n";
                    return 0;
               };
               break;
          case 'q':
               q_flag = true;
               break;
          case 'r':
               try {
                    r_flag = true;
                    std::string input = optarg;
                    auto i {input.find_first_of ( ':' ) };
                    r_opt_n = input.substr ( 0,i );
                    input = input.substr ( i+1 );
                    auto j {input.find_first_of ( '/' ) };
                    r_opt_p = std::stoi ( input.substr ( 0,j ) );
                    r_opt_q = std::stoi ( input.substr ( j+1 ) );
                    if (i == std::string::npos || j == std::string::npos ){
                         throw "input error";
                    };
               } catch ( ... ) {
                    std::cerr << "INPUT ERROR: The option '-r' needs to be followed by an.\n"
                              << "             expression of the form '<name>:<p>/<q>'.\n"
                              << "             Type 'kht++ --help' for more info.\n";
                    return 0;
               };
               break;
          case 's':
               s_flag = true;
               break;
          case 't':
               t_flag = true;
               break;
          case 'v':
               v_flag = true;
               break;
          case 'w':
               w_flag = true;
               break;
          case 'c':
               try {
                    c_opt = std::stoi ( optarg );
                    if ( std::find ( std::begin ( coeffs_all ),
                                     std::end ( coeffs_all ),
                                     c_opt ) == std::end ( coeffs_all ) && c_opt!=0 ) {
                         std::cerr << "INPUT ERROR: The option '-c"
                                   << optarg
                                   << "' is unknown.\n"
                                   << "             Type 'kht++ --help' for more info.\n";
                         return 0;
                    } else {
                         coeffs.push_back ( c_opt );
                    };
               } catch ( ... ) {
                    std::cerr << "INPUT ERROR: The option '-c' needs to be followed by an integer.\n"
                              << "             Type 'kht++ --help' for more info.\n";
                    return 0;
               };
               break;
          default:
               std::cerr << "INPUT ERROR: Encountered unknown option '"
                         << optresult << "'\n"
                         << "             Type 'kht++ --help' for more info.\n";
               return 0;
          };
     };
     

     
     //
     // Process coefficients
     //
     if ( coeffs.empty() ) {
          coeffs.push_back ( default_coeff );
     }
     if ( a_flag ) {
          coeffs = coeffs_all;
     };
     


     //
     // Process exceptional flags
     //
     if ( h_flag ) {
          // help mode
          //            This is kht++, version 0.1, a program for computing Khovanov invariants
          //           "                                                                      "
          std::cout << "This is kht++, version "
                    << std::string ( VERSION )
                    << ", a program for computing Khovanov invariants for links and tangles.\n\n"
                    << file_to_string("sources"+file_sep+"Help.md");
          return 0;
     };
     if ( t_flag ) {
          std::cout << "Tests still need to be implemented.\n";
          // testing mode
          //Test_Coefficients<Z_mod<3>>(0);
//           Test_Cob<Z_mod<2>> ( 0 );
          //Test_Complex<int>(0);
//           Test_Complex<Z_mod<2>> ( 0 );
          //SparseMatrixTestAlgebra();
          //Test_Precurves<int>();
//           Test_PrecomputedAlgebra<Z_mod<2>>();
          Test_ChainConversion<Z_mod<2>>();
//           Test_ChainConversion<Z_mod<5>>();
          return 0;
     };
     if ( v_flag ) {
          std::cout << "kht++ "
                    << std::string ( VERSION )
                    << "\n"
                    << file_to_string("sources"+file_sep+"Copyright.md")
                    << "\n"
                    << file_to_string("sources"+file_sep+"License-short.md");
          return 0;
     };
     if ( sum_flag ){
          switch ( coeffs.front() ) {
               {
               case 2:
#define VALUE Z_mod<2>
#include "headers/tanglesums.h"
               }
               {
               case 3:
#define VALUE Z_mod<3>
#include "headers/tanglesums.h"
               }
               {
               case 5:
#define VALUE Z_mod<5>
#include "headers/tanglesums.h"
               }
               {
               case 7:
#define VALUE Z_mod<7>
#include "headers/tanglesums.h"
               }
               {
               case custom_coeff:
#define VALUE Z_mod<custom_coeff>
#include "headers/tanglesums.h"
               }
               {
               case 0:
#define rationals
#define VALUE Q
#include "headers/tanglesums.h"
#undef rationals
               }
               {
               default:
#define VALUE Z_mod<default_coeff>
#include "headers/tanglesums.h"
               }
          };
          return 0;
     }; 



     //
     // Process input files
     //
     std::vector<File> files;
     while ( optind < argc ) {
          File file { argv[optind++] };
          file.remove_any_extension();
          if ( file.is_valid() && !file.is_empty() ) {
               if ( file.parent().is_empty() ) {
                    std::cerr << "Please put your file in a subdirectory, such as 'examples'.\n";
                    return 1;
               };
               files.push_back ( file );
          };
     };
     // remove duplicates
     std::sort ( files.begin(), files.end() );
     files.erase ( std::unique ( files.begin(), files.end() ), files.end() );
     if ( r_flag ) {
          rational_quotient ( r_opt_p,
                              r_opt_q,
                              r_opt_n,
                              metadata,
                              files );
     } else if ( b_flag ) {
          std::string tanglestring;
          int min {0};
          int max {0};
          for ( auto const &e : b_opt_braid ){
               if (e < min){
                    min = e;
               } else if ( e> max ) {
                    max = e;
               };
          };
          if ( -min > max ){
               max = -min;
          };
          for ( int i = b_opt_top_orient.size(); i < max+1; ++i){
               tanglestring += "r" + std::to_string(i) + ".";
          };
          for ( auto const &e : b_opt_braid ){
               if ( e < 0 ){
                    tanglestring += "y" + std::to_string(-e-1) + ".";
               } else {
                    tanglestring += "x" + std::to_string(e-1) + ".";
               };
          };
          for ( int i = max; i > b_opt_top_orient.size()-1; --i){
               tanglestring += "u" + std::to_string(i) + ".";
          };
          tanglestring.pop_back();
          tanglestring = simplify(tanglestring,b_opt_top_orient.size() );
          Tangle T {tanglestring,b_opt_top_orient,true};
          File file {"examples/braids/" + b_opt_name};
          file.parent().create_directories();
          T.to_file(file);
          conditional_append (file.fullname()+".kht",
                              "% generated from braid " + stringL(b_opt_braid));
          conditional_append (file.fullname()+".kht",
                              "% " + b_opt_comment );
          files.push_back ( file );
          
     } else if ( files.empty() ) {
          interactive ( metadata,files );
     };



     //
     // Main part of program
     //
     for ( const File &file : files ) {
          std::cout << ">>>\n";
          std::cout << ">>> Computation for '" << file.fullname() << "':\n";
          std::cout << ">>>\n";
          Tangle T0 { file };
          T0.record_new_orientations ( file );
          T0.to_svg ( metadata,file.fullname() + "/" + file.name() + "-transparent",false,true );
          T0.to_svg ( metadata,file.fullname() + "/" + file.name() );
          T0.print ();
          if ( T0.is_symmetric() ) {
               if ( q_flag ) {
                    T0 = T0.quotient();
               } else {
                    T0 = T0.complete_symmetry();
               };
          } else if ( q_flag ) {
               std::cout << "I cannot take the quotient. Ignoring '-q'-flag.\n";
          };



          //
          // exceptional flags
          //
          std::string global_options {""}; //options added to filename
          if ( d_flag || o_flag || q_flag || s_flag ) {
               global_options += "-";
          };
          if ( d_flag ) {
               global_options += "d";
          };
          if ( o_flag ) {
               global_options += "o";
          };
          if ( prefix_flag ) {
               global_options += ("P." + p_opt_prefix + ".");
          };
          if ( q_flag ) {
               global_options += "q";
          };
          if ( s_flag ) {
               global_options += "s";
          };



          if ( d_flag ) {
               std::cout << "-d flag set, so doubling up tangle.\n";
               T0.doubled();
               T0.to_svg ( metadata,file.fullname() + "/" + file.name() + "-doubled" );
          };
          if ( prefix_flag ) {
               std::cout << "Adding " << p_opt_prefix << " to the top of the tangle.\n" << std::flush;
               try {
                    T0 = Tangle(p_opt_prefix,p_opt_new_top).add (T0.tanglestring());
               } catch ( ... ) {
                    std::cerr << "INPUT ERROR: The option '-P' needs to be followed by a\n"
                              << "             valid tangle input string with orientation info.\n"
                              << "             Type 'kht++ --help' for more info.\n";
                    return 0;
               };
          };
          
          if (p_opt_suffix.empty()){
               p_opt_suffix.push_back("");
          };
          for ( auto s : p_opt_suffix ) {
               // Test if the tangle strings that we add after a (possibly long computation) are actually valid. 
               try {
                    T0.add (s);
               } catch ( ... ) {
                    std::cerr << "INPUT ERROR: The option '-p' needs to be followed by a\n"
                              << "             valid tangle input string;\n"
                              << "             '"
                              << s
                              << "' is not."
                              << "             Type 'kht++ --help' for more info.\n";
                    return 0;
               };
          };
          if ( s_flag ) {
               T0.simplify_diagram();
               T0.to_svg ( metadata,file.fullname() + "/" + file.name() + "-s" );
               conditional_append (
                    file.fullname() + ".kht",
                    "% Simplifies with options '" +
                    global_options +
                    "' to " +
                    T0.tanglestring() );
          };



          if ( w_flag == false ) {
               //
               // compute invariants
               //
               Complex<CobObj,CobMor,Z_mod<integer_simulant>> cxZ;
               if ( coeffs.size() > 1 ) {
                    cxZ = T0.CobComplex<Z_mod<integer_simulant>>();
                    auto cxxBNr=cxZ.to_BN();
                    // attempt to store file; but this does not work because cxZ is a class object, not a struct object; see
                    // https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
                    // or
                    // https://medium.com/@zekumorudoragonhatto/how-to-work-with-binary-files-in-c-520a852ee04a
                    //
//                     std::fstream outfile;
//                     outfile.open ( file.directory() + "/" + file.name() + "/cxZ.dat", std::ios::binary | std::ios::out | ios::trunc);
//                     outfile.write ( ( char* ) &cxZ, sizeof ( cxZ ) );
//                     outfile.close();
               };
//                std::fstream infile;
//                infile.open ( file.directory() + "/" + file.name() + "/cxZ.dat", std::ios::binary | std::ios::in );
//                infile.read ( ( char * ) & cxZ, sizeof ( Complex<CobObj,CobMor,int> ) );
//                infile.close();
               //
               for ( const int &c_opt : coeffs ) {
                    //
                    // record options
                    //
                    std::string fn_options {""}; //options added to filename
                    fn_options += "-c" + std::to_string ( c_opt ) + global_options;
                    //
                    switch ( c_opt ) {
                         {
                         case 2:
#define VALUE Z_mod<2>
#include "headers/computations.h"
                         }
                         {
                         case 3:
#define VALUE Z_mod<3>
#include "headers/computations.h"
                         }
                         {
                         case 5:
#define VALUE Z_mod<5>
#include "headers/computations.h"
                         }
                         {
                         case 7:
#define VALUE Z_mod<7>
#include "headers/computations.h"
                         }
                         {
                         case custom_coeff:
#define VALUE Z_mod<custom_coeff>
#include "headers/computations.h"
                         }
                         {
                         case 0:
#define rationals
#define VALUE Q
#include "headers/computations.h"
#undef rationals
                         }
                         {
                         default:
#define VALUE Z_mod<default_coeff>
#include "headers/computations.h"
                         }
                    };
               };
          };
          //
          // Compile (and open) html file
          //
          to_html ( file );
          html_summary ( file );
          if ( w_flag && !w_opened ) {
               int exit_status { std::system ( ( "xdg-open " + file.fullname() + ".html" ).c_str() ) };
               if ( exit_status != 0 ) {
                    exit_status = std::system ( ( "open " + file.fullname() + ".html" ).c_str() );
               };
               if ( exit_status != 0 ) {
                    std::cout << "Please install 'xdg-open' (linux) or 'open' (windows/mac).\n"
                              << std::flush;
               };
               w_opened = true;
          };
     };
     return 0;
}

