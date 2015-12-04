// Created on: 1995-03-14
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


#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_MarkerShape.hxx>
#include <DrawTrSurf_Polygon2D.hxx>
#include <Poly.hxx>
#include <Poly_Polygon2D.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_Polygon2D,Draw_Drawable2D)

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

