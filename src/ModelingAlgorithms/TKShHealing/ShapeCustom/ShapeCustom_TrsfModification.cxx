// Created on: 1999-03-09
// Created by: Roman LYGIN
// Copyright (c) 1999-1999 Matra Datavision
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

#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_TVertex.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <ShapeCustom_TrsfModification.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_TrsfModification, BRepTools_TrsfModification)

//=================================================================================================

ShapeCustom_TrsfModification::ShapeCustom_TrsfModification(const gp_Trsf& T)
    : BRepTools_TrsfModification(T)
{
}

//=================================================================================================

bool ShapeCustom_TrsfModification::NewSurface(const TopoDS_Face&         F,
                                              occ::handle<Geom_Surface>& S,
                                              TopLoc_Location&           L,
                                              double&                    Tol,
                                              bool&                      RevWires,
                                              bool&                      RevFace)
{
  bool result = BRepTools_TrsfModification::NewSurface(F, S, L, Tol, RevWires, RevFace);
  Tol = (*((occ::handle<BRep_TFace>*)&F.TShape()))->Tolerance() * std::abs(Trsf().ScaleFactor());
  return result;
}

//=================================================================================================

bool ShapeCustom_TrsfModification::NewCurve(const TopoDS_Edge&       E,
                                            occ::handle<Geom_Curve>& C,
                                            TopLoc_Location&         L,
                                            double&                  Tol)
{
  bool result = BRepTools_TrsfModification::NewCurve(E, C, L, Tol);
  Tol = (*((occ::handle<BRep_TEdge>*)&E.TShape()))->Tolerance() * std::abs(Trsf().ScaleFactor());
  return result;
}

//=================================================================================================

bool ShapeCustom_TrsfModification::NewPoint(const TopoDS_Vertex& V, gp_Pnt& P, double& Tol)
{
  bool result = BRepTools_TrsfModification::NewPoint(V, P, Tol);
  Tol = (*((occ::handle<BRep_TVertex>*)&V.TShape()))->Tolerance() * std::abs(Trsf().ScaleFactor());
  return result;
}

//=================================================================================================

bool ShapeCustom_TrsfModification::NewCurve2d(const TopoDS_Edge&         E,
                                              const TopoDS_Face&         F,
                                              const TopoDS_Edge&         NewE,
                                              const TopoDS_Face&         NewF,
                                              occ::handle<Geom2d_Curve>& C,
                                              double&                    Tol)
{
  bool result = BRepTools_TrsfModification::NewCurve2d(E, F, NewE, NewF, C, Tol);
  Tol = (*((occ::handle<BRep_TEdge>*)&E.TShape()))->Tolerance() * std::abs(Trsf().ScaleFactor());
  return result;
}

//=================================================================================================

bool ShapeCustom_TrsfModification::NewParameter(const TopoDS_Vertex& V,
                                                const TopoDS_Edge&   E,
                                                double&              P,
                                                double&              Tol)
{
  bool result = BRepTools_TrsfModification::NewParameter(V, E, P, Tol);
  Tol = (*((occ::handle<BRep_TVertex>*)&V.TShape()))->Tolerance() * std::abs(Trsf().ScaleFactor());
  return result;
}
