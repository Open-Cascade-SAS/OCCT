// Created on: 1996-01-12
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



#include <DrawDim_PlanarRadius.ixx>
#include <DrawDim.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Circle.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <BRep_Tool.hxx>
#include <TCollection_AsciiString.hxx>
#include <Draw_Color.hxx>
#include <ElCLib.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : DrawDim_PlanarRadius
//purpose  : 
//=======================================================================

DrawDim_PlanarRadius::DrawDim_PlanarRadius(const TopoDS_Face& face, const TopoDS_Shape& c)
{
  myPlane  = face;
  myCircle = c;
}


//=======================================================================
//function : DrawDim_PlanarRadius
//purpose  : 
//=======================================================================

DrawDim_PlanarRadius::DrawDim_PlanarRadius(const TopoDS_Shape& c)
{
  myCircle = c;
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawDim_PlanarRadius::DrawOn(Draw_Display& dis) const
{  
  if (myCircle.ShapeType() == TopAbs_EDGE) {
    Standard_Real f,l;
    Handle(Geom_Curve) curve = BRep_Tool::Curve (TopoDS::Edge(myCircle),f,l);
    if (curve->IsKind(STANDARD_TYPE(Geom_Circle))) {
      gp_Circ circle = Handle(Geom_Circle)::DownCast(curve)->Circ();
      const gp_Pnt& first = circle.Location();
      TopoDS_Vertex vf, vl;
      TopExp::Vertices(TopoDS::Edge(myCircle),vf,vl);    
      const gp_Pnt last = BRep_Tool::Pnt(vf);
      //
      dis.Draw (first,last);
      gp_Pnt p ((first.X()+ last.X())/2,(first.Y()+ last.Y())/2,(first.Z()+ last.Z())/2);
      DrawText(p,dis);
      return;
    }
  }  
  cout << " DrawDim_PlanarRadius::DrawOn : dimension error" << endl;
}
