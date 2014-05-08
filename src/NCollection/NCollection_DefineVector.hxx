// Created on: 2002-04-23
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

//            Automatically created from NCollection_Vector.hxx by GAWK


#ifndef NCollection_DefineVector_HeaderFile
#define NCollection_DefineVector_HeaderFile

#include <NCollection_Vector.hxx>

//  Class NCollection_Vector (dynamic array of objects)
// This class is similar to NCollection_Array1  though the indices always start
// at 0 (in Array1 the first index must be specified)
// The Vector is always created with 0 length. It can be enlarged by two means:
//   1. Calling the method Append (val) - then "val" is added to the end of the
//      vector (the vector length is incremented)
//   2. Calling the method SetValue (i, val)  - if "i" is greater than or equal
//      to the current length of the vector,  the vector is enlarged to accomo-
// The methods Append and SetValue return  a non-const reference  to the copied
// object  inside  the vector.  This reference  is guaranteed to be valid until
// the vector is destroyed. It can be used to access the vector member directly
// or to pass its address to other data structures.
// The vector iterator remembers the length of the vector  at the moment of the
// creation or initialisation of the iterator.   Therefore the iteration begins
// at index 0  and stops at the index equal to (remembered_length-1).  It is OK
// to enlarge the vector during the iteration.

#define DEFINE_VECTOR(_ClassName_, _BaseCollection_, TheItemType)              \
typedef NCollection_Vector<TheItemType > _ClassName_;

#endif
