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

//            Automatically created from NCollection_Array1.hxx by GAWK
// Purpose:     The class Array2 represents unidimensional arrays 
//              of fixed size known at run time. 
//              The range of the index is user defined.
//              An array1 can be constructed with a "C array".
//              This functionality is useful to call methods expecting
//              an Array1. It allows to carry the bounds inside the arrays.
// Examples:    Item tab[100]; //  An example with a C array
//              Array1OfItem ttab (tab[0],1,100);
//              Array1OfItem tttab (ttab(10),10,20); // a slice of ttab
//              If you want to reindex an array from 1 to Length do :
//              Array1 tab1(tab(tab.Lower()),1,tab.Length());
// Warning:     Programs client of such a class must be independant
//              of the range of the first element. Then, a C++ for
//              loop must be written like this
//              for (i = A.Lower(); i <= A.Upper(); i++)
// Changes:     In  comparison  to  TCollection  the  flag  isAllocated  was
//              renamed into myDeletable (alike in  the Array2).  For naming
//              compatibility the method IsAllocated remained in class along
//              with IsDeletable.

#ifndef NCollection_DefineArray1_HeaderFile
#define NCollection_DefineArray1_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Array1.hxx>

// *********************************************** Template for Array1 class

#define DEFINE_ARRAY1(_ClassName_, _BaseCollection_, TheItemType)              \
typedef NCollection_Array1<TheItemType > _ClassName_;

#endif
