// Created on: 1995-07-24
// Created by: Modelistation
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


#include <Adaptor3d_Surface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_WFPoleSurface.hxx>
#include <TColgp_Array2OfPnt.hxx>

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

