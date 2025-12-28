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
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_TypeOfSensitivity.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveTriangulation, Select3D_SensitiveSet)

namespace
{
static int NbOfFreeEdges(const occ::handle<Poly_Triangulation>& theTriangulation)
{
  int          aNbFree = 0;
  Poly_Connect aPoly(theTriangulation);
  int          aTriangleNodes[3];
  for (int aTrgIdx = 1; aTrgIdx <= theTriangulation->NbTriangles(); aTrgIdx++)
  {
    aPoly.Triangles(aTrgIdx, aTriangleNodes[0], aTriangleNodes[1], aTriangleNodes[2]);
    for (int aNodeIdx = 0; aNodeIdx < 3; ++aNodeIdx)
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
  const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
  const occ::handle<Poly_Triangulation>&    theTrg,
  const TopLoc_Location&                    theInitLoc,
  const bool                                theIsInterior)
    : Select3D_SensitiveSet(theOwnerId),
      myTriangul(theTrg),
      myInitLocation(theInitLoc),
      myPrimitivesNb(0)
{
  myInvInitLocation   = myInitLocation.Transformation().Inverted();
  mySensType          = theIsInterior ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
  int    aNbTriangles = 0;
  gp_XYZ aCenter(0.0, 0.0, 0.0);
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
    myBVHPrimIndexes                      = new NCollection_HArray1<int>(0, myPrimitivesNb - 1);
    NCollection_Array1<int>& aBVHPrimIdxs = myBVHPrimIndexes->ChangeArray1();

    if (!theIsInterior)
    {
      int anEdgeIdx                       = 1;
      myFreeEdges                         = new NCollection_HArray1<int>(1, 2 * myPrimitivesNb);
      NCollection_Array1<int>& aFreeEdges = myFreeEdges->ChangeArray1();
      Poly_Connect             aPoly(myTriangul);
      int                      aTriangle[3];
      int                      aTrNodeIdx[3];
      for (int aTriangleIdx = 1; aTriangleIdx <= aNbTriangles; aTriangleIdx++)
      {
        aPoly.Triangles(aTriangleIdx, aTriangle[0], aTriangle[1], aTriangle[2]);
        myTriangul->Triangle(aTriangleIdx).Get(aTrNodeIdx[0], aTrNodeIdx[1], aTrNodeIdx[2]);
        const gp_Pnt aTriNodes[3] = {myTriangul->Node(aTrNodeIdx[0]),
                                     myTriangul->Node(aTrNodeIdx[1]),
                                     myTriangul->Node(aTrNodeIdx[2])};
        aCenter += (aTriNodes[0].XYZ() + aTriNodes[1].XYZ() + aTriNodes[2].XYZ()) / 3.0;
        for (int aVertIdx = 0; aVertIdx < 3; aVertIdx++)
        {
          int aNextVert = (aVertIdx + 1) % 3;
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
      int aTrNodeIdx[3];
      for (int aTrIdx = 1; aTrIdx <= aNbTriangles; aTrIdx++)
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
      for (int aTriangleIdx = 1; aTriangleIdx <= aNbTriangles; ++aTriangleIdx)
      {
        aBVHPrimIdxs(aTriangleIdx - 1) = aTriangleIdx - 1;
      }
    }
    else
    {
      int aStartIdx = myFreeEdges->Lower();
      int anEndIdx  = myFreeEdges->Upper();
      for (int aFreeEdgesIdx = aStartIdx; aFreeEdgesIdx <= anEndIdx; aFreeEdgesIdx += 2)
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
  const occ::handle<SelectMgr_EntityOwner>&    theOwnerId,
  const occ::handle<Poly_Triangulation>&       theTrg,
  const TopLoc_Location&                       theInitLoc,
  const occ::handle<NCollection_HArray1<int>>& theFreeEdges,
  const gp_Pnt&                                theCOG,
  const bool                                   theIsInterior)
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
    myBVHPrimIndexes = new NCollection_HArray1<int>(0, myPrimitivesNb - 1);
    if (theIsInterior)
    {
      for (int aTriangleIdx = 1; aTriangleIdx <= myPrimitivesNb; ++aTriangleIdx)
      {
        myBVHPrimIndexes->SetValue(aTriangleIdx - 1, aTriangleIdx - 1);
      }
    }
    else
    {
      int aStartIdx = myFreeEdges->Lower();
      int anEndIdx  = myFreeEdges->Upper();
      for (int aFreeEdgesIdx = aStartIdx; aFreeEdgesIdx <= anEndIdx; aFreeEdgesIdx += 2)
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
int Select3D_SensitiveTriangulation::Size() const
{
  return myPrimitivesNb;
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of triangle/edge with index theIdx
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveTriangulation::Box(const int theIdx) const
{
  int                      aPrimIdx = myBVHPrimIndexes->Value(theIdx);
  NCollection_Vec3<double> aMinPnt(RealLast());
  NCollection_Vec3<double> aMaxPnt(RealFirst());

  if (mySensType == Select3D_TOS_INTERIOR)
  {
    int aNode1, aNode2, aNode3;
    myTriangul->Triangle(aPrimIdx + 1).Get(aNode1, aNode2, aNode3);

    const gp_Pnt aPnt1 = myTriangul->Node(aNode1);
    const gp_Pnt aPnt2 = myTriangul->Node(aNode2);
    const gp_Pnt aPnt3 = myTriangul->Node(aNode3);

    aMinPnt = NCollection_Vec3<double>(std::min(aPnt1.X(), std::min(aPnt2.X(), aPnt3.X())),
                                       std::min(aPnt1.Y(), std::min(aPnt2.Y(), aPnt3.Y())),
                                       std::min(aPnt1.Z(), std::min(aPnt2.Z(), aPnt3.Z())));
    aMaxPnt = NCollection_Vec3<double>(std::max(aPnt1.X(), std::max(aPnt2.X(), aPnt3.X())),
                                       std::max(aPnt1.Y(), std::max(aPnt2.Y(), aPnt3.Y())),
                                       std::max(aPnt1.Z(), std::max(aPnt2.Z(), aPnt3.Z())));
  }
  else
  {
    int          aNodeIdx1 = myFreeEdges->Value(myFreeEdges->Lower() + aPrimIdx);
    int          aNodeIdx2 = myFreeEdges->Value(myFreeEdges->Lower() + aPrimIdx + 1);
    const gp_Pnt aNode1    = myTriangul->Node(aNodeIdx1);
    const gp_Pnt aNode2    = myTriangul->Node(aNodeIdx2);

    aMinPnt = NCollection_Vec3<double>(std::min(aNode1.X(), aNode2.X()),
                                       std::min(aNode1.Y(), aNode2.Y()),
                                       std::min(aNode1.Z(), aNode2.Z()));
    aMaxPnt = NCollection_Vec3<double>(std::max(aNode1.X(), aNode2.X()),
                                       std::max(aNode1.Y(), aNode2.Y()),
                                       std::max(aNode1.Z(), aNode2.Z()));
  }

  return Select3D_BndBox3d(aMinPnt, aMaxPnt);
}

//=================================================================================================

bool Select3D_SensitiveTriangulation::Matches(SelectBasics_SelectingVolumeManager& theMgr,
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
double Select3D_SensitiveTriangulation::Center(const int theIdx, const int theAxis) const
{
  const Select3D_BndBox3d&       aBox    = Box(theIdx);
  const NCollection_Vec3<double> aCenter = (aBox.CornerMin() + aBox.CornerMax()) * 0.5;
  return aCenter[theAxis];
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2 in array
//=======================================================================
void Select3D_SensitiveTriangulation::Swap(const int theIdx1, const int theIdx2)
{
  int anElemIdx1 = myBVHPrimIndexes->Value(theIdx1);
  int anElemIdx2 = myBVHPrimIndexes->Value(theIdx2);

  myBVHPrimIndexes->ChangeValue(theIdx1) = anElemIdx2;
  myBVHPrimIndexes->ChangeValue(theIdx2) = anElemIdx1;
}

//=================================================================================================

bool Select3D_SensitiveTriangulation::LastDetectedTriangle(Poly_Triangle& theTriangle) const
{
  const int anIndex = LastDetectedTriangleIndex();
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
bool Select3D_SensitiveTriangulation::overlapsElement(SelectBasics_PickResult& thePickResult,
                                                      SelectBasics_SelectingVolumeManager& theMgr,
                                                      int  theElemIdx,
                                                      bool theIsFullInside)
{
  if (theIsFullInside)
  {
    return true;
  }

  const int aPrimitiveIdx = myBVHPrimIndexes->Value(theElemIdx);
  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    int aSegmStartIdx = myFreeEdges->Value(aPrimitiveIdx * 2 + 1);
    int aSegmEndIdx   = myFreeEdges->Value(aPrimitiveIdx * 2 + 2);

    const gp_Pnt anEdgePnts[2] = {myTriangul->Node(aSegmStartIdx), myTriangul->Node(aSegmEndIdx)};
    NCollection_Array1<gp_Pnt> anEdgePntsArr(anEdgePnts[0], 1, 2);
    bool isMatched = theMgr.OverlapsPolygon(anEdgePntsArr, Select3D_TOS_BOUNDARY, thePickResult);
    return isMatched;
  }
  else
  {
    int aNode1, aNode2, aNode3;
    myTriangul->Triangle(aPrimitiveIdx + 1).Get(aNode1, aNode2, aNode3);
    const gp_Pnt aPnt1 = myTriangul->Node(aNode1);
    const gp_Pnt aPnt2 = myTriangul->Node(aNode2);
    const gp_Pnt aPnt3 = myTriangul->Node(aNode3);
    return theMgr.OverlapsTriangle(aPnt1, aPnt2, aPnt3, Select3D_TOS_INTERIOR, thePickResult);
  }
}

//=================================================================================================

bool Select3D_SensitiveTriangulation::elementIsInside(SelectBasics_SelectingVolumeManager& theMgr,
                                                      int  theElemIdx,
                                                      bool theIsFullInside)
{
  if (theIsFullInside)
  {
    return true;
  }

  const int aPrimitiveIdx = myBVHPrimIndexes->Value(theElemIdx);
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
    int aNode1, aNode2, aNode3;
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
double Select3D_SensitiveTriangulation::distanceToCOG(SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter(myCDG3D);
}

//=================================================================================================

occ::handle<Select3D_SensitiveEntity> Select3D_SensitiveTriangulation::GetConnected()
{
  bool                                         isInterior = mySensType == Select3D_TOS_INTERIOR;
  occ::handle<Select3D_SensitiveTriangulation> aNewEntity =
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
  for (int aX = 0; aX <= 1; ++aX)
  {
    for (int aY = 0; aY <= 1; ++aY)
    {
      for (int aZ = 0; aZ <= 1; ++aZ)
      {
        gp_Pnt aVertex = gp_Pnt(aX == 0 ? myBndBox.CornerMin().x() : myBndBox.CornerMax().x(),
                                aY == 0 ? myBndBox.CornerMin().y() : myBndBox.CornerMax().y(),
                                aZ == 0 ? myBndBox.CornerMin().z() : myBndBox.CornerMax().z());
        aVertex.Transform(myInitLocation.Transformation());
        aBndBox.Add(NCollection_Vec3<double>(aVertex.X(), aVertex.Y(), aVertex.Z()));
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
    myBndBox.Add(NCollection_Vec3<double>(aCachedBox.CornerMin().X(),
                                          aCachedBox.CornerMin().Y(),
                                          aCachedBox.CornerMin().Z()));
    myBndBox.Add(NCollection_Vec3<double>(aCachedBox.CornerMax().X(),
                                          aCachedBox.CornerMax().Y(),
                                          aCachedBox.CornerMax().Z()));
    return;
  }
  else if (myTriangul->HasGeometry())
  {
    for (int aNodeIdx = 1; aNodeIdx <= myTriangul->NbNodes(); ++aNodeIdx)
    {
      const gp_Pnt aNode = myTriangul->Node(aNodeIdx);
      myBndBox.Add(NCollection_Vec3<double>(aNode.X(), aNode.Y(), aNode.Z()));
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
int Select3D_SensitiveTriangulation::NbSubElements() const
{
  return myTriangul->NbNodes();
}

//=================================================================================================

bool Select3D_SensitiveTriangulation::HasInitLocation() const
{
  return !myInitLocation.IsIdentity();
}

//=================================================================================================

gp_GTrsf Select3D_SensitiveTriangulation::InvInitLocation() const
{
  return myInvInitLocation;
}

//=================================================================================================

void Select3D_SensitiveTriangulation::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Select3D_SensitiveSet)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTriangul.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myInitLocation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mySensType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPrimitivesNb)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBndBox)
}
