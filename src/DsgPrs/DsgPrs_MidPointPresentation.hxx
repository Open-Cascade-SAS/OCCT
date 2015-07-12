// Created on: 2000-10-20
// Created by: Julia DOROVSKIKH
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _DsgPrs_MidPointPresentation_HeaderFile
#define _DsgPrs_MidPointPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <Standard_Boolean.hxx>
class Prs3d_Presentation;
class gp_Ax2;
class gp_Pnt;
class gp_Circ;
class gp_Elips;



class DsgPrs_MidPointPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! draws the representation of a MidPoint between
  //! two vertices.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const gp_Ax2& theAxe, const gp_Pnt& MidPoint, const gp_Pnt& Position, const gp_Pnt& AttachPoint, const Standard_Boolean first);
  
  //! draws the representation of a MidPoint between
  //! two lines or linear segments.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const gp_Ax2& theAxe, const gp_Pnt& MidPoint, const gp_Pnt& Position, const gp_Pnt& AttachPoint, const gp_Pnt& Point1, const gp_Pnt& Point2, const Standard_Boolean first);
  
  //! draws the representation of a MidPoint between
  //! two entire circles or two circular arcs.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const gp_Circ& aCircle, const gp_Pnt& MidPoint, const gp_Pnt& Position, const gp_Pnt& AttachPoint, const gp_Pnt& Point1, const gp_Pnt& Point2, const Standard_Boolean first);
  
  //! draws the representation of a MidPoint between
  //! two entire ellipses or two elliptic arcs.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const gp_Elips& anElips, const gp_Pnt& MidPoint, const gp_Pnt& Position, const gp_Pnt& AttachPoint, const gp_Pnt& Point1, const gp_Pnt& Point2, const Standard_Boolean first);




protected:





private:





};







#endif // _DsgPrs_MidPointPresentation_HeaderFile
