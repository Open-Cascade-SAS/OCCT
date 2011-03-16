// File:	DrawTrSurf_Polygon3D.cxx
// Created:	Fri Mar 10 15:19:08 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


#include <DrawTrSurf_Polygon3D.ixx>
#include <Poly.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>


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

