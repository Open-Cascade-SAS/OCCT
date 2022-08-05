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

//=======================================================================
//function : BRepExtrema_ProximityValueTool
//purpose  : Creates new unitialized proximity tool
//=======================================================================
BRepExtrema_ProximityValueTool::BRepExtrema_ProximityValueTool()
: myDistance (std::numeric_limits<Standard_Real>::max()),
  myIsDone (Standard_False),
  myNbSamples1(0),
  myNbSamples2(0)
{}

//=======================================================================
//function : BRepExtrema_ProximityValueTool
//purpose  : Creates new proximity tool for the given element sets
//=======================================================================
BRepExtrema_ProximityValueTool::BRepExtrema_ProximityValueTool (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                                                const Handle(BRepExtrema_TriangleSet)& theSet2,
                                                                const BRepExtrema_ShapeList& theShapeList1,
                                                                const BRepExtrema_ShapeList& theShapeList2)
: myDistance (std::numeric_limits<Standard_Real>::max()),
  myIsDone (Standard_False),
  myNbSamples1(0),
  myNbSamples2(0)
{
  LoadTriangleSets (theSet1, theSet2);
  LoadShapeLists (theShapeList1, theShapeList2);
}

//=======================================================================
//function : LoadTriangleSets
//purpose  : Loads the given element sets into the proximity tool
//=======================================================================
void BRepExtrema_ProximityValueTool::LoadTriangleSets (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                                       const Handle(BRepExtrema_TriangleSet)& theSet2)
{
  mySet1 = theSet1;
  mySet2 = theSet2;

  myIsDone = Standard_False;
}

//=======================================================================
//function : LoadTriangleSets
//purpose  : Loads the given list of subshapes into the proximity tool
//=======================================================================
void BRepExtrema_ProximityValueTool::LoadShapeLists (const BRepExtrema_ShapeList& theShapeList1,
                                                     const BRepExtrema_ShapeList& theShapeList2)
{
  myShapeList1 = theShapeList1;
  myShapeList2 = theShapeList2;

  myIsDone = Standard_False;
}

//=======================================================================
//function : SetNbSamplePoints
//purpose  : Sets number of sample points used for proximity calculation for each shape
//=======================================================================
void BRepExtrema_ProximityValueTool::SetNbSamplePoints(const Standard_Integer theSamples1,
                                                       const Standard_Integer theSamples2)
{
  myNbSamples1 = theSamples1;
  myNbSamples2 = theSamples2;

  myIsDone = Standard_False;
}

//=======================================================================
//function : computeProximityValue
//purpose  : Returns the computed proximity value from first BVH to another one
//=======================================================================
Standard_Real BRepExtrema_ProximityValueTool::computeProximityDist (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                                                    const Standard_Integer theNbSamples1,
                                                                    const Handle(BRepExtrema_TriangleSet)& theSet2,
                                                                    const BRepExtrema_ShapeList& theShapeList1,
                                                                    const BRepExtrema_ShapeList& theShapeList2,
                                                                    BVH_Vec3d& thePoint1,
                                                                    BVH_Vec3d& thePoint2,
                                                                    ProxPnt_Status& thePointStatus1,
                                                                    ProxPnt_Status& thePointStatus2) const
{
  BRepExtrema_ProximityDistTool aProxDistTool (theSet1, theNbSamples1, theSet2, theShapeList1, theShapeList2);
  aProxDistTool.Perform();

  if (!aProxDistTool.IsDone())
    return -1.;

  aProxDistTool.ProximityPoints (thePoint1, thePoint2);
  aProxDistTool.ProximityPointsStatus (thePointStatus1, thePointStatus2);

  return aProxDistTool.ProximityDistance();
}

//=======================================================================
//function : Perform
//purpose  : Performs the computation of the proximity value
//=======================================================================
void BRepExtrema_ProximityValueTool::Perform (Standard_Real& theTolerance)
{
  myIsDone = Standard_False;

  // max(min) dist from the 1st shape to the 2nd one
  BVH_Vec3d aP1_1, aP1_2;
  ProxPnt_Status aPointStatus1_1 = ProxPnt_Status::ProxPnt_Status_UNKNOWN;
  ProxPnt_Status aPointStatus1_2 = ProxPnt_Status::ProxPnt_Status_UNKNOWN;

  Standard_Real aProximityDist1 = computeProximityDist (mySet1, myNbSamples1, mySet2, myShapeList1, myShapeList2,
                                                        aP1_1, aP1_2, aPointStatus1_1, aPointStatus1_2);

  if (aProximityDist1 < 0.)
    return;

  // max(min) dist from the 2nd shape to t he 1st one
  BVH_Vec3d aP2_1, aP2_2;
  ProxPnt_Status aPointStatus2_1 = ProxPnt_Status::ProxPnt_Status_UNKNOWN;
  ProxPnt_Status aPointStatus2_2 = ProxPnt_Status::ProxPnt_Status_UNKNOWN;

  Standard_Real aProximityDist2 = computeProximityDist (mySet2, myNbSamples2, mySet1, myShapeList2, myShapeList1,
                                                        aP2_2, aP2_1, aPointStatus2_2, aPointStatus2_1);

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

  myIsDone = Standard_True;
  theTolerance = myDistance;
}
