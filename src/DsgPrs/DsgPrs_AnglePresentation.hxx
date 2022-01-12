// Created on: 1995-02-07
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

#ifndef _DsgPrs_AnglePresentation_HeaderFile
#define _DsgPrs_AnglePresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Prs3d_Drawer.hxx>
#include <Standard_Real.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Prs3d_Presentation.hxx>

class TCollection_ExtendedString;
class gp_Circ;
class gp_Pnt;
class gp_Dir;
class gp_Ax1;


//! A framework for displaying angles.
class DsgPrs_AnglePresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Draws the presenation of the full angle of a cone.
  //! VminCircle - a circle at V parameter = Vmin
  //! VmaxCircle - a circle at V parameter = Vmax
  //! aCircle - a circle at V parameter from projection of aPosition to axis of the cone
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real aVal, const TCollection_ExtendedString& aText, const gp_Circ& aCircle, const gp_Pnt& aPosition, const gp_Pnt& Apex, const gp_Circ& VminCircle, const gp_Circ& VmaxCircle, const Standard_Real aArrowSize);
  
  //! Draws the representation of the angle
  //! defined by dir1 and dir2, centered on
  //! CenterPoint, using the offset point OffsetPoint.
  //! Lines are drawn to points AttachmentPoint1 and AttachmentPoint2
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real theval, const gp_Pnt& CenterPoint, const gp_Pnt& AttachmentPoint1, const gp_Pnt& AttachmentPoint2, const gp_Dir& dir1, const gp_Dir& dir2, const gp_Pnt& OffsetPoint);
  
  //! Same  as above, but <thevalstring> contains conversion
  //! in Session units....
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real theval, const TCollection_ExtendedString& thevalstring, const gp_Pnt& CenterPoint, const gp_Pnt& AttachmentPoint1, const gp_Pnt& AttachmentPoint2, const gp_Dir& dir1, const gp_Dir& dir2, const gp_Pnt& OffsetPoint);
  
  //! Same  as above, may add one  or
  //! two Arrows  according to  <ArrowSide>  value
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real theval, const TCollection_ExtendedString& thevalstring, const gp_Pnt& CenterPoint, const gp_Pnt& AttachmentPoint1, const gp_Pnt& AttachmentPoint2, const gp_Dir& dir1, const gp_Dir& dir2, const gp_Pnt& OffsetPoint, const DsgPrs_ArrowSide ArrowSide);
  
  //! Same  as above, but axisdir contains the axis direction
  //! useful for Revol that can be opened with 180 degrees
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real theval, const TCollection_ExtendedString& thevalstring, const gp_Pnt& CenterPoint, const gp_Pnt& AttachmentPoint1, const gp_Pnt& AttachmentPoint2, const gp_Dir& dir1, const gp_Dir& dir2, const gp_Dir& axisdir, const gp_Pnt& OffsetPoint);
  
  //! Same  as above,may add one  or
  //! two Arrows  according to  <ArrowSide>  value
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real theval, const TCollection_ExtendedString& thevalstring, const gp_Pnt& CenterPoint, const gp_Pnt& AttachmentPoint1, const gp_Pnt& AttachmentPoint2, const gp_Dir& dir1, const gp_Dir& dir2, const gp_Dir& axisdir, const Standard_Boolean isPlane, const gp_Ax1& AxisOfSurf, const gp_Pnt& OffsetPoint, const DsgPrs_ArrowSide ArrowSide);
  
  //! simple representation of a poor lonesome angle dimension
  //! Draw a line from <theCenter>   to <AttachmentPoint1>, then operates
  //! a rotation around the perpmay add one  or
  //! two Arrows  according to  <ArrowSide>  value.  The
  //! attributes (color,arrowsize,...) are driven by the Drawer.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Real theval, const gp_Pnt& theCenter, const gp_Pnt& AttachmentPoint1, const gp_Ax1& theAxe, const DsgPrs_ArrowSide ArrowSide);




protected:





private:





};







#endif // _DsgPrs_AnglePresentation_HeaderFile
