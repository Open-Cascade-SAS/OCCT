// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <ShapePersistent_BRep.hxx>

#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_Curve3D.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_PolygonOnClosedTriangulation.hxx>
#include <BRep_PolygonOnSurface.hxx>
#include <BRep_PolygonOnClosedSurface.hxx>
#include <BRep_CurveOn2Surfaces.hxx>

#include <BRep_TVertex.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>


enum
{
  ParameterMask   = 1,
  RangeMask       = 2,
  DegeneratedMask = 4
};


//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void ShapePersistent_BRep::PointRepresentation::Read
  (StdObjMgt_ReadData& theReadData)
    { theReadData >> myLocation >> myParameter >> myNext; }

//=======================================================================
//function : Import
//purpose  : Import transient object from the persistent data
//=======================================================================
void ShapePersistent_BRep::PointRepresentation::Import
  (BRep_ListOfPointRepresentation& thePoints) const
{
  thePoints.Clear();
  Handle(PointRepresentation) aPoint = this;
  for (; aPoint; aPoint = aPoint->myNext)
    thePoints.Prepend (aPoint->import());
}

Handle(BRep_PointRepresentation)
  ShapePersistent_BRep::PointRepresentation::import() const
    { return NULL; }

void ShapePersistent_BRep::PointOnCurve::Read
  (StdObjMgt_ReadData& theReadData)
{
  PointRepresentation::Read (theReadData);
  theReadData >> myCurve;
}

Handle(BRep_PointRepresentation)
  ShapePersistent_BRep::PointOnCurve::import() const
{
  Handle(Geom_Curve) aCurve;
  if (myCurve)
    aCurve = myCurve->Import();

  return new BRep_PointOnCurve
    (myParameter, aCurve, myLocation.Import());
}

void ShapePersistent_BRep::PointsOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  PointRepresentation::Read (theReadData);
  theReadData >> mySurface;
}

void ShapePersistent_BRep::PointOnCurveOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  PointsOnSurface::Read (theReadData);
  theReadData >> myPCurve;
}

Handle(BRep_PointRepresentation)
  ShapePersistent_BRep::PointOnCurveOnSurface::import() const
{
  Handle(Geom2d_Curve) aPCurve;
  if (myPCurve)
    aPCurve = myPCurve->Import();

  Handle(Geom_Surface) aSurface;
  if (mySurface)
    aSurface = mySurface->Import();

  return new BRep_PointOnCurveOnSurface
    (myParameter, aPCurve, aSurface, myLocation.Import());
}

void ShapePersistent_BRep::PointOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  PointsOnSurface::Read (theReadData);
  theReadData >> myParameter2;
}

Handle(BRep_PointRepresentation)
  ShapePersistent_BRep::PointOnSurface::import() const
{
  Handle(Geom_Surface) aSurface;
  if (mySurface)
    aSurface = mySurface->Import();

  return new BRep_PointOnSurface
    (myParameter, myParameter2, aSurface, myLocation.Import());
}

//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void ShapePersistent_BRep::CurveRepresentation::Read
  (StdObjMgt_ReadData& theReadData)
    { theReadData >> myLocation >> myNext; }

//=======================================================================
//function : Import
//purpose  : Import transient object from the persistent data
//=======================================================================
void ShapePersistent_BRep::CurveRepresentation::Import
  (BRep_ListOfCurveRepresentation& theCurves) const
{
  theCurves.Clear();
  Handle(CurveRepresentation) aCurve = this;
  for (; aCurve; aCurve = aCurve->myNext)
    theCurves.Prepend (aCurve->import());
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::CurveRepresentation::import() const
    { return NULL; }

void ShapePersistent_BRep::GCurve::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> myFirst >> myLast;
}

void ShapePersistent_BRep::Curve3D::Read
  (StdObjMgt_ReadData& theReadData)
{
  GCurve::Read (theReadData);
  theReadData >> myCurve3D;
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::Curve3D::import() const
{
  Handle(Geom_Curve) aCurve3D;
  if (myCurve3D)
    aCurve3D = myCurve3D->Import();

  Handle(BRep_Curve3D) aRepresentation =
    new BRep_Curve3D (aCurve3D, myLocation.Import());

  aRepresentation->SetRange (myFirst, myLast);
  return aRepresentation;
}

void ShapePersistent_BRep::CurveOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  GCurve::Read (theReadData);
  theReadData >> myPCurve >> mySurface >> myUV1 >> myUV2;
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::CurveOnSurface::import() const
{
  Handle(Geom2d_Curve) aPCurve;
  if (myPCurve)
    aPCurve = myPCurve->Import();

  Handle(Geom_Surface) aSurface;
  if (mySurface)
    aSurface = mySurface->Import();

  Handle(BRep_CurveOnSurface) aRepresentation =
    new BRep_CurveOnSurface (aPCurve, aSurface, myLocation.Import());

  aRepresentation->SetUVPoints (myUV1, myUV2);
  aRepresentation->SetRange (myFirst, myLast);

  return aRepresentation;
}

void ShapePersistent_BRep::CurveOnClosedSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveOnSurface::Read (theReadData);
  theReadData >> myPCurve2 >> myContinuity >> myUV21 >> myUV22;
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::CurveOnClosedSurface::import() const
{
  Handle(Geom2d_Curve) aPCurve;
  if (myPCurve)
    aPCurve = myPCurve->Import();

  Handle(Geom2d_Curve) aPCurve2;
  if (myPCurve2)
    aPCurve2 = myPCurve2->Import();

  Handle(Geom_Surface) aSurface;
  if (mySurface)
    aSurface = mySurface->Import();

  GeomAbs_Shape aContinuity = static_cast<GeomAbs_Shape> (myContinuity);

  Handle(BRep_CurveOnClosedSurface) aRepresentation =
    new BRep_CurveOnClosedSurface
      (aPCurve, aPCurve2, aSurface, myLocation.Import(), aContinuity);

  aRepresentation->SetUVPoints  (myUV1  , myUV2 );
  aRepresentation->SetUVPoints2 (myUV21 , myUV22);
  aRepresentation->SetRange     (myFirst, myLast);

  return aRepresentation;
}

void ShapePersistent_BRep::Polygon3D::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> myPolygon3D;
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::Polygon3D::import() const
{
  Handle(Poly_Polygon3D) aPolygon3D;
  if (myPolygon3D)
    aPolygon3D = myPolygon3D->Import();

  return new BRep_Polygon3D (aPolygon3D, myLocation.Import());
}

void ShapePersistent_BRep::PolygonOnTriangulation::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> myPolygon >> myTriangulation;
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::PolygonOnTriangulation::import() const
{
  Handle(Poly_PolygonOnTriangulation) aPolygon;
  if (myPolygon)
    aPolygon = myPolygon->Import();

  Handle(Poly_Triangulation) aTriangulation;
  if (myTriangulation)
    aTriangulation = myTriangulation->Import();

  return new BRep_PolygonOnTriangulation
    (aPolygon, aTriangulation, myLocation.Import());
}

void ShapePersistent_BRep::PolygonOnClosedTriangulation::Read
  (StdObjMgt_ReadData& theReadData)
{
  PolygonOnTriangulation::Read (theReadData);
  theReadData >> myPolygon2;
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::PolygonOnClosedTriangulation::import() const
{
  Handle(Poly_PolygonOnTriangulation) aPolygon;
  if (myPolygon)
    aPolygon = myPolygon->Import();

  Handle(Poly_PolygonOnTriangulation) aPolygon2;
  if (myPolygon2)
    aPolygon2 = myPolygon2->Import();

  Handle(Poly_Triangulation) aTriangulation;
  if (myTriangulation)
    aTriangulation = myTriangulation->Import();

  return new BRep_PolygonOnClosedTriangulation
    (aPolygon, aPolygon2, aTriangulation, myLocation.Import());
}

void ShapePersistent_BRep::PolygonOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> myPolygon2D >> mySurface;
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::PolygonOnSurface::import() const
{
  Handle(Poly_Polygon2D) aPolygon2D;
  if (myPolygon2D)
    aPolygon2D = myPolygon2D->Import();

  Handle(Geom_Surface) aSurface;
  if (mySurface)
    aSurface = mySurface->Import();

  return new BRep_PolygonOnSurface (aPolygon2D, aSurface, myLocation.Import());
}

void ShapePersistent_BRep::PolygonOnClosedSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  PolygonOnSurface::Read (theReadData);
  theReadData >> myPolygon2;
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::PolygonOnClosedSurface::import() const
{
  Handle(Poly_Polygon2D) aPolygon2D;
  if (myPolygon2D)
    aPolygon2D = myPolygon2D->Import();

  Handle(Poly_Polygon2D) aPolygon2;
  if (myPolygon2)
    aPolygon2 = myPolygon2->Import();

  Handle(Geom_Surface) aSurface;
  if (mySurface)
    aSurface = mySurface->Import();

  return new BRep_PolygonOnClosedSurface
    (aPolygon2D, aPolygon2, aSurface, myLocation.Import());
}

void ShapePersistent_BRep::CurveOn2Surfaces::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> mySurface >> mySurface2 >> myLocation2 >> myContinuity;
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::CurveOn2Surfaces::import() const
{
  Handle(Geom_Surface) aSurface;
  if (mySurface)
    aSurface = mySurface->Import();

  Handle(Geom_Surface) aSurface2;
  if (mySurface2)
    aSurface2 = mySurface2->Import();

  GeomAbs_Shape aContinuity = static_cast<GeomAbs_Shape> (myContinuity);

  return new BRep_CurveOn2Surfaces
    (aSurface, aSurface2, myLocation.Import(), myLocation2.Import(), aContinuity);
}

//=======================================================================
//function : createTShape
//purpose  : Create transient TShape object
//=======================================================================
Handle(TopoDS_TShape) ShapePersistent_BRep::pTVertex::createTShape() const
{
  Handle(BRep_TVertex) aTVertex = new BRep_TVertex;

  aTVertex->Tolerance (myTolerance);
  aTVertex->Pnt       (myPnt);

  myPoints->Import (aTVertex->ChangePoints());

  return aTVertex;
}

//=======================================================================
//function : createTShape
//purpose  : Create transient TShape object
//=======================================================================
Handle(TopoDS_TShape) ShapePersistent_BRep::pTEdge::createTShape() const
{
  Handle(BRep_TEdge) aTEdge = new BRep_TEdge;

  aTEdge->Tolerance     (myTolerance);
  aTEdge->SameParameter ((myFlags & ParameterMask)   != 0);
  aTEdge->SameRange     ((myFlags & RangeMask)       != 0);
  aTEdge->Degenerated   ((myFlags & DegeneratedMask) != 0);

  myCurves->Import (aTEdge->ChangeCurves());

  return aTEdge;
}

//=======================================================================
//function : createTShape
//purpose  : Create transient TShape object
//=======================================================================
Handle(TopoDS_TShape) ShapePersistent_BRep::pTFace::createTShape() const
{
  Handle(BRep_TFace) aTFace = new BRep_TFace;

  aTFace->NaturalRestriction (myNaturalRestriction);
  aTFace->Tolerance          (myTolerance);
  aTFace->Location           (myLocation.Import());

  if (mySurface)
    aTFace->Surface (mySurface->Import());

  if (myTriangulation)
    aTFace->Triangulation (myTriangulation->Import());

  return aTFace;
}
