// File:        NCollection_DefineArray1.hxx
// Created:     15.04.02 17:05:16
// Author:      Alexander Kartomin (akm)
//              <a-kartomin@opencascade.com>
//            Automatically created from NCollection_Array1.hxx by GAWK
// Copyright:   Open Cascade 2002
//              
// Purpose:     The class Array2 represents unidimensional arrays 
//              of fixed size known at run time. 
//              The range of the index is user defined.
//              An array1 can be constructed with a "C array".
//              This functionality is useful to call methods expecting
//              an Array1. It allows to carry the bounds inside the arrays.
//              
// Examples:    Item tab[100]; //  An example with a C array
//              Array1OfItem ttab (tab[0],1,100);
//              
//              Array1OfItem tttab (ttab(10),10,20); // a slice of ttab
//              
//              If you want to reindex an array from 1 to Length do :
//              
//              Array1 tab1(tab(tab.Lower()),1,tab.Length());
//                          
// Warning:     Programs client of such a class must be independant
//              of the range of the first element. Then, a C++ for
//              loop must be written like this
//              
//              for (i = A.Lower(); i <= A.Upper(); i++)
//              
// Changes:     In  comparison  to  TCollection  the  flag  isAllocated  was
//              renamed into myDeletable (alike in  the Array2).  For naming
//              compatibility the method IsAllocated remained in class along
//              with IsDeletable.
//              

#ifndef NCollection_DefineArray1_HeaderFile
#define NCollection_DefineArray1_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Array1.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// *********************************************** Template for Array1 class

#define DEFINE_ARRAY1(_ClassName_, _BaseCollection_, TheItemType)              \
        typedef NCollection_Array1<TheItemType > _ClassName_;

#endif
