// Created on: 1996-03-19
// Created by: Flore Lantheaume
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

#ifndef _DsgPrs_Chamf2dPresentation_HeaderFile
#define _DsgPrs_Chamf2dPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Prs3d_Presentation.hxx>

class gp_Pnt;
class TCollection_ExtendedString;

//! Framework for display of 2D chamfers.
class DsgPrs_Chamf2dPresentation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Defines the display of elements showing 2D chamfers on shapes.
  //! These include the text aText, the point of attachment,
  //! aPntAttach and the end point aPntEnd.
  //! These arguments are added to the presentation
  //! object aPresentation. Their display attributes are
  //! defined by the attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const gp_Pnt&                     aPntAttach,
                                  const gp_Pnt&                     aPntEnd,
                                  const TCollection_ExtendedString& aText);

  //! Defines the display of texts, symbols and icons used
  //! to present 2D chamfers.
  //! These include the text aText, the point of attachment,
  //! aPntAttach and the end point aPntEnd.
  //! These arguments are added to the presentation
  //! object aPresentation. Their display attributes are
  //! defined by the attribute manager aDrawer. The arrow
  //! at the point of attachment has a display defined by a
  //! value of the enumeration DsgPrs_Arrowside.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const gp_Pnt&                     aPntAttach,
                                  const gp_Pnt&                     aPntEnd,
                                  const TCollection_ExtendedString& aText,
                                  const DsgPrs_ArrowSide            ArrowSide);

protected:
private:
};

#endif // _DsgPrs_Chamf2dPresentation_HeaderFile
