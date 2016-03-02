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
  if (myCurve.IsNull())
    return NULL;

  return new BRep_PointOnCurve
    (myParameter, myCurve->Import(), myLocation.Import());
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
  if (mySurface.IsNull() || myPCurve.IsNull())
    return NULL;

  return new BRep_PointOnCurveOnSurface
    (myParameter, myPCurve->Import(), mySurface->Import(), myLocation.Import());
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
  if (mySurface.IsNull())
    return NULL;

  return new BRep_PointOnSurface
    (myParameter, myParameter2, mySurface->Import(), myLocation.Import());
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
  Handle(Geom_Curve) aCurve;
  if (myCurve3D)
    aCurve = myCurve3D->Import();

  Handle(BRep_Curve3D) aRepresentation =
    new BRep_Curve3D (aCurve, myLocation.Import());

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
  if (myPCurve.IsNull() || mySurface.IsNull())
    return NULL;

  Handle(BRep_CurveOnSurface) aCurve =
    new BRep_CurveOnSurface (myPCurve->Import(),
                             mySurface->Import(),
                             myLocation.Import());

  aCurve->SetUVPoints (myUV1, myUV2);
  aCurve->SetRange (myFirst, myLast);

  return aCurve;
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
  if (myPCurve.IsNull() || mySurface.IsNull() || myPCurve2.IsNull())
    return NULL;

  Handle(BRep_CurveOnClosedSurface) aCurve =
    new BRep_CurveOnClosedSurface (myPCurve->Import(),
                                   myPCurve2->Import(),
                                   mySurface->Import(),
                                   myLocation.Import(),
                                   myContinuity);

  aCurve->SetUVPoints  (myUV1  , myUV2 );
  aCurve->SetUVPoints2 (myUV21 , myUV22);
  aCurve->SetRange     (myFirst, myLast);

  return aCurve;
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
  if (myPolygon3D.IsNull())
    return NULL;

  return new BRep_Polygon3D (myPolygon3D->Import(), myLocation.Import());
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
  if (myPolygon.IsNull() || myTriangulation.IsNull())
    return NULL;

  return new BRep_PolygonOnTriangulation (myPolygon->Import(),
                                          myTriangulation->Import(),
                                          myLocation.Import());
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
  if (myPolygon.IsNull() || myTriangulation.IsNull() || myPolygon2.IsNull())
    return NULL;

  return new BRep_PolygonOnClosedTriangulation (myPolygon->Import(),
                                                myPolygon2->Import(),
                                                myTriangulation->Import(),
                                                myLocation.Import());
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
  if (myPolygon2D.IsNull() || mySurface.IsNull())
    return NULL;

  return new BRep_PolygonOnSurface (myPolygon2D->Import(),
                                    mySurface->Import(),
                                    myLocation.Import());
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
  if (myPolygon2D.IsNull() || mySurface.IsNull() || myPolygon2.IsNull())
    return NULL;

  return new BRep_PolygonOnClosedSurface (myPolygon2D->Import(),
                                          myPolygon2->Import(),
                                          mySurface->Import(),
                                          myLocation.Import());
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
  if (mySurface.IsNull() || mySurface2.IsNull())
    return NULL;

  return new BRep_CurveOn2Surfaces (mySurface->Import(),
                                    mySurface2->Import(),
                                    myLocation.Import(),
                                    myLocation2.Import(),
                                    myContinuity);
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
  aTEdge->SameParameter (myFlags & ParameterMask);
  aTEdge->SameRange     (myFlags & RangeMask);
  aTEdge->Degenerated   (myFlags & DegeneratedMask);

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
