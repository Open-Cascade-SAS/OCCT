// Created on: 1996-01-10
// Created by: Denis PASCAL
// Copyright (c) 1996-1999 Matra Datavision
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



#include <DrawDim_PlanarDistance.ixx>
#include <DrawDim.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>  
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx> 
#include <gp_Lin.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <TopExp.hxx>



//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================

void DrawDim_PlanarDistance::Draw 
(const gp_Pnt& point, const TopoDS_Edge& edge, Draw_Display& dis) const
{    
  Standard_Real f,l;
  Handle(Geom_Curve) line = BRep_Tool::Curve(edge,f,l);
  GeomAPI_ProjectPointOnCurve pj (point,Handle(Geom_Curve)::DownCast(line));
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
  cout << " DrawDim_PlanarDistance::DrawOn : dimension error" << endl;
}
