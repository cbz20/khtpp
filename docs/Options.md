
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



\page Options Options

@m_footernavigation

\section help Help

- \c -h , \c &ndash;&ndash;help 

Running \code{.sh} ./khtpp --help \endcode displays \ref main "this" help file and exits the program. 



\section coeffs Coefficients 

- \code{.sh} -c<i>\endcode , \code{.sh} --coeff<i> \endcode

    - \code{.cpp} i=2,3,5,7,11\endcode : Compute over the finite field \f$\mathbb{Z}/p\f$ where \f$p=\f$\c i. 
Support for other primes may be easily added, by setting \ref default_coeff equal to the desired prime in the source file \ref headers/constants.h and recompiling the program.
    - \code{.cpp} i=0 \endcode : Compute over the field of rational integers \f$\mathbb{Q}\f$.
    \warning The implementation of rational numbers is still in an experimental state. You should not trust the output, especially for large computations, since the program does not check for integer overflows. 
    \todo Detect integer overflows for rational coefficients.

- \c -a , \c &ndash;&ndash;allcoeff

Computes over all available fields of coefficients. 
 
Support for other fields of coefficients may be easily added, since the implementation is based on templates.  For details, see \ref headers/Coefficients.h.



\section DoubleStrands Doubling all tangle strands

- \c -d , \c &ndash;&ndash;double

This doubles all tangle strands before computing any invariants. 
For 1-1-knots, the framing of the resulting cap-trivial tangle is chosen such that the linking number between the two strands is 0. 
For example, for the trefoil

\image html "3_1-c2.svg" width=3cm

running 

\code{.sh}
./kht++ -d examples/knots/3_1.kht
\endcode

computes the invariants of the tangle 

\image html "3_1-c2-d.svg" width=3cm



\section Optimize Optimizing tangles by adding twists

- \c -o , \c &ndash;&ndash;optimize

Attempts to minimize the number of generators in the reduced cobordism complex by adding twists to the tangle ends. For example, running 

\code{.sh}
./kht++ -o examples/pretzeltangles/PT2m3.kht
\endcode

computes the invariants of the tangle 

\image html "PT2m3-c2-o.svg" width=3cm

although the input tangle is

\image html "PT2m3-c2.svg" width=3cm

The bottom two crossings were added automatically, because the invariant \f$\widetilde{\mathrm{BN}}(T)\f$ of the first tangle is

\m_div{m-text-center}\verbatim⬮<—D——⬮<~⬯<—⬯~>⬮—>⬮~>⬮—>⬮~>⬯\endverbatim \m_enddiv

wheras the invariant \f$\widetilde{\mathrm{BN}}(T)\f$ of the tangle with the extra crossings consist of two objects fewer:

\m_div{m-text-center} \verbatim⬯<—D——⬯~>⬮—>⬮~>⬮—>⬮~>⬯\endverbatim \m_enddiv



\section Suffix Appending tangle strings

- \code{.sh} -p <tanglestring>\endcode , \code{.sh} --append <tanglestring>\endcode

Appends a tangle string after applying any other options that modify a given tangle. When this option is used multiple times, the tangle strings are appended separately. 

For example, to compute the invariant of a Conway tangle obtained by cutting open a Whitehead double of some knot, I only need to run the program on a (1,1)-tangle representation of this knot, combining the doubling option \ref DoubleStrands "-d" with the option \code{.sh} -p l1.y0.y2.u1\endcode.



\section Prefix Prepending tangle strings

- \code{.sh} -P <tanglestring>,<new top orientation>\endcode , \code{.sh} --prepend <tanglestring>,<new top orientation>\endcode

Prepends a tangle string after applying any other options that modify a given tangle. \code{.sh}<new top orientation>\endcode is a comma separated list of \c 0 and \c 1 . The option \ref Prefix "-P" can only be used at most once. 

For example, in conjunction with the options \ref DoubleStrands "-d" and \ref Suffix "-p", the option \ref Prefix "-P" can be used to effectively compute Rasmussen invariants of twisted satellite knots, such as the following ten cables of the trefoil knot: 

\code{.sh}
./kht++ examples/tests/3_1.kht -d -Pr1,1 -px0.u1 -px0.x0.x0.u1 \
-px0.x0.x0.x0.x0.u1 -px0.x0.x0.x0.x0.x0.x0.u1 -px0.x0.x0.x0.x0.x0.x0.x0.x0.u1 \
-py0.u1 -py0.y0.y0.u1 -py0.y0.y0.y0.y0.u1 -py0.y0.y0.y0.y0.y0.y0.y0.y0.u1 \
-py0.y0.y0.y0.y0.y0.y0.u1
\endcode



\section QuotientTangles Quotients of strongly invertible knots

- \c -q , \c &ndash;&ndash;quotient (see also \ref TwoBridge)

This option was developed and extensively used for the paper \cite KWZ_strong_inversions. It computes the invariants of the quotient tangle of a strongly invertible knot specified by a \ref StrongInversions "symmetric diagram". The quotient tangle is framed such that the closure with the rational tangle of slope 0 is the unknot and the rational closure of slope \f$\infty\f$ corresponds to surgery along the longitude for the knot. Unfortunately, this means that to make these tangles follow the (arguable better) conventions used in \cite Watson2010, \cite Watson2012, and \cite Watson2017, one needs to rotate the tangles by 90 degrees. 

For example, the quotient of the strongly invertible knot given by the diagram

\image html "s3_1.svg" width=3cm

is equal to 

\image html "s3_1-c2-q.svg" width=5cm

It is generally a good idea to combine computations for quotient tangles with the option \ref SimplifyTangles "-s".


\section TwoBridge Defining strongly invertible 2-bridge knots

- \code{.sh} -r <name>:<p>/<q>\endcode , \code{.sh} --rational <name>:<p>/<q> \endcode (see also \ref QuotientTangles)

Computes all symmetric diagrams of the 2-bridge knot corresponding to the fraction \code{.sh}<p>/<q>\endcode and saves them in one or more files \code{.sh}<name>.kht\endcode or \code{.sh}<name>-{1,2}.kht\endcode in the directory \code{.sh} examples/strong_inversions/2-bridge-knots\endcode; see \ref StrongInversions, \ref Sakuma, and \cite Sakuma for details. 



\section SimplifyTangles Simplifying tangle input data

- \c -s , \c &ndash;&ndash;simplify

Attempts to simplify the diagram before computing any invariants by doing some isotopies and Reidemeister moves. This can be useful for reducing the computation time. 



\section TangleSums Tangle sums/tensor products of multicurves

- \code{.sh} --sum <s1>:<s2>\endcode , \code{.sh} --sumnc <s1>:<s2> \endcode

The first variant \code{.sh} --sum <s1>:<s2>\endcode computes the tensor product of two multicurves. This corresponds to taking the tangle sum of two Conway tangles (ie stacking the first tangle on top of the other). \code{.sh} <s1>\endcode and \code{.sh} <s2>\endcode specify the full relative paths of the text files containing the curves for the first and second tangle, respectively. Such files are stored by the program in separate folders next to the standard .kht-files.

For example, after running \code{.sh} ./kht++ examples/pretzeltangles/PT2m3.kht\endcode, the folder \c examples/pretzeltangles/PT2m3/ contains a file \c cxBNr-c2 , which (apart from metadata) consists of the following line: 

\code{.sh}
1) h^-2 q^-7 δ^-3/2 ⬮<—D——⬮<~⬯<—⬯~>⬮—>⬮~>⬮—>⬮~>⬯
\endcode

After running \code{.sh} ./kht++ -d examples/tests/3_1.kht\endcode, the folder \c examples/tests/3_1/ contains a file \c cxBNr-c2-d :

\code{.sh}
1) h^6 q^12 δ^  0 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
2) h^0 q^ 1 δ^1/2 ⬮——D—>⬮~>⬮—>⬮~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯
\endcode 

So running

\code{.sh}
./kht++ --sum examples/pretzeltangles/PT2m3/cxBNr-c2:examples/tests/3_1/cxBNr-c2-d 
\endcode 

computes the tensor product of this curve with itself and produces the following output:

\code{.sh}
Reading in chain from file 'examples/pretzeltangles/PT2m3/cxBNr-c2'...
...understood:h^-2 q^-7 δ^-3/2 ⬮<—D——⬮<~⬯<—⬯~>⬮—>⬮~>⬮—>⬮~>⬯
Reading in chain from file 'examples/tests/3_1/cxBNr-c2-d'...
...understood:h^6 q^12 δ^0 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
...understood:h^0 q^1 δ^1/2 ⬮——D—>⬮~>⬮—>⬮~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯
First summand:
1) h^-2 q^-7 δ^-3/2 ⬮<—D——⬮<~⬯<—⬯~>⬮—>⬮~>⬮—>⬮~>⬯
Second summand:
1) h^6 q^12 δ^  0 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
2) h^0 q^ 1 δ^1/2 ⬮——D—>⬮~>⬮—>⬮~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯
Computing the tangle sum. The resulting complex has 441 generators. Now cancelling.
Cleaned-up complex in 30 iterations and 0.278s.
Tangle sum:
1) h^ 6 q^ 8 δ^  -2 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
2) h^ 1 q^ 0 δ^  -1 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
3) h^ 2 q^ 2 δ^  -1 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
4) h^ 3 q^ 4 δ^  -1 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
5) h^-1 q^-2 δ^   0 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
6) h^-5 q^-8 δ^   1 ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—
7) h^-2 q^-5 δ^-1/2 ⬮——D—>⬮~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯
\endcode
 
The second variant \code{.sh} --sumnc <s1>:<s2>\endcode does the same as the first, except that only the non-compact components of the multicurves are considered in the input. In the example above, this would have the effect of ignoring the first component of the second multicurve. If the input curves contain many compact curves, this makes the computation run much faster than \code{.sh} --sum <s1>:<s2>\endcode. This option is useful if one is only interested in the non-compact curve of a tangle sum. 

Any pair of input files can be used in these options, as long as the multicurves are stored in the standard output format for multicurves (see \ref Output).

\warning This option produces the wrong result if one works with any coefficients other than \f$\mathbb{Z}/2\f$. 
\todo Make this option work for all coefficients (ie add signs from tensor product sign rule).

\section Tests Tests

- \c -t , \c &ndash;&ndash;tests

Runs some tests and exits the program. This option is only intended for the purpose of debugging. 



\section VersionOutput Version

- \c -v , \c &ndash;&ndash;version

Returns the version of the program and exits the program. 
                          

\section Web Recompile and open output in browser

- \c -w , \c &ndash;&ndash;web

Prints the tangle input and (re)compiles the output for the specified file(s) using all previously computed data. 
No new computations of tangle invariants are made with this option. 
If the program \c xdg-open (or \c open) is installed, this option will also open the output file for the first specified tangle using the default program for .html-files. 
