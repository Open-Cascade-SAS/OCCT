// Created on: 1995-07-24
// Created by: Modelistation
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

#include <StdPrs_WFPoleSurface.ixx>

#include <Graphic3d_Group.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>

static void AddPoles(const Handle (Prs3d_Presentation)& aPresentation,
                     const TColgp_Array2OfPnt&          A,
                     const Handle (Prs3d_Drawer)&       aDrawer)
{
  Standard_Integer i,j;
  const Standard_Integer n = A.ColLength();
  const Standard_Integer m = A.RowLength();

  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(aDrawer->UIsoAspect()->Aspect());    
  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(n*m,n);
  for (i=1; i<=n; i++){
    aPrims->AddBound(m);
    for (j=1; j<=m; j++)
      aPrims->AddVertex(A(i,j));
  }
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(aDrawer->VIsoAspect()->Aspect());    
  aPrims = new Graphic3d_ArrayOfPolylines(n*m,m);
  for (j=1; j<=m; j++){
    aPrims->AddBound(n);
    for (i=1; i<=n; i++)
      aPrims->AddVertex(A(i,j));
  }
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void StdPrs_WFPoleSurface::Add (const Handle (Prs3d_Presentation)& aPresentation,
				const Adaptor3d_Surface&             aSurface,
				const Handle (Prs3d_Drawer)&       aDrawer)
{

  GeomAbs_SurfaceType SType = aSurface.GetType();
  if (SType == GeomAbs_BezierSurface || SType == GeomAbs_BSplineSurface) {
    Standard_Integer n , m;
    if (SType == GeomAbs_BezierSurface) {
      Handle(Geom_BezierSurface) B = aSurface.Bezier();
      n = aSurface.NbUPoles();
      m = aSurface.NbVPoles();
      TColgp_Array2OfPnt A(1,n,1,m);
      (aSurface.Bezier())->Poles(A);
      AddPoles(aPresentation, A, aDrawer);
    }
    else if (SType == GeomAbs_BSplineSurface) {
      Handle(Geom_BSplineSurface) B = aSurface.BSpline();
      n = (aSurface.BSpline())->NbUPoles();
      m = (aSurface.BSpline())->NbVPoles();
      TColgp_Array2OfPnt A(1,n,1,m);
      (aSurface.BSpline())->Poles(A);
      AddPoles(aPresentation, A, aDrawer);
    }

  }
}

