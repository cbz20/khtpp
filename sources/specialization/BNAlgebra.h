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



// define struct Label
template class Label<VALUE>;
template class BNMor<VALUE>;

template bool label_is_zero ( const Label<VALUE> &label );
template bool BNMorNonZero ( const int &row,
                    const int &col,
                    const BNMor<VALUE> &value );

#ifndef nonconvertibles
template BNMor<VALUE> BNMor<Z_mod<integer_simulant>>::to_coeffs() const;
#endif

#undef VALUE
