// Created on: 1995-03-10
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
#include <DrawTrSurf_Polygon3D.hxx>
#include <Poly.hxx>
#include <Poly_Polygon3D.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_Polygon3D,Draw_Drawable3D)

//=======================================================================
//function : DrawTrSurf_Polygon3D
//purpose  : 
//=======================================================================
DrawTrSurf_Polygon3D::DrawTrSurf_Polygon3D(const Handle(Poly_Polygon3D)& P):
   myPolygon3D(P),
   myNodes(Standard_False)
{
  
}

//=======================================================================
//function : Polygon3D
//purpose  : 
//=======================================================================

Handle(Poly_Polygon3D) DrawTrSurf_Polygon3D::Polygon3D() const 
{
  return myPolygon3D;
}

//=======================================================================
//function : ShowNodes
//purpose  : 
//=======================================================================

void DrawTrSurf_Polygon3D::ShowNodes(const Standard_Boolean B)
{
  myNodes = B;
}

//=======================================================================
//function : ShowNodes
//purpose  : 
//=======================================================================

Standard_Boolean DrawTrSurf_Polygon3D::ShowNodes() const 
{
  return myNodes;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawTrSurf_Polygon3D::DrawOn(Draw_Display& dis) const 
{
  dis.SetColor(Draw_jaune);
  
  const TColgp_Array1OfPnt& Points = myPolygon3D->Nodes();

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

Handle(Draw_Drawable3D) DrawTrSurf_Polygon3D::Copy() const 
{
  return new DrawTrSurf_Polygon3D(myPolygon3D);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void DrawTrSurf_Polygon3D::Dump(Standard_OStream& S) const 
{
  Poly::Dump(myPolygon3D, S);
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void DrawTrSurf_Polygon3D::Whatis(Draw_Interpretor& I) const 
{
  I << "polygon3D";
}

