// Created on: 2009-04-06
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataXtd_Geometry, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataXtd_Geometry::GetID()
{
  static Standard_GUID TDataXtd_GeometryID("2a96b604-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_GeometryID;
}

//=================================================================================================

occ::handle<TDataXtd_Geometry> TDataXtd_Geometry::Set(const TDF_Label& L)
{
  occ::handle<TDataXtd_Geometry> A;
  if (!L.FindAttribute(TDataXtd_Geometry::GetID(), A))
  {
    A = new TDataXtd_Geometry();
    //    A->SetType(TDataXtd_ANY_GEOM);
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

bool TDataXtd_Geometry::Point(const TDF_Label& L, gp_Pnt& G)
{
  occ::handle<TNaming_NamedShape> NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    return Point(NS, G);
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Point(const occ::handle<TNaming_NamedShape>& NS, gp_Pnt& G)
{
  const TopoDS_Shape& shape = TNaming_Tool::GetShape(NS);
  if (shape.IsNull())
    return false;
  if (shape.ShapeType() == TopAbs_VERTEX)
  {
    const TopoDS_Vertex& vertex = TopoDS::Vertex(shape);
    G                           = BRep_Tool::Pnt(TopoDS::Vertex(vertex));
    return true;
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Axis(const TDF_Label& L, gp_Ax1& G)
{
  occ::handle<TNaming_NamedShape> NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    return Axis(NS, G);
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Axis(const occ::handle<TNaming_NamedShape>& NS, gp_Ax1& G)
{
  gp_Lin lin;
  if (Line(NS, lin))
  {
    G = lin.Position();
    return true;
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Line(const TDF_Label& L, gp_Lin& G)
{
  occ::handle<TNaming_NamedShape> NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    return Line(NS, G);
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Line(const occ::handle<TNaming_NamedShape>& NS, gp_Lin& G)
{
  const TopoDS_Shape& shape = TNaming_Tool::GetShape(NS);
  if (shape.IsNull())
    return false;
  if (shape.ShapeType() == TopAbs_EDGE)
  {
    const TopoDS_Edge& edge = TopoDS::Edge(shape);
    double      first, last;
    // TopLoc_Location loc;
    occ::handle<Geom_Curve> curve = BRep_Tool::Curve(edge, first, last);
    if (!curve.IsNull())
    {
      if (curve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
        curve = (occ::down_cast<Geom_TrimmedCurve>(curve))->BasisCurve();
      occ::handle<Geom_Line> C = occ::down_cast<Geom_Line>(curve);
      if (!C.IsNull())
      {
        G = C->Lin();
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Circle(const TDF_Label& L, gp_Circ& G)
{
  occ::handle<TNaming_NamedShape> NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    return Circle(NS, G);
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Circle(const occ::handle<TNaming_NamedShape>& NS, gp_Circ& G)
{
  const TopoDS_Shape& shape = TNaming_Tool::GetShape(NS);
  if (shape.IsNull())
    return false;
  if (shape.ShapeType() == TopAbs_EDGE)
  {
    const TopoDS_Edge& edge = TopoDS::Edge(shape);
    double      first, last;
    // TopLoc_Location loc;
    occ::handle<Geom_Curve> curve = BRep_Tool::Curve(edge, first, last);
    if (!curve.IsNull())
    {
      if (curve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
        curve = (occ::down_cast<Geom_TrimmedCurve>(curve))->BasisCurve();
      occ::handle<Geom_Circle> C = occ::down_cast<Geom_Circle>(curve);
      if (!C.IsNull())
      {
        G = C->Circ();
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Ellipse(const TDF_Label& L, gp_Elips& G)
{
  occ::handle<TNaming_NamedShape> NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    return Ellipse(NS, G);
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Ellipse(const occ::handle<TNaming_NamedShape>& NS, gp_Elips& G)
{
  const TopoDS_Shape& shape = TNaming_Tool::GetShape(NS);
  if (shape.IsNull())
    return false;
  if (shape.ShapeType() == TopAbs_EDGE)
  {
    const TopoDS_Edge& edge = TopoDS::Edge(shape);
    double      first, last;
    occ::handle<Geom_Curve> curve = BRep_Tool::Curve(edge, first, last);
    if (!curve.IsNull())
    {
      if (curve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
        curve = (occ::down_cast<Geom_TrimmedCurve>(curve))->BasisCurve();
      occ::handle<Geom_Ellipse> C = occ::down_cast<Geom_Ellipse>(curve);
      if (!C.IsNull())
      {
        G = C->Elips();
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Plane(const TDF_Label& L, gp_Pln& G)
{
  occ::handle<TNaming_NamedShape> NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    return Plane(NS, G);
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Plane(const occ::handle<TNaming_NamedShape>& NS, gp_Pln& G)
{
  const TopoDS_Shape& shape = TNaming_Tool::GetShape(NS);
  if (shape.IsNull())
    return false;
  if (shape.ShapeType() == TopAbs_FACE)
  {
    const TopoDS_Face&   face    = TopoDS::Face(shape);
    occ::handle<Geom_Surface> surface = BRep_Tool::Surface(face);
    if (!surface.IsNull())
    {
      if (surface->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
        surface = occ::down_cast<Geom_RectangularTrimmedSurface>(surface)->BasisSurface();
      occ::handle<Geom_Plane> S = occ::down_cast<Geom_Plane>(surface);
      if (!S.IsNull())
      {
        G = S->Pln();
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Cylinder(const TDF_Label& L, gp_Cylinder& G)
{
  occ::handle<TNaming_NamedShape> NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    return Cylinder(NS, G);
  }
  return false;
}

//=================================================================================================

bool TDataXtd_Geometry::Cylinder(const occ::handle<TNaming_NamedShape>& NS, gp_Cylinder& G)
{
  const TopoDS_Shape& shape = TNaming_Tool::GetShape(NS);
  if (shape.IsNull())
    return false;
  if (shape.ShapeType() == TopAbs_FACE)
  {
    const TopoDS_Face&   face    = TopoDS::Face(shape);
    occ::handle<Geom_Surface> surface = BRep_Tool::Surface(face);
    if (!surface.IsNull())
    {
      if (surface->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
        surface = occ::down_cast<Geom_RectangularTrimmedSurface>(surface)->BasisSurface();
      occ::handle<Geom_CylindricalSurface> S = occ::down_cast<Geom_CylindricalSurface>(surface);
      if (!S.IsNull())
      {
        G = S->Cylinder();
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

TDataXtd_GeometryEnum TDataXtd_Geometry::Type(const TDF_Label& L)
{
  occ::handle<TNaming_NamedShape> NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    return Type(NS);
  }
  return TDataXtd_ANY_GEOM;
}

//=================================================================================================

TDataXtd_GeometryEnum TDataXtd_Geometry::Type(const occ::handle<TNaming_NamedShape>& NS)
{
  TDataXtd_GeometryEnum type(TDataXtd_ANY_GEOM);
  const TopoDS_Shape&   shape = TNaming_Tool::GetShape(NS);
  switch (shape.ShapeType())
  {
    case TopAbs_VERTEX: {
      type = TDataXtd_POINT;
      break;
    }
    case TopAbs_EDGE: {
      const TopoDS_Edge& edge = TopoDS::Edge(shape);
      double      first, last;
      // TopLoc_Location loc;
      occ::handle<Geom_Curve> curve = BRep_Tool::Curve(edge, first, last);
      if (!curve.IsNull())
      {
        if (curve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
        {
          curve = (occ::down_cast<Geom_TrimmedCurve>(curve))->BasisCurve();
        }
        if (curve->IsInstance(STANDARD_TYPE(Geom_Line)))
        {
          type = TDataXtd_LINE;
        }
        else if (curve->IsInstance(STANDARD_TYPE(Geom_Circle)))
        {
          type = TDataXtd_CIRCLE;
        }
        else if (curve->IsInstance(STANDARD_TYPE(Geom_Ellipse)))
        {
          type = TDataXtd_ELLIPSE;
        }
      }
#ifdef OCCT_DEBUG
      else
      {
        throw Standard_Failure("curve Null dans TDataXtd_Geometry");
      }
#endif
      break;
    }
    case TopAbs_FACE: {
      const TopoDS_Face&   face    = TopoDS::Face(shape);
      occ::handle<Geom_Surface> surface = BRep_Tool::Surface(face);
      if (!surface.IsNull())
      {
        if (surface->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
        {
          surface = occ::down_cast<Geom_RectangularTrimmedSurface>(surface)->BasisSurface();
        }
        if (surface->IsInstance(STANDARD_TYPE(Geom_CylindricalSurface)))
        {
          type = TDataXtd_CYLINDER;
        }
        else if (surface->IsInstance(STANDARD_TYPE(Geom_Plane)))
        {
          type = TDataXtd_PLANE;
        }
      }
#ifdef OCCT_DEBUG
      else
      {
        throw Standard_Failure("surface Null dans TDataXtd_Geometry");
      }
#endif
      break;
    }
    default:
      break;
  }
  return type;
}

//=================================================================================================

TDataXtd_Geometry::TDataXtd_Geometry()
    : myType(TDataXtd_ANY_GEOM)
{
}

//=================================================================================================

TDataXtd_GeometryEnum TDataXtd_Geometry::GetType() const
{
  return myType;
}

//=================================================================================================

void TDataXtd_Geometry::SetType(const TDataXtd_GeometryEnum G)
{
  // OCC2932 correction
  if (myType == G)
    return;

  Backup();
  myType = G;
}

//=================================================================================================

const Standard_GUID& TDataXtd_Geometry::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataXtd_Geometry::NewEmpty() const
{
  return new TDataXtd_Geometry();
}

//=================================================================================================

void TDataXtd_Geometry::Restore(const occ::handle<TDF_Attribute>& With)
{
  myType = occ::down_cast<TDataXtd_Geometry>(With)->GetType();
}

//=================================================================================================

void TDataXtd_Geometry::Paste(const occ::handle<TDF_Attribute>& Into,
                              const occ::handle<TDF_RelocationTable>&) const
{
  occ::down_cast<TDataXtd_Geometry>(Into)->SetType(myType);
}

//=================================================================================================

Standard_OStream& TDataXtd_Geometry::Dump(Standard_OStream& anOS) const
{
  anOS << "Geometry ";
  TDataXtd::Print(GetType(), anOS);
  return anOS;
}
