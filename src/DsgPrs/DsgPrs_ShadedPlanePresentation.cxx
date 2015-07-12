// Created on: 1997-12-05
// Created by: Robert COUBLANC
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


#include <DsgPrs_ShadedPlanePresentation.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void DsgPrs_ShadedPlanePresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
					 const Handle(Prs3d_Drawer)& aDrawer,	     
					 const gp_Pnt& aPt1, 
					 const gp_Pnt& aPt2,
					 const gp_Pnt& aPt3)
{
  Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
  TheGroup->SetPrimitivesAspect(aDrawer->PlaneAspect()->EdgesAspect()->Aspect());
  TheGroup->SetPrimitivesAspect(aDrawer->ShadingAspect()->Aspect());

  Handle(Graphic3d_ArrayOfPolygons) aPrims = new Graphic3d_ArrayOfPolygons(4);
  aPrims->AddVertex(aPt1);
  aPrims->AddVertex(aPt2);
  aPrims->AddVertex(aPt3);
  aPrims->AddVertex(aPt1);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
}
