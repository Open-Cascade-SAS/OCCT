// Created on: 1995-03-14
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



#include <DrawTrSurf_Polygon2D.ixx>
#include <Poly.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>

//=======================================================================
//function : DrawTrSurf_Polygon2D
//purpose  : 
//=======================================================================

DrawTrSurf_Polygon2D::DrawTrSurf_Polygon2D(const Handle(Poly_Polygon2D)& P):
   myPolygon2D(P),
   myNodes(Standard_False)
{
}

//=======================================================================
//function : Polygon2D
//purpose  : 
//=======================================================================

Handle(Poly_Polygon2D) DrawTrSurf_Polygon2D::Polygon2D() const 
{
  return myPolygon2D;
}

//=======================================================================
//function : ShowNodes
//purpose  : 
//=======================================================================

void DrawTrSurf_Polygon2D::ShowNodes(const Standard_Boolean B)
{
  myNodes = B;
}

//=======================================================================
//function : ShowNodes
//purpose  : 
//=======================================================================

Standard_Boolean DrawTrSurf_Polygon2D::ShowNodes() const 
{
  return myNodes;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawTrSurf_Polygon2D::DrawOn(Draw_Display& dis) const 
{
  dis.SetColor(Draw_jaune);

  
  const TColgp_Array1OfPnt2d& Points = myPolygon2D->Nodes();

  for (Standard_Integer i = Points.Lower(); i <= Points.Upper()-1; i++) {
    dis.Draw(Points(i), Points(i+1));
  }
  

  if (myNodes) { 
    for (Standard_Integer i = Points.Lower(); i <= Points.Upper(); i++) {
      dis.DrawMarker(Points(i), Draw_X);
    }
  }

}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DrawTrSurf_Polygon2D::Copy() const 
{
  return new DrawTrSurf_Polygon2D(myPolygon2D);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void DrawTrSurf_Polygon2D::Dump(Standard_OStream& S) const 
{
  Poly::Dump(myPolygon2D, S);
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void DrawTrSurf_Polygon2D::Whatis(Draw_Interpretor& I) const 
{
  I << "polygon2D";
}

