// Created on: 1995-07-24
// Created by: Modelistation
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

#ifndef _StdPrs_Plane_HeaderFile
#define _StdPrs_Plane_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <Standard_Boolean.hxx>

class Prs3d_Presentation;
class Adaptor3d_Surface;


//! A framework to display infinite planes.
class StdPrs_Plane  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines display of infinite planes.
  //! The infinite plane aPlane is added to the display
  //! aPresentation, and the attributes of the display are
  //! defined by the attribute manager aDrawer.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Adaptor3d_Surface& aPlane, const Handle(Prs3d_Drawer)& aDrawer);
  
  //! returns true if the distance between the point (X,Y,Z) and the
  //! plane is less than aDistance.
  Standard_EXPORT static Standard_Boolean Match (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Adaptor3d_Surface& aPlane, const Handle(Prs3d_Drawer)& aDrawer);

};

#endif // _StdPrs_Plane_HeaderFile
