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



template struct Z_mod<VALUE>;

template bool is_id ( const Z_mod<VALUE> &n );
template bool coeff_is_0 ( const Z_mod<VALUE> &n );
template std::string coeff_to_string ( const Z_mod<VALUE> &i );
template int which_P ( const Z_mod<VALUE> &n );
template std::string which_coeff ( const Z_mod<VALUE> &n );
template std::ostream& operator<< ( std::ostream &out,
                                    const Z_mod<VALUE> &n );
template Z_mod<VALUE> operator+ ( const Z_mod<VALUE> &n1,
                                  const Z_mod<VALUE> &n2 );
template void operator+= ( Z_mod<VALUE> &n1,
                           const Z_mod<VALUE> &n2 );
template Z_mod<VALUE> operator- ( const Z_mod<VALUE> &n );
template Z_mod<VALUE> operator* ( const Z_mod<VALUE> &n1,
                                  const Z_mod<VALUE> &n2 );
template Z_mod<VALUE> operator* ( const int_coeff &n1,
                                  const Z_mod<VALUE> &n2 );
template Z_mod<VALUE> operator* ( const Z_mod<VALUE> &n1,
                                  const int_coeff &n2 );
template void operator*= ( Z_mod<VALUE> &n,
                           const int_coeff &i );
template void operator*= ( Z_mod<VALUE> &n1,
                           const Z_mod<VALUE> &n2 );

#ifndef nonprime
template<>
bool coeff_is_inv ( const Z_mod<VALUE> &n )
{
     // check if n is invertible.
     return n.n % VALUE != 0;
};
template<>
Z_mod<VALUE> operator/ ( const int_coeff &n1,const Z_mod<VALUE> &n2 )
{
     if ( !coeff_is_inv(n2) ) {
          std::cerr << "Z_mod<P>::operator/ ( int_coeff,Z_mod<P> ): Cannot divide by a non-invertible element!"
                    << std::endl;
     } else {
          int_coeff out = 1;
          // using little Fermat: a^p=a mod p, so a^-1=a^(p-2)
          for ( int it = 2; it < VALUE; ++it ) {
               out *= n2.n;
          };
          return Z_mod<VALUE> ( n1 * out );
     };
     return Z_mod<VALUE> ( 1 ); // THE PROGRAM SHOULD NEVER GET HERE.
};
#else
template<>
bool coeff_is_inv ( const Z_mod<VALUE> &n )
{
     return ( n.n == 1 ) || ( n.n == -1 );
};
template<>
Z_mod<VALUE> operator/ ( const int_coeff &n1,const Z_mod<VALUE> &n2 )
{
     if ( !coeff_is_inv(n2) ) {
          std::cerr << "Z_mod<P>::operator/ ( int_coeff,Z_mod<P> ): Cannot divide by a non-invertible element!"
                    << std::endl;
     } else if (is_id(n2)) {
          return Z_mod<VALUE> ( n1 );
     };
     return Z_mod<VALUE> ( -n2 );
};
#endif

#undef VALUE

