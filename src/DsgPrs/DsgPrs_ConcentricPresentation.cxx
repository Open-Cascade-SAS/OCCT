// Created on: 1996-03-18
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


#include <DsgPrs_ConcentricPresentation.hxx>
#include <ElCLib.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>

void DsgPrs_ConcentricPresentation::Add(
			   const Handle(Prs3d_Presentation)& aPresentation,
			   const Handle(Prs3d_Drawer)& aDrawer,
			   const gp_Pnt& aCenter,
			   const Standard_Real aRadius,
			   const gp_Dir& aNorm,
			   const gp_Pnt& aPoint)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();

  //Creation et discretisation du plus gros cercle
  gp_Circ Circ(gp_Ax2(aCenter,aNorm), aRadius);
  const Standard_Integer nbp = 50;
  const Standard_Real dteta = (2. * M_PI)/nbp;

  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(2*nbp+6,4);

  gp_Pnt pt1 = ElCLib::Value(0., Circ);
  aPrims->AddBound(nbp+1);
  aPrims->AddVertex(pt1);
  Standard_Real ucur = dteta;
  Standard_Integer i ;
  for (i = 2; i<=nbp; i++, ucur += dteta)
    aPrims->AddVertex(ElCLib::Value(ucur, Circ));
  aPrims->AddVertex(pt1);

  //Creation et discretisation du plus petit cercle
  Circ.SetRadius(0.5*aRadius);
  pt1 = ElCLib::Value(0., Circ);
  aPrims->AddBound(nbp+1);
  aPrims->AddVertex(pt1);
  ucur = dteta;
  for (i = 2; i<=nbp; i++, ucur += dteta)
    aPrims->AddVertex(ElCLib::Value(ucur, Circ));
  aPrims->AddVertex(pt1);

  //Creation de la croix
     //1er segment
  gp_Dir vecnorm(aPoint.XYZ() - aCenter.XYZ());
  gp_Vec vec(vecnorm);
  vec.Multiply(aRadius);
  gp_Pnt p1 = aCenter.Translated(vec);
  gp_Pnt p2 = aCenter.Translated(-vec);

  aPrims->AddBound(2);
  aPrims->AddVertex(p1);
  aPrims->AddVertex(p2);

     //2ieme segment
  vec.Cross(aNorm);
  vecnorm.SetCoord(vec.X(), vec.Y(), vec.Z() );
  vec.SetXYZ(vecnorm.XYZ());
  vec.Multiply(aRadius);
  p1 = aCenter.Translated(vec);
  p2 = aCenter.Translated(-vec);

  aPrims->AddBound(2);
  aPrims->AddVertex(p1);
  aPrims->AddVertex(p2);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
}
