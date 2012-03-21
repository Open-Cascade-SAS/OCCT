// Created on: 1995-03-06
// Created by: Laurent PAINNOT
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



#include <DrawTrSurf_Triangulation.ixx>
#include <Poly_Connect.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <gp_Pnt.hxx>
#include <Draw_Color.hxx>
#include <Poly.hxx>

//#ifdef WNT
#include <stdio.h>
//#endif

//=======================================================================
//function : DrawTrSurf_Triangulation
//purpose  : 
//=======================================================================

DrawTrSurf_Triangulation::DrawTrSurf_Triangulation
(const Handle(Poly_Triangulation)& T): 
    myTriangulation(T), 
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

Handle(Poly_Triangulation) DrawTrSurf_Triangulation::Triangulation() const 
{
  return myTriangulation;
}

//=======================================================================
//function : ShowNodes
//purpose  : 
//=======================================================================

void DrawTrSurf_Triangulation::ShowNodes(const Standard_Boolean B)
{
  myNodes = B;
}

//=======================================================================
//function : ShowNodes
//purpose  : 
//=======================================================================

Standard_Boolean DrawTrSurf_Triangulation::ShowNodes() const 
{
  return myNodes;
}

//=======================================================================
//function : ShowTriangles
//purpose  : 
//=======================================================================

void DrawTrSurf_Triangulation::ShowTriangles(const Standard_Boolean B)
{
  myTriangles = B;
}

//=======================================================================
//function : ShowTriangles
//purpose  : 
//=======================================================================

Standard_Boolean DrawTrSurf_Triangulation::ShowTriangles() const 
{
  return myTriangles;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawTrSurf_Triangulation::DrawOn(Draw_Display& dis) const 
{
  // Display the edges
  Standard_Integer i,n;

  const TColgp_Array1OfPnt& Nodes = myTriangulation->Nodes();
  
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

  // texts
  char text[50];
  if (myNodes) {
    dis.SetColor(Draw_jaune);
    n = myTriangulation->NbNodes();
    for (i = 1; i <= n; i++) {
      sprintf(text,"%d",i);
      dis.DrawString(Nodes(i),text);
    }
  }

  if (myTriangles) {
    dis.SetColor(Draw_vert);
    n = myTriangulation->NbTriangles();
    Standard_Integer t[3],j;
    const Poly_Array1OfTriangle& triangle = myTriangulation->Triangles();
    for (i = 1; i <= n; i++) {
      triangle(i).Get(t[0],t[1],t[2]);
      gp_Pnt P(0,0,0);
      gp_XYZ& bary = P.ChangeCoord();
      for (j = 0; j < 3; j++)
	bary.Add(Nodes(t[j]).Coord());
      bary.Multiply(1./3.);

      sprintf(text,"%d",i);
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

void DrawTrSurf_Triangulation::Dump(Standard_OStream& S) const 
{
  Poly::Dump(myTriangulation,S);
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void DrawTrSurf_Triangulation::Whatis(Draw_Interpretor& I) const 
{
  I << "triangulation";
}

