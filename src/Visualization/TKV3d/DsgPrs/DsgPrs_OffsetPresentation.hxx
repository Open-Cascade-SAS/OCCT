// Created on: 1996-09-18
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

#ifndef _DsgPrs_OffsetPresentation_HeaderFile
#define _DsgPrs_OffsetPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>

class TCollection_ExtendedString;
class gp_Pnt;
class gp_Dir;

//! A framework to define display of offsets.
class DsgPrs_OffsetPresentation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Defines the display of elements showing offset shapes.
  //! These include the two points of attachment
  //! AttachmentPoint1 and AttachmentPoint1, the two
  //! directions aDirection and aDirection2, and the offset point OffsetPoint.
  //! These arguments are added to the presentation
  //! object aPresentation. Their display attributes are
  //! defined by the attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const TCollection_ExtendedString& aText,
                                  const gp_Pnt&                     AttachmentPoint1,
                                  const gp_Pnt&                     AttachmentPoint2,
                                  const gp_Dir&                     aDirection,
                                  const gp_Dir&                     aDirection2,
                                  const gp_Pnt&                     OffsetPoint);

  //! draws the representation of axes alignment Constraint
  //! between the point AttachmentPoint1 and the
  //! point AttachmentPoint2, along direction
  //! aDirection, using the offset point OffsetPoint.
  Standard_EXPORT static void AddAxes(const Handle(Prs3d_Presentation)& aPresentation,
                                      const Handle(Prs3d_Drawer)&       aDrawer,
                                      const TCollection_ExtendedString& aText,
                                      const gp_Pnt&                     AttachmentPoint1,
                                      const gp_Pnt&                     AttachmentPoint2,
                                      const gp_Dir&                     aDirection,
                                      const gp_Dir&                     aDirection2,
                                      const gp_Pnt&                     OffsetPoint);

protected:
private:
};

#endif // _DsgPrs_OffsetPresentation_HeaderFile
