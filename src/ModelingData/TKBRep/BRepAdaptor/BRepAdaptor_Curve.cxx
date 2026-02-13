// Created on: 1993-02-19
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepAdaptor_Curve.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Standard_NullObject.hxx>
#include <TopoDS_Face.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepAdaptor_Curve, GeomAdaptor_TransformedCurve)

//==================================================================================================

BRepAdaptor_Curve::BRepAdaptor_Curve() = default;

//==================================================================================================

BRepAdaptor_Curve::BRepAdaptor_Curve(const TopoDS_Edge& E)
{
  Initialize(E);
}

//==================================================================================================

BRepAdaptor_Curve::BRepAdaptor_Curve(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  Initialize(E, F);
}

//==================================================================================================

occ::handle<Adaptor3d_Curve> BRepAdaptor_Curve::ShallowCopy() const
{
  occ::handle<BRepAdaptor_Curve> aCopy = new BRepAdaptor_Curve();

  const occ::handle<Adaptor3d_Curve> aCurve     = myCurve.ShallowCopy();
  const GeomAdaptor_Curve&           aGeomCurve = *(occ::down_cast<GeomAdaptor_Curve>(aCurve));
  aCopy->myCurve                                = aGeomCurve;

  if (!myConSurf.IsNull())
  {
    aCopy->myConSurf = occ::down_cast<Adaptor3d_CurveOnSurface>(myConSurf->ShallowCopy());
  }
  aCopy->myTrsf = myTrsf;
  aCopy->myEdge = myEdge;

  return aCopy;
}

//==================================================================================================

void BRepAdaptor_Curve::Reset()
{
  myCurve.Reset();
  myConSurf.Nullify();
  myEdge.Nullify();
  myTrsf = gp_Trsf();
}

//==================================================================================================

void BRepAdaptor_Curve::Initialize(const TopoDS_Edge& E)
{
  myConSurf.Nullify();
  myEdge = E;
  double pf, pl;

  TopLoc_Location         L;
  occ::handle<Geom_Curve> C = BRep_Tool::Curve(E, L, pf, pl);

  if (!C.IsNull())
  {
    myCurve.Load(C, pf, pl);
  }
  else
  {
    occ::handle<Geom2d_Curve> PC;
    occ::handle<Geom_Surface> S;
    BRep_Tool::CurveOnSurface(E, PC, S, L, pf, pl);
    if (!PC.IsNull())
    {
      occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
      HS->Load(S);
      occ::handle<Geom2dAdaptor_Curve> HC = new Geom2dAdaptor_Curve();
      HC->Load(PC, pf, pl);
      myConSurf = new Adaptor3d_CurveOnSurface();
      myConSurf->Load(HC, HS);
    }
    else
    {
      throw Standard_NullObject("BRepAdaptor_Curve::No geometry");
    }
  }
  myTrsf = L.Transformation();
}

//==================================================================================================

void BRepAdaptor_Curve::Initialize(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  myConSurf.Nullify();

  myEdge = E;
  TopLoc_Location           L;
  double                    pf, pl;
  occ::handle<Geom_Surface> S  = BRep_Tool::Surface(F, L);
  occ::handle<Geom2d_Curve> PC = BRep_Tool::CurveOnSurface(E, F, pf, pl);

  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
  HS->Load(S);
  occ::handle<Geom2dAdaptor_Curve> HC = new Geom2dAdaptor_Curve();
  HC->Load(PC, pf, pl);
  myConSurf = new Adaptor3d_CurveOnSurface();
  myConSurf->Load(HC, HS);

  myTrsf = L.Transformation();
}

//==================================================================================================

const TopoDS_Edge& BRepAdaptor_Curve::Edge() const
{
  return myEdge;
}

//==================================================================================================

double BRepAdaptor_Curve::Tolerance() const
{
  return BRep_Tool::Tolerance(myEdge);
}

//==================================================================================================

occ::handle<Adaptor3d_Curve> BRepAdaptor_Curve::Trim(const double First,
                                                     const double Last,
                                                     const double Tol) const
{
  // Create a copy preserving the edge reference with modified bounds.
  occ::handle<BRepAdaptor_Curve> res;
  if (myConSurf.IsNull())
  {
    double                  pf = FirstParameter(), pl = LastParameter();
    occ::handle<Geom_Curve> C = myCurve.Curve();
    const_cast<GeomAdaptor_Curve*>(&myCurve)->Load(C, First, Last);
    res = new BRepAdaptor_Curve(*this);
    const_cast<GeomAdaptor_Curve*>(&myCurve)->Load(C, pf, pl);
  }
  else
  {
    occ::handle<Adaptor3d_CurveOnSurface> sav = myConSurf;
    const_cast<occ::handle<Adaptor3d_CurveOnSurface>&>(myConSurf) =
      occ::down_cast<Adaptor3d_CurveOnSurface>(myConSurf->Trim(First, Last, Tol));
    res                                                           = new BRepAdaptor_Curve(*this);
    const_cast<occ::handle<Adaptor3d_CurveOnSurface>&>(myConSurf) = sav;
  }
  return res;
}
