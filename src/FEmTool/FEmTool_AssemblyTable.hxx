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

#ifndef FEmTool_AssemblyTable_HeaderFile
#define FEmTool_AssemblyTable_HeaderFile

#include <TColStd_HArray1OfInteger.hxx>
#include <NCollection_Array2.hxx>

//! To define the  table  [Freedom's degree] [Dimension,Element]
//! which gives Index  of Freedom's degree in the assembly problem.

typedef NCollection_Array2<Handle(TColStd_HArray1OfInteger)> FEmTool_AssemblyTable;


#endif
