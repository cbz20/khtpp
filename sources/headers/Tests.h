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



#ifndef TESTS
#define TESTS
#include <iostream>
#include <vector>
#include <Eigen/SparseCore>
#include "Coefficients.h"
#include "Cob.h"
#include "BNAlgebra.h"
#include "Complex.h"
#include "Curves.h"
#include "constants.h"

/// \file Tests.h
/// \brief declares some tests for checking some features of the program

/// test for coefficients (Coefficients.h)
template<typename Coeff>
bool Test_ChainConversion(const int &details=0);

template<typename Coeff>
bool Test_Coefficients(const int &details=0);

/// test for cobordism category (Cob.h)
template<typename Coeff>
bool Test_Cob(const int &details=0);

/// test for complexes (Complex.h)
template<typename Coeff>
bool Test_Complex(const int &details=0);

// test for precurves (Precurves.h)
// template<typename Coeff>
// bool Test_Precurves(const int &details=0);

/// test for precomputed algebra (Cob.h)
template<typename Coeff>
bool Test_PrecomputedAlgebra(const int &details=0);

///< test for sparse matrices
// bool SparseMatrixTestAlgebra();



#endif
