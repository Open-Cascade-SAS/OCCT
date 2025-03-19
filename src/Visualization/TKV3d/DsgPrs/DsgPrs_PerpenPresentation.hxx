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

#ifndef _DsgPrs_PerpenPresentation_HeaderFile
#define _DsgPrs_PerpenPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>

class gp_Pnt;

//! A framework to define display of perpendicular
//! constraints between shapes.
class DsgPrs_PerpenPresentation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Defines the display of elements showing
  //! perpendicular constraints between shapes.
  //! These include the two axis points pAx1 and pAx2,
  //! the two points pnt1 and pnt2, the offset point
  //! OffsetPoint and the two Booleans intOut1} and intOut2{.
  //! These arguments are added to the presentation
  //! object aPresentation. Their display attributes are
  //! defined by the attribute manager aDrawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const gp_Pnt&                     pAx1,
                                  const gp_Pnt&                     pAx2,
                                  const gp_Pnt&                     pnt1,
                                  const gp_Pnt&                     pnt2,
                                  const gp_Pnt&                     OffsetPoint,
                                  const Standard_Boolean            intOut1,
                                  const Standard_Boolean            intOut2);

protected:
private:
};

#endif // _DsgPrs_PerpenPresentation_HeaderFile
