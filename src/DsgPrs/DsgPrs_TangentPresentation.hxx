// Created on: 1996-01-16
// Created by: Jean-Pierre COMBE
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _DsgPrs_TangentPresentation_HeaderFile
#define _DsgPrs_TangentPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <Standard_Real.hxx>
class Prs3d_Presentation;
class gp_Pnt;
class gp_Dir;


//! A framework to define display of tangents.
class DsgPrs_TangentPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the point OffsetPoint, the direction aDirection
  //! and the length aLength to the presentation object aPresentation.
  //! The display attributes of the tangent are defined by
  //! the attribute manager aDrawer.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const gp_Pnt& OffsetPoint, const gp_Dir& aDirection, const Standard_Real aLength);




protected:





private:





};







#endif // _DsgPrs_TangentPresentation_HeaderFile
