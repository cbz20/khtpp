
This file is part of kht++.

kht++ is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as 
published by the Free Software Foundation, either version 3 of 
the License, or (at your option) any later version.

kht++ is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General 
Public License along with kht++.  If not, see 
<https://www.gnu.org/licenses/>.



\page Input

There are three ways to tell kht++ which tangle you would like to compute the invariants for: 

- via an \ref Interactive "interactive dialogue" (recommended for beginners), 
- via a \ref FileDefinition ".kht-file", or
- from existing .kht-file by passing certain \ref NewFromOld "optional arguments". 

\section Interactive Interactive tangle definition

Run the program without any arguments:

\code{.sh}
./kht++
\endcode

This will start a dialogue in which you can specify the name of your tangle as well as a location for where the input and output data should be stored. The location should be in the same path as the directory from which you run the program. The program will then ask for the number of tangle ends at the top as well as for their orientation. The number of tangle ends should be positive. Knots and links are regarded as 1-1-tangles. 

For example, the dialogue might look like this:

\code{.sh}
Please specify a path for this project: 
(default 'examples/tests') 
              
Please specify a name for this project: 
(default 'test'; only use alphanumeric characters, '-', and '_')
my-test-1    
Specify the number of tangle ends at the top:
2
Specify the orientation of each strand at the top: (1=↑,0=↓)
0: 0
1: 1
\endcode

Next, you will be asked to specify a Morse presentation for your tangle. By this we mean a decomposition into tangle slices, i.e. elementary top-bottom-tangles with a single crossing, a single cap, or a single cup. Each slice is specified by a word of the form \code{.sh}<c><i>\endcode where

- the  \anchor slice-type  slice-type \code{.sh}<c>\endcode is one of the following letters: \c x, \c y, \c l, \c r, and \c u. These letters are translated into elementary tangles  as follows:

    - \c x : ⤬ (crossing with over-strand starting at top left corner)
    - \c y : ⤫ (crossing with over-strand starting at top right corner)
    - \c l : ↶ (cap, oriented to the left)
    - \c r : ↷ (cap, oriented to the right)
    - \c u : ∪ (cup)

- The \anchor slice-index  slice-index \code{.sh}<i>\endcode is the index of the left strand in a crossing/cap/cup, counted from the left, starting at 0. 

Specify a word and press @m_class{m-label m-warning} **Enter**. Then continue with the remaining tangle slices in the same way. The tangle slices that you enter will be shown in the terminal and build up your tangle step-by-step. If you find a mistake, you can delete the previous slice by typing \c k. If you want to start from scratch, you can quit the program by typing \c q. Once you have entered the whole tangle, you type \code{.sh}.\endcode (full stop). 

For example, the following code specifies the rational tangle of slope 2:

\image html "my-test-1-c2.svg" width=3cm

\code{.sh}
===tangle slices===
1 l2    ↓ ↑  ↶ 
| x1    ↓  ⤬  ↑ 
| y0     ⤫  ↑ ↑ 
4 u1    ↓   ͝  /
\endcode

\note If you enter a slice which does not define a valid tangle (for example, if the \ref slice-index is incompatible with the number of top tangle ends), the program will complain and ask you to reconsider.  If you enter a slice that connects two tangle ends that are oriented in an incompatible way, the program will change the orientation of one of the strands so that the orientations match. 

The program will then compute the tangle invariants. See \ref Output for how to interpret the output. 

\section FileDefinition Tangle definition via a .kht-file

The interactive method of defining a tangle produces a number of files in the path previously specified. In the running example above, the folder \c examples/tests now contains

- the file \c my-test-1.kht: This is a text file containing the input tangle data; more on this below.
- the file \c my-test-1.html: it contains a summary of the input and the invariants; see \ref Output on how to interpret the latter. 
- the folder \c my-test-1; it contains several .svg-drawings of your tangle and various text files that contain the computed invariants. 

Let us have a closer look at the .kht-file.  It should look something like this:

\code{.sh}
% my-test-1
l2.x1.y0.u1
,0,1
\endcode

The character \c % in the first line signifies that this is a comment (like e.g. in \f$\text{\LaTeX}\f$) and will be ignored by the program. You might want to add more comments to help you remember later, why you were interested in this tangle. More generally, anything between the first occurence of the character \c % and the end of the line will be ignored. 

The second line consists of a \code{.sh}.\endcode-separated list of words that specify the slices of your tangle, ordered from top to bottom when read from left to right. This list can be split over multiple lines. 
The last slice should be followed by a comma (\code{.sh},\endcode) and a \code{.sh},\endcode-separated list of zeros (\c 0) and  ones (\c 1). Each of these represent the orientation of a top tangle end, ordered from left to right, using the following convention:

- \c 1 means that the tangle end points upwards (↑);
- \c 0 means that it points downwards (↓).

Given such a .kht-file, we can run the computation as follows:

\code{.sh}
./kht++ examples/tests/my-test-1.kht
\endcode

The suffix \code{.sh}.kht\endcode may be omitted.  You can also run computations for multiple .kht-files in one go:

\code{.sh}
./kht++ <path-to-file-1> ... <path-to-file-N>
\endcode

When you do this, note that any \ref Options that you specify will apply to all computations. 

\subsection StrongInversions Strongly invertible knots

There is a special input format for strongly invertible knots. These are knots with a certain symmetry, see for instance \cite Sakuma for details. The trefoil knot admits a unique strong inversion, which is realized by the following diagram:

\image html "s3_1.svg" width=3cm

This diagram shows the symmetry axis as a vertical dotted line and one half of the diagram; the other half is obtained by rotation about the vertical axis by 180 degrees. The .kht-file for this strongly invertible knot looks as follows:

\code{.sh}
%  strong inversion on 3_1
s.l1.y0.y0.y1.u2
,1
\endcode

The prefix \code{.sh}s.\endcode signifies that the data should be interpreted as a strongly invertible knot. 
Only one orientation should be specified (as in the third line above), and it may be \c 0 or \c 1. 
There should be the same number of caps as there are cups. 
Moreover, the symmetrized diagram should have a single component only (so we get a knot, as opposed to a link with more than one component). 
Finally, neither caps nor cups may have slice index \c 0, otherwise they would intersect the axis; in particular, neither the top cap nor the bottommost cup should be specified.

For 2-bridge knots, one can use the parameter \c -r to generate all strong inversions, see \ref TwoBridge "here" for details. 

\note Strongly invertible knots are not implemented in the \ref Interactive. 


\section NewFromOld New from old

There are several ways to compute the invariants of new tangles from existing .kht-files by passing some optional parameters to kht++:


- \c -d : \ref DoubleStrands
- \c -q : \ref QuotientTangles
- \c -o : \ref Optimize 


