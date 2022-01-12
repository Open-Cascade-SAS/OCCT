// Created on: 1998-01-26
// Created by: Sergey ZARITCHNY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _DsgPrs_EllipseRadiusPresentation_HeaderFile
#define _DsgPrs_EllipseRadiusPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Prs3d_Drawer.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Prs3d_Presentation.hxx>

class TCollection_ExtendedString;
class gp_Pnt;
class gp_Elips;
class Geom_OffsetCurve;

class DsgPrs_EllipseRadiusPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! draws a  Radius  (Major  or  Minor)
  //! representation for whole ellipse  case
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real theval, const TCollection_ExtendedString& aText, const gp_Pnt& AttachmentPoint, const gp_Pnt& anEndOfArrow, const gp_Pnt& aCenter, const Standard_Boolean IsMaxRadius, const DsgPrs_ArrowSide ArrowSide);
  
  //! draws a  Radius  (Major  or  Minor) representation
  //! for arc of an ellipse  case
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real theval, const TCollection_ExtendedString& aText, const gp_Elips& anEllipse, const gp_Pnt& AttachmentPoint, const gp_Pnt& anEndOfArrow, const gp_Pnt& aCenter, const Standard_Real uFirst, const Standard_Boolean IsInDomain, const Standard_Boolean IsMaxRadius, const DsgPrs_ArrowSide ArrowSide);
  
  //! draws a  Radius  (Major  or  Minor) representation
  //! for arc of an offset  curve  from  ellipse
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real theval, const TCollection_ExtendedString& aText, const Handle(Geom_OffsetCurve)& aCurve, const gp_Pnt& AttachmentPoint, const gp_Pnt& anEndOfArrow, const gp_Pnt& aCenter, const Standard_Real uFirst, const Standard_Boolean IsInDomain, const Standard_Boolean IsMaxRadius, const DsgPrs_ArrowSide ArrowSide);




protected:





private:





};







#endif // _DsgPrs_EllipseRadiusPresentation_HeaderFile
