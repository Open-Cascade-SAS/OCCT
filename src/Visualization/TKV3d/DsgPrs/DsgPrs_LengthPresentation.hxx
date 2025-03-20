// Created on: 1993-06-03
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _DsgPrs_LengthPresentation_HeaderFile
#define _DsgPrs_LengthPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Prs3d_Drawer.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Prs3d_Presentation.hxx>

class TCollection_ExtendedString;
class gp_Pnt;
class gp_Dir;
class gp_Pln;
class Geom_Surface;

//! Framework for displaying lengths.
//! The length displayed is indicated by line segments
//! and text alone or by a combination of line segment,
//! text and   arrows at either or both of its ends.
class DsgPrs_LengthPresentation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Draws a line segment representing a length in the
  //! display aPresentation.
  //! This segment joins the points AttachmentPoint1 and
  //! AttachmentPoint2, along the direction aDirection.
  //! The text aText will be displayed at the offset point OffsetPoint.
  //! The line and text attributes are specified by the
  //! attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const TCollection_ExtendedString& aText,
                                  const gp_Pnt&                     AttachmentPoint1,
                                  const gp_Pnt&                     AttachmentPoint2,
                                  const gp_Dir&                     aDirection,
                                  const gp_Pnt&                     OffsetPoint);

  //! Draws a line segment representing a length in the
  //! display aPresentation.
  //! This segment joins the points AttachmentPoint1 and
  //! AttachmentPoint2, along the direction aDirection.
  //! The text aText will be displayed at the offset point
  //! OffsetPoint. The value of the enumeration ArrowSide
  //! controls whether arrows will be displayed at either or
  //! both ends of the length.
  //! The line, text and arrow attributes are specified by the
  //! attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const TCollection_ExtendedString& aText,
                                  const gp_Pnt&                     AttachmentPoint1,
                                  const gp_Pnt&                     AttachmentPoint2,
                                  const gp_Dir&                     aDirection,
                                  const gp_Pnt&                     OffsetPoint,
                                  const DsgPrs_ArrowSide            ArrowSide);

  //! Draws a line segment representing a length in the
  //! display aPresentation.
  //! This segment joins the points AttachmentPoint1 and
  //! AttachmentPoint2, along the direction aDirection.
  //! The text aText will be displayed at the offset point
  //! OffsetPoint. The value of the enumeration ArrowSide
  //! controls whether arrows will be displayed at either or
  //! both ends of the length.
  //! The plane PlaneOfFaces is used if length is null.
  //! The line, text and arrow attributes are specified by the
  //! attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const TCollection_ExtendedString& aText,
                                  const gp_Pnt&                     AttachmentPoint1,
                                  const gp_Pnt&                     AttachmentPoint2,
                                  const gp_Pln&                     PlaneOfFaces,
                                  const gp_Dir&                     aDirection,
                                  const gp_Pnt&                     OffsetPoint,
                                  const DsgPrs_ArrowSide            ArrowSide);

  //! Draws a line segment representing a length in the
  //! display aPresentation.
  //! This segment joins the points AttachmentPoint1 and
  //! AttachmentPoint2, along the direction
  //! aDirection.   AttachmentPoint2 lies on the curvilinear
  //! faces SecondSurf. The text aText will be displayed at
  //! the offset point OffsetPoint. The value of the
  //! enumeration ArrowSide controls whether arrows will
  //! be displayed at either or both ends of the length.
  //! The line, text and arrow attributes are specified by the
  //! attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const TCollection_ExtendedString& aText,
                                  const Handle(Geom_Surface)&       SecondSurf,
                                  const gp_Pnt&                     AttachmentPoint1,
                                  const gp_Pnt&                     AttachmentPoint2,
                                  const gp_Dir&                     aDirection,
                                  const gp_Pnt&                     OffsetPoint,
                                  const DsgPrs_ArrowSide            ArrowSide);

  //! Draws a line segment representing a length in the
  //! display aPresentation.
  //! This segment joins the points AttachmentPoint1 and
  //! AttachmentPoint2, along the direction aDirection.
  //! The value of the enumeration ArrowSide controls
  //! whether arrows will be displayed at either or both ends of the length.
  //! The line and arrow attributes are specified by the attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const gp_Pnt&                     Pt1,
                                  const gp_Pnt&                     Pt2,
                                  const DsgPrs_ArrowSide            ArrowSide);

protected:
private:
};

#endif // _DsgPrs_LengthPresentation_HeaderFile
