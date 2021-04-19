// Created on: 2002-04-15
// Created by: Alexander Kartomin (akm)
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
// Warning:     Programs client of such a class must be independent
//              of the range of the first element. Then, a C++ for
//              loop must be written like this
//              for (i = A.Lower(); i <= A.Upper(); i++)
// Changes:     In  comparison  to  TCollection  the  flag  isAllocated  was
//              renamed into myDeletable (alike in  the Array2).  For naming
//              compatibility the method IsAllocated remained in class along
//              with IsDeletable.

#ifndef NCollection_DefineArray1_HeaderFile
#define NCollection_DefineArray1_HeaderFile

#include <NCollection_Array1.hxx>

// *********************************************** Template for Array1 class

#define DEFINE_ARRAY1(_ClassName_, _BaseCollection_, TheItemType)              \
typedef NCollection_Array1<TheItemType > _ClassName_;

#endif
