// Created on: 1997-02-10
// Created by: Odile Olivier
// Copyright (c) 1997-1999 Matra Datavision
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

#include <DsgPrs_XYZAxisPresentation.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_Text.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_TextAspect.hxx>

//=================================================================================================

void DsgPrs_XYZAxisPresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
                                     const Handle(Prs3d_LineAspect)&   aLineAspect,
                                     const gp_Dir&                     aDir,
                                     const Standard_Real               aVal,
                                     const Standard_CString            theText,
                                     const gp_Pnt&                     aPfirst,
                                     const gp_Pnt&                     aPlast)
{
  Handle(Graphic3d_Group) G = aPresentation->CurrentGroup();
  G->SetPrimitivesAspect(aLineAspect->Aspect());

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPfirst);
  aPrims->AddVertex(aPlast);
  G->AddPrimitiveArray(aPrims);

  Prs3d_Arrow::Draw(aPresentation->CurrentGroup(), aPlast, aDir, M_PI / 180. * 10., aVal / 10.);

  if (*theText != '\0')
  {
    Handle(Graphic3d_Text) aText = new Graphic3d_Text(1.0f / 81.0f);
    aText->SetText(theText);
    aText->SetPosition(aPlast);
    aPresentation->CurrentGroup()->AddText(aText);
  }
}

void DsgPrs_XYZAxisPresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
                                     const Handle(Prs3d_LineAspect)&   aLineAspect,
                                     const Handle(Prs3d_ArrowAspect)&  anArrowAspect,
                                     const Handle(Prs3d_TextAspect)&   aTextAspect,
                                     const gp_Dir&                     aDir,
                                     const Standard_Real               aVal,
                                     const Standard_CString            theText,
                                     const gp_Pnt&                     aPfirst,
                                     const gp_Pnt&                     aPlast)
{
  Handle(Graphic3d_Group) G = aPresentation->CurrentGroup();
  G->SetPrimitivesAspect(aLineAspect->Aspect());

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPfirst);
  aPrims->AddVertex(aPlast);
  G->AddPrimitiveArray(aPrims);

  G->SetPrimitivesAspect(anArrowAspect->Aspect());
  Prs3d_Arrow::Draw(aPresentation->CurrentGroup(),
                    aPlast,
                    aDir,
                    anArrowAspect->Angle(),
                    aVal / 10.);

  G->SetPrimitivesAspect(aTextAspect->Aspect());

  if (*theText != '\0')
  {
    Handle(Graphic3d_Text) aText = new Graphic3d_Text(1.0f / 81.0f);
    aText->SetText(theText);
    aText->SetPosition(aPlast);
    aPresentation->CurrentGroup()->AddText(aText);
  }
}
