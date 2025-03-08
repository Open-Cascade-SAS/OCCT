// Created on: 1997-05-15
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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

#include <Select3D_SensitiveTriangulation.hxx>

#include <Poly.hxx>
#include <Poly_Connect.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_TypeOfSensitivity.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveTriangulation, Select3D_SensitiveSet)

namespace
{
static Standard_Integer NbOfFreeEdges(const Handle(Poly_Triangulation)& theTriangulation)
{
  Standard_Integer aNbFree = 0;
  Poly_Connect     aPoly(theTriangulation);
  Standard_Integer aTriangleNodes[3];
  for (Standard_Integer aTrgIdx = 1; aTrgIdx <= theTriangulation->NbTriangles(); aTrgIdx++)
  {
    aPoly.Triangles(aTrgIdx, aTriangleNodes[0], aTriangleNodes[1], aTriangleNodes[2]);
    for (Standard_Integer aNodeIdx = 0; aNodeIdx < 3; ++aNodeIdx)
    {
      if (aTriangleNodes[aNodeIdx] == 0)
      {
        ++aNbFree;
      }
    }
  }
  return aNbFree;
}
} // namespace

//=================================================================================================

Select3D_SensitiveTriangulation::Select3D_SensitiveTriangulation(
  const Handle(SelectMgr_EntityOwner)& theOwnerId,
  const Handle(Poly_Triangulation)&    theTrg,
  const TopLoc_Location&               theInitLoc,
  const Standard_Boolean               theIsInterior)
    : Select3D_SensitiveSet(theOwnerId),
      myTriangul(theTrg),
      myInitLocation(theInitLoc),
      myPrimitivesNb(0)
{
  myInvInitLocation             = myInitLocation.Transformation().Inverted();
  mySensType                    = theIsInterior ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
  Standard_Integer aNbTriangles = 0;
  gp_XYZ           aCenter(0.0, 0.0, 0.0);
  if (!theTrg->HasGeometry())
  {
    if (myTriangul->HasCachedMinMax())
    {
      aCenter = 0.5
                * (myTriangul->CachedMinMax().CornerMin().XYZ()
                   + myTriangul->CachedMinMax().CornerMax().XYZ());
    }
  }
  else
  {
    aNbTriangles                          = myTriangul->NbTriangles();
    myPrimitivesNb                        = theIsInterior ? aNbTriangles : NbOfFreeEdges(theTrg);
    myBVHPrimIndexes                      = new TColStd_HArray1OfInteger(0, myPrimitivesNb - 1);
    TColStd_Array1OfInteger& aBVHPrimIdxs = myBVHPrimIndexes->ChangeArray1();

    if (!theIsInterior)
    {
      Standard_Integer anEdgeIdx          = 1;
      myFreeEdges                         = new TColStd_HArray1OfInteger(1, 2 * myPrimitivesNb);
      TColStd_Array1OfInteger& aFreeEdges = myFreeEdges->ChangeArray1();
      Poly_Connect             aPoly(myTriangul);
      Standard_Integer         aTriangle[3];
      Standard_Integer         aTrNodeIdx[3];
      for (Standard_Integer aTriangleIdx = 1; aTriangleIdx <= aNbTriangles; aTriangleIdx++)
      {
        aPoly.Triangles(aTriangleIdx, aTriangle[0], aTriangle[1], aTriangle[2]);
        myTriangul->Triangle(aTriangleIdx).Get(aTrNodeIdx[0], aTrNodeIdx[1], aTrNodeIdx[2]);
        const gp_Pnt aTriNodes[3] = {myTriangul->Node(aTrNodeIdx[0]),
                                     myTriangul->Node(aTrNodeIdx[1]),
                                     myTriangul->Node(aTrNodeIdx[2])};
        aCenter += (aTriNodes[0].XYZ() + aTriNodes[1].XYZ() + aTriNodes[2].XYZ()) / 3.0;
        for (Standard_Integer aVertIdx = 0; aVertIdx < 3; aVertIdx++)
        {
          Standard_Integer aNextVert = (aVertIdx + 1) % 3;
          if (aTriangle[aVertIdx] == 0)
          {
            aFreeEdges(anEdgeIdx)     = aTrNodeIdx[aVertIdx];
            aFreeEdges(anEdgeIdx + 1) = aTrNodeIdx[aNextVert];
            anEdgeIdx += 2;
          }
        }
      }
    }
    else
    {
      Standard_Integer aTrNodeIdx[3];
      for (Standard_Integer aTrIdx = 1; aTrIdx <= aNbTriangles; aTrIdx++)
      {
        myTriangul->Triangle(aTrIdx).Get(aTrNodeIdx[0], aTrNodeIdx[1], aTrNodeIdx[2]);
        const gp_Pnt aTriNodes[3] = {myTriangul->Node(aTrNodeIdx[0]),
                                     myTriangul->Node(aTrNodeIdx[1]),
                                     myTriangul->Node(aTrNodeIdx[2])};
        aCenter += (aTriNodes[0].XYZ() + aTriNodes[1].XYZ() + aTriNodes[2].XYZ()) / 3.0;
      }
    }

    if (theIsInterior)
    {
      for (Standard_Integer aTriangleIdx = 1; aTriangleIdx <= aNbTriangles; ++aTriangleIdx)
      {
        aBVHPrimIdxs(aTriangleIdx - 1) = aTriangleIdx - 1;
      }
    }
    else
    {
      Standard_Integer aStartIdx = myFreeEdges->Lower();
      Standard_Integer anEndIdx  = myFreeEdges->Upper();
      for (Standard_Integer aFreeEdgesIdx = aStartIdx; aFreeEdgesIdx <= anEndIdx;
           aFreeEdgesIdx += 2)
      {
        aBVHPrimIdxs((aFreeEdgesIdx - aStartIdx) / 2) = (aFreeEdgesIdx - aStartIdx) / 2;
      }
    }
  }
  if (aNbTriangles != 0)
  {
    aCenter /= aNbTriangles;
  }
  myCDG3D = gp_Pnt(aCenter);
  computeBoundingBox();
}

//=================================================================================================

Select3D_SensitiveTriangulation::Select3D_SensitiveTriangulation(
  const Handle(SelectMgr_EntityOwner)&    theOwnerId,
  const Handle(Poly_Triangulation)&       theTrg,
  const TopLoc_Location&                  theInitLoc,
  const Handle(TColStd_HArray1OfInteger)& theFreeEdges,
  const gp_Pnt&                           theCOG,
  const Standard_Boolean                  theIsInterior)
    : Select3D_SensitiveSet(theOwnerId),
      myTriangul(theTrg),
      myInitLocation(theInitLoc),
      myCDG3D(theCOG),
      myFreeEdges(theFreeEdges),
      myPrimitivesNb(0)
{
  myInvInitLocation = myInitLocation.Transformation().Inverted();
  mySensType        = theIsInterior ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
  if (theTrg->HasGeometry())
  {
    myPrimitivesNb   = theIsInterior ? theTrg->NbTriangles() : theFreeEdges->Length() / 2;
    myBVHPrimIndexes = new TColStd_HArray1OfInteger(0, myPrimitivesNb - 1);
    if (theIsInterior)
    {
      for (Standard_Integer aTriangleIdx = 1; aTriangleIdx <= myPrimitivesNb; ++aTriangleIdx)
      {
        myBVHPrimIndexes->SetValue(aTriangleIdx - 1, aTriangleIdx - 1);
      }
    }
    else
    {
      Standard_Integer aStartIdx = myFreeEdges->Lower();
      Standard_Integer anEndIdx  = myFreeEdges->Upper();
      for (Standard_Integer aFreeEdgesIdx = aStartIdx; aFreeEdgesIdx <= anEndIdx;
           aFreeEdgesIdx += 2)
      {
        myBVHPrimIndexes->SetValue((aFreeEdgesIdx - aStartIdx) / 2,
                                   (aFreeEdgesIdx - aStartIdx) / 2);
      }
    }
  }
}

//=======================================================================
// function : Size
// purpose  : Returns the length of array of triangles or edges
//=======================================================================
Standard_Integer Select3D_SensitiveTriangulation::Size() const
{
  return myPrimitivesNb;
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of triangle/edge with index theIdx
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveTriangulation::Box(const Standard_Integer theIdx) const
{
  Standard_Integer aPrimIdx = myBVHPrimIndexes->Value(theIdx);
  SelectMgr_Vec3   aMinPnt(RealLast());
  SelectMgr_Vec3   aMaxPnt(RealFirst());

  if (mySensType == Select3D_TOS_INTERIOR)
  {
    Standard_Integer aNode1, aNode2, aNode3;
    myTriangul->Triangle(aPrimIdx + 1).Get(aNode1, aNode2, aNode3);

    const gp_Pnt aPnt1 = myTriangul->Node(aNode1);
    const gp_Pnt aPnt2 = myTriangul->Node(aNode2);
    const gp_Pnt aPnt3 = myTriangul->Node(aNode3);

    aMinPnt = SelectMgr_Vec3(Min(aPnt1.X(), Min(aPnt2.X(), aPnt3.X())),
                             Min(aPnt1.Y(), Min(aPnt2.Y(), aPnt3.Y())),
                             Min(aPnt1.Z(), Min(aPnt2.Z(), aPnt3.Z())));
    aMaxPnt = SelectMgr_Vec3(Max(aPnt1.X(), Max(aPnt2.X(), aPnt3.X())),
                             Max(aPnt1.Y(), Max(aPnt2.Y(), aPnt3.Y())),
                             Max(aPnt1.Z(), Max(aPnt2.Z(), aPnt3.Z())));
  }
  else
  {
    Standard_Integer aNodeIdx1 = myFreeEdges->Value(myFreeEdges->Lower() + aPrimIdx);
    Standard_Integer aNodeIdx2 = myFreeEdges->Value(myFreeEdges->Lower() + aPrimIdx + 1);
    const gp_Pnt     aNode1    = myTriangul->Node(aNodeIdx1);
    const gp_Pnt     aNode2    = myTriangul->Node(aNodeIdx2);

    aMinPnt = SelectMgr_Vec3(Min(aNode1.X(), aNode2.X()),
                             Min(aNode1.Y(), aNode2.Y()),
                             Min(aNode1.Z(), aNode2.Z()));
    aMaxPnt = SelectMgr_Vec3(Max(aNode1.X(), aNode2.X()),
                             Max(aNode1.Y(), aNode2.Y()),
                             Max(aNode1.Z(), aNode2.Z()));
  }

  return Select3D_BndBox3d(aMinPnt, aMaxPnt);
}

//=================================================================================================

Standard_Boolean Select3D_SensitiveTriangulation::Matches(
  SelectBasics_SelectingVolumeManager& theMgr,
  SelectBasics_PickResult&             thePickResult)
{
  if (myTriangul->HasGeometry())
  {
    return Select3D_SensitiveSet::Matches(theMgr, thePickResult);
  }

  Select3D_BndBox3d aBndBox = BoundingBox();
  if (!aBndBox.IsValid())
  {
    return false;
  }

  if (!theMgr.IsOverlapAllowed()) // check for inclusion
  {
    bool isInside = true;
    return theMgr.OverlapsBox(aBndBox.CornerMin(), aBndBox.CornerMax(), &isInside) && isInside;
  }
  if (!theMgr.OverlapsBox(aBndBox.CornerMin(),
                          aBndBox.CornerMax(),
                          thePickResult)) // check for overlap
  {
    return false;
  }
  thePickResult.SetDistToGeomCenter(theMgr.DistToGeometryCenter(myCDG3D));
  return true;
}

//=======================================================================
// function : Center
// purpose  : Returns geometry center of triangle/edge with index theIdx
//            in array along the given axis theAxis
//=======================================================================
Standard_Real Select3D_SensitiveTriangulation::Center(const Standard_Integer theIdx,
                                                      const Standard_Integer theAxis) const
{
  const Select3D_BndBox3d& aBox    = Box(theIdx);
  const SelectMgr_Vec3     aCenter = (aBox.CornerMin() + aBox.CornerMax()) * 0.5;
  return aCenter[theAxis];
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2 in array
//=======================================================================
void Select3D_SensitiveTriangulation::Swap(const Standard_Integer theIdx1,
                                           const Standard_Integer theIdx2)
{
  Standard_Integer anElemIdx1 = myBVHPrimIndexes->Value(theIdx1);
  Standard_Integer anElemIdx2 = myBVHPrimIndexes->Value(theIdx2);

  myBVHPrimIndexes->ChangeValue(theIdx1) = anElemIdx2;
  myBVHPrimIndexes->ChangeValue(theIdx2) = anElemIdx1;
}

//=================================================================================================

bool Select3D_SensitiveTriangulation::LastDetectedTriangle(Poly_Triangle& theTriangle) const
{
  const Standard_Integer anIndex = LastDetectedTriangleIndex();
  if (anIndex != -1)
  {
    theTriangle = myTriangul->Triangle(anIndex);
    return true;
  }
  return false;
}

//=================================================================================================

bool Select3D_SensitiveTriangulation::LastDetectedTriangle(Poly_Triangle& theTriangle,
                                                           gp_Pnt         theTriNodes[3]) const
{
  if (!LastDetectedTriangle(theTriangle))
  {
    return false;
  }

  theTriNodes[0] =
    myTriangul->Node(theTriangle.Value(1)).Transformed(myInitLocation.Transformation());
  ;
  theTriNodes[1] =
    myTriangul->Node(theTriangle.Value(2)).Transformed(myInitLocation.Transformation());
  ;
  theTriNodes[2] =
    myTriangul->Node(theTriangle.Value(3)).Transformed(myInitLocation.Transformation());
  ;
  return true;
}

//=======================================================================
// function : overlapsElement
// purpose  : Checks whether the element with index theIdx overlaps the
//            current selecting volume
//=======================================================================
Standard_Boolean Select3D_SensitiveTriangulation::overlapsElement(
  SelectBasics_PickResult&             thePickResult,
  SelectBasics_SelectingVolumeManager& theMgr,
  Standard_Integer                     theElemIdx,
  Standard_Boolean                     theIsFullInside)
{
  if (theIsFullInside)
  {
    return Standard_True;
  }

  const Standard_Integer aPrimitiveIdx = myBVHPrimIndexes->Value(theElemIdx);
  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    Standard_Integer aSegmStartIdx = myFreeEdges->Value(aPrimitiveIdx * 2 + 1);
    Standard_Integer aSegmEndIdx   = myFreeEdges->Value(aPrimitiveIdx * 2 + 2);

    const gp_Pnt anEdgePnts[2] = {myTriangul->Node(aSegmStartIdx), myTriangul->Node(aSegmEndIdx)};
    TColgp_Array1OfPnt anEdgePntsArr(anEdgePnts[0], 1, 2);
    Standard_Boolean   isMatched =
      theMgr.OverlapsPolygon(anEdgePntsArr, Select3D_TOS_BOUNDARY, thePickResult);
    return isMatched;
  }
  else
  {
    Standard_Integer aNode1, aNode2, aNode3;
    myTriangul->Triangle(aPrimitiveIdx + 1).Get(aNode1, aNode2, aNode3);
    const gp_Pnt aPnt1 = myTriangul->Node(aNode1);
    const gp_Pnt aPnt2 = myTriangul->Node(aNode2);
    const gp_Pnt aPnt3 = myTriangul->Node(aNode3);
    return theMgr.OverlapsTriangle(aPnt1, aPnt2, aPnt3, Select3D_TOS_INTERIOR, thePickResult);
  }
}

//=================================================================================================

Standard_Boolean Select3D_SensitiveTriangulation::elementIsInside(
  SelectBasics_SelectingVolumeManager& theMgr,
  Standard_Integer                     theElemIdx,
  Standard_Boolean                     theIsFullInside)
{
  if (theIsFullInside)
  {
    return Standard_True;
  }

  const Standard_Integer aPrimitiveIdx = myBVHPrimIndexes->Value(theElemIdx);
  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    const gp_Pnt aSegmPnt1 = myTriangul->Node(myFreeEdges->Value(aPrimitiveIdx * 2 + 1));
    const gp_Pnt aSegmPnt2 = myTriangul->Node(myFreeEdges->Value(aPrimitiveIdx * 2 + 2));
    if (theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Polyline)
    {
      SelectBasics_PickResult aDummy;
      return theMgr.OverlapsSegment(aSegmPnt1, aSegmPnt2, aDummy);
    }
    return theMgr.OverlapsPoint(aSegmPnt1) && theMgr.OverlapsPoint(aSegmPnt2);
  }
  else
  {
    Standard_Integer aNode1, aNode2, aNode3;
    myTriangul->Triangle(aPrimitiveIdx + 1).Get(aNode1, aNode2, aNode3);

    const gp_Pnt aPnt1 = myTriangul->Node(aNode1);
    const gp_Pnt aPnt2 = myTriangul->Node(aNode2);
    const gp_Pnt aPnt3 = myTriangul->Node(aNode3);
    if (theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Polyline)
    {
      SelectBasics_PickResult aDummy;
      return theMgr.OverlapsTriangle(aPnt1, aPnt2, aPnt3, mySensType, aDummy);
    }
    return theMgr.OverlapsPoint(aPnt1) && theMgr.OverlapsPoint(aPnt2)
           && theMgr.OverlapsPoint(aPnt3);
  }
}

//=======================================================================
// function : distanceToCOG
// purpose  : Calculates distance from the 3d projection of used-picked
//            screen point to center of the geometry
//=======================================================================
Standard_Real Select3D_SensitiveTriangulation::distanceToCOG(
  SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter(myCDG3D);
}

//=================================================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveTriangulation::GetConnected()
{
  Standard_Boolean                        isInterior = mySensType == Select3D_TOS_INTERIOR;
  Handle(Select3D_SensitiveTriangulation) aNewEntity =
    new Select3D_SensitiveTriangulation(myOwnerId,
                                        myTriangul,
                                        myInitLocation,
                                        myFreeEdges,
                                        myCDG3D,
                                        isInterior);

  return aNewEntity;
}

//=======================================================================
// function : applyTransformation
// purpose  : Inner function for transformation application to bounding
//            box of the triangulation
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveTriangulation::applyTransformation()
{
  if (!HasInitLocation())
    return myBndBox;

  Select3D_BndBox3d aBndBox;
  for (Standard_Integer aX = 0; aX <= 1; ++aX)
  {
    for (Standard_Integer aY = 0; aY <= 1; ++aY)
    {
      for (Standard_Integer aZ = 0; aZ <= 1; ++aZ)
      {
        gp_Pnt aVertex = gp_Pnt(aX == 0 ? myBndBox.CornerMin().x() : myBndBox.CornerMax().x(),
                                aY == 0 ? myBndBox.CornerMin().y() : myBndBox.CornerMax().y(),
                                aZ == 0 ? myBndBox.CornerMin().z() : myBndBox.CornerMax().z());
        aVertex.Transform(myInitLocation.Transformation());
        aBndBox.Add(Select3D_Vec3(aVertex.X(), aVertex.Y(), aVertex.Z()));
      }
    }
  }

  return aBndBox;
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns bounding box of the triangulation. If location
//            transformation is set, it will be applied
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveTriangulation::BoundingBox()
{
  if (!myBndBox.IsValid())
  {
    computeBoundingBox();
  }
  return applyTransformation();
}

//=================================================================================================

void Select3D_SensitiveTriangulation::computeBoundingBox()
{
  myBndBox.Clear();

  if (myTriangul->HasCachedMinMax())
  {
    // Use cached MeshData_Data bounding box if it exists
    Bnd_Box aCachedBox = myTriangul->CachedMinMax();
    myBndBox.Add(SelectMgr_Vec3(aCachedBox.CornerMin().X(),
                                aCachedBox.CornerMin().Y(),
                                aCachedBox.CornerMin().Z()));
    myBndBox.Add(SelectMgr_Vec3(aCachedBox.CornerMax().X(),
                                aCachedBox.CornerMax().Y(),
                                aCachedBox.CornerMax().Z()));
    return;
  }
  else if (myTriangul->HasGeometry())
  {
    for (Standard_Integer aNodeIdx = 1; aNodeIdx <= myTriangul->NbNodes(); ++aNodeIdx)
    {
      const gp_Pnt aNode = myTriangul->Node(aNodeIdx);
      myBndBox.Add(SelectMgr_Vec3(aNode.X(), aNode.Y(), aNode.Z()));
    }
  }
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of triangulation. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_SensitiveTriangulation::CenterOfGeometry() const
{
  return myCDG3D;
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of nodes in triangulation
//=======================================================================
Standard_Integer Select3D_SensitiveTriangulation::NbSubElements() const
{
  return myTriangul->NbNodes();
}

//=================================================================================================

Standard_Boolean Select3D_SensitiveTriangulation::HasInitLocation() const
{
  return !myInitLocation.IsIdentity();
}

//=================================================================================================

gp_GTrsf Select3D_SensitiveTriangulation::InvInitLocation() const
{
  return myInvInitLocation;
}

//=================================================================================================

void Select3D_SensitiveTriangulation::DumpJson(Standard_OStream& theOStream,
                                               Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Select3D_SensitiveSet)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTriangul.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myInitLocation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mySensType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPrimitivesNb)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBndBox)
}
