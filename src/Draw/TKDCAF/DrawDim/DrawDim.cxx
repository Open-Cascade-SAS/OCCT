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

#include <DrawDim.hxx>

#include <BRep_Tool.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Text3D.hxx>
#include <ElCLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=================================================================================================

void DrawDim::AllCommands(Draw_Interpretor& theCommands)
{
  PlanarDimensionCommands(theCommands);
}

//=================================================================================================

void DrawDim::DrawShapeName(const TopoDS_Shape& ashape, const char* aname)
{
  gp_Pnt                  position;
  TCollection_AsciiString t(" ");
  switch (ashape.ShapeType())
  {
    case TopAbs_EDGE: {
      double      f, l, parameter;
      occ::handle<Geom_Curve> curve = BRep_Tool::Curve(TopoDS::Edge(ashape), f, l);
      if (curve->IsKind(STANDARD_TYPE(Geom_Line)))
      {
        parameter = (f + l) / 2.;
        position  = ElCLib::Value(parameter, occ::down_cast<Geom_Line>(curve)->Lin());
      }
      else if (curve->IsKind(STANDARD_TYPE(Geom_Circle)))
      {
        parameter = (f + l) / 2.;
        if (f > l)
          parameter = parameter + M_PI;
        position = ElCLib::Value(parameter, occ::down_cast<Geom_Circle>(curve)->Circ());
      }
    }
    break;
    case TopAbs_VERTEX: {
      position = BRep_Tool::Pnt(TopoDS::Vertex(ashape));
    }
    break;
    default:
      break;
  }
  t += aname; // Name();
  occ::handle<Draw_Text3D> text = new Draw_Text3D(position, t.ToCString(), Draw_blanc);
  dout << text;
}

//=================================================================================================

bool DrawDim::Pln(const TopoDS_Face& f, gp_Pln& p)
{
  occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(BRep_Tool::Surface(f));
  if (!P.IsNull())
  {
    p = P->Pln();
    return true;
  }
  return false;
}

//=================================================================================================

bool DrawDim::Lin(const TopoDS_Edge& e,
                              gp_Lin&            l,
                              bool&  infinite,
                              double&     first,
                              double&     last)
{
  double     f1, l1;
  occ::handle<Geom_Line> L = occ::down_cast<Geom_Line>(BRep_Tool::Curve(e, f1, l1));
  if (!L.IsNull())
  {
    TopoDS_Vertex vf, vl;
    TopExp::Vertices(TopoDS::Edge(e), vf, vl);
    if (vf.IsNull() && vl.IsNull())
    {
      infinite = true;
      l        = L->Lin();
      return true;
    }
    else if (vf.IsNull() || vl.IsNull())
    {
      throw Standard_DomainError("DrawDim::Lin : semi infinite edge");
    }
    else
    {
      l        = L->Lin();
      infinite = true;
      first    = f1;
      last     = l1;
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool DrawDim::Circ(const TopoDS_Edge& e,
                               gp_Circ&           c,
                               double&     first,
                               double&     last)
{
  double       f1, l1;
  occ::handle<Geom_Circle> C = occ::down_cast<Geom_Circle>(BRep_Tool::Curve(e, f1, l1));
  if (!C.IsNull())
  {
    c     = C->Circ();
    first = f1;
    last  = l1;
    return true;
  }
  return false;
}

//=================================================================================================

gp_Pnt DrawDim::Nearest(const TopoDS_Shape& ashape, const gp_Pnt& apoint)
{
  double   dist = RealLast();
  double   curdist;
  gp_Pnt          result;
  gp_Pnt          curpnt;
  TopExp_Explorer explo(ashape, TopAbs_VERTEX);
  while (explo.More())
  {
    curpnt  = BRep_Tool::Pnt(TopoDS::Vertex(explo.Current()));
    curdist = apoint.Distance(curpnt);
    if (curdist < dist)
    {
      result = curpnt;
      dist   = curdist;
    }
    explo.Next();
  }
  return result;
}
