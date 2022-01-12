// Created on: 1995-03-06
// Created by: Laurent PAINNOT
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

#include <DrawTrSurf_Triangulation.hxx>

#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Poly.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <stdio.h>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_Triangulation,Draw_Drawable3D)

//=======================================================================
//function : DrawTrSurf_Triangulation
//purpose  :
//=======================================================================
DrawTrSurf_Triangulation::DrawTrSurf_Triangulation (const Handle(Poly_Triangulation)& T)
: myTriangulation(T),
  myNodes(Standard_False),
  myTriangles(Standard_False)
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
  Standard_Integer n[3];
  for (i = 1; i <= nbTriangles; i++) {
    pc.Triangles(i,t[0],t[1],t[2]);
    T->Triangle (i).Get (n[0],n[1],n[2]);
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
//function : DrawOn
//purpose  :
//=======================================================================
void DrawTrSurf_Triangulation::DrawOn (Draw_Display& dis) const
{
  // Display the edges
  Standard_Integer i,n;

  // free edges

  dis.SetColor(Draw_rouge);
  const TColStd_Array1OfInteger& Free = myFree->Array1();
  n = Free.Length() / 2;
  for (i = 1; i <= n; i++) {
    dis.Draw (myTriangulation->Node (Free[2*i-1]),
              myTriangulation->Node (Free[2*i]));
  }
  
  // internal edges

  dis.SetColor(Draw_bleu);
  const TColStd_Array1OfInteger& Internal = myInternals->Array1();
  n = Internal.Length() / 2;
  for (i = 1; i <= n; i++) {
    dis.Draw (myTriangulation->Node (Internal[2*i-1]),
              myTriangulation->Node (Internal[2*i]));
  }

  // texts
  char text[50];
  if (myNodes) {
    dis.SetColor(Draw_jaune);
    n = myTriangulation->NbNodes();
    for (i = 1; i <= n; i++) {
      Sprintf(text,"%d",i);
      dis.DrawString (myTriangulation->Node (i), text);
    }
  }

  if (myTriangles) {
    dis.SetColor(Draw_vert);
    n = myTriangulation->NbTriangles();
    Standard_Integer t[3],j;
    for (i = 1; i <= n; i++) {
      myTriangulation->Triangle (i).Get (t[0],t[1],t[2]);
      gp_Pnt P(0,0,0);
      gp_XYZ& bary = P.ChangeCoord();
      for (j = 0; j < 3; j++)
	bary.Add (myTriangulation->Node (t[j]).Coord());
      bary.Multiply(1./3.);

      Sprintf(text,"%d",i);
      dis.DrawString(P,text);
    }
  }
}

//=======================================================================
//function : Copy
//purpose  :
//=======================================================================
Handle(Draw_Drawable3D) DrawTrSurf_Triangulation::Copy() const
{
  return new DrawTrSurf_Triangulation(myTriangulation);
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
void DrawTrSurf_Triangulation::Dump (Standard_OStream& S) const
{
  Poly::Dump(myTriangulation,S);
}

//=======================================================================
//function : Save
//purpose  :
//=======================================================================
void DrawTrSurf_Triangulation::Save (Standard_OStream& theStream) const
{
#if !defined(_MSC_VER) && !defined(__sgi) && !defined(IRIX)
  std::ios::fmtflags aFlags = theStream.flags();
  theStream.setf (std::ios::scientific, std::ios::floatfield);
  theStream.precision (15);
#else
  long aForm = theStream.setf (std::ios::scientific);
  std::streamsize aPrec = theStream.precision (15);
#endif
  Poly::Write (myTriangulation, theStream);
#if !defined(_MSC_VER) && !defined(__sgi) && !defined(IRIX)
  theStream.setf (aFlags);
#else
  theStream.setf (aForm);
  theStream.precision (aPrec);
#endif
}

//=======================================================================
//function : Restore
//purpose  :
//=======================================================================
Handle(Draw_Drawable3D) DrawTrSurf_Triangulation::Restore (Standard_IStream& theStream)
{
  return new DrawTrSurf_Triangulation (Poly::ReadTriangulation (theStream));
}

//=======================================================================
//function : Whatis
//purpose  :
//=======================================================================
void DrawTrSurf_Triangulation::Whatis (Draw_Interpretor& I) const
{
  I << "triangulation";
}
