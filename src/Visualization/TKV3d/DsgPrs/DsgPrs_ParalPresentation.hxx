// Created on: 1995-11-28
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

#ifndef _DsgPrs_ParalPresentation_HeaderFile
#define _DsgPrs_ParalPresentation_HeaderFile

#include <Prs3d_Drawer.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Prs3d_Presentation.hxx>

class TCollection_ExtendedString;
class gp_Pnt;
class gp_Dir;

//! A framework to define display of relations of parallelism between shapes.
class DsgPrs_ParalPresentation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Defines the display of elements showing relations of
  //! parallelism between shapes.
  //! These include the two points of attachment
  //! AttachmentPoint1 and AttachmentPoint1, the
  //! direction aDirection, and the offset point OffsetPoint.
  //! These arguments are added to the presentation
  //! object aPresentation. Their display attributes are
  //! defined by the attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const TCollection_ExtendedString& aText,
                                  const gp_Pnt&                     AttachmentPoint1,
                                  const gp_Pnt&                     AttachmentPoint2,
                                  const gp_Dir&                     aDirection,
                                  const gp_Pnt&                     OffsetPoint);

  //! Defines the display of elements showing relations of
  //! parallelism between shapes.
  //! These include the two points of attachment
  //! AttachmentPoint1 and AttachmentPoint1, the
  //! direction aDirection, the offset point OffsetPoint and
  //! the text aText.
  //! These arguments are added to the presentation
  //! object aPresentation. Their display attributes are
  //! defined by the attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const TCollection_ExtendedString& aText,
                                  const gp_Pnt&                     AttachmentPoint1,
                                  const gp_Pnt&                     AttachmentPoint2,
                                  const gp_Dir&                     aDirection,
                                  const gp_Pnt&                     OffsetPoint,
                                  const DsgPrs_ArrowSide            ArrowSide);

protected:
private:
};

#endif // _DsgPrs_ParalPresentation_HeaderFile
