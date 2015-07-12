// Created on: 1996-08-21
// Created by: Jacques MINOT
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

#ifndef _DsgPrs_DiameterPresentation_HeaderFile
#define _DsgPrs_DiameterPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Prs3d_Presentation;
class TCollection_ExtendedString;
class gp_Pnt;
class gp_Circ;


//! A framework for displaying diameters in shapes.
class DsgPrs_DiameterPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Draws the diameter of the circle aCircle displayed in
  //! the presentation aPresentation and with attributes
  //! defined by the attribute manager aDrawer. The point
  //! AttachmentPoint defines the point of contact
  //! between the circle and the diameter presentation.
  //! The value of the enumeration ArrowSide controls
  //! whether arrows will be displayed at either or both
  //! ends of the length. The text aText labels the diameter.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const TCollection_ExtendedString& aText, const gp_Pnt& AttachmentPoint, const gp_Circ& aCircle, const DsgPrs_ArrowSide ArrowSide, const Standard_Boolean IsDiamSymbol);
  
  //! Draws the diameter of the arc anArc displayed in the
  //! presentation aPresentation and with attributes
  //! defined by the attribute manager aDrawer. The point
  //! AttachmentPoint defines the point of contact
  //! between the arc and the diameter presentation. The
  //! value of the enumeration ArrowSide controls whether
  //! arrows will be displayed at either or both ends of the
  //! length. The parameters uFirst and uLast define the
  //! first and last points of the arc. The text aText labels the diameter.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const TCollection_ExtendedString& aText, const gp_Pnt& AttachmentPoint, const gp_Circ& aCircle, const Standard_Real uFirst, const Standard_Real uLast, const DsgPrs_ArrowSide ArrowSide, const Standard_Boolean IsDiamSymbol);




protected:





private:





};







#endif // _DsgPrs_DiameterPresentation_HeaderFile
