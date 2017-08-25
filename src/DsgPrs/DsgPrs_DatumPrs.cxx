// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <DsgPrs_DatumPrs.hxx>
#include <DsgPrs_XYZAxisPresentation.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void DsgPrs_DatumPrs::Add (const Handle(Prs3d_Presentation)& thePresentation,
                           const gp_Ax2& theDatum,
                           const Handle(Prs3d_Drawer)& theDrawer)
{
  Handle(Prs3d_DatumAspect) aDatumAspect = theDrawer->DatumAspect();
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePresentation);

  gp_Ax2 anAxis (theDatum);
  gp_Pnt anOrigin = anAxis.Location();
  gp_Dir aXDir = anAxis.XDirection();
  gp_Dir aYDir = anAxis.YDirection();
  gp_Dir aZDir = anAxis.Direction();

  Standard_Real anAxisLength;
  const Standard_Boolean toDrawLabels = theDrawer->DatumAspect()->ToDrawLabels();

  Prs3d_DatumAxes anAxes = aDatumAspect->DatumAxes();
  Handle(Prs3d_ArrowAspect) anArrowAspect = aDatumAspect->ArrowAspect();
  Handle(Prs3d_TextAspect) aTextAspect = theDrawer->TextAspect();

  if ((anAxes & Prs3d_DA_XAxis) != 0)
  {
    anAxisLength = aDatumAspect->Attribute (Prs3d_DA_XAxisLength);
    const gp_Pnt aPoint1 (anOrigin.XYZ() + aXDir.XYZ()*anAxisLength);
    DsgPrs_XYZAxisPresentation::Add (thePresentation, aDatumAspect->LineAspect(Prs3d_DP_XAxis), anArrowAspect,
                                     aTextAspect, aXDir, anAxisLength, toDrawLabels ? "X" : "", anOrigin, aPoint1);
  }

  if ((anAxes & Prs3d_DA_YAxis) != 0)
  {
    anAxisLength = aDatumAspect->Attribute (Prs3d_DA_YAxisLength);
    const gp_Pnt aPoint2 (anOrigin.XYZ() + aYDir.XYZ()*anAxisLength);
    DsgPrs_XYZAxisPresentation::Add (thePresentation, aDatumAspect->LineAspect(Prs3d_DP_YAxis), anArrowAspect,
                                     aTextAspect, aYDir, anAxisLength, toDrawLabels ? "Y" : "", anOrigin, aPoint2);
  }

  if ((anAxes & Prs3d_DA_ZAxis) != 0)
  {
    anAxisLength = aDatumAspect->Attribute (Prs3d_DA_ZAxisLength);
    const gp_Pnt aPoint3 (anOrigin.XYZ() + aZDir.XYZ()*anAxisLength);
    DsgPrs_XYZAxisPresentation::Add (thePresentation, aDatumAspect->LineAspect(Prs3d_DP_ZAxis), anArrowAspect,
                                     aTextAspect, aZDir, anAxisLength, toDrawLabels ? "Z" : "", anOrigin, aPoint3);
  }
}
