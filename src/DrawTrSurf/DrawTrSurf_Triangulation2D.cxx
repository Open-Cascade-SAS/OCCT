// Created on: 1997-07-22
// Created by: Laurent PAINNOT
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


#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_Drawable3D.hxx>
#include <DrawTrSurf_Triangulation2D.hxx>
#include <gp_Pnt2d.hxx>
#include <Poly.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_Triangulation2D,Draw_Drawable2D)

#ifdef _MSC_VER
#include <stdio.h>
#endif

//=======================================================================
//function : DrawTrSurf_Triangulation2D
//purpose  : 
//=======================================================================

DrawTrSurf_Triangulation2D::DrawTrSurf_Triangulation2D
(const Handle(Poly_Triangulation)& T): 
    myTriangulation(T)
{
  // Build the connect tool
  Poly_Connect pc(T);

  Standard_Integer i,j,nFree, nInternal, nbTriangles = T->NbTriangles();
  Standard_Integer t[3];

  // count the free edges
  nFree = 0;
  for (i = 1; i <= nbTriangles; i++) {
    pc.Triangles(i,t[0],t[1],t[2]);
    for (j = 0; j < 3; j++)
      if (t[j] == 0) nFree++;
  }

  // allocate the arrays
  myFree = new TColStd_HArray1OfInteger(1,2*nFree);
  nInternal = (3*nbTriangles - nFree) / 2;
  myInternals = new TColStd_HArray1OfInteger(1,2*nInternal);

  TColStd_Array1OfInteger& Free     = myFree->ChangeArray1();
  TColStd_Array1OfInteger& Internal = myInternals->ChangeArray1();

  Standard_Integer fr = 1, in = 1;
  const Poly_Array1OfTriangle& triangles = T->Triangles();
  Standard_Integer n[3];
  for (i = 1; i <= nbTriangles; i++) {
    pc.Triangles(i,t[0],t[1],t[2]);
    triangles(i).Get(n[0],n[1],n[2]);
    for (j = 0; j < 3; j++) {
      Standard_Integer k = (j+1) % 3;
      if (t[j] == 0) {
	Free(fr)   = n[j];
	Free(fr+1) = n[k];
	fr += 2;
      }
      // internal edge if this triangle has a lower index than the adjacent
      else if (i < t[j]) {
	Internal(in)   = n[j];
	Internal(in+1) = n[k];
	in += 2;
      }
    }
  }
}

//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

Handle(Poly_Triangulation) DrawTrSurf_Triangulation2D::Triangulation() const 
{
  return myTriangulation;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawTrSurf_Triangulation2D::DrawOn(Draw_Display& dis) const 
{
  // Display the edges
  Standard_Integer i,n;
  if (myTriangulation->HasUVNodes()) {
    
    const TColgp_Array1OfPnt2d& Nodes = myTriangulation->UVNodes();
    
    // free edges
    
    dis.SetColor(Draw_rouge);
    const TColStd_Array1OfInteger& Free = myFree->Array1();
    n = Free.Length() / 2;
    for (i = 1; i <= n; i++) {
      dis.Draw(Nodes(Free(2*i-1)),Nodes(Free(2*i)));
    }
    
    // internal edges
    
    dis.SetColor(Draw_bleu);
    const TColStd_Array1OfInteger& Internal = myInternals->Array1();
    n = Internal.Length() / 2;
    for (i = 1; i <= n; i++) {
      dis.Draw(Nodes(Internal(2*i-1)),Nodes(Internal(2*i)));
    }
    
  }
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DrawTrSurf_Triangulation2D::Copy() const 
{
  return new DrawTrSurf_Triangulation2D(myTriangulation);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void DrawTrSurf_Triangulation2D::Dump(Standard_OStream& S) const 
{
  Poly::Dump(myTriangulation,S);
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void DrawTrSurf_Triangulation2D::Whatis(Draw_Interpretor& I) const 
{
  I << "triangulation";
}

