// Created on: 1995-11-28
// Created by: Jean-Pierre COMBE
// Copyright (c) 1995-1999 Matra Datavision
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



#include <DsgPrs_PerpenPresentation.ixx>

#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <ElCLib.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakeDir.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <TCollection_AsciiString.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Array1OfVertex.hxx>

#include <StdPrs_Point.hxx>

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
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  LA->LineAspect()->SetTypeOfLine(Aspect_TOL_SOLID); // ou DOT ou DOTDASH
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  Graphic3d_Array1OfVertex V1(1,2);
  Graphic3d_Array1OfVertex V2(1,2);
  Quantity_Length X1,Y1,Z1;
  Quantity_Length X2,Y2,Z2;
  
  // 1er segment
  OffsetPoint.Coord(X1,Y1,Z1);
  V1(1).SetCoord(X1,Y1,Z1);
  pAx1.Coord(X2,Y2,Z2);
  V1(2).SetCoord(X2,Y2,Z2);  //ou directt dir1.XYZ
  
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V1);
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  // 2e segment
  pAx2.Coord(X2,Y2,Z2);
  V2(1).SetCoord(X1,Y1,Z1);
  V2(2).SetCoord(X2,Y2,Z2);

  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V2);
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  // points attache
  Graphic3d_Array1OfVertex V3(1,2);
  if (intOut1) {
    pAx1.Coord(X1,Y1,Z1);
    V3(1).SetCoord(X1,Y1,Z1);
    pnt1.Coord(X2,Y2,Z2);
    V3(2).SetCoord(X2,Y2,Z2);
    LA->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT); // ou DOT ou DOTDASH
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V3);
  }
  if (intOut2) {
    pAx2.Coord(X1,Y1,Z1);
    V3(1).SetCoord(X1,Y1,Z1);
    pnt2.Coord(X2,Y2,Z2);
    V3(2).SetCoord(X2,Y2,Z2);
    LA->LineAspect()->SetTypeOfLine(Aspect_TOL_DOT); // ou DOT ou DOTDASH
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->
      SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V3);
  }

  // Symbol
  Graphic3d_Array1OfVertex V4(1,3);
  gp_Vec vec1(gce_MakeDir(OffsetPoint,pAx1));
  gp_Vec vec2(gce_MakeDir(OffsetPoint,pAx2));
  Standard_Real dist1(OffsetPoint.Distance(pAx1));
  Standard_Real dist2(OffsetPoint.Distance(pAx2));
  vec1 *= dist1;
  vec1 *= .2;
  vec2 *= dist2;
  vec2 *= .2;

  gp_Pnt pAx11 = OffsetPoint.Translated(vec1);
  gp_Pnt pAx22 = OffsetPoint.Translated(vec2);
  gp_Pnt p_symb = pAx22.Translated(vec1);

  pAx11.Coord(X1,Y1,Z1);
  V4(1).SetCoord(X1,Y1,Z1);
  p_symb.Coord(X1,Y1,Z1);
  V4(2).SetCoord(X1,Y1,Z1);
  pAx22.Coord(X1,Y1,Z1);
  V4(3).SetCoord(X1,Y1,Z1);
  
  LA->LineAspect()->SetTypeOfLine(Aspect_TOL_SOLID); 
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V4);
  Prs3d_Root::NewGroup(aPresentation);
}




