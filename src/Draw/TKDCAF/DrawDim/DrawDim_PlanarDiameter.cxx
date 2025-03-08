// Created on: 1998-11-25
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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
#include <DrawDim_PlanarDiameter.hxx>
#include <ElCLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawDim_PlanarDiameter, DrawDim_PlanarDimension)

//=================================================================================================

DrawDim_PlanarDiameter::DrawDim_PlanarDiameter(const TopoDS_Face& face, const TopoDS_Shape& c)
{
  myPlane  = face;
  myCircle = c;
}

//=================================================================================================

DrawDim_PlanarDiameter::DrawDim_PlanarDiameter(const TopoDS_Shape& c)
{
  myCircle = c;
}

//=================================================================================================

void DrawDim_PlanarDiameter::DrawOn(Draw_Display& dis) const
{
  if (myCircle.ShapeType() == TopAbs_EDGE)
  {
    Standard_Real      f, l;
    Handle(Geom_Curve) curve = BRep_Tool::Curve(TopoDS::Edge(myCircle), f, l);
    if (curve->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      gp_Circ       circle = Handle(Geom_Circle)::DownCast(curve)->Circ();
      TopoDS_Vertex vf, vl;
      TopExp::Vertices(TopoDS::Edge(myCircle), vf, vl);
      const gp_Pnt  first    = BRep_Tool::Pnt(vf);
      Standard_Real parfirst = ElCLib::Parameter(circle, first);
      Standard_Real parlast  = (parfirst + M_PI);
      gp_Pnt        last     = ElCLib::Value(parlast, circle);
      //
      dis.Draw(first, last);
      gp_Pnt p((first.X() + last.X()) / 2, (first.Y() + last.Y()) / 2, (first.Z() + last.Z()) / 2);
      DrawText(p, dis);
      return;
    }
  }
#ifdef OCCT_DEBUG
  std::cout << " DrawDim_PlanarDiameter::DrawOn : dimension error" << std::endl;
#endif
}
