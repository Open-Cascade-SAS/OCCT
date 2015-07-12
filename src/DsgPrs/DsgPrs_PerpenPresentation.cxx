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


#include <DsgPrs_PerpenPresentation.hxx>
#include <ElCLib.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakeLin.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <StdPrs_Point.hxx>
#include <TCollection_AsciiString.hxx>

void DsgPrs_PerpenPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const gp_Pnt& pAx1,
				     const gp_Pnt& pAx2,
				     const gp_Pnt& pnt1,
				     const gp_Pnt& pnt2,
				     const gp_Pnt& OffsetPoint,
				     const Standard_Boolean intOut1,
				     const Standard_Boolean intOut2)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  LA->LineAspect()->SetTypeOfLine(Aspect_TOL_SOLID); // ou DOT ou DOTDASH
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  // segments
  Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPolylines(6,2);

  aPrims->AddBound(3);
  aPrims->AddVertex(OffsetPoint);
  aPrims->AddVertex(pAx1);
  aPrims->AddVertex(pAx2);

  // Symbol
  gp_Vec vec1(gce_MakeDir(OffsetPoint,pAx1));
  gp_Vec vec2(gce_MakeDir(OffsetPoint,pAx2));
  vec1 *= .2 * OffsetPoint.Distance(pAx1);
  vec2 *= .2 * OffsetPoint.Distance(pAx2);

  gp_Pnt pAx11 = OffsetPoint.Translated(vec1);
  gp_Pnt pAx22 = OffsetPoint.Translated(vec2);
  gp_Pnt p_symb = pAx22.Translated(vec1);

  aPrims->AddBound(3);
  aPrims->AddVertex(pAx11);
  aPrims->AddVertex(p_symb);
  aPrims->AddVertex(pAx22);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // points attache
  if (intOut1 || intOut2)
  {
    LA->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT); // ou DOT ou DOTDASH
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

    if (intOut1) {
      aPrims = new Graphic3d_ArrayOfSegments(2);
      aPrims->AddVertex(pAx1);
      aPrims->AddVertex(pnt1);
      Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
    }
    if (intOut2) {
      aPrims = new Graphic3d_ArrayOfSegments(2);
      aPrims->AddVertex(pAx2);
      aPrims->AddVertex(pnt2);
      Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
    }
  }
}
