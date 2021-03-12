
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

@m_footernavigation

kht++ is a command line application written in C++. It has been tested in linux only. 

\section Installation

There are two installation options: You can either compile the software yourself (recommended) or download the precompiled binaries.

\subsection Compiling Compiling kht++ from source
<ol>
<li>
You will need a C++ compiler supporting the C++17 standard, such as [clang++](https://clang.llvm.org/) or [g++](https://gcc.gnu.org/).
We also recommend that you install the version control system [git](https://git-scm.com/).
On ubuntu 20.04, you can install these prerequisites by running the following command:

    sudo apt install update
    sudo apt install build-essential clang git

</li>
<li>
kht++ depends on the C++ Standard Template Library and the <a href="https://eigen.tuxfamily.org">Eigen Template Library</a>. The former will probably be installed on your system already, once you have installed a compiler.  To install the latter, download the tar/zip file from the <a href="https://eigen.tuxfamily.org">Eigen Website</a> and decompress it into some folder. Since Eigen is a pure template library, this is all you need to do with it. 
</li>
<li>
Next, you need the kht++ source files. They are available from the [kht++ github repository](https://github.com/cbz20/khtpp).
If you have a [github](https://github.com/) account, you clone the repository in the usual way:
    
    git clone git@github.com:cbz20/khtpp.git        # via ssh, or
    git clone https://github.com/cbz20/khtpp.git    # via https

If you do not have a github account, you can clone the repository with the following command

    git clone git://github.com/cbz20/khtpp.git

You can also download the source files [directly](https://github.com/cbz20/khtpp/archive/main.zip), but this is not recommended. 
</li>
<li>
Navigate to the folder \c khtpp

    cd khtpp

and open the file \c makefile in a text editor. The first line should read as follows:

    PATH_EIGEN = ../libraries/Eigen

Replace the path \code{.sh}../libraries/Eigen\endcode as appropriate, such that it points to the folder containing the Eigen Template Library. If you want to compile using g++ instead of clang++, you also need to comment/uncomment the relevant lines in the \c makefile setting the variable \c CXX. Then run 

    make

This creates a number of binary files, of which \c kht++ is the main one. If you did not encounter any error message or warning, you are now ready to use the program.  If you are eager to see kht++ in action, skip to the \ref Examples. 
</li>
</ol>



\subsection Binaries

If you prefer to use precompiled binaries, you can download them [here](https://cbz20.raspberryip.com/code/khtpp/bin/khtpp-v0.1-ubuntu-20.04). 
They are currently only available for ubuntu 20.04. 



\subsection CompilingDocs Compiling the documentation

For the documentation, you need [Doxygen](https://github.com/doxygen/doxygen) (version ≥ 1.9.1) and [m.css](https://mcss.mosra.cz/documentation/doxygen/). The latter should either be installed into the directory \c docs, or you need to modify the path in the \c makefile appropriately. Then run

    make docs

The main page of the documentation is \code{.sh}docs/html/index.html\endcode. 




\section Examples Basic Examples 

\subsection FirstExample A first example 

Let us compute the Bar-Natan homology of a trefoil knot. For this, run 

\code{.sh}
./kht++ examples/tests/3_1.kht
\endcode

The output should look similar to the following:

\code{.sh}
>>>
>>> Computation for 'examples/tests/3_1':
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

You can also view this output in your browser by either opening the file \c examples/tests/3_1.html directly or typing 

\code{.sh}
./kht++ examples/tests/3_1.kht -w
\endcode

For more details on the parameter \c -w, see \ref Web. 

\subsection SecondExample A second example 

Let us compute the immersed curve invariant for the (2,-3)-pretzel tangle:

\image html "PT2m3.svg" width=3cm

For this, run 

\code{.sh}
./kht++ examples/tests/PT2m3.kht
\endcode

The output should look similar to the following:

\code{.sh}
>>>
>>> Computation for 'examples/tests/PT2m3':
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
./kht++ examples/tests/PT2m3.kht -w
\endcode

\subsection FurtherReading Further Reading

You might be wondering now what the above outputs mean and how to do computations for your own knots, links, and tangles.  These questions are answered on the following pages:

- \ref Input
- \ref Output
- \ref Options
