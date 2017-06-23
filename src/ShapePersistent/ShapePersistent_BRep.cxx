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

#include <Standard_NullObject.hxx>

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

#include <Geom_Surface.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>

enum
{
  ParameterMask   = 1,
  RangeMask       = 2,
  DegeneratedMask = 4
};


//=======================================================================
// PointRepresentation
//=======================================================================
void ShapePersistent_BRep::PointRepresentation::Read
  (StdObjMgt_ReadData& theReadData)
    { theReadData >> myLocation >> myParameter >> myNext; }

void ShapePersistent_BRep::PointRepresentation::Write
  (StdObjMgt_WriteData& theWriteData) const
    { theWriteData << myLocation << myParameter << myNext; }

void ShapePersistent_BRep::PointRepresentation::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  myLocation.PChildren(theChildren);
  theChildren.Append(myNext);
}

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

//=======================================================================
// PointOnCurve
//=======================================================================
void ShapePersistent_BRep::PointOnCurve::Read
  (StdObjMgt_ReadData& theReadData)
{
  PointRepresentation::Read (theReadData);
  theReadData >> myCurve;
}

void ShapePersistent_BRep::PointOnCurve::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  PointRepresentation::Write (theWriteData);
  theWriteData << myCurve;
}

void ShapePersistent_BRep::PointOnCurve::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  PointRepresentation::PChildren(theChildren);
  theChildren.Append(myCurve);
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

//=======================================================================
// PointsOnSurface
//=======================================================================
void ShapePersistent_BRep::PointsOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  PointRepresentation::Read (theReadData);
  theReadData >> mySurface;
}

void ShapePersistent_BRep::PointsOnSurface::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  PointRepresentation::Write (theWriteData);
  theWriteData << mySurface;
}

void ShapePersistent_BRep::PointsOnSurface::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  PointRepresentation::PChildren(theChildren);
  theChildren.Append(mySurface);
}

//=======================================================================
// PointOnCurveOnSurface
//=======================================================================
void ShapePersistent_BRep::PointOnCurveOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  PointsOnSurface::Read (theReadData);
  theReadData >> myPCurve;
}

void ShapePersistent_BRep::PointOnCurveOnSurface::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  PointsOnSurface::Write (theWriteData);
  theWriteData << myPCurve;
}

void ShapePersistent_BRep::PointOnCurveOnSurface::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  PointRepresentation::PChildren(theChildren);
  theChildren.Append(myPCurve);
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

//=======================================================================
// PointOnSurface
//=======================================================================
void ShapePersistent_BRep::PointOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  PointsOnSurface::Read (theReadData);
  theReadData >> myParameter2;
}

void ShapePersistent_BRep::PointOnSurface::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  PointsOnSurface::Write(theWriteData);
  theWriteData << myParameter2;
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
// CurveRepresentation
//=======================================================================
void ShapePersistent_BRep::CurveRepresentation::Read
  (StdObjMgt_ReadData& theReadData)
    { theReadData >> myLocation >> myNext; }

void ShapePersistent_BRep::CurveRepresentation::Write
  (StdObjMgt_WriteData& theWriteData) const
    { theWriteData << myLocation << myNext; }

void ShapePersistent_BRep::CurveRepresentation::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  myLocation.PChildren(theChildren);
  theChildren.Append(myNext);
}

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

//=======================================================================
// GCurve
//=======================================================================
void ShapePersistent_BRep::GCurve::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> myFirst >> myLast;
}

void ShapePersistent_BRep::GCurve::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  CurveRepresentation::Write(theWriteData);
  theWriteData << myFirst << myLast;
}

//=======================================================================
// Curve3D
//=======================================================================
void ShapePersistent_BRep::Curve3D::Read
  (StdObjMgt_ReadData& theReadData)
{
  GCurve::Read (theReadData);
  theReadData >> myCurve3D;
}

void ShapePersistent_BRep::Curve3D::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  GCurve::Write (theWriteData);
  theWriteData << myCurve3D;
}

void ShapePersistent_BRep::Curve3D::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  GCurve::PChildren(theChildren);
  theChildren.Append(myCurve3D);
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

//=======================================================================
// CurveOnSurface
//=======================================================================
void ShapePersistent_BRep::CurveOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  GCurve::Read (theReadData);
  theReadData >> myPCurve >> mySurface >> myUV1 >> myUV2;
}

void ShapePersistent_BRep::CurveOnSurface::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  GCurve::Write (theWriteData);
  theWriteData << myPCurve << mySurface << myUV1 << myUV2;
}

void ShapePersistent_BRep::CurveOnSurface::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  GCurve::PChildren(theChildren);
  theChildren.Append(myPCurve);
  theChildren.Append(mySurface);
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

//=======================================================================
// CurveOnClosedSurface
//=======================================================================
void ShapePersistent_BRep::CurveOnClosedSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveOnSurface::Read (theReadData);
  theReadData >> myPCurve2 >> myContinuity >> myUV21 >> myUV22;
}

void ShapePersistent_BRep::CurveOnClosedSurface::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  CurveOnSurface::Write (theWriteData);
  theWriteData << myPCurve2 << myContinuity << myUV21 << myUV22;
}

void ShapePersistent_BRep::CurveOnClosedSurface::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  CurveOnSurface::PChildren(theChildren);
  theChildren.Append(myPCurve2);
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

//=======================================================================
// Polygon3D
//=======================================================================
void ShapePersistent_BRep::Polygon3D::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> myPolygon3D;
}

void ShapePersistent_BRep::Polygon3D::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  CurveRepresentation::Write (theWriteData);
  theWriteData << myPolygon3D;
}

void ShapePersistent_BRep::Polygon3D::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  CurveRepresentation::PChildren(theChildren);
  theChildren.Append(myPolygon3D);
}

Handle(BRep_CurveRepresentation)
  ShapePersistent_BRep::Polygon3D::import() const
{
  Handle(Poly_Polygon3D) aPolygon3D;
  if (myPolygon3D)
    aPolygon3D = myPolygon3D->Import();

  return new BRep_Polygon3D (aPolygon3D, myLocation.Import());
}

//=======================================================================
// PolygonOnTriangulation
//=======================================================================
void ShapePersistent_BRep::PolygonOnTriangulation::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> myPolygon >> myTriangulation;
}

void ShapePersistent_BRep::PolygonOnTriangulation::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  CurveRepresentation::Write (theWriteData);
  theWriteData << myPolygon << myTriangulation;
}

void ShapePersistent_BRep::PolygonOnTriangulation::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  CurveRepresentation::PChildren(theChildren);
  theChildren.Append(myPolygon);
  theChildren.Append(myTriangulation);
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

//=======================================================================
// PolygonOnClosedTriangulation
//=======================================================================
void ShapePersistent_BRep::PolygonOnClosedTriangulation::Read
  (StdObjMgt_ReadData& theReadData)
{
  PolygonOnTriangulation::Read (theReadData);
  theReadData >> myPolygon2;
}

void ShapePersistent_BRep::PolygonOnClosedTriangulation::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  PolygonOnTriangulation::Write (theWriteData);
  theWriteData << myPolygon2;
}

void ShapePersistent_BRep::PolygonOnClosedTriangulation::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  PolygonOnTriangulation::PChildren(theChildren);
  theChildren.Append(myPolygon2);
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

//=======================================================================
// PolygonOnSurface
//=======================================================================
void ShapePersistent_BRep::PolygonOnSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> myPolygon2D >> mySurface;
}

void ShapePersistent_BRep::PolygonOnSurface::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  CurveRepresentation::Write (theWriteData);
  theWriteData << myPolygon2D << mySurface;
}

void ShapePersistent_BRep::PolygonOnSurface::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  CurveRepresentation::PChildren(theChildren);
  theChildren.Append(myPolygon2D);
  theChildren.Append(mySurface);
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

//=======================================================================
// PolygonOnClosedSurface
//=======================================================================
void ShapePersistent_BRep::PolygonOnClosedSurface::Read
  (StdObjMgt_ReadData& theReadData)
{
  PolygonOnSurface::Read (theReadData);
  theReadData >> myPolygon2;
}

void ShapePersistent_BRep::PolygonOnClosedSurface::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  PolygonOnSurface::Write (theWriteData);
  theWriteData << myPolygon2;
}

void ShapePersistent_BRep::PolygonOnClosedSurface::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  PolygonOnSurface::PChildren(theChildren);
  theChildren.Append(myPolygon2);
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

//=======================================================================
// CurveOn2Surfaces
//=======================================================================
void ShapePersistent_BRep::CurveOn2Surfaces::Read
  (StdObjMgt_ReadData& theReadData)
{
  CurveRepresentation::Read (theReadData);
  theReadData >> mySurface >> mySurface2 >> myLocation2 >> myContinuity;
}

void ShapePersistent_BRep::CurveOn2Surfaces::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  CurveRepresentation::Write (theWriteData);
  theWriteData << mySurface << mySurface2 << myLocation2 << myContinuity;
}

void ShapePersistent_BRep::CurveOn2Surfaces::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  CurveRepresentation::PChildren(theChildren);
  theChildren.Append(mySurface);
  theChildren.Append(mySurface2);
  myLocation2.PChildren(theChildren);
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

  if (myPoints)
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

  if (myCurves)
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

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::TVertex::pTObjectT)
ShapePersistent_BRep::Translate (const TopoDS_Vertex& theVertex,
                                 StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(BRep_TVertex) TTV = Handle(BRep_TVertex)::DownCast (theVertex.TShape());

  Handle(TVertex::pTObjectT) PTV = new TVertex::pTObjectT;

  PTV->myPnt = TTV->Pnt();
  PTV->myTolerance = TTV->Tolerance();

  BRep_ListIteratorOfListOfPointRepresentation anItPR(TTV->Points());

  Handle(PointRepresentation) PPR, CPPR;
  while (anItPR.More())
  {
    const Handle(BRep_PointRepresentation)& PR = anItPR.Value();
    if (PR->IsPointOnCurve()) {
      Handle(PointOnCurve) POC = Translate(PR->Parameter(), 
                                           PR->Curve(), 
                                           PR->Location(), 
                                           theMap);
      CPPR = POC;
    }
    else if (PR->IsPointOnCurveOnSurface()) {
      Handle(PointOnCurveOnSurface) POCS = Translate(PR->Parameter(), 
                                                     PR->PCurve(), 
                                                     PR->Surface(), 
                                                     PR->Location(), 
                                                     theMap);
      CPPR = POCS;
    }
    else if (PR->IsPointOnSurface()) {
      Handle(PointOnSurface) POS = Translate(PR->Parameter(), 
                                             PR->Parameter2(), 
                                             PR->Surface(), 
                                             PR->Location(), 
                                             theMap);
      CPPR = POS;
    }

    CPPR->myNext = PPR;
    PPR = CPPR;
    anItPR.Next();
  }

  PTV->myPoints = PPR;

  return PTV;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::TEdge::pTObjectT)
ShapePersistent_BRep::Translate (const TopoDS_Edge& theEdge,
                                 StdObjMgt_TransientPersistentMap& theMap,
                                 ShapePersistent_TriangleMode theTriangleMode)
{
  Handle(BRep_TEdge) TTE = Handle(BRep_TEdge)::DownCast (theEdge.TShape());

  Handle(TEdge::pTObjectT) PTE = new TEdge::pTObjectT;

  PTE->myTolerance = TTE->Tolerance();
  if (TTE->SameParameter()) PTE->myFlags |= ParameterMask;
  if (TTE->SameRange())     PTE->myFlags |= RangeMask;
  if (TTE->Degenerated())   PTE->myFlags |= DegeneratedMask;

  // Representations
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TTE->Curves());

  Handle(CurveRepresentation) PCR, CPCR;
  Handle(BRep_GCurve) GC;
  Standard_Real f, l;

  while (itcr.More()) 
  {
    const Handle(BRep_CurveRepresentation)& CR = itcr.Value();
    GC = Handle(BRep_GCurve)::DownCast(CR);
    if (!GC.IsNull()) 
    {
      GC->Range(f, l);
      // CurveRepresentation is Curve3D
      if (CR->IsCurve3D()) {
        Handle(Curve3D) C3D = Translate(CR->Curve3D(), f, l, CR->Location(), theMap);
        CPCR = C3D;
      }
      // CurveRepresentation is CurveOnSurface
      else if (CR->IsCurveOnSurface()) 
      {
        Handle(BRep_CurveOnSurface)& theCOS = (Handle(BRep_CurveOnSurface)&) CR;
        Handle(CurveOnSurface) COS;
        // CurveRepresentation is CurveOnSurface
        if (!CR->IsCurveOnClosedSurface()) 
        {
          COS = Translate(CR->PCurve(), f, l, CR->Surface(), CR->Location(), theMap);
        }
        // CurveRepresentation is CurveOnClosedSurface
        else 
        {
          // get UVPoints for the CurveOnClosedSurface definition.
          Handle(BRep_CurveOnClosedSurface)& theCOCS =
            (Handle(BRep_CurveOnClosedSurface)&) CR;
          gp_Pnt2d Pnt21, Pnt22;
          theCOCS->UVPoints2(Pnt21, Pnt22);
          Handle(CurveOnClosedSurface) COCS = Translate(CR->PCurve(), CR->PCurve2(), 
                                                        f, l, CR->Surface(), 
                                                        CR->Location(), CR->Continuity(), 
                                                        theMap);
          COCS->myUV21 = Pnt21;
          COCS->myUV22 = Pnt22;
          COS = COCS;
        }

        // get UVPoints for the CurveOnSurface definition.
        gp_Pnt2d Pnt1, Pnt2;
        theCOS->UVPoints(Pnt1, Pnt2);
        COS->myUV1 = Pnt1;
        COS->myUV2 = Pnt2;
        CPCR = COS;
      }
    }
    // CurveRepresentation is CurveOn2Surfaces
    else if (CR->IsRegularity()) 
    {
      Handle(CurveOn2Surfaces) R = Translate(CR->Surface(), CR->Surface2(), 
                                             CR->Location(), CR->Location2(), 
                                             CR->Continuity(), 
                                             theMap);
      CPCR = R;
    }
    // CurveRepresentation is Polygon or Triangulation
    else if (theTriangleMode == ShapePersistent_WithTriangle) {
      // CurveRepresentation is Polygon3D
      if (CR->IsPolygon3D()) {
        Handle(Polygon3D) P3D = Translate(CR->Polygon3D(), CR->Location(), theMap);
        CPCR = P3D;
      }
      // CurveRepresentation is PolygonOnSurface
      else if (CR->IsPolygonOnSurface()) 
      {
        // CurveRepresentation is PolygonOnClosedSurface
        if (CR->IsPolygonOnClosedSurface()) {
          Handle(PolygonOnClosedSurface) PolOCS = Translate(CR->Polygon(), CR->Polygon2(), 
                                                            CR->Surface(), CR->Location(), 
                                                            theMap);
          CPCR = PolOCS;
        }
        // CurveRepresentation is PolygonOnSurface
        else 
        {
          Handle(PolygonOnSurface) PolOS = Translate(CR->Polygon(), CR->Surface(), 
                                                     CR->Location(), theMap);
          CPCR = PolOS;
        }
      }
      // CurveRepresentation is PolygonOnTriangulation
      else if (CR->IsPolygonOnTriangulation()) 
      {
        // CurveRepresentation is PolygonOnClosedTriangulation
        if (CR->IsPolygonOnClosedTriangulation()) 
        {
          Handle(PolygonOnClosedTriangulation) PolOCT = Translate(CR->PolygonOnTriangulation(), 
                                                                  CR->PolygonOnTriangulation2(), 
                                                                  CR->Triangulation(), 
                                                                  CR->Location(), 
                                                                  theMap);
          CPCR = PolOCT;
        }
        // CurveRepresentation is PolygonOnTriangulation
        else 
        {
          Handle(PolygonOnTriangulation) PolOT = Translate(CR->PolygonOnTriangulation(), 
                                                           CR->Triangulation(), 
                                                           CR->Location(), 
                                                           theMap);
          CPCR = PolOT;
        }
      }
    }
    else 
    {
      // jumps the curve representation
      itcr.Next();
      continue;
    }

    Standard_NullObject_Raise_if(CPCR.IsNull(), "Null CurveRepresentation");

    CPCR->myNext = PCR;
    PCR = CPCR;
    itcr.Next();
  }

  // set
  PTE->myCurves = PCR;

  return PTE;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::TFace::pTObjectT)
ShapePersistent_BRep::Translate (const TopoDS_Face& theFace,
                                 StdObjMgt_TransientPersistentMap& theMap,
                                 ShapePersistent_TriangleMode theTriangleMode)
{
  Handle(BRep_TFace) TTF = Handle(BRep_TFace)::DownCast (theFace.TShape());

  Handle(TFace::pTObjectT) PTF = new TFace::pTObjectT;

  PTF->myTolerance = TTF->Tolerance();
  PTF->myLocation = StdObject_Location::Translate(TTF->Location(), theMap);
  PTF->myNaturalRestriction = TTF->NaturalRestriction();

  // Surface
  PTF->mySurface = ShapePersistent_Geom::Translate(TTF->Surface(), theMap);

  // Triangulation
  if (theTriangleMode == ShapePersistent_WithTriangle) {
    PTF->myTriangulation = ShapePersistent_Poly::Translate(TTF->Triangulation(), theMap);
  }

  return PTF;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::PointOnCurve)
ShapePersistent_BRep::Translate (Standard_Real theParam,
                                 const Handle(Geom_Curve)& theCurve,
                                 const TopLoc_Location& theLoc,
                                 StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(PointOnCurve) aPPonC = new PointOnCurve;
  aPPonC->myParameter = theParam;
  aPPonC->myCurve = ShapePersistent_Geom::Translate(theCurve, theMap);
  aPPonC->myLocation = StdObject_Location::Translate(theLoc, theMap);
  return aPPonC;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::PointOnCurveOnSurface)
ShapePersistent_BRep::Translate(Standard_Real theParam,
                                const Handle(Geom2d_Curve)& theCurve,
                                const Handle(Geom_Surface)& theSurf,
                                const TopLoc_Location& theLoc,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(PointOnCurveOnSurface) aPPonConS = new PointOnCurveOnSurface;
  aPPonConS->myParameter = theParam;
  aPPonConS->myPCurve = ShapePersistent_Geom2d::Translate(theCurve, theMap);
  aPPonConS->mySurface = ShapePersistent_Geom::Translate(theSurf, theMap);
  aPPonConS->myLocation = StdObject_Location::Translate(theLoc, theMap);
  return aPPonConS;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::PointOnSurface)
ShapePersistent_BRep::Translate(Standard_Real theParam,
                                Standard_Real theParam2,
                                const Handle(Geom_Surface)& theSurf,
                                const TopLoc_Location& theLoc,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(PointOnSurface) aPonS = new PointOnSurface;
  aPonS->myParameter = theParam;
  aPonS->myParameter2 = theParam2;
  aPonS->mySurface = ShapePersistent_Geom::Translate(theSurf, theMap);
  aPonS->myLocation = StdObject_Location::Translate(theLoc, theMap);
  return aPonS;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::CurveOnSurface)
ShapePersistent_BRep::Translate(const Handle(Geom2d_Curve)& theCurve,
                                const Standard_Real theFirstParam,
                                const Standard_Real theLastParam,
                                const Handle(Geom_Surface)& theSurf,
                                const TopLoc_Location& theLoc,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(CurveOnSurface) aPConS = new CurveOnSurface;
  aPConS->myPCurve = ShapePersistent_Geom2d::Translate(theCurve, theMap);
  aPConS->myFirst = theFirstParam;
  aPConS->myLast = theLastParam;
  aPConS->mySurface = ShapePersistent_Geom::Translate(theSurf, theMap);
  aPConS->myLocation = StdObject_Location::Translate(theLoc, theMap);
  return aPConS;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::CurveOnClosedSurface)
ShapePersistent_BRep::Translate(const Handle(Geom2d_Curve)& theCurve,
                                const Handle(Geom2d_Curve)& theCurve2,
                                const Standard_Real theFirstParam,
                                const Standard_Real theLastParam,
                                const Handle(Geom_Surface)& theSurf,
                                const TopLoc_Location& theLoc,
                                const GeomAbs_Shape theContinuity,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(CurveOnClosedSurface) aPConCS = new CurveOnClosedSurface;
  aPConCS->myPCurve = ShapePersistent_Geom2d::Translate(theCurve, theMap);
  aPConCS->myPCurve2 = ShapePersistent_Geom2d::Translate(theCurve2, theMap);
  aPConCS->myFirst = theFirstParam;
  aPConCS->myLast = theLastParam;
  aPConCS->mySurface = ShapePersistent_Geom::Translate(theSurf, theMap);
  aPConCS->myLocation = StdObject_Location::Translate(theLoc, theMap);
  aPConCS->myContinuity = theContinuity;
  return aPConCS;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::CurveOn2Surfaces)
ShapePersistent_BRep::Translate(const Handle(Geom_Surface)& theSurf,
                                const Handle(Geom_Surface)& theSurf2,
                                const TopLoc_Location& theLoc, 
                                const TopLoc_Location& theLoc2,
                                const GeomAbs_Shape theContinuity,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(CurveOn2Surfaces) aPCon2S = new CurveOn2Surfaces;
  aPCon2S->mySurface = ShapePersistent_Geom::Translate(theSurf, theMap);
  aPCon2S->mySurface2 = ShapePersistent_Geom::Translate(theSurf2, theMap);
  aPCon2S->myLocation = StdObject_Location::Translate(theLoc, theMap);
  aPCon2S->myLocation2 = StdObject_Location::Translate(theLoc2, theMap);
  aPCon2S->myContinuity = theContinuity;
  return aPCon2S;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::Curve3D)
ShapePersistent_BRep::Translate (const Handle(Geom_Curve)& theCurve,
                                 const Standard_Real theFirstParam,
                                 const Standard_Real theLastParam,
                                 const TopLoc_Location& theLoc,
                                 StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(Curve3D) aPCurve3D = new Curve3D;
  aPCurve3D->myCurve3D = ShapePersistent_Geom::Translate(theCurve, theMap);
  aPCurve3D->myLocation = StdObject_Location::Translate(theLoc, theMap);
  aPCurve3D->myFirst = theFirstParam;
  aPCurve3D->myLast = theLastParam;
  return aPCurve3D;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::Polygon3D)
ShapePersistent_BRep::Translate(const Handle(Poly_Polygon3D)& thePoly,
                                const TopLoc_Location& theLoc,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(Polygon3D) aPPoly = new Polygon3D;
  aPPoly->myPolygon3D = ShapePersistent_Poly::Translate(thePoly, theMap);
  aPPoly->myLocation = StdObject_Location::Translate(theLoc, theMap);
  return aPPoly;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::PolygonOnClosedSurface)
ShapePersistent_BRep::Translate(const Handle(Poly_Polygon2D)& thePoly,
                                const Handle(Poly_Polygon2D)& thePoly2,
                                const Handle(Geom_Surface)& theSurf,
                                const TopLoc_Location& theLoc,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(PolygonOnClosedSurface) aPPonCS = new PolygonOnClosedSurface;
  aPPonCS->myPolygon2D = ShapePersistent_Poly::Translate(thePoly, theMap);
  aPPonCS->myPolygon2 = ShapePersistent_Poly::Translate(thePoly2, theMap);
  aPPonCS->mySurface = ShapePersistent_Geom::Translate(theSurf, theMap);
  aPPonCS->myLocation = StdObject_Location::Translate(theLoc, theMap);
  return aPPonCS;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::PolygonOnSurface)
ShapePersistent_BRep::Translate(const Handle(Poly_Polygon2D)& thePoly,
                                const Handle(Geom_Surface)& theSurf,
                                const TopLoc_Location& theLoc,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(PolygonOnSurface) aPPonS = new PolygonOnSurface;
  aPPonS->myPolygon2D = ShapePersistent_Poly::Translate(thePoly, theMap);
  aPPonS->mySurface = ShapePersistent_Geom::Translate(theSurf, theMap);
  aPPonS->myLocation = StdObject_Location::Translate(theLoc, theMap);
  return aPPonS;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::PolygonOnClosedTriangulation)
ShapePersistent_BRep::Translate(const Handle(Poly_PolygonOnTriangulation)& thePolyOnTriang,
                                const Handle(Poly_PolygonOnTriangulation)& thePolyOnTriang2,
                                const Handle(Poly_Triangulation)& thePolyTriang,
                                const TopLoc_Location& theLoc,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(PolygonOnClosedTriangulation) aPPonCS = new PolygonOnClosedTriangulation;
  aPPonCS->myPolygon = ShapePersistent_Poly::Translate(thePolyOnTriang, theMap);
  aPPonCS->myPolygon2 = ShapePersistent_Poly::Translate(thePolyOnTriang2, theMap);
  aPPonCS->myTriangulation = ShapePersistent_Poly::Translate(thePolyTriang, theMap);
  aPPonCS->myLocation = StdObject_Location::Translate(theLoc, theMap);
  return aPPonCS;
}

//=======================================================================
//function : Translate
//purpose  : Translates a shape to its persistent avatar
//=======================================================================
Handle(ShapePersistent_BRep::PolygonOnTriangulation)
ShapePersistent_BRep::Translate(const Handle(Poly_PolygonOnTriangulation)& thePolyOnTriang,
                                const Handle(Poly_Triangulation)& thePolyTriang,
                                const TopLoc_Location& theLoc,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(PolygonOnTriangulation) aPPonT = new PolygonOnTriangulation;
  aPPonT->myPolygon = ShapePersistent_Poly::Translate(thePolyOnTriang, theMap);
  aPPonT->myTriangulation = ShapePersistent_Poly::Translate(thePolyTriang, theMap);
  aPPonT->myLocation = StdObject_Location::Translate(theLoc, theMap);
  return aPPonT;
}
