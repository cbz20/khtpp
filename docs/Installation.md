
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



\page GettingStarted Getting Started

kht++ is a command line application written in C++. It has been tested in linux only. 

\section Installation

There are two installation options: You can either compile the software yourself (recommended) or download the precompiled binaries.

\subsection Compiling Compiling from source
<ol>
<li>
You will need a C++ compiler supporting the C++17 standard, such as clang++. On ubuntu 20.04, you can install these prerequisites by running the following command:

    sudo apt install update
    sudo apt install build-essential clang

</li>
<li>
Download the kht++ source files following the instructions on github. 
In the following, we will assume that the parent folder containing the file \c makefile is called \c kht.
</li>
<li>
kht++ depends on the C++ Standard Template Library and the <a href="https://eigen.tuxfamily.org">Eigen Template Library</a>. The former will probably be installed on your system already, once you have installed a compiler.  To install the latter, download the tar/zip file from the <a href="https://eigen.tuxfamily.org">Eigen Website</a> and decompress it into some folder. Since Eigen is a pure template library, this is all you need to do with it. 
</li>
<li>
Navigate to the folder \c kht and open the file \c makefile in a text editor. The first line should read as follows:

    PATH_EIGEN = ../libraries/Eigen

Replace the path \code{.sh}../libraries/Eigen\endcode as appropriate, such that it points to the folder containing the Eigen Template Library. Then, still in the folder \c kht, run

    make

This creates a number of binary files, of which \c kht++ is the main one. If you did not encounter any error message or warning, you are now ready to use the program.
</li>
</ol>

\todo add instructions and link to github

\subsection Binaries Binary files
If you do not want to compile kht++ on your own, you can download the binary files here:

\todo link to binary files for ubuntu 20.04


\section Examples Basic Examples 



\subsection FirstExample A first example 

Let us compute the Bar-Natan homology of a trefoil knot. For this, run 

\code{.sh}
./kht++ examples/knots/3_1.kht
\endcode

The output should look similar to the following:

\code{.sh}
>>>
>>> Computation for 'examples/knots/3_1':
>>>
1 r1	↑  ↷ 
| y0	 ⤫  ↓ 
| y0	 ⤫  ↓ 
| y0	 ⤫  ↓ 
5 u1	↑   ͝
>>> computations for -c2:
Computed cobordism complex with 3 generators in 0.002s.
The complex is already loop-type.
1) h^0 q^2 δ^1 ⬮
2) h^2 q^6 δ^1 ⬮——H—>⬮
\endcode

You can also view this output in your browser by either opening the file \c examples/knots/3_1.html directly or typing 

\code{.sh}
./kht++ examples/knots/3_1.kht -w
\endcode

For more details on the parameter \c -w, see \ref Web. 

\subsection SecondExample A second example 

Let us compute the immersed curve invariant for the (2,-3)-pretzel tangle:

\image html "PT2m3.svg" width=3cm

For this, run 

\code{.sh}
./kht++ examples/pretzeltangles/PT2m3.kht
\endcode

The output should look similar to the following:

\code{.sh}
>>>
>>> Computation for 'examples/pretzeltangles/PT2m3':
>>>
1 l1	↑  ↶ 
| x2	↑ ↓  ⤬ 
| y0	 ⤫  ↑ ↑ 
| x2	↓ ↑  ⤬ 
5 y0	 ⤫  ↑ ↑ 
| x2	↑ ↓  ⤬ 
7 u1	↑   ͝
>>> computations for -c2:
Computed cobordism complex with 9 generators in 0.008s.
Cleaned-up complex in 10 iterations and 0.001s.
1) h^-2 q^-7 δ^-3/2 ⬮<—D——⬮<~⬯<—⬯~>⬮—>⬮~>⬮—>⬮~>⬯
Cleaned-up complex in 10 iterations and 0.004s.
1) h^-5 q^-11 δ^-1/2 ⬯~~S~>⬮—>⬮~>⬮<—⬮<~⬯<—     r(1/2) q^-26/3 δ|^-1
2) h^-6 q^-13 δ^-1/2 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—     s2(0) q^-8 δ|^-1
\endcode

Again, you can view a nicely formatted output by rerunning the previous command with the option \c -w:

\code{.sh}
./kht++ examples/pretzeltangles/PT2m3.kht -w
\endcode

\subsection FurtherReading Further Reading

You might be wondering now what the above outputs mean and how to do computations for your own knots, links, and tangles.  These questions are answered on the following pages:

- \ref Input
- \ref Output
- \ref Options
