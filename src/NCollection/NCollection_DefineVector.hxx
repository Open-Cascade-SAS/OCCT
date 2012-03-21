// Created on: 2002-04-23
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//            Automatically created from NCollection_Vector.hxx by GAWK


#ifndef NCollection_DefineVector_HeaderFile
#define NCollection_DefineVector_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
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
