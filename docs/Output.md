
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



\page Output

When doing a computation, kht++ usually generates three kinds of output: 

- direct feedback in the terminal;
- a summary of all computations in form of a .html-file;
- a folder of the same name as the given \ref FileDefinition ".kht-file" (without the .kht-suffix) in the same path as the .kht-file, containing all computed data pertaining to this particular .kht-file, namely:
    - .svg-files with pictures of the computed tangles
    - text files named \c cxCKh, \c cxBNr, and \c cxKhr containing any computed invariants \f$\widetilde{\mathrm{CKh}}(L)\f$, \f$\widetilde{\mathrm{BN}}(T)\f$, and \f$\widetilde{\mathrm{Kh}}(T)\f$, respectively. 

The most user-friendly output is the .html-file, which can be opened directly from your terminal after a computation by re-running the previous command with the added \ref Web "Option -w". The feedback in the terminal is primarily intended for human consumption. The data files, on the other hand, are primarily intended for automatic post-processing, for example via a bash-script. Most \ref Options "optional parameters", such as the ones specifying the field of coeffiecients, are appended to the names of the data files. 
    
    
\section UnderstandingOutput Interpreting the output

Below, we will focus on the invariants of tangles with two ends (i.e. knots and links) and tangles with four ends (i.e. Conway tangles). 
We will follow the terminology and conventions in \cite KWZ.

\subsection LinkInvariants The Khovanov invariant for knots and links

The Khovanov invariant \f$\widetilde{\mathrm{CKh}}(L)\f$ of a knot or link \f$L\f$ is a complex (a.k.a. type D structure) over the polynomial ring \f$k[H]\f$, where \f$k\f$ is some field (or ring) of coefficients. What is usually known as reduced Khovanov homology is the chain complex obtained by setting \f$H=0\f$. When \f$k\f$ is a field, irreducible complexes are classified by non-negative integers:

- \f$C_0\f$ consists of a single copy of \f$k[H]\f$ (i.e. a single object) with no differential: 

        ⬮

- Given an integer \f$n>0\f$, \f$C_n\f$ consists of two copies of \f$k\f$ with a single differential which is multiplication by some positive power of \f$H\f$:

        ⬮——H^n—>⬮


The complexes \f$\widetilde{\mathrm{CKh}}(L)\f$ carry three gradings: a quantum grading \f$q\f$, a homological grading \f$h\f$, and a third grading \f$\delta\f$. These gradings are also known as a bigrading, since they are related by the identity
\f[
\tfrac{1}{2}q=h+\delta
\f]
For example, the computation for the trefoil knot

\image html "3_1-c2.svg" width=2cm

looks as follows:

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

Let us got through this output line by line:

- The first eight lines identify the relevant \ref FileDefinition ".kht-file" and give us a textual representation of the tangle (see \ref Input)
- The line \code{.sh}>>> computations for -c2:\endcode says that the computation was done over the field of 2 elements \f$\mathbb{Z}/2\f$; see \ref Options for details.
- The next line gives us some indication for the complexity of the computation and the invariant.
- The feedback \code{.sh}The complex is already loop-type.\endcode means that no additional isotopy was necessary to achieve the decomposition into irreducible summands. For more complicated invariants, this might instead read \code{.sh}Cleaned-up complex in 10 iterations and 0.000s\endcode.
- The last two lines tell us that \f$\widetilde{\mathrm{CKh}}(L)\f$ of the trefoil knot is the direct sum of a copy of \f$C_0\f$ and a copy of \f$C_1\f$, shifted in bigradings. For \f$n>0\f$, only the bigrading of the source of the differential is specified. 



\subsection TangleInvariants Invariants for Conway tangles

The Khovanov invariants \f$\widetilde{\mathrm{BN}}(T)\f$ and \f$\widetilde{\mathrm{Kh}}(T)\f$ of a Conway tangle \f$T\f$ are complexes over the quiver algebra \f$\mathcal{B}\f$ over some field \f$k\f$. This algebra has two idempotents, which we will denote by \c ⬮ and \c ⬯. The morphisms are linear combinations of the following basic morphisms:

- the two idempotents; 
- positive integer powers of \f$D\f$, going between idempotents of the _same_ type: 
\m_div{m-text-center}\verbatim ⬮——D^n—>⬮    or    ⬯——D^n—>⬯ \endverbatim\m_enddiv
- positive _even_ powers of \f$S\f$, going between idempotents of the _same_ type:
\m_div{m-text-center}\verbatim ⬮~~S^2n~>⬮    or    ⬯~~S^2n~>⬯\endverbatim\m_enddiv
- positive _odd_ powers of \f$S\f$, going between idempotents of _opposite_ types:
\m_div{m-text-center}\verbatim ⬯~~S^2n-1~>⬮    or    ⬮~~S^2n-1~>⬯\endverbatim\m_enddiv

Complexes over the \f$k\f$-algebra \f$\mathcal{B}\f$ were classified in \cite KWZ in terms of cyclic or linear chains of idempotents \c ⬮ and \c ⬯ and differentials between adjacent idempotents that are labelled alternatingly by powers of \f$S\f$ and powers of \f$D\f$, multiplied by some scalar. Here is an example of an irreducible complex over \f$\mathcal{B}\f$:
\m_div{m-text-center}\verbatim ⬯~~S~>⬮—>⬮~>⬮—>⬮~>⬯—>⬯<~⬮<—⬮<~⬮<—⬮<~⬯<—\endverbatim\m_enddiv
This should be read cyclically, i.e. the last arrow connects the first generator to the last. Except for the very first arrow, we will always suppress the labels of differentials if those labels are equal to \f$D\f$, \f$S\f$, or \f$S^2\f$. 


\warning 
According to the classification of complexes over the algebra \f$\mathcal{B}\f$, irreducible complexes may actually carry local systems \cite KWZ; however, no example of a Conway tangle is known whose Khovanov invariants carry higher-dimensional local systems. 
Such local systems cannot be detected by kht++. 
More precisely, if kht++ fails to clean-up a complex for a four-ended tangle (i.e. fails to decompose it into irreducible summands), this can either mean that you need to run the computation longer, or that you have found a higher-dimensional local system. 
In any case, if this occurs, please \ref Contact "get in touch".

Let us consider an example. The output for the (2,-3)-pretzel tangle 

\image html "PT2m3.svg" width=3cm

looks something like this:

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

The invariant \f$\widetilde{\mathrm{BN}}(T)\f$ is always shown first. In this case, it is equal to a linear chain

\m_div{m-text-center}\verbatim ⬮<—D——⬮<~⬯<—⬯~>⬮—>⬮~>⬮—>⬮~>⬯\endverbatim\m_enddiv

The specified bigrading at the start of the line is the bigrading of the first generator. 
The invariant \f$\widetilde{\mathrm{Kh}}(T)\f$ consists of two components in our example. 
Again, only the bigrading of the first generator is given. 

\subsubsection RationalVsSpecialCurves Recognition of rational and special components

We expect that for any Conway tangle \f$T\f$, the components of \f$\widetilde{\mathrm{Kh}}(T)\f$ are heavily restricted.
Up to an overall shift in bigrading, each component should belong to one of two families of components, namely rational components and special components. 
We denote the former by \f$\mathbf{r}_n(\tfrac{p}{q})\f$ and the latter by \f$\mathbf{s}_n(\tfrac{p}{q})\f$, where \f$\tfrac{p}{q}\in\mathbb{Q}\mathrm{P}^1\f$ is called the slope and \f$n\in\mathbb{Z}\f$ the length of the component.
Note that this dichotomy between rational and special is very reminiscent of the dichotomy for the Heegaard Floer tangle invariant \f$\mathrm{HFT}(T)\f$ \cite pqMod \cite pqSym. 

In the last two lines of the output in the previous example, you may have noticed the expressions \code{.sh}r(1/2) q^-26/3 δ|^-1\endcode and \code{.sh}s2(0) q^-8 δ|^-1\endcode. These expressions tell you that kht++ identified the first component as a rational component of length 1 and slope \f$\tfrac{1}{2}\f$ and the second component as a special component of length 2 and slope 0. The following quantum gradings are averages of the quantum gradings of all generators in the respective component. The expression \code{.sh}δ|^-1\endcode signifies that the vertical \f$\delta\f$-grading both components is equal to -1; the horizontal \f$\delta\f$-grading would be denoted by \c δ_; see \cite KWZ_thin for details. 


\warning 
The recognition of components of \f$\widetilde{\mathrm{Kh}}(T)\f$ is not always correct, because kht++ presently does not know about rational components of length greater than 1. 

\todo Implement identification of rational components of arbitrary length. 



\subsection MoreTangleEnds Invariants for tangles with more than four ends

Complexes over the cobordism category for tangles with more than four ends are currently poorly understood. Consequently, kht++ does not know how to handle the output in a reader-friendly way. For such tangles, the cobordism complexes are stored in data files of the form \code{.sh}cx-<options>\endcode.  If you would like to understand this output, take a look at the code documentation for the \ref Cob.h "implementation" of Bar-Natan's universal cobordism category or \ref Contact "get in touch". 




