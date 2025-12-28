// Created on: 1998-08-26
// Created by: Julia GERASIMOVA
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

#include <BRepOffsetAPI_MakeFilling.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

BRepOffsetAPI_MakeFilling::BRepOffsetAPI_MakeFilling(const int    Degree,
                                                     const int    NbPtsOnCur,
                                                     const int    NbIter,
                                                     const bool   Anisotropie,
                                                     const double Tol2d,
                                                     const double Tol3d,
                                                     const double TolAng,
                                                     const double TolCurv,
                                                     const int    MaxDeg,
                                                     const int    MaxSegments)
    : myFilling(Degree,
                NbPtsOnCur,
                NbIter,
                Anisotropie,
                Tol2d,
                Tol3d,
                TolAng,
                TolCurv,
                MaxDeg,
                MaxSegments)
{
}

//=================================================================================================

void BRepOffsetAPI_MakeFilling::SetConstrParam(const double Tol2d,
                                               const double Tol3d,
                                               const double TolAng,
                                               const double TolCurv)
{
  myFilling.SetConstrParam(Tol2d, Tol3d, TolAng, TolCurv);
}

//=================================================================================================

void BRepOffsetAPI_MakeFilling::SetResolParam(const int  Degree,
                                              const int  NbPtsOnCur,
                                              const int  NbIter,
                                              const bool Anisotropie)
{
  myFilling.SetResolParam(Degree, NbPtsOnCur, NbIter, Anisotropie);
}

//=================================================================================================

void BRepOffsetAPI_MakeFilling::SetApproxParam(const int MaxDeg, const int MaxSegments)
{
  myFilling.SetApproxParam(MaxDeg, MaxSegments);
}

//=================================================================================================

void BRepOffsetAPI_MakeFilling::LoadInitSurface(const TopoDS_Face& Surf)
{
  myFilling.LoadInitSurface(Surf);
}

//=======================================================================
// function : Add
// purpose  : adds an edge as a constraint
//======================================================================
int BRepOffsetAPI_MakeFilling::Add(const TopoDS_Edge&  Constr,
                                   const GeomAbs_Shape Order,
                                   const bool          IsBound)
{
  return myFilling.Add(Constr, Order, IsBound);
}

//=======================================================================
// function : Add
// purpose  : adds an edge with supporting face as a constraint
//======================================================================
int BRepOffsetAPI_MakeFilling::Add(const TopoDS_Edge&  Constr,
                                   const TopoDS_Face&  Support,
                                   const GeomAbs_Shape Order,
                                   const bool          IsBound)
{
  return myFilling.Add(Constr, Support, Order, IsBound);
}

//=======================================================================
// function : Add
// purpose  : adds a "free constraint": face without edge
//======================================================================
int BRepOffsetAPI_MakeFilling::Add(const TopoDS_Face& Support, const GeomAbs_Shape Order)
{
  return myFilling.Add(Support, Order);
}

//=======================================================================
// function : Add
// purpose  : adds a point constraint
//======================================================================
int BRepOffsetAPI_MakeFilling::Add(const gp_Pnt& Point)
{
  return myFilling.Add(Point);
}

//=======================================================================
// function : Add
// purpose  : adds a point constraint on a face
//======================================================================
int BRepOffsetAPI_MakeFilling::Add(const double        U,
                                   const double        V,
                                   const TopoDS_Face&  Support,
                                   const GeomAbs_Shape Order)
{
  return myFilling.Add(U, V, Support, Order);
}

//=======================================================================
// function : Build
// purpose  : builds the resulting face
//======================================================================
void BRepOffsetAPI_MakeFilling::Build(const Message_ProgressRange& /*theRange*/)
{
  myFilling.Build();
  myShape = myFilling.Face();
}

//=================================================================================================

bool BRepOffsetAPI_MakeFilling::IsDone() const
{
  return myFilling.IsDone();
}

//=======================================================================
// function : Generated
// purpose  : returns the new edge (first in list) made from old edge "S"
//=======================================================================
const NCollection_List<TopoDS_Shape>& BRepOffsetAPI_MakeFilling::Generated(const TopoDS_Shape& S)
{
  return myFilling.Generated(S);
}

//==========================================================================
// function : G0Error
// purpose  : returns maximum distance from boundary to the resulting surface
//==========================================================================
double BRepOffsetAPI_MakeFilling::G0Error() const
{
  return myFilling.G0Error();
}

//=======================================================================
// function : G1Error
// purpose  : returns maximum angle between the resulting surface
//           and constraint surfaces at boundaries
//======================================================================
double BRepOffsetAPI_MakeFilling::G1Error() const
{
  return myFilling.G1Error();
}

//=======================================================================
// function : G2Error
// purpose  : returns maximum difference of curvature between
//           the resulting surface and constraint surfaces at boundaries
//======================================================================
double BRepOffsetAPI_MakeFilling::G2Error() const
{
  return myFilling.G2Error();
}

//==========================================================================
// function : G0Error
// purpose  : returns maximum distance between the constraint number Index
//           and the resulting surface
//==========================================================================
double BRepOffsetAPI_MakeFilling::G0Error(const int Index)
{
  return myFilling.G0Error(Index);
}

//==========================================================================
// function : G1Error
// purpose  : returns maximum angle between the constraint number Index
//           and the resulting surface
//==========================================================================
double BRepOffsetAPI_MakeFilling::G1Error(const int Index)
{
  return myFilling.G1Error(Index);
}

//==========================================================================
// function : G2Error
// purpose  : returns maximum difference of curvature between
//           the constraint number Index and the resulting surface
//==========================================================================
double BRepOffsetAPI_MakeFilling::G2Error(const int Index)
{
  return myFilling.G2Error(Index);
}
