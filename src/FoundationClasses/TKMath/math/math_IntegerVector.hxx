// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _math_IntegerVector_HeaderFile
#define _math_IntegerVector_HeaderFile

#include <math_VectorBase.hxx>

//! This class implements the real IntegerVector abstract data type.
//! IntegerVectors can have an arbitrary range which must be define at
//! the declaration and cannot be changed after this declaration.
//! Example:
//! @code
//!    math_IntegerVector V1(-3, 5); // an IntegerVector with range [-3..5]
//! @endcode
//!
//! IntegerVector is copied through assignment:
//! @code
//!    math_IntegerVector V2( 1, 9);
//!    ....
//!    V2 = V1;
//!    V1(1) = 2.0; // the IntegerVector V2 will not be modified.
//! @endcode
//!
//! The Exception RangeError is raised when trying to access outside
//! the range of an IntegerVector :
//! @code
//!    V1(11) = 0 // --> will raise RangeError;
//! @endcode
//!
//! The Exception DimensionError is raised when the dimensions of two
//! IntegerVectors are not compatible :
//! @code
//!    math_IntegerVector V3(1, 2);
//!    V3 = V1;    // --> will raise DimensionError;
//!    V1.Add(V3)  // --> will raise DimensionError;
//! @endcode
using math_IntegerVector = math_VectorBase<int>;

#endif
