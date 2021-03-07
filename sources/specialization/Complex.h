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



template class Complex_Base<CobObj,CobMor,VALUE>;
template class Complex_Base<BNObj,BNMor,VALUE>;
template class Complex<CobObj,CobMor,VALUE>;
template class Complex<BNObj,BNMor,VALUE>;

#ifndef nonconvertibles
template Complex<BNObj,BNMor,VALUE> to_coeff ( const Complex<BNObj,BNMor,Z_mod<integer_simulant>> &cx );
template Complex<CobObj,CobMor,VALUE> to_coeff ( const Complex<CobObj,CobMor,Z_mod<integer_simulant>> &cx );
#endif

template bool NonZero ( const Eigen::Index &row, 
               const Eigen::Index &col, 
               const CobMor<VALUE> &value );
template bool NonZero ( const Eigen::Index &row, 
               const Eigen::Index &col, 
               const BNMor<VALUE> &value );

#undef VALUE
