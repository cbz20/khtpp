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



#ifndef AUX_SYS_CPP
#define AUX_SYS_CPP
#ifndef AUX_SYS_H
#include "../headers/aux_sys.h"
#endif


/// \file aux_sys.cpp
/// \brief implements File class, i/o functions and string operations

File::File ( std::string path )
{
     std::size_t splitPos = path.find_first_of ( std::filesystem::path::preferred_separator );
     while ( splitPos != std::string::npos ) {
          if ( splitPos > 1 ) {
               // make sure the new entry is non-empty
               pathvector.push_back ( path.substr ( 0, splitPos ) );
          };
          path = path.substr ( splitPos+1 );
          splitPos = path.find_first_of ( std::filesystem::path::preferred_separator );
     };
     if ( !path.empty() ) {
          pathvector.push_back ( path );
     };
};

bool File::is_valid() const
{
     for ( const auto &entry : pathvector ) {
          if ( !my_alpha_num ( entry ) ) {
               return false;
          };
     };
     return true;
};

bool File::is_empty() const
{
     return pathvector.empty();
};

void File::remove_any_extension ()
{
     if ( !pathvector.empty() ) {
          pathvector.back() = pathvector.back().substr ( 0, pathvector.back().find_first_of ( "." ) );
          if ( pathvector.back().empty() ) {
               pathvector.pop_back();
          };
     };
};

size_t File::level() const
{
     return pathvector.size()-1;
};

void File::create_directories ( ) const
{
     for ( const auto &d : this->directories() ) {
          std::filesystem::create_directory ( d.fullname() );
     };
};

std::string File::name () const
{
     return pathvector.back();
};

File File::parent () const
{
     File output {*this};
     if ( !pathvector.empty() ) {
          output.pathvector.pop_back();
     };
     return output;
};

std::string File::fullname() const
{
     return this->directory ( pathvector.size() );
};

std::string File::directory ( const int &branch ) const
{
     std::string output;
     for ( int i = 0; i < branch ; ++i ) {
          output += pathvector[i];
          output += std::filesystem::path::preferred_separator;
     };
     if ( !output.empty() ) {
          output.pop_back();
     };
     return output;
};

std::vector<File> File::directories ( ) const
{
     std::string path {};
     std::vector<File> output;
     for ( size_t i = 0; i < pathvector.size() ; ++i ) {
          path += pathvector[i];
          output.push_back ( File ( path ) );
          path += file_sep;
     };
     return output;
};

bool File::operator< ( const File &file2 ) const
{
     return version_compare ( this->fullname(),file2.fullname() );
};

bool File::operator== ( const File &file2 ) const
{
     return this->fullname() == file2.fullname();
};

void File::operator/= ( const std::string &s )
{
     File file {s};
     for ( const auto &entry : file.pathvector ) {
          pathvector.push_back ( entry );
     };
};

void File::operator+= ( const std::string &s )
{
     File file {s};
     pathvector.back() += file.pathvector.front();
     file.pathvector.erase ( file.pathvector.begin() );
     for ( const auto &entry : file.pathvector ) {
          pathvector.push_back ( entry );
     };
};

File File::operator/ ( const std::string &s ) const
{
     File output {*this};
     output /= s;
     return output;
};

File File::operator+ ( const std::string &s ) const
{
     File output {*this};
     output += s;
     return output;
};



template<typename T>
std::string stringLL ( const T &LList )
{
     std::string str="[";
     for ( const auto &part : LList ) {
          str+="[ ";
          for ( const auto &i : part ) {
               str+=std::to_string ( i )+" ";
          };
          str+="]";
     };
     str+="]";
     return str;
};
template std::string stringLL ( const std::vector<std::vector<bool>> &LList );///< template specialization of stringLL() for bool STL vectors
template std::string stringLL ( const std::vector<std::vector<unsigned int>> &LList );///< template specialization of stringLL() for unsigned int STL vectors

template<typename T>
std::string stringL ( const T &List )
{
     if ( List.empty() ) {
          return "[]";
     };
     std::string str="[";
     for ( const int &i : List ) {
          str+=std::to_string ( i )+",";
     };
     str.pop_back();
     str+="]";
     return str;
}


template std::string stringL ( const std::vector<bool> &LList );///< template specialization of stringL() for bool STL vectors
template std::string stringL ( const std::list<unsigned int> &LList );///< template specialization of stringL() for unsigned int STL lists
template std::string stringL ( const std::vector<unsigned int> &LList );///< template specialization of stringL() for unsigned int STL vectors
template std::string stringL ( const std::vector<int> &LList );///< template specialization of stringL() for int STL vectors
template std::string stringL ( const std::vector<long> &LList );///< template specialization of stringL() for long STL vectors
template std::string stringL ( const std::vector<unsigned long> &LList );///< template specialization of stringL() for unsigned long STL vectors


std::string fill_front ( const std::string &s, const std::string &max )
{
     std::string output {""};
     for ( size_t i=0; i<max.size()-s.size(); ++i ) {
          output += " ";
     };
     output += s;
     return output;
};

std::string to_percentage ( const size_t &p,const size_t &q )
{
     std::string output{""};
     if ( 10*p<q ) {
          output+=" ";
     };
     output += std::to_string ( ( 100*p ) /q ) + "\%";
     return output;
};

std::string spin ( const size_t &counter )
{
     std::string output{""};
     if ( counter%4==1 ) {
          output += "/";
     } else if ( counter%4==2 ) {
          output += "–";
     } else if ( counter%4==3 ) {
          output += "\\";
     } else {
          output += "|";
     };
     return output;
};

std::string time_to_string ( const std::chrono::milliseconds &duration )
{
     std::string output{""};
     int millisec = ( int ) duration.count();
     if ( millisec < 60000 ) {
          // if less than 1 min, give millisec as decimals of seconds
          output += std::to_string ( millisec/1000 );
          output += ".";
          millisec = millisec%1000;
          output += std::to_string ( millisec/100 );
          millisec = millisec%100;
          output += std::to_string ( millisec/10 );
          millisec = millisec%10;
          output += std::to_string ( millisec );
          output += "s";
          return output;
     };
     // break time down into hours, minutes and seconds.
     int sec {millisec/1000};
     if ( sec>3600 ) {
          output += std::to_string ( sec/3600 );
          output += "h";
          sec = sec%3600;
     };
     output += std::to_string ( sec/60 );
     output += "min";
     sec = sec%60;
     output += std::to_string ( sec );
     output += "s";
     return output;
};

std::string to_string ( const bool &b )
{
     if ( b ) {
          return "true";
     };
     return "false";
};

bool version_compare ( const std::string &s1,const std::string &s2 )
{
     size_t i1 {0};
     size_t i2 {0};
     std::string temp1;
     std::string temp2;
     while ( i1 < s1.size() && i2 < s2.size() ) {
          if ( isdigit ( s1[i1] ) && isdigit ( s2[i2] ) ) {
               // fill the digit buffer for s1
               temp1 = s1[i1];
               while ( i1+1 < s1.size() ) {
                    ++i1;
                    if ( isdigit ( s1[i1] ) ) {
                         temp1 += s1[i1];
                    } else {
                         break;
                    };
               };
               // now do the same for s2
               temp2 = s2[i2];
               while ( i2+1 < s2.size() ) {
                    ++i2;
                    if ( isdigit ( s2[i2] ) ) {
                         temp2 += s2[i2];
                    } else {
                         break;
                    };
               };
               // compare the two digit buffers
               if ( std::stoi ( temp1 ) != std::stoi ( temp2 ) ) {
                    return std::stoi ( temp1 ) < std::stoi ( temp2 );
               };
          } else if ( s1[i1] == s2[i2] ) {
               ++i1;
               ++i2;
          } else {
               return s1[i1] < s2[i2];
          };
     };
     // one of the two strings is equal to the start of the other
     return s1.size() < s2.size();
};

void replace_all ( std::string &input,const std::string &s_old,const std::string &s_new )
{
     size_t start_pos = 0;
     while ( ( start_pos = input.find ( s_old, start_pos ) ) != std::string::npos ) {
          input.replace ( start_pos, s_old.length(), s_new );
          start_pos += s_new.length();
     };
};



bool my_alpha_num ( const std::string &s )
{
     for ( auto e : s ) {
          if ( !isalnum ( e ) && e!='_' && e!='-' ) {
               return false;
          };
     };
     return true;
};

bool yes_no_dialogue ( const std::string &message )
{
     std::cout << message << " (y/n)";
     std::string answer = "";
     bool loop=1;
     while ( loop == 1 ) {
          std::cin >> answer;
          std::cin.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );
          // there is a trailing '\n' that we need to ignore if getline is called next
          if ( answer.empty() ) {
               std::cout << "Please try again. Enter 'y' for 'yes' and 'n' for 'no'.";
          } else if ( answer.size() == 1 && answer.front() == 'y' ) {
               return true;
          } else if ( answer.size() == 1 && answer.front() == 'n' ) {
               return false;
          } else {
               std::cout << "Please try again. Enter 'y' for 'yes' and 'n' for 'no'.";
          };
     };
     return false;
}

void conditional_append (
     const std::string &filename,
     const std::string &line )
{
     // first test if line already exists
     bool found {false};
     std::ifstream in ( filename );
     std::string str;
     while ( std::getline ( in, str ) ) {
          if ( str == line ) {
               found = true;
               break;
          };
     };
     if ( found == false ) {
          std::ofstream file;
          file.open ( filename, std::ios_base::app );
          file << line << "\n";
          file.close();
     };
};



std::string file_to_string ( const std::string &fname )
{
     std::string output;
     std::fstream fs ( fname, std::fstream::in );
     if ( fs ) { // Check if the opening has not failed
          std::string line;
          while ( std::getline ( fs, line ) ) {
               output += line;
               output += "\n";
          };
     };
     fs.close();
     return output;
};



std::string html_header ( const File &file )
{
     std::string output;
     output += file_to_string ( "html/header_begin" );
     //
     output += "<link rel='stylesheet' href='";
     for ( size_t i = 0; i < file.level(); ++i ) {
          output += ".." + file_sep;
     };
     output += "html/style.css'>";
     //
     output += "<link rel='icon' type='image/png' href='";
     for ( size_t i = 0; i < file.level(); ++i ) {
          output += ".." + file_sep;
     };
     output += "html/raspberryip.png'>";
     //
     output += ( "<title>" + file.fullname() + "</title>\n" );
     output += file_to_string ( "html/scripts" );
     output += "</head>\n\n\n\n";
     return output;
};


void to_html ( const File &file )
{
     std::ofstream html ( file.fullname() + ".html" );

     html << html_header ( file );

     html << "<body onLoad=\"toggleDisplayClass('invariants');\">\n";

     html << file_to_string ( "html/overlays" );

     html << "<span style='float:right'>"
          << "<span id=\"invariants-all\" onclick=\"toggleDisplayClass('invariants');\">"
          << "show all invariants"
          << "</span>"
          << "|"
          << "<span class=\"metadata-button\" id=\"metadata-all\" onclick=\"toggleDisplayClass('metadata');\">"
          << "show all metadata"
          << "</span>"
          << "</span>";

     html << "<h1 style='float:left; text-align: left'>\n"
          << "<a id='link_to_docs' title='Take me to the documentation!' href='";
          int iter { int ( file.level() ) };
          for ( int i = 0; i < iter; ++i ) {
               html << ".." + file_sep;
          };
          html << docs_mainpage+"'>kht++</a>: \n"
          << html_navigation_title ( file.parent() )
          << "<span onclick=\"toggleDisplay('dummy','tangledata');\">"
          << file.name()
          << "</span>"
          << "<span id='dummy-all' style='display:none'></span>\n"
          << "</h1>\n"
          << "\n\n\n\n";

     html << "\n\n\n<h2>\n"
          << "Input data and context"
          << "\n</h2>\n";

     //
     // original tangle data with svg
     //
     html << "<div class='tangle-data' id='tangledata' style='display:'>\n";
     html << "<img class=\"tangle_svg_small\" \n"
          << "     src=\"" << file.name() << file_sep << file.name() << "-transparent.svg\" \n"
          << "     title=\"" << file.name() << file_sep << file.name() << "\" \n"
          << "     onClick=\"on('" << file.name() << file_sep << file.name()  << "')\">\n"
          << "<div class='tangle-data'>\n";
     //
     std::fstream fs ( file.fullname() + ".kht", std::fstream::in );
     if ( fs ) { // Check if the opening has not failed
          std::string line;
          while ( std::getline ( fs, line ) ) {
               html << line
                    << "<br>\n";
          };
     };
     fs.close();
     html << "</div>\n"
          << "</div>\n";


     //
     // read in available tangle invariants
     //
     std::list<File> cxBNrs;
     std::list<File> cxKhrs;
     std::list<File> cxCKhs;
     std::list<File> svgs;
     std::string input_file;
     for ( const auto &f : std::filesystem::directory_iterator ( file.fullname() ) ) {
          input_file = f.path();
          if ( input_file.find ( "cxBNr" ) != std::string::npos ) {
               cxBNrs.push_back ( File ( input_file ) );
          } else if ( input_file.find ( "cxKhr" ) != std::string::npos ) {
               cxKhrs.push_back ( File ( input_file ) );
          } else if ( input_file.find ( "cxCKh" ) != std::string::npos ) {
               cxCKhs.push_back ( File ( input_file ) );
          } else if ( input_file.find ( ".svg" ) != std::string::npos ) {
               svgs.push_back ( File ( input_file ) );
          };
     };
     cxBNrs.sort();
     cxKhrs.sort();
     cxCKhs.sort();
     svgs.sort();
     std::vector<std::string> complexnames {
          "Bar-Natan multicurve \\(\\mathrm{\\widetilde{BN}}(T)\\)",
          "Khovanov multicurve \\(\\mathrm{\\widetilde{Kh}}(T)\\)",
          "Khovanov type D structure \\(\\mathrm{\\widetilde{CKh}}(L)\\)"};
     std::vector<std::list<File>> complexlists;
     complexlists.push_back ( cxBNrs );
     complexlists.push_back ( cxKhrs );
     complexlists.push_back ( cxCKhs );



     //
     // add tangle invariants to html file
     //
     for ( size_t index = 0; index < complexnames.size(); ++index ) {
          if ( complexlists[index].empty() == false ) {
               html << "\n\n\n<h2>\n"
                    << complexnames[index]
                    << "\n</h2>\n";
          };
          for ( const auto &f : complexlists[index] ) {
               std::fstream fs ( f.fullname(), std::fstream::in );
               if ( fs ) { // Check if the opening has not failed
                    bool is_first {true};
                    std::string line;
                    html << "\n<h4>\n"
                         << "<span onclick=\"toggleDisplay('invariants','"
                         << f.name()
                         << "');\"\n"
                         << "      title='click to show tangle invariants'>\n"
                         << "options "
                         << f.name().substr ( 5 )
                         << "\n</span>\n"
                         << "<span onClick=\"on('"
                         << file.name() + file_sep + file.name() + f.name().substr ( 5 )
                         << "');\" \n"
                         << "      class='metadata-button'\n"
                         << "      title='click to show tangle diagram'>\n"
                         << "(diagram)\n"
                         << "</span>\n"
                         << "<span onclick=\"toggleDisplay('metadata','meta-"
                         << f.name()
                         << "');\" \n"
                         << "      title='click to show metadata'\n"
                         << "      class='metadata-button'>\n(metadata)\n</span>\n"
                         << "</h4>\n";
                    while ( std::getline ( fs, line ) ) {
                         if ( is_first ) {
                              html << "<div class='metadata' id='meta-"
                                   << f.name()
                                   << "' style='display:none'>\n"
                                   << line
                                   << "\n</div>\n"
                                   << "<div class='invariants' id='"
                                   << f.name()
                                   << "' style='display:none'>\n";
                              is_first = false;
                         } else {
                              replace_all ( line," ","&#160;" );
                              html << line << "<br>\n";
                         };
                    }
                    html << "</div>\n";
                    fs.close();
               }
               html << "<br>\n";
          };
     };
     // list Rasmussen's s-invariants:
     if ( !cxCKhs.empty() ) {
          html << "\n\n\n<h2>\n"
               << "Rasmussen's s-invariant"
               << "\n</h2>\n";
     };
     for ( const auto &f : cxCKhs ) {
          std::fstream fs ( f.fullname(), std::fstream::in );
          if ( fs ) { // Check if the opening has not failed
               bool is_first {true};
               std::string line;
               html << "\n<h4>\n"
                    << "<span onclick=\"toggleDisplay('invariants','s-"
                    << f.name()
                    << "');\"\n"
                    << "      title='click to show tangle invariants'>\n"
                    << "options "
                    << f.name().substr ( 5 )
                    << "\n</span>\n"
                    << "<span onClick=\"on('"
                    << file.name() + file_sep + file.name() + f.name().substr ( 5 )
                    << "');\" \n"
                    << "      class='metadata-button'\n"
                    << "      title='click to show tangle diagram'>\n"
                    << "(diagram)\n"
                    << "</span>\n"
                    << "<span onclick=\"toggleDisplay('metadata','meta-s-"
                    << f.name()
                    << "');\" \n"
                    << "      title='click to show metadata'\n"
                    << "      class='metadata-button'>\n(metadata)\n</span>\n"
                    << "</h4>\n";
               while ( std::getline ( fs, line ) ) {
                    if ( is_first ) {
                         html << "<div class='metadata' id='meta-s-"
                              << f.name()
                              << "' style='display:none'>\n"
                              << line
                              << "\n</div>\n"
                              << "<div class='invariants' id='s-"
                              << f.name()
                              << "' style='display:none'>\n";
                         is_first = false;
                    } else {
                         if ( line.find_first_of("H") == std::string::npos ){
                              // remove anything before and including "q^"
                              line = line.substr(line.find_first_of("q")+2);
                              // remove any leading spaces
                              line = line.substr(line.find_first_not_of(" "));
                              // cut after the integer
                              line = line.substr(0,line.find_first_of(" "));
                              html << line << "<br>\n";
                         };
                    };
               }
               html << "</div>\n";
               fs.close();
          }
          html << "<br>\n";
     };
     
     html << "\n";
     html << "</body>\n";
     html.close();
};

void html_summary ( const File &file )
{
     auto directories {file.directories() };
     directories.pop_back(); // ignore tangle html file
     std::reverse ( std::begin ( directories ), std::end ( directories ) );
     for ( const auto &directory : directories ) {
          std::ofstream html ( directory.fullname() + ".html" );
          html << "<!--html_summary-->\n"
               << html_header ( directory )
               << "<body>\n";
          // title
          html << "<h1 style='float:left; text-align: left'>\n"
               << "<a id='link_to_docs' title='Take me to the documentation!' href='";
          int iter { int ( directory.level() ) };
          for ( int i = 0; i < iter; ++i ) {
               html << ".." + file_sep;
          };
          html << docs_mainpage+"'>kht++</a>: \n"
               << html_navigation_title ( directory.parent() )
               << directory.name()
               << "</h1><div style='clear: both;'></div>\n\n\n\n";

          std::list<File> files;
          File temp_file;
          std::string input_file;
          for ( const auto &f : std::filesystem::directory_iterator ( directory.fullname() ) ) {
               input_file = f.path();
               if ( input_file.find ( ".html" ) != std::string::npos ) {
                    temp_file = File ( input_file );
                    temp_file.remove_any_extension();
                    files.push_back ( temp_file );
               };
          };
          files.sort();
          html << "<br>\n<div class='title-container'>";
          for ( auto &f : files ) {
               // determine if the file is a folder or not
               std::ifstream infile ( f.fullname()+".html" );
               std::string firstline;
               if ( infile.good() ) {
                    std::getline ( infile,firstline );
               };
               html << "<h4>"
                    << "<a href='"
                    << directory.name()
                    << file_sep
                    << f.name()
                    << ".html'>";
               if ( firstline.find ( "html_summary" ) != std::string::npos ) {
                    html << "&#128193; ";
               };
               html << f.name()
                    << "</a></h4>\n";
          };
          html << "</div>\n\n";
          html << "</body>\n";
          html.close();
     }
};

std::string html_navigation_title ( const File &file )
{
     if ( file.is_empty() ) {
          return "";
     } else {
          std::string html {};
          int iter { int ( file.level() ) };
          for ( const auto &d : file.directories() ) {
               html += "<a style='white-space: nowrap;' href='..";
               for ( int i = 0; i < iter; ++i ) {
                    html += file_sep + "..";
               };
               html += file_sep;
               html += d.name();
               html += ".html'>";
               html += d.name();
               html += "</a>";
               html += file_sep;
               --iter;
          };
          return html;
     };
}

#endif
