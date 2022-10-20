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

#include <BRepExtrema_ProximityDistTool.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <Poly_Connect.hxx>
#include <Standard_NullValue.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : BRepExtrema_ProximityDistTool
//purpose  : Creates new unitialized tool
//=======================================================================
BRepExtrema_ProximityDistTool::BRepExtrema_ProximityDistTool()
: myMinDistance (std::numeric_limits<Standard_Real>::max()),
  myProxDist (-1.),
  myPntStatus1 (ProxPnt_Status_UNKNOWN),
  myPntStatus2 (ProxPnt_Status_UNKNOWN),
  myNbSamples1 (0),
  myProxVtxIdx1 (-1)
{
}

//=======================================================================
//function : BRepExtrema_ProximityDistTool
//purpose  : Creates new tool for the given element sets
//=======================================================================
BRepExtrema_ProximityDistTool::BRepExtrema_ProximityDistTool (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                                              const Standard_Integer theNbSamples1,
                                                              const BVH_Array3d& theAddVertices1,
                                                              const NCollection_Vector<ProxPnt_Status>& theAddStatus1,
                                                              const Handle(BRepExtrema_TriangleSet)& theSet2,
                                                              const BRepExtrema_ShapeList& theShapeList1,
                                                              const BRepExtrema_ShapeList& theShapeList2)
: myMinDistance (std::numeric_limits<Standard_Real>::max()),
  myProxDist (-1.),
  myPntStatus1 (ProxPnt_Status_UNKNOWN),
  myPntStatus2 (ProxPnt_Status_UNKNOWN),
  myNbSamples1 (theNbSamples1),
  myProxVtxIdx1 (-1)
{
  LoadTriangleSets (theSet1, theSet2);
  LoadShapeLists (theShapeList1, theShapeList2);
  LoadAdditionalPointsFirstSet (theAddVertices1, theAddStatus1);
}

//=======================================================================
//function : LoadTriangleSets
//purpose  : Loads the given element sets into the tool
//=======================================================================
void BRepExtrema_ProximityDistTool::LoadTriangleSets (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                                      const Handle(BRepExtrema_TriangleSet)& theSet2)
{
  mySet1 = theSet1;
  mySet2 = theSet2;
}

//=======================================================================
//function : LoadShapeLists
//purpose  : Loads the given list of subshapes into the proximity tool
//=======================================================================
void BRepExtrema_ProximityDistTool::LoadShapeLists (const BRepExtrema_ShapeList& theShapeList1,
                                                    const BRepExtrema_ShapeList& theShapeList2)
{
  myShapeList1 = theShapeList1;
  myShapeList2 = theShapeList2;
}

//=======================================================================
//function : LoadAdditionalPointsFirstSet
//purpose  : Loads given additional vertices and their statuses
//=======================================================================
void BRepExtrema_ProximityDistTool::LoadAdditionalPointsFirstSet (const BVH_Array3d& theAddVertices1,
                                                                  const NCollection_Vector<ProxPnt_Status>& theAddStatus1)
{
  myAddVertices1 = theAddVertices1;
  myAddStatus1 = theAddStatus1;
}

//=======================================================================
//function : goThroughtSet1
//purpose  : Goes throught vertices from the 1st set
//=======================================================================
void BRepExtrema_ProximityDistTool::goThroughtSet1 (const BVH_Array3d& theVertices1,
                                                    const Standard_Boolean theIsAdditionalSet)
{
  Standard_Integer aVtxSize = (Standard_Integer)theVertices1.size();
  Standard_Integer aVtxStep = Max (myNbSamples1 <= 0 ? 1 : aVtxSize / myNbSamples1, 1);
  for (Standard_Integer aVtxIdx = 0; aVtxIdx < aVtxSize; aVtxIdx += aVtxStep)
  {
    myDistance = std::numeric_limits<Standard_Real>::max();
    myMinDistance = std::numeric_limits<Standard_Real>::max();
    myIsDone = Standard_False;
    SetObject (theVertices1[aVtxIdx]);

    ComputeDistance();

    if (!IsDone() && myProxDist < 0.) return;

    if (IsDone() && myDistance > myProxDist)
    {
      myPnt1 = theVertices1[aVtxIdx];
      myPnt2 = myExtremaPoint;
      myProxDist = myDistance;
      myProxVtxIdx1 = aVtxIdx;
      myIsProxVtx1FromAddSet = theIsAdditionalSet;
      myProxPrjState = myExtPrjState;
    }
  }
}

//=======================================================================
//function : Perform
//purpose  : Performs searching of the proximity distance
//=======================================================================
void BRepExtrema_ProximityDistTool::Perform()
{
  SetBVHSet (mySet2.get());
  goThroughtSet1 (mySet1->GetVertices(), Standard_False);
  goThroughtSet1 (myAddVertices1, Standard_True);

  myIsDone = myProxDist > -1.;
  if (myIsDone)
  {
    DefineStatusProxPnt();
  }
}

static Standard_Real pointBoxSquareMaxDistance (const BVH_Vec3d& thePoint,
                                                const BVH_Vec3d& theCMin,
                                                const BVH_Vec3d& theCMax)
{
  Standard_Real aDist = 0;
  for (int i = 0; i < 3; ++i)
  {
    if (thePoint[i] <= 0.5 * (theCMax[i] + theCMin[i])) { Standard_Real d = theCMax[i] - thePoint[i]; d *= d; aDist += d; }
    else { Standard_Real d = thePoint[i] - theCMin[i]; d *= d; aDist += d; }
  }
  return aDist;
}

//=======================================================================
//function : Branch rejection
//purpose  : Defines the rules for node rejection by bounding box
//=======================================================================
Standard_Boolean BRepExtrema_ProximityDistTool::RejectNode (const BVH_Vec3d& theCornerMin,
                                                            const BVH_Vec3d& theCornerMax,
                                                            Standard_Real& theMetric) const
{
  theMetric = sqrt (BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance (myObject,
                                                                         theCornerMin,
                                                                         theCornerMax));

  Standard_Real aMaxMetric = sqrt (pointBoxSquareMaxDistance (myObject,
                                                              theCornerMin,
                                                              theCornerMax));

  return theMetric > myDistance || aMaxMetric < myProxDist;
}

//=======================================================================
//function : Leaf acceptance
//purpose  : Defines the rules for leaf acceptance
//=======================================================================
Standard_Boolean BRepExtrema_ProximityDistTool::Accept (const Standard_Integer theTrgIdx,
                                                        const Standard_Real&)
{
  BVH_Vec3d aTrgVert1;
  BVH_Vec3d aTrgVert2;
  BVH_Vec3d aTrgVert3;

  BVH_PrjState aBVH_PrjState;
  Standard_Integer aNumberOfFirstNode = -1;
  Standard_Integer aNumberOfLastNode = -1;

  mySet2->GetVertices (theTrgIdx, aTrgVert1, aTrgVert2, aTrgVert3);

  BVH_Vec3d aNearestPnt = BVH_Tools<Standard_Real, 3>::PointTriangleProjection (myObject,
                                                                                aTrgVert1, aTrgVert2, aTrgVert3,
                                                                                &aBVH_PrjState,
                                                                                &aNumberOfFirstNode, &aNumberOfLastNode);

  PrjState aPrjState (theTrgIdx, aBVH_PrjState, aNumberOfFirstNode, aNumberOfLastNode);
  BVH_Vec3d aDirect = myObject - aNearestPnt;
  Standard_Real aSqDistance = aDirect.Dot(aDirect);

  if (aSqDistance > Precision::SquareConfusion()) // point belongs to triangle
  {
    const BVH_Vec3d aAB = aTrgVert2 - aTrgVert1;

    BVH_Vec3d aNorm;
    if (aTrgVert2.IsEqual (aTrgVert3)) // is this degenerate triangle (= segment)
    {
      const BVH_Vec3d aAP = myObject - aTrgVert1;
      aNorm = BVH_Vec3d::Cross (BVH_Vec3d::Cross (aAP, aAB), aAB);
    }
    else
    {
      const BVH_Vec3d aAC = aTrgVert3 - aTrgVert1;
      aNorm = BVH_Vec3d::Cross (aAB, aAC);
    }

    Standard_Real aNormSqLen = aNorm.Dot (aNorm);

    // check if the distance is under perpendicular
    const BVH_Vec3d aCrossCross = BVH_Vec3d::Cross (aDirect, aNorm);
    Standard_Real aCrossCrossSqLen = aCrossCross.Dot (aCrossCross);
    if (aCrossCrossSqLen > Precision::SquareConfusion() * aSqDistance * aNormSqLen)
    {
      // the distance is not under perpendicular
      if (myMinDistance - sqrt (aSqDistance) > Precision::Confusion())
      {
        myMinDistance = sqrt (aSqDistance);
        myMinDistPoint = aNearestPnt;
        myMinPrjState = aPrjState;
      }

      return Standard_False;
    }
  }

  // the distance is under perpendicular
  if (myDistance - sqrt (aSqDistance) > Precision::Confusion())
  {
    myDistance = sqrt (aSqDistance);
    myExtremaPoint = aNearestPnt;
    myExtPrjState = aPrjState;

    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : ComputeDistance
//purpose  : Computes the distance between object and BVH tree
//=======================================================================
Standard_Real BRepExtrema_ProximityDistTool::ComputeDistance()
{
  myIsDone = this->Select() > 0;

  if (!myIsDone)
  {
    if (myMinDistance < std::numeric_limits<Standard_Real>::max())
    {
      myExtremaPoint = myMinDistPoint;
      myExtPrjState = myMinPrjState;
      myIsDone = true;
    }

    myDistance = myMinDistance;
  }

  return myDistance;
}

//=======================================================================
//function : IsNodeOnBorder
//purpose  : Returns true if the node is on the boarder
//=======================================================================
Standard_Boolean BRepExtrema_ProximityDistTool::IsNodeOnBorder (const Standard_Integer theNodeIdx,
                                                                const Handle(Poly_Triangulation)& theTr)
{
  Poly_Connect aPolyConnect (theTr);

  Standard_Integer aContTrg; //index of triangle containing exploring node
  for (aPolyConnect.Initialize (theNodeIdx); aPolyConnect.More(); aPolyConnect.Next())
  {
    aContTrg = aPolyConnect.Value();

    Standard_Integer aContTrgNodes[3];
    theTr->Triangle (aContTrg).Get (aContTrgNodes[0], aContTrgNodes[1], aContTrgNodes[2]); //indices of nodes of the triangle

    Standard_Integer aAdjTrg[3];
    aPolyConnect.Triangles (aContTrg, aAdjTrg[0], aAdjTrg[1], aAdjTrg[2]); //indices of adjacent triangles
  
    for (Standard_Integer j = 0; j < 3; j++)
    {
      Standard_Integer k = (j + 1) % 3;
      if (aAdjTrg[j] == 0) //free segment of triangle
      {
        //aContTrgNodes[j], aContTrgNodes[k] are ends of free segment and it is a part of border
        if (aContTrgNodes[j] == theNodeIdx || aContTrgNodes[k] == theNodeIdx)
        {
          return Standard_True;
        }
      }
    }
  }

  return Standard_False;
}

//=======================================================================
//function : IsEdgeOnBorder
//purpose  : Returns true if the edge is on the boarder
//=======================================================================
Standard_Boolean BRepExtrema_ProximityDistTool::IsEdgeOnBorder (const Standard_Integer theTrgIdx,
                                                                const Standard_Integer theFirstEdgeNodeIdx,
                                                                const Standard_Integer theSecondEdgeNodeIdx,
                                                                const Handle(Poly_Triangulation)& theTr)
{
  Poly_Connect aPolyConnect (theTr);

  Standard_Integer aAdjTrg[3];
  aPolyConnect.Triangles (theTrgIdx, aAdjTrg[0], aAdjTrg[1], aAdjTrg[2]); //indices of adjacent triangles

  for (Standard_Integer j = 0; j < 3; j++)
  {
    Standard_Integer k = (j + 1) % 3;
    if (aAdjTrg[j] == 0) //free segment of triangle
    {
      //are ends of free segment and it is a part of border
      if (j == theFirstEdgeNodeIdx &&
          k == theSecondEdgeNodeIdx)
      {
        return Standard_True;
      }
    }
  }

  return Standard_False;
}

//=======================================================================
//function : defineStatusProxPnt1
//purpose  : Defines the status of proximity point from 1st BVH
//=======================================================================
void BRepExtrema_ProximityDistTool::defineStatusProxPnt1()
{
  if (myIsProxVtx1FromAddSet)
  {
    myPntStatus1 = myAddStatus1[myProxVtxIdx1];
    return;
  }

  Standard_Integer aFaceID1 = mySet1->GetShapeIDOfVtx (myProxVtxIdx1);

  if (myShapeList1 (aFaceID1).ShapeType() == TopAbs_EDGE)
  {
    const BVH_Array3d& aVertices1 = mySet1->GetVertices();
    Standard_Integer aVtxSize = (Standard_Integer)aVertices1.size();
    Standard_Integer aLastIdx = aVtxSize - 1;

    if ((aVertices1[0] - aVertices1[aLastIdx]).Modulus() < Precision::Confusion()) // if closed
    {
      myPntStatus1 = ProxPnt_Status_MIDDLE;
      return;
    }

    if (myProxVtxIdx1 == 0 || myProxVtxIdx1 == aLastIdx)
    {
      myPntStatus1 = ProxPnt_Status_BORDER;
    }
    else
    {
      myPntStatus1 = ProxPnt_Status_MIDDLE;
    }
  }
  else if (myShapeList1 (aFaceID1).ShapeType() == TopAbs_FACE)
  {
    Standard_Integer aNodeIdx = mySet1->GetVtxIdxInShape (myProxVtxIdx1) + 1;

    TopLoc_Location aLocation;
    const TopoDS_Face& aF = TopoDS::Face (myShapeList1 (aFaceID1));
    Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation (aF, aLocation);

    if (IsNodeOnBorder (aNodeIdx, aTr))
    {
      myPntStatus1 = ProxPnt_Status_BORDER;
    }
    else
    {
      myPntStatus1 = ProxPnt_Status_MIDDLE;
    }
  }
}

//=======================================================================
//function : defineStatusProxPnt2
//purpose  : Defines the status of proximity point from 2nd BVH
//=======================================================================
void BRepExtrema_ProximityDistTool::defineStatusProxPnt2()
{
  Standard_Integer aTrgIdx = myProxPrjState.GetTrgIdx();
  Standard_Integer aFaceID2 = mySet2->GetFaceID (aTrgIdx);

  if (myShapeList2 (aFaceID2).ShapeType() == TopAbs_EDGE)
  {
    if (myProxPrjState.GetPrjState() == BVH_PrjState::BVH_PrjStateInTriangle_INNER)
    {
      return;
    }
    else
    {
      const BVH_Array3d& aVertices2 = mySet2->GetVertices();
      Standard_Integer aVtxSize = (Standard_Integer)aVertices2.size();
      Standard_Integer aLastIdx = aVtxSize - 1;

      if ((aVertices2[0] - aVertices2[aLastIdx]).Modulus() < Precision::Confusion()) // if closed
      {
        myPntStatus2 = ProxPnt_Status_MIDDLE;
        return;
      }

      NCollection_Array1<Standard_Integer> aVtxIndicesOfTrg;
      mySet2->GetVtxIndices (aTrgIdx, aVtxIndicesOfTrg);

      Standard_Integer aFirstNodeNum = myProxPrjState.GetNumberOfFirstNode();
      Standard_Integer aFirstVtxIdx = aVtxIndicesOfTrg[aFirstNodeNum];

      if (myProxPrjState.GetPrjState() == BVH_PrjState::BVH_PrjStateInTriangle_VERTEX)
      {
        if (aFirstVtxIdx == 0 || aFirstVtxIdx == aLastIdx)
        {
          myPntStatus2 = ProxPnt_Status_BORDER;
        }
        else
        {
          myPntStatus2 = ProxPnt_Status_MIDDLE;
        }
      }
      else if (myProxPrjState.GetPrjState() == BVH_PrjState::BVH_PrjStateInTriangle_EDGE)
      {
        Standard_Integer aLastNodeNum = myProxPrjState.GetNumberOfLastNode();
        Standard_Integer aLastVtxIdx = aVtxIndicesOfTrg[aLastNodeNum];

        // it could be border only in case projection is on a degenerated edge
        if (aFirstVtxIdx == aLastVtxIdx && (aFirstVtxIdx == 0 || aFirstVtxIdx == aLastIdx))
        {
          myPntStatus2 = ProxPnt_Status_BORDER;
        }
        else
        {
          myPntStatus2 = ProxPnt_Status_MIDDLE;
        }
      }
    }
  }
  else if (myShapeList2 (aFaceID2).ShapeType() == TopAbs_FACE)
  {
    if (myProxPrjState.GetPrjState() == BVH_PrjState::BVH_PrjStateInTriangle_INNER)
    {
      myPntStatus2 = ProxPnt_Status_MIDDLE;
    }
    else
    {
      TopLoc_Location aLocation;
      const TopoDS_Face& aF = TopoDS::Face (myShapeList2 (aFaceID2));
      Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation (aF, aLocation);

      NCollection_Array1<Standard_Integer> aVtxIndicesOfTrg;
      mySet2->GetVtxIndices (aTrgIdx, aVtxIndicesOfTrg);

      if (myProxPrjState.GetPrjState() == BVH_PrjState::BVH_PrjStateInTriangle_VERTEX)
      {
        Standard_Integer aNodeNum = myProxPrjState.GetNumberOfFirstNode();
        Standard_Integer aNodeIdx = mySet2->GetVtxIdxInShape (aVtxIndicesOfTrg[aNodeNum]) + 1;

        if (IsNodeOnBorder (aNodeIdx, aTr))
        {
          myPntStatus2 = ProxPnt_Status_BORDER;
        }
        else
        {
          myPntStatus2 = ProxPnt_Status_MIDDLE;
        }
      }
      else if (myProxPrjState.GetPrjState() == BVH_PrjState::BVH_PrjStateInTriangle_EDGE)
      {
        Standard_Integer aTrgIdxInShape = mySet2->GetTrgIdxInShape (aTrgIdx) + 1;

        if (IsEdgeOnBorder (aTrgIdxInShape,
                            myProxPrjState.GetNumberOfFirstNode(),
                            myProxPrjState.GetNumberOfLastNode(),
                            aTr))
        {
          myPntStatus2 = ProxPnt_Status_BORDER;
        }
        else
        {
          myPntStatus2 = ProxPnt_Status_MIDDLE;
        }
      } //else if (myProxPrjState.GetPrjState() == BVH_PrjState::BVH_PrjStateInTriangle_EDGE)
    }
  } //else if (myShapeList1 (aFaceID1).ShapeType() == TopAbs_FACE)
}

//=======================================================================
//function : DefineStatusProxPnt
//purpose  : Defines the status of proximity points
//=======================================================================
void BRepExtrema_ProximityDistTool::DefineStatusProxPnt()
{
  // define the status of proximity point from 1st BVH
  defineStatusProxPnt1();

  // define the status of proximity point from 2nd BVH
  defineStatusProxPnt2();
}
