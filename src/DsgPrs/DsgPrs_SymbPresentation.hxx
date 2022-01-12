// Created on: 1995-12-08
// Created by: Jean-Pierre COMBE
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

#ifndef _DsgPrs_SymbPresentation_HeaderFile
#define _DsgPrs_SymbPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>

class TCollection_ExtendedString;
class gp_Pnt;

//! A framework to define display of symbols.
class DsgPrs_SymbPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the text aText and the point OffsetPoint to the
  //! presentation object aPresentation.
  //! The display attributes of the shaded plane are
  //! defined by the attribute manager aDrawer.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const TCollection_ExtendedString& aText, const gp_Pnt& OffsetPoint);




protected:





private:





};







#endif // _DsgPrs_SymbPresentation_HeaderFile
