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



#ifndef COEFFICIENTS_CPP
#define COEFFICIENTS_CPP
#ifndef COEFFICIENTS_H
#include "../headers/Coefficients.h"
#include "../headers/constants.h"
#endif


/// \file Coefficients.cpp
/// \brief implements all classes that represent rings of coefficients




// template specialization

#define VALUE 2
#include "../specialization/Coefficients.h"
#define VALUE 3
#include "../specialization/Coefficients.h"
#define VALUE 5
#include "../specialization/Coefficients.h"
#define VALUE 7
#include "../specialization/Coefficients.h"
#define VALUE custom_coeff
#include "../specialization/Coefficients.h"
#define nonprime
#define VALUE integer_simulant
#include "../specialization/Coefficients.h"
#undef nonprime

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Z
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
std::string coeff_to_string ( const int_coeff &i )
{
     return std::to_string ( i );
};

bool coeff_is_inv ( const int_coeff &n )
{
     return ( n == 1 ) || ( n == -1 );
};

bool is_id ( const int_coeff &n )
{
     return n == 1;
};

bool coeff_is_0 ( const int_coeff &n )
{
     return n == 0;
};
std::string which_coeff ( const int_coeff &i )
{
     return "Z";
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Z_mod<P>
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

template<prime P>
bool is_id ( const Z_mod<P> &n )
{
     // check if n is identity.
     return ( n.n - 1 ) % P == 0;
};

template<prime P>
bool coeff_is_0 ( const Z_mod<P> &n )
{
     // check if n is equal to 0.
     return n.n % P == 0;
};

template<prime P>
std::string coeff_to_string ( const Z_mod<P> &i )
{
     if ( is_id ( i ) ) {
          return "1";
     } else if ( is_id ( -i ) ) {
          return "-1";
     } else if ( i.n < 0 ) {
          return std::to_string ( i.n + P );
     };
     return std::to_string ( i.n );
};

template<prime P>
prime which_P ( const Z_mod<P> &n )
{
     // Use 'which_P(Coeff(0))' to determine which prime is
     // currently being used.
     return P;
};

template<prime P>
std::string which_coeff ( const Z_mod<P> &n )
{
     return "Z_mod" + std::to_string ( P );
};

template<prime P>
std::ostream& operator<< (
     std::ostream &out,
     const Z_mod<P> &n )
{
     // nice output, so it behaves like integers
     if ( n.n < 0 ) {
          out << n.n+P;
     } else {
          out << n.n;
     };
     return out;
};

template<prime P>
Z_mod<P> operator+ ( const Z_mod<P> &n1,const Z_mod<P> &n2 )
{
     return Z_mod<P> ( n1.n+n2.n );
};

template<prime P>
void operator += ( Z_mod<P> &n1,const Z_mod<P> &n2 )
{
     n1.n = ( n1.n + n2.n ) % P;
};

template<prime P>
Z_mod<P> operator- ( const Z_mod<P> &n )
{
     return Z_mod<P> ( -n.n );
};

template<prime P>
Z_mod<P> operator* ( const Z_mod<P> &n1,const Z_mod<P> &n2 )
{
     return Z_mod<P> ( n1.n * n2.n );
};

template<prime P>
Z_mod<P> operator* ( const int_coeff &n1,const Z_mod<P> &n2 )
{
     return Z_mod<P> ( n1 * n2.n );
};

template<prime P>
Z_mod<P> operator* ( const Z_mod<P> &n1,const int_coeff &n2 )
{
     return Z_mod<P> ( n1.n*n2 );
};

template<prime P>
void operator *= ( Z_mod<P> &n,const int_coeff &i )
{
     n.n = ( n.n*i ) %P;
};

template<prime P>
void operator *= ( Z_mod<P> &n1,const Z_mod<P> &n2 )
{
     n1.n = ( n1.n * n2.n ) % P;
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  Q (Rationals)
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


Q Q::reduce()
{
     int_coeff c { gcd ( p,q ) };
     p = p/c;
     q = q/c;
     return *this;
};

int_coeff gcd ( int_coeff p, int_coeff q )
{
     // recursive implementation of the greatest common divisor
     if ( p == 0 ) {
          return q;
     } else {
          return gcd ( q % p,p );
     }
};


std::vector<int> continued_fraction ( int p, int q )
{
     /// \todo make sure this function is safe (user input)
     std::vector<int> output {};
     if ( q < 0 ) {
          p *= -1;
          q *= -1;
     };
     int next {p/q};
     int r {0};
     while ( q != 1 ) {
          //  (a/b)*b + a%b == a
          // a/b truncating towards zero
          r = p % q;
          if ( next % 2 != 0 ) {
               if ( r > 0 ) {
                    next += 1;
                    r -= q;
               } else if ( r < 0 ) {
                    next -= 1;
                    r += q;
               };
          };
          output.push_back ( next );
          p = q;
          q = r;
          if ( q < 0 ) {
               p *= -1;
               q *= -1;
          };
          next = p / q;
     };
     output.push_back ( next );
     return output;
}

bool coeff_is_inv ( const Q &r )
{
     // check if p/q is invertible.
     return r.p != 0;
};
bool is_id ( const Q &r )
{
     // check if p/q is the identity.
     return r.p == r.q;
};
bool coeff_is_0 ( const Q &r )
{
     // check if n is equal to 0.
     return r.p == 0;
};
std::string coeff_to_string ( Q r )
{
     r.reduce();
     if ( r.q == 1 ) {
          return std::to_string ( r.p );
     } else if ( r.q == -1 ) {
          return std::to_string ( -r.p );
     } else if ( r.q<0 ) {
          r.p = -r.p;
          r.q = -r.q;
     };
     return std::to_string ( r.p )+"/"+std::to_string ( r.q );
};
std::string which_coeff ( const Q &r )
{
     return "Q";
};

std::ostream& operator<< ( std::ostream &out, const Q &r )
{
     // nice output, so it behaves like integers
     if ( r.q < 0 ) {
          out << -r.p << "/" << -r.q;
     } else {
          out << r.p << "/" << r.q;
     };
     return out;
};

bool operator< ( const Q &r1,const Q &r2 )
{
     if ( r2.q * r1.q > 0 ) {
          return r1.p * r2.q < r1.q * r2.p;
     };
     return r1.p * r2.q > r1.q * r2.p;
};
bool operator == ( const Q &r1,const Q &r2 )
{
     return r1.p * r2.q == r1.q * r2.p;
};
bool operator != ( const Q &r1,const Q &r2 )
{
     return r1.p * r2.q != r1.q * r2.p;
};
Q operator+ ( const Q &r1,const Q &r2 )
{
     return Q ( r1.p * r2.q + r2.p * r1.q,r1.q * r2.q ).reduce();
};
void operator += ( Q &r1,const Q &r2 )
{
     r1.p *= r2.q;
     r1.p += r2.p * r1.q;
     r1.q *= r2.q;
     r1.reduce();
};
Q operator- ( const Q &r )
{
     return Q ( -r.p,r.q );
};

Q operator* ( const Q &r1,const Q &r2 )
{
     return Q ( r1.p * r2.p,r1.q * r2.q ).reduce();
};
Q operator* ( const int_coeff &n1,const Q &r2 )
{
     return Q ( n1 * r2.p,r2.q ).reduce();
};
Q operator* ( const Q &r1,const int_coeff &n2 )
{
     return Q ( n2 * r1.p,r1.q ).reduce();
};
void operator *= ( Q &r,const int_coeff &n )
{
     r.p *= n;
     r.reduce();
};
void operator *= ( Q &r1,const Q &r2 )
{
     r1.p *= r2.p;
     r1.q *= r2.q;
     r1.reduce();
};

Q operator/ ( const int_coeff &n1,const Q &r2 )
{
     if ( r2.p == 0 ) {
          std::cerr << "Q::operator/(int_coeff,Q): Cannot divide by zero!";
          abort();
     }
     return Q ( n1 * r2.q,r2.p ).reduce();
};



#endif
