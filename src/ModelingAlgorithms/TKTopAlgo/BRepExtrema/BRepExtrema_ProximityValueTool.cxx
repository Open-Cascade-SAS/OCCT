// Created on: 2022-08-08
// Created by: Kseniya NOSULKO
// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <BRepExtrema_ProximityValueTool.hxx>
#include <BRepExtrema_ProximityDistTool.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepGProp.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <GProp_GProps.hxx>
#include <Poly_Connect.hxx>
#include <TopoDS.hxx>

//=======================================================================
// function : BRepExtrema_ProximityValueTool
// purpose  : Creates new uninitialized proximity tool
//=======================================================================
BRepExtrema_ProximityValueTool::BRepExtrema_ProximityValueTool()
    : myIsRefinementRequired1(Standard_False),
      myIsRefinementRequired2(Standard_False),
      myDistance(std::numeric_limits<Standard_Real>::max()),
      myIsDone(Standard_False),
      myNbSamples1(0),
      myNbSamples2(0)
{
  // Should be initialized later
  myIsInitS1 = myIsInitS2 = Standard_False;
}

//=======================================================================
// function : BRepExtrema_ProximityValueTool
// purpose  : Creates new proximity tool for the given element sets
//=======================================================================
BRepExtrema_ProximityValueTool::BRepExtrema_ProximityValueTool(
  const Handle(BRepExtrema_TriangleSet)& theSet1,
  const Handle(BRepExtrema_TriangleSet)& theSet2,
  const BRepExtrema_ShapeList&           theShapeList1,
  const BRepExtrema_ShapeList&           theShapeList2)
    : myIsRefinementRequired1(Standard_False),
      myIsRefinementRequired2(Standard_False),
      myDistance(std::numeric_limits<Standard_Real>::max()),
      myIsDone(Standard_False),
      myNbSamples1(0),
      myNbSamples2(0)
{
  LoadShapeLists(theShapeList1, theShapeList2);
  LoadTriangleSets(theSet1, theSet2);
}

//=======================================================================
// function : LoadTriangleSets
// purpose  : Loads the given element sets into the proximity tool
//=======================================================================
void BRepExtrema_ProximityValueTool::LoadTriangleSets(
  const Handle(BRepExtrema_TriangleSet)& theSet1,
  const Handle(BRepExtrema_TriangleSet)& theSet2)
{
  mySet1 = theSet1;
  mySet2 = theSet2;

  MarkDirty();
}

//=======================================================================
// function : calcEdgeRefinementStep
// purpose  : Calculates the edge refinement step
//=======================================================================
static Standard_Real calcEdgeRefinementStep(const TopoDS_Edge&     theEdge,
                                            const Standard_Integer theNbNodes)
{
  if (theNbNodes < 2)
    return 0;

  BRepAdaptor_Curve aBAC(theEdge);
  Standard_Real     aLen = GCPnts_AbscissaPoint::Length(aBAC);
  return aLen / (Standard_Real)(theNbNodes - 1);
}

//=======================================================================
// function : calcFaceRefinementStep
// purpose  : Calculates the face refinement step as an approximate square
// (Shape area / number triangles) * 2
//=======================================================================
static Standard_Real calcFaceRefinementStep(const TopoDS_Face&     theFace,
                                            const Standard_Integer theNbTrg)
{
  if (theNbTrg < 1)
    return 0;

  GProp_GProps props;
  BRepGProp::SurfaceProperties(theFace, props);
  Standard_Real aArea = props.Mass();
  return 2 * (aArea / (Standard_Real)theNbTrg);
}

//=======================================================================
// function : getInfoForRefinement
// purpose  : Gets shape data for further refinement
//=======================================================================
Standard_Boolean BRepExtrema_ProximityValueTool::getInfoForRefinement(
  const TopoDS_Shape& theShape,
  TopAbs_ShapeEnum&   theShapeType,
  Standard_Integer&   theNbNodes,
  Standard_Real&      theStep)
{
  if (theShape.ShapeType() == TopAbs_FACE)
  {
    theShapeType   = TopAbs_FACE;
    TopoDS_Face aF = TopoDS::Face(theShape);

    TopLoc_Location            aLocation;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(aF, aLocation);

    if (aTriangulation.IsNull())
    {
      return Standard_False;
    }

    theNbNodes              = aTriangulation->NbNodes();
    Standard_Integer aNbTrg = aTriangulation->NbTriangles();
    theStep                 = calcFaceRefinementStep(aF, aNbTrg);
  }
  else if (theShape.ShapeType() == TopAbs_EDGE)
  {
    theShapeType   = TopAbs_EDGE;
    TopoDS_Edge aE = TopoDS::Edge(theShape);

    TopLoc_Location        aLocation;
    Handle(Poly_Polygon3D) aPolygon = BRep_Tool::Polygon3D(aE, aLocation);

    if (aPolygon.IsNull())
    {
      return Standard_False;
    }

    theNbNodes = aPolygon->NbNodes();
    theStep    = calcEdgeRefinementStep(aE, theNbNodes);
  }
  else
  {
    return Standard_False;
  }

  if (theStep < Precision::Confusion())
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
// function : LoadTriangleSets
// purpose  : Loads the given list of subshapes into the proximity tool
//=======================================================================
void BRepExtrema_ProximityValueTool::LoadShapeLists(const BRepExtrema_ShapeList& theShapeList1,
                                                    const BRepExtrema_ShapeList& theShapeList2)
{
  myShapeList1 = theShapeList1;
  myShapeList2 = theShapeList2;

  myShape1   = theShapeList1(0);
  myIsInitS1 = getInfoForRefinement(myShape1, myShapeType1, myNbNodes1, myStep1);

  myShape2   = theShapeList2(0);
  myIsInitS2 = getInfoForRefinement(myShape2, myShapeType2, myNbNodes2, myStep2);

  MarkDirty();
}

//=======================================================================
// function : SetNbSamplePoints
// purpose  : Sets number of sample points used for proximity calculation for each shape
//=======================================================================
void BRepExtrema_ProximityValueTool::SetNbSamplePoints(const Standard_Integer theSamples1,
                                                       const Standard_Integer theSamples2)
{
  myNbSamples1 = theSamples1;
  myNbSamples2 = theSamples2;

  MarkDirty();
}

//=======================================================================
// function : computeProximityValue
// purpose  : Returns the computed proximity value from first BVH to another one
//=======================================================================
Standard_Real BRepExtrema_ProximityValueTool::computeProximityDist(
  const Handle(BRepExtrema_TriangleSet)&    theSet1,
  const Standard_Integer                    theNbSamples1,
  const BVH_Array3d&                        theAddVertices1,
  const NCollection_Vector<ProxPnt_Status>& theAddStatus1,
  const Handle(BRepExtrema_TriangleSet)&    theSet2,
  const BRepExtrema_ShapeList&              theShapeList1,
  const BRepExtrema_ShapeList&              theShapeList2,
  BVH_Vec3d&                                thePoint1,
  BVH_Vec3d&                                thePoint2,
  ProxPnt_Status&                           thePointStatus1,
  ProxPnt_Status&                           thePointStatus2) const
{
  BRepExtrema_ProximityDistTool aProxDistTool(theSet1,
                                              theNbSamples1,
                                              theAddVertices1,
                                              theAddStatus1,
                                              theSet2,
                                              theShapeList1,
                                              theShapeList2);
  aProxDistTool.Perform();

  if (!aProxDistTool.IsDone())
    return -1.;

  aProxDistTool.ProximityPoints(thePoint1, thePoint2);
  aProxDistTool.ProximityPointsStatus(thePointStatus1, thePointStatus2);

  return aProxDistTool.ProximityDistance();
}

//=======================================================================
// function : getEdgeAdditionalVertices
// purpose  : Gets additional vertices and their statuses on the edge with the input step
//=======================================================================
Standard_Boolean BRepExtrema_ProximityValueTool::getEdgeAdditionalVertices(
  const TopoDS_Edge&                  theEdge,
  const Standard_Real                 theStep,
  BVH_Array3d&                        theAddVertices,
  NCollection_Vector<ProxPnt_Status>& theAddStatuses)
{
  BRepAdaptor_Curve aBAC(theEdge);

  if (!aBAC.Is3DCurve() || theStep < Precision::Confusion())
  {
    return Standard_False;
  }

  Standard_Real    aLen            = GCPnts_AbscissaPoint::Length(aBAC);
  Standard_Integer aNbSamplePoints = (Standard_Integer)(aLen / theStep) + 1;

  GCPnts_QuasiUniformAbscissa aGCPnts(aBAC, Max(3, aNbSamplePoints));

  if (!aGCPnts.IsDone())
    return Standard_False;

  Standard_Integer aNbNodes = aGCPnts.NbPoints();
  for (Standard_Integer aVertIdx = 2; aVertIdx < aNbNodes; ++aVertIdx) // don't add extreme points
  {
    Standard_Real aPar = aGCPnts.Parameter(aVertIdx);
    gp_Pnt        aP   = aBAC.Value(aPar);

    theAddVertices.push_back(BVH_Vec3d(aP.X(), aP.Y(), aP.Z()));
    theAddStatuses.Append(ProxPnt_Status::ProxPnt_Status_MIDDLE);
  }

  return Standard_True;
}

//=======================================================================
// function : doRecurTrgSplit
// purpose  : Splits the triangle into two ones recursively, halving the longest side
//           until the area of ​​the current triangle > input step
//! @param theTrg points of the triangle to be split
//! @param theEdgesStatus status of triangle edges - on the border or middle of the face
//! @param theTol telerance used in search of coincidence points
//! @param theStep minimum area of ​​the resulting triangle
//! @param theAddVertices vertices obtained halving sides
//! @param theAddStatuses status of obtained vertices - on the border or middle of the face,
//! from triangulation of which the input triangle is
//=======================================================================
void BRepExtrema_ProximityValueTool::doRecurTrgSplit(
  const gp_Pnt (&theTrg)[3],
  const ProxPnt_Status (&theEdgesStatus)[3],
  const Standard_Real                 theTol,
  const Standard_Real                 theStep,
  BVH_Array3d&                        theAddVertices,
  NCollection_Vector<ProxPnt_Status>& theAddStatuses)
{
  gp_XYZ        aTrgSide1 = theTrg[1].Coord() - theTrg[0].Coord();
  gp_XYZ        aTrgSide2 = theTrg[2].Coord() - theTrg[0].Coord();
  Standard_Real aTrgArea  = 0.5 * aTrgSide1.CrossMagnitude(aTrgSide2);

  if (aTrgArea - theStep < Precision::SquareConfusion())
    return;

  Standard_Real    aD[3]{theTrg[0].Distance(theTrg[1]),
                         theTrg[1].Distance(theTrg[2]),
                         theTrg[2].Distance(theTrg[0])};
  Standard_Integer aBisectedEdgeIdx =
    aD[0] > aD[1] ? (aD[0] > aD[2] ? 0 : 2) : (aD[1] > aD[2] ? 1 : 2);
  gp_Pnt aCenterOfMaxSide(theTrg[aBisectedEdgeIdx].Coord());
  aCenterOfMaxSide.BaryCenter(0.5, theTrg[(aBisectedEdgeIdx + 1) % 3], 0.5);

  Bnd_Box aBox;
  aBox.Add(aCenterOfMaxSide);
  aBox.Enlarge(theTol);
  myInspector.SetCurrent(aCenterOfMaxSide.Coord());
  myCells.Inspect(aBox.CornerMin().XYZ(), aBox.CornerMax().XYZ(), myInspector);

  if (myInspector.IsNeedAdd()) // is point aCenterOfMaxSide unique
  {
    BVH_Vec3d aBisectingPnt(aCenterOfMaxSide.X(), aCenterOfMaxSide.Y(), aCenterOfMaxSide.Z());
    theAddVertices.push_back(aBisectingPnt);
    theAddStatuses.Append(theEdgesStatus[aBisectedEdgeIdx]);
    myInspector.Add(aCenterOfMaxSide.Coord());
    myCells.Add(static_cast<BRepExtrema_VertexInspector::Target>(theAddVertices.size()),
                aBox.CornerMin().XYZ(),
                aBox.CornerMax().XYZ());
  }

  gp_Pnt         aTrg1[3]         = {theTrg[0], theTrg[1], theTrg[2]};
  gp_Pnt         aTrg2[3]         = {theTrg[0], theTrg[1], theTrg[2]};
  ProxPnt_Status aEdgesStatus1[3] = {theEdgesStatus[0], theEdgesStatus[1], theEdgesStatus[2]};
  ProxPnt_Status aEdgesStatus2[3] = {theEdgesStatus[0], theEdgesStatus[1], theEdgesStatus[2]};
  switch (aBisectedEdgeIdx)
  {
    case 0:
      aTrg1[0] = aTrg2[1] = aCenterOfMaxSide;
      aEdgesStatus1[2] = aEdgesStatus2[1] = ProxPnt_Status::ProxPnt_Status_MIDDLE;
      break;
    case 1:
      aTrg1[1] = aTrg2[2] = aCenterOfMaxSide;
      aEdgesStatus1[0] = aEdgesStatus2[2] = ProxPnt_Status::ProxPnt_Status_MIDDLE;
      break;
    case 2:
      aTrg1[2] = aTrg2[0] = aCenterOfMaxSide;
      aEdgesStatus1[1] = aEdgesStatus2[0] = ProxPnt_Status::ProxPnt_Status_MIDDLE;
      break;
  }

  doRecurTrgSplit(aTrg1, aEdgesStatus1, theTol, theStep, theAddVertices, theAddStatuses);
  doRecurTrgSplit(aTrg2, aEdgesStatus2, theTol, theStep, theAddVertices, theAddStatuses);
}

static Standard_Real getModelRange(const TopLoc_Location&            theLocation,
                                   const Handle(Poly_Triangulation)& theTr)
{
  Bnd_Box aBox;
  theTr->MinMax(aBox, theLocation.Transformation());
  Standard_Real aXm = 0.0, aYm = 0.0, aZm = 0.0, aXM = 0.0, aYM = 0.0, aZM = 0.0;
  aBox.Get(aXm, aYm, aZm, aXM, aYM, aZM);
  Standard_Real aRange = aXM - aXm;
  aRange               = Max(aRange, aYM - aYm);
  aRange               = Max(aRange, aZM - aZm);

  return aRange;
}

static void getNodesOfTrg(const Standard_Integer            theTriIdx,
                          const TopLoc_Location&            theLocation,
                          const Handle(Poly_Triangulation)& theTr,
                          gp_Pnt (&theTrg)[3])
{
  Standard_Integer aVtxIdx1;
  Standard_Integer aVtxIdx2;
  Standard_Integer aVtxIdx3;

  theTr->Triangle(theTriIdx).Get(aVtxIdx1, aVtxIdx2, aVtxIdx3);

  gp_Pnt aVtx1 = theTr->Node(aVtxIdx1);
  aVtx1.Transform(theLocation);
  theTrg[0] = aVtx1;

  gp_Pnt aVtx2 = theTr->Node(aVtxIdx2);
  aVtx2.Transform(theLocation);
  theTrg[1] = aVtx2;

  gp_Pnt aVtx3 = theTr->Node(aVtxIdx3);
  aVtx3.Transform(theLocation);
  theTrg[2] = aVtx3;
}

// Gets status of triangle edges - on the border or middle of the face
static void getEdgesStatus(const Standard_Integer            theTriIdx,
                           const Handle(Poly_Triangulation)& theTr,
                           ProxPnt_Status (&theEdgesStatus1)[3])
{
  for (Standard_Integer j = 0; j < 3; j++)
  {
    Standard_Integer k = (j + 1) % 3;

    if (BRepExtrema_ProximityDistTool::IsEdgeOnBorder(theTriIdx, j, k, theTr))
    {
      theEdgesStatus1[j] = ProxPnt_Status::ProxPnt_Status_BORDER;
    }
    else
    {
      theEdgesStatus1[j] = ProxPnt_Status::ProxPnt_Status_MIDDLE;
    }
  }
}

//=======================================================================
// function : getFaceAdditionalVertices
// purpose  : Gets additional vertices and their statuses on the face with the input step (triangle
// square)
//=======================================================================
Standard_Boolean BRepExtrema_ProximityValueTool::getFaceAdditionalVertices(
  const TopoDS_Face&                  theFace,
  const Standard_Real                 theStep,
  BVH_Array3d&                        theAddVertices,
  NCollection_Vector<ProxPnt_Status>& theAddStatuses)
{
  constexpr Standard_Real aTol = Precision::Confusion();

  TopLoc_Location            aLocation;
  Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(theFace, aLocation);

  if (aTr.IsNull())
  {
    return Standard_False;
  }

  myCells.Reset(Max(aTol, getModelRange(aLocation, aTr) / IntegerLast()));

  for (Standard_Integer aTriIdx = 1; aTriIdx <= aTr->NbTriangles(); ++aTriIdx)
  {
    gp_Pnt         aTrg[3];
    ProxPnt_Status aEdgesStatus[3];
    getNodesOfTrg(aTriIdx, aLocation, aTr, aTrg);
    getEdgesStatus(aTriIdx, aTr, aEdgesStatus);
    doRecurTrgSplit(aTrg, aEdgesStatus, aTol, theStep, theAddVertices, theAddStatuses);
  }

  return Standard_True;
}

//=======================================================================
// function : getShapesVertices
// purpose  : Gets additional vertices on shapes with refining a coarser one if it's needed
//=======================================================================
Standard_Boolean BRepExtrema_ProximityValueTool::getShapesAdditionalVertices()
{
  // estimate the density of meshes of shapes to add points to a coarcer one
  // target steps for refinement
  Standard_Real aStep1 = myStep1;
  Standard_Real aStep2 = myStep2;

  if ((myShapeType1 == TopAbs_EDGE) && (myShapeType2 == TopAbs_EDGE))
  {
    if (myNbSamples1 > myNbNodes1) // 1st edge needs refinement
    {
      aStep1                  = calcEdgeRefinementStep(TopoDS::Edge(myShape1), myNbSamples1);
      myIsRefinementRequired1 = Standard_True;
    }

    if (myNbSamples2 > myNbNodes2) // 2nd edge needs refinement
    {
      aStep2                  = calcEdgeRefinementStep(TopoDS::Edge(myShape2), myNbSamples2);
      myIsRefinementRequired2 = Standard_True;
    }

    if (aStep1 / aStep2 > 2.) // 1st edge needs refinement
    {
      myIsRefinementRequired1 = Standard_True;
      aStep1                  = aStep2;
    }
    else if (aStep2 / aStep1 > 2.) // 2nd edge needs refinement
    {
      myIsRefinementRequired2 = Standard_True;
      aStep2                  = aStep1;
    }

    if (myIsRefinementRequired1)
    {
      if (!getEdgeAdditionalVertices(TopoDS::Edge(myShape1), aStep1, myAddVertices1, myAddStatus1))
      {
        return Standard_False;
      }
    }

    if (myIsRefinementRequired2)
    {
      if (!getEdgeAdditionalVertices(TopoDS::Edge(myShape2), aStep2, myAddVertices2, myAddStatus2))
      {
        return Standard_False;
      }
    }
  }
  else if ((myShapeType1 == TopAbs_FACE) && (myShapeType2 == TopAbs_FACE))
  {
    if (aStep1 / aStep2 > 2) // 1st face needs refinement
    {
      myIsRefinementRequired1 = Standard_True;
      aStep1                  = myStep2;
    }
    else if (aStep2 / aStep1 > 2.) // 2nd face needs refinement
    {
      myIsRefinementRequired2 = Standard_True;
      aStep2                  = myStep1;
    }

    if (myIsRefinementRequired1)
    {
      return getFaceAdditionalVertices(TopoDS::Face(myShape1),
                                       aStep1,
                                       myAddVertices1,
                                       myAddStatus1);
    }

    if (myIsRefinementRequired2)
    {
      return getFaceAdditionalVertices(TopoDS::Face(myShape2),
                                       aStep2,
                                       myAddVertices2,
                                       myAddStatus2);
    }
  }

  return Standard_True;
}

//=======================================================================
// function : Perform
// purpose  : Performs the computation of the proximity value
//=======================================================================
void BRepExtrema_ProximityValueTool::Perform(Standard_Real& theTolerance)
{
  if (!myIsInitS1 || !myIsInitS2 || (myShapeType1 != myShapeType2))
    return;

  // get vertices on shapes with refining a coarser mesh if it's needed
  if (!getShapesAdditionalVertices())
    return;

  // max(min) dist from the 1st shape to the 2nd one
  BVH_Vec3d      aP1_1, aP1_2;
  ProxPnt_Status aPointStatus1_1 = ProxPnt_Status::ProxPnt_Status_UNKNOWN;
  ProxPnt_Status aPointStatus1_2 = ProxPnt_Status::ProxPnt_Status_UNKNOWN;

  Standard_Real aProximityDist1 = computeProximityDist(mySet1,
                                                       myNbSamples1,
                                                       myAddVertices1,
                                                       myAddStatus1,
                                                       mySet2,
                                                       myShapeList1,
                                                       myShapeList2,
                                                       aP1_1,
                                                       aP1_2,
                                                       aPointStatus1_1,
                                                       aPointStatus1_2);

  if (aProximityDist1 < 0.)
    return;

  // max(min) dist from the 2nd shape to t he 1st one
  BVH_Vec3d      aP2_1, aP2_2;
  ProxPnt_Status aPointStatus2_1 = ProxPnt_Status::ProxPnt_Status_UNKNOWN;
  ProxPnt_Status aPointStatus2_2 = ProxPnt_Status::ProxPnt_Status_UNKNOWN;

  Standard_Real aProximityDist2 = computeProximityDist(mySet2,
                                                       myNbSamples2,
                                                       myAddVertices2,
                                                       myAddStatus2,
                                                       mySet1,
                                                       myShapeList2,
                                                       myShapeList1,
                                                       aP2_2,
                                                       aP2_1,
                                                       aPointStatus2_2,
                                                       aPointStatus2_1);

  if (aProximityDist2 < 0.)
    return;

  // min dist of the two max(min) dists
  if (aProximityDist1 < aProximityDist2)
  {
    myDistance = aProximityDist1;
    myPnt1.SetCoord(aP1_1.x(), aP1_1.y(), aP1_1.z());
    myPnt2.SetCoord(aP1_2.x(), aP1_2.y(), aP1_2.z());
    myPntStatus1 = aPointStatus1_1;
    myPntStatus2 = aPointStatus1_2;
  }
  else
  {
    myDistance = aProximityDist2;
    myPnt1.SetCoord(aP2_1.x(), aP2_1.y(), aP2_1.z());
    myPnt2.SetCoord(aP2_2.x(), aP2_2.y(), aP2_2.z());
    myPntStatus1 = aPointStatus2_1;
    myPntStatus2 = aPointStatus2_2;
  }

  myIsDone     = Standard_True;
  theTolerance = myDistance;
}

//=======================================================================
// function : Inspect
// purpose  : Used for selection and storage of coinciding nodes
//=======================================================================
NCollection_CellFilter_Action BRepExtrema_VertexInspector::Inspect(const Standard_Integer theTarget)
{
  myIsNeedAdd = Standard_True;

  const gp_XYZ& aPnt = myPoints.Value(theTarget - 1);
  Standard_Real aDx, aDy, aDz;
  aDx = myCurrent.X() - aPnt.X();
  aDy = myCurrent.Y() - aPnt.Y();
  aDz = myCurrent.Z() - aPnt.Z();

  if ((aDx * aDx <= myTol) && (aDy * aDy <= myTol) && (aDz * aDz <= myTol))
    myIsNeedAdd = Standard_False;

  return CellFilter_Keep;
}