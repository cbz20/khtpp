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



#ifndef AUX_SYS_H
#define AUX_SYS_H
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <filesystem>
#include "constants.h"

/// \file aux_sys.h
/// \brief declares File class, i/o functions and string operations 

const std::string file_sep {std::filesystem::path::preferred_separator}; ///< file separator (depends on operating system)

typedef std::vector<unsigned int> IndexL;///< list of indices
typedef std::vector<IndexL> IndexLL;///< list of lists of indices
typedef int Grading;///< grading

/// consists of three integers, one for each grading (quantum, homological, δ), specifying the number of characters that should be reserved for this grading in text output. This is used to align the gradings in string representations of objects (BNObj,CobObj). 

struct max_gr_str {
    
     /// reserved space for homological grading
     size_t h;
     /// reserved space for quantum grading
     size_t q;
     /// reserved space for δ-grading
     size_t delta;
     /// standard constructor
     max_gr_str ( size_t h = 0,
                  size_t q = 0,
                  size_t delta = 0 );
};

/// consists of a list of strings that specify a file path when concatenated along \ref file_sep
    
/// Could be replaced by the STL class filesystem::path, see https://en.cppreference.com/w/cpp/filesystem/path
/// 
class File {
private:
    std::vector<std::string> pathvector;
public:
    File ( const std::string path = "");///< standard constructor, splitting  \c path along \ref file_sep into a list of strings
    
    size_t level() const;///< size of the list of strings
    void create_directories() const;///< creates directories, if they do not exist yet
    void remove_any_extension();///< removes the extension of the file, ie the first \c . in the last string of the list and any following character
    bool is_valid() const;///< true if \ref my_alpha_num() evaluates true on every string in the list
    bool is_empty() const;///< true if the length of the list is 0.
    std::string name() const;///< returns last string in the list. We assume the list is not empty.
    std::string fullname() const;///< concatenates all strings with \ref file_sep. This is the inverse of the constructor. 
    File parent() const; ///< returns a File obtained by removing the last element from the list (if possible)
    std::string directory( const int &branch ) const;///< string obtained by concatenating the first \c branch-1 strings with \ref file_sep. Results in undefined behaviour if \c branch is bigger than the length of the list
    std::vector<File> directories() const;///< list of all parent directories and the file itself, ordered by level()
    bool operator< (const File &file2 ) const;///< comparison operator
    bool operator== (const File &file2 ) const;///< true if the two files are identical
    void operator/= (const std::string &s  );///< adds a string to the list
    void operator+= (const std::string &s  );///< adds a string to the last element of the list
    File operator/ (const std::string &s  ) const;///< adds a string to the list
    File operator+ (const std::string &s  ) const;///< adds a string to the last element of the list
};

template<typename T>
std::string stringLL ( const T &LList );///< string representative of a list of list of elements
template<typename T>
std::string stringL ( const T &List );///< string representative of a list of elements
std::string fill_front ( const std::string &s, 
                         const std::string &max );///< adds whitespace to the front of a string \c s such that its length becomes equal to \c max
std::string to_percentage ( const size_t &p,
                            const size_t &q );///< converts fraction p/q with \f$0\leq p<q\f$ to percentage

std::string spin ( const size_t &counter );///< status indicator (working)
std::string time_to_string ( const std::chrono::milliseconds &duration );///< convert time to a string indicating ms, s, min, h, as appropriate
std::string to_string ( const bool &b );///< converts boolean value into string

/// comparison operator that interprets substrings of consecutive digits as integers before doing the comparison; 

/// Inspired by the command line tool \code{.sh} sort --version-sort \endcode.
bool version_compare( const std::string &s1,const std::string &s2 );
void replace_all ( std::string &input,
                   const std::string &s_old,
                   const std::string &s_new );///< replaces all occurences of \c s_old in \c input as \c s_new. 

bool my_alpha_num ( const std::string &s);///< true if all characters are alphanumeric or \c _ or \c -
bool yes_no_dialogue ( const std::string &message );///< prints \c message in terminal and accepts a yes/no no answer from the user
void conditional_append (
     const std::string &filename,
     const std::string &line );///< append \c line to the text file \c filename, unless this line already exists in the file
//void check_path ( const std::string &fpath );
std::string file_to_string ( const std::string &fname );///< reads in a file as a string
std::string html_header ( const File &file );///< defines header of html files
void to_html ( const File &file );///< compiles a html file of all information that has ever been computed for the file \c file
void html_summary ( const File &file );///< recompiles all html files linking to the file \c file
std::string html_navigation_title ( const File &file );///< main heading of the html pages allowing to navigate in the file structure


#endif
