// Created on: 1995-03-08
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StdSelect_TypeOfEdge_HeaderFile
#define _StdSelect_TypeOfEdge_HeaderFile

//! Provides values for different types of edges. These
//! values are used to filter edges in frameworks
//! inheriting StdSelect_EdgeFilter.
enum StdSelect_TypeOfEdge
{
StdSelect_AnyEdge,
StdSelect_Line,
StdSelect_Circle
};

#endif // _StdSelect_TypeOfEdge_HeaderFile
