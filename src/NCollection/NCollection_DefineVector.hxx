// File:      NCollection_DefineVector.hxx
// Created:   23.04.02 19:24:33
// Author:    Alexander GRIGORIEV
//            Automatically created from NCollection_Vector.hxx by GAWK
// Copyright: Open Cascade 2002


#ifndef NCollection_DefineVector_HeaderFile
#define NCollection_DefineVector_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Vector.hxx>

#ifdef WNT
// Disable the warning: "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

//  Class NCollection_Vector (dynamic array of objects)
//
// This class is similar to NCollection_Array1  though the indices always start
// at 0 (in Array1 the first index must be specified)
//
// The Vector is always created with 0 length. It can be enlarged by two means:
//   1. Calling the method Append (val) - then "val" is added to the end of the
//      vector (the vector length is incremented)
//   2. Calling the method SetValue (i, val)  - if "i" is greater than or equal
//      to the current length of the vector,  the vector is enlarged to accomo-
//      date this index
//
// The methods Append and SetValue return  a non-const reference  to the copied
// object  inside  the vector.  This reference  is guaranteed to be valid until
// the vector is destroyed. It can be used to access the vector member directly
// or to pass its address to other data structures.
//
// The vector iterator remembers the length of the vector  at the moment of the
// creation or initialisation of the iterator.   Therefore the iteration begins
// at index 0  and stops at the index equal to (remembered_length-1).  It is OK
// to enlarge the vector during the iteration.

#define DEFINE_VECTOR(_ClassName_, _BaseCollection_, TheItemType)              \
        typedef NCollection_Vector<TheItemType > _ClassName_;

#endif
