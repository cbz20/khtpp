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



#ifndef nonconvertibles
template CobMor<VALUE> CobMor<Z_mod<integer_simulant>>::to_coeffs() const;
#endif
template class CobMor<VALUE>;
template class Deco<VALUE>;
template bool deco_is_zero ( const Deco<VALUE> &deco );

template bool CobNonZero (
     const int& row,
     const int& col,
     const CobMor<VALUE> &value );

#undef VALUE
