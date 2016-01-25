// Created on: 1996-01-10
// Created by: Denis PASCAL
// Copyright (c) 1996-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <Draw_Display.hxx>
#include <DrawDim.hxx>
#include <DrawDim_PlanarDistance.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawDim_PlanarDistance,DrawDim_PlanarDimension)

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================
void DrawDim_PlanarDistance::Draw 
(const gp_Pnt& point, const TopoDS_Edge& edge, Draw_Display& dis) const
{    
  Standard_Real f,l;
  Handle(Geom_Curve) line = BRep_Tool::Curve(edge,f,l);
  GeomAPI_ProjectPointOnCurve pj (point, line);
  if (pj.NbPoints() == 1) { 
    gp_Pnt first = point;
    gp_Pnt last = pj.Point(1);
    dis.Draw (first,last);  

    gp_Pnt p ((first.X()+ last.X())/2,(first.Y()+ last.Y())/2,(first.Z()+ last.Z())/2);
    DrawText(p,dis);
  }
}

//=======================================================================
//function : DrawDim_PlanarDistance
//purpose  : 
//=======================================================================

DrawDim_PlanarDistance::DrawDim_PlanarDistance (const TopoDS_Face& face,
						const TopoDS_Shape& geom1, 
						const TopoDS_Shape& geom2)
{
  myPlane = face; myGeom1 = geom1; myGeom2 = geom2;
}

//=======================================================================
//function : DrawDim_PlanarDistance
//purpose  : 
//=======================================================================

DrawDim_PlanarDistance::DrawDim_PlanarDistance (const TopoDS_Shape& geom1, 
						const TopoDS_Shape&  geom2)
{  
  myGeom1 = geom1; myGeom2 = geom2;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawDim_PlanarDistance::DrawOn(Draw_Display& dis) const 
{
  if (myGeom1.ShapeType() == TopAbs_VERTEX && myGeom2.ShapeType() == TopAbs_VERTEX) {
    gp_Pnt first = BRep_Tool::Pnt(TopoDS::Vertex(myGeom1));
    gp_Pnt last  = BRep_Tool::Pnt(TopoDS::Vertex(myGeom2));  
    dis.Draw (first,last);  

    gp_Pnt p ((first.X()+ last.X())/2,(first.Y()+ last.Y())/2,(first.Z()+ last.Z())/2);
    DrawText(p,dis);
    return;
  }

  else if (myGeom1.ShapeType() == TopAbs_VERTEX && myGeom2.ShapeType() == TopAbs_EDGE) {
    gp_Pnt point = BRep_Tool::Pnt(TopoDS::Vertex(myGeom1)); 
    Draw (point,TopoDS::Edge(myGeom2),dis);
    return;
  }  

  else if (myGeom1.ShapeType() == TopAbs_EDGE && myGeom2.ShapeType() == TopAbs_VERTEX) {
    gp_Pnt point = BRep_Tool::Pnt(TopoDS::Vertex(myGeom2)); 
    Draw (point,TopoDS::Edge(myGeom1),dis);
    return;
  }

  else if (myGeom1.ShapeType() == TopAbs_EDGE && myGeom2.ShapeType() == TopAbs_EDGE) {
    Standard_Real f,l;
    Handle(Geom_Curve) C = BRep_Tool::Curve (TopoDS::Edge(myGeom1),f,l);  
    if (!C.IsNull()) {
      Handle(Geom_Line) L  = Handle(Geom_Line)::DownCast(C);  
      if (!L.IsNull()) {
	gp_Pnt point = L->Lin().Location();  
	TopoDS_Edge edge = TopoDS::Edge(myGeom2); 
	Draw (point,edge,dis);
	return;
      }
    }
  } 
#ifdef OCCT_DEBUG
  cout << " DrawDim_PlanarDistance::DrawOn : dimension error" << endl;
#endif
}
