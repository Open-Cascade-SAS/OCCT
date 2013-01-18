// Created on: 1996-03-18
// Created by: Flore Lantheaume
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <DsgPrs_ConcentricPresentation.ixx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>

#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include <ElCLib.hxx>

void DsgPrs_ConcentricPresentation::Add(
			   const Handle(Prs3d_Presentation)& aPresentation,
			   const Handle(Prs3d_Drawer)& aDrawer,
			   const gp_Pnt& aCenter,
			   const Standard_Real aRadius,
			   const gp_Dir& aNorm,
			   const gp_Pnt& aPoint)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();

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
