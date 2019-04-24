// Created on: 1995-03-01
// Created by: Arnaud BOUZY
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

#ifndef _DsgPrs_RadiusPresentation_HeaderFile
#define _DsgPrs_RadiusPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Prs3d_Presentation.hxx>

class TCollection_ExtendedString;
class gp_Pnt;
class gp_Circ;


//! A framework to define display of radii.
class DsgPrs_RadiusPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the point AttachmentPoint, the circle aCircle,
  //! the text aText, and the parameters firstparam and
  //! lastparam to the presentation object aPresentation.
  //! The display attributes of these elements is defined by
  //! the attribute manager aDrawer.
  //! If the Boolean drawFromCenter is false, the
  //! arrowhead will point towards the center of aCircle.
  //! If the Boolean reverseArrow is true, the arrowhead
  //! will point away from the attachment point.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const TCollection_ExtendedString& aText, const gp_Pnt& AttachmentPoint, const gp_Circ& aCircle, const Standard_Real firstparam, const Standard_Real lastparam, const Standard_Boolean drawFromCenter = Standard_True, const Standard_Boolean reverseArrow = Standard_False);
  
  //! Adds the point AttachmentPoint, the circle aCircle,
  //! the text aText, and the parameters firstparam and
  //! lastparam to the presentation object aPresentation.
  //! The display attributes of these elements is defined by
  //! the attribute manager aDrawer.
  //! The value of the enumeration Arrowside determines
  //! the type of arrow displayed: whether there will be
  //! arrowheads at both ends or only one, for example.
  //! If the Boolean drawFromCenter is false, the
  //! arrowhead will point towards the center of aCircle.
  //! If the Boolean reverseArrow is true, the arrowhead
  //! will point away from the attachment point.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const TCollection_ExtendedString& aText, const gp_Pnt& AttachmentPoint, const gp_Circ& aCircle, const Standard_Real firstparam, const Standard_Real lastparam, const DsgPrs_ArrowSide ArrowSide, const Standard_Boolean drawFromCenter = Standard_True, const Standard_Boolean reverseArrow = Standard_False);
  
  //! Adds the circle aCircle, the text aText, the points
  //! AttachmentPoint, Center and EndOfArrow to the
  //! presentation object aPresentation.
  //! The display attributes of these elements is defined by
  //! the attribute manager aDrawer.
  //! The value of the enumeration Arrowside determines
  //! the type of arrow displayed: whether there will be
  //! arrowheads at both ends or only one, for example.
  //! If the Boolean drawFromCenter is false, the
  //! arrowhead will point towards the center of aCircle.
  //! If the Boolean reverseArrow is true, the arrowhead
  //! will point away from the attachment point.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const TCollection_ExtendedString& aText, const gp_Pnt& AttachmentPoint, const gp_Pnt& Center, const gp_Pnt& EndOfArrow, const DsgPrs_ArrowSide ArrowSide, const Standard_Boolean drawFromCenter = Standard_True, const Standard_Boolean reverseArrow = Standard_False);




protected:





private:





};







#endif // _DsgPrs_RadiusPresentation_HeaderFile
