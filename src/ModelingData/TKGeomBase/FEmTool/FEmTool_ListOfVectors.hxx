// Created on: 1997-10-29
// Created by: Roman BORISOV
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

#ifndef FEmTool_ListOfVectors_HeaderFile
#define FEmTool_ListOfVectors_HeaderFile

#include <TColStd_HArray1OfReal.hxx>
#include <NCollection_List.hxx>

//! To  define  list  of  segments with  non-zero  coefficients of constraint

typedef NCollection_List<Handle(TColStd_HArray1OfReal)> FEmTool_ListOfVectors;
typedef NCollection_List<Handle(TColStd_HArray1OfReal)>::Iterator
  FEmTool_ListIteratorOfListOfVectors;

#endif
