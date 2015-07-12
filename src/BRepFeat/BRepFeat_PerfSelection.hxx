// Created on: 1995-06-13
// Created by: Jacques GOUSSARD
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

#ifndef _BRepFeat_PerfSelection_HeaderFile
#define _BRepFeat_PerfSelection_HeaderFile

//! To declare the type of selection semantics for local operation Perform methods
//! -   NoSelection
//! -   SelectionFU - selection of a face up to which a
//! local operation will be performed
//! -   SelectionU - selection of a point up to which a
//! local operation will be performed
//! -   SelectionSh - selection of a shape on which a
//! local operation will be performed
//! -   SelectionShU - selection of a shape up to which a
//! local operation will be performed.
enum BRepFeat_PerfSelection
{
BRepFeat_NoSelection,
BRepFeat_SelectionFU,
BRepFeat_SelectionU,
BRepFeat_SelectionSh,
BRepFeat_SelectionShU
};

#endif // _BRepFeat_PerfSelection_HeaderFile
