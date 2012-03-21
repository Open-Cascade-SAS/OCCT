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
#include <Graphic3d_Array1OfVertex.hxx>
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
  Standard_Integer nbp = 50;
  Standard_Real dteta = (2 * M_PI)/nbp;
  Graphic3d_Array1OfVertex V(1,nbp+1);
  gp_Pnt ptcur;
  Standard_Real ucur = 0;
  Standard_Integer i ;
  for ( i = 1; i<=nbp; i++) {
    ptcur = ElCLib::Value(ucur, Circ);
    V(i).SetCoord(ptcur.X(), ptcur.Y(),ptcur.Z());
    ucur = ucur + dteta;
  }
  V(nbp+1).SetCoord( V(1).X(), V(1).Y(), V(1).Z());
  Prs3d_Root::CurrentGroup(aPresentation)
    ->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  //Creation et discretisation du plus petit cercle
  Circ.SetRadius(aRadius/2);
  ucur = 0;
  for ( i = 1; i<=nbp; i++) {
    ptcur = ElCLib::Value(ucur, Circ);
    V(i).SetCoord(ptcur.X(), ptcur.Y(),ptcur.Z());
    ucur = ucur + dteta;
  }
  V(nbp+1).SetCoord( V(1).X(), V(1).Y(), V(1).Z());
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)
    ->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  //Creation de la croix
     //1er segment
  gp_Dir vecnorm(aPoint.XYZ() - aCenter.XYZ() );
  gp_Vec vec(vecnorm);
  vec.Multiply(aRadius);
  gp_Pnt p1 = aCenter.Translated(vec);
  gp_Pnt p2 = aCenter.Translated(-vec);
  
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->
    SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Graphic3d_Array1OfVertex VExt(1,2);
  VExt(1).SetCoord(p1.X(), p1.Y(), p1.Z());
  VExt(2).SetCoord(p2.X(), p2.Y(), p2.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VExt);

     //2ieme segment
  vec.Cross(aNorm);
  vecnorm.SetCoord(vec.X(), vec.Y(), vec.Z() );
  vec.SetXYZ(vecnorm.XYZ());
  vec.Multiply(aRadius);
  p1 = aCenter.Translated(vec);
  p2 = aCenter.Translated(-vec);
  VExt(1).SetCoord(p1.X(), p1.Y(), p1.Z());
  VExt(2).SetCoord(p2.X(), p2.Y(), p2.Z());

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->
    SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VExt);

}
