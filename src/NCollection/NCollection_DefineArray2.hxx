// Created on: 2002-04-15
// Created by: Alexander Kartomin (akm)
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

//            Automatically created from NCollection_Array2.hxx by GAWK
// Purpose:   The class Array2 represents bi-dimensional arrays 
//            of fixed size known at run time. 
//            The ranges of indices are user defined.
// Warning:   Programs clients of such class must be independant
//            of the range of the first element. Then, a C++ for
//            loop must be written like this
//            for (i = A.LowerRow(); i <= A.UpperRow(); i++)
//              for (j = A.LowerCol(); j <= A.UpperCol(); j++)

#ifndef NCollection_DefineArray2_HeaderFile
#define NCollection_DefineArray2_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Array2.hxx>

// *********************************************** Template for Array2 class

#define DEFINE_ARRAY2(_ClassName_, _BaseCollection_, TheItemType)              \
typedef NCollection_Array2<TheItemType > _ClassName_;

#endif
