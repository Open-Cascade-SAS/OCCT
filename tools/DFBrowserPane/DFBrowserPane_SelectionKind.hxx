// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef DFBrowserPane_SelectionKind_H
#define DFBrowserPane_SelectionKind_H

//! Selection kinds returned by attribute pane
enum DFBrowserPane_SelectionKind
{
  DFBrowserPane_SelectionKind_ExportToBREP,        //!< Click on "Export to BREP" button
  DFBrowserPane_SelectionKind_ExportToShapeViewer, //!< Click on "Export to ShapeViewer" button
  DFBrowserPane_SelectionKind_LabelReferences,     //!< Select label's reference (reserved)
  DFBrowserPane_SelectionKind_AttributeReferences  //!< Select Attribute reference (reserved)
};

#endif // _DFBrowserPane_SelectionKind_H
