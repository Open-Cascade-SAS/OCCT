// Created on: 2016-02-18
// Created by: Varvara POSKONINA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <MeshVS_CommonSensitiveEntity.hxx>

#include <MeshVS_Buffer.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_CommonSensitiveEntity, Select3D_SensitiveSet)

//=================================================================================================

MeshVS_CommonSensitiveEntity::MeshVS_CommonSensitiveEntity(
  const Handle(SelectMgr_EntityOwner)& theOwner,
  const Handle(MeshVS_Mesh)&           theParentMesh,
  const MeshVS_MeshSelectionMethod     theSelMethod)
    : Select3D_SensitiveSet(theOwner),
      myDataSource(theParentMesh->GetDataSource()),
      mySelMethod(theSelMethod)
{
  myMaxFaceNodes = 0;
  theParentMesh->GetDrawer()->GetInteger(MeshVS_DA_MaxFaceNodes, myMaxFaceNodes);
  Standard_ASSERT_RAISE(
    myMaxFaceNodes > 0,
    "The maximal amount of nodes in a face must be greater than zero to create sensitive entity");
  gp_XYZ aCenter(0.0, 0.0, 0.0);

  if (mySelMethod == MeshVS_MSM_NODES)
  {
    Standard_Integer                  aNbSelectableNodes = 0;
    const TColStd_PackedMapOfInteger& anAllNodesMap      = myDataSource->GetAllNodes();
    for (TColStd_MapIteratorOfPackedMapOfInteger aNodesIter(anAllNodesMap); aNodesIter.More();
         aNodesIter.Next())
    {
      const Standard_Integer aNodeIdx = aNodesIter.Key();
      if (theParentMesh->IsSelectableNode(aNodeIdx))
      {
        const gp_Pnt aVertex = getVertexByIndex(aNodeIdx);
        aCenter += aVertex.XYZ();
        myBndBox.Add(SelectMgr_Vec3(aVertex.X(), aVertex.Y(), aVertex.Z()));
        ++aNbSelectableNodes;
        myItemIndexes.Append(aNodeIdx);
      }
    }

    // increase sensitivity for vertices detection
    SetSensitivityFactor(8);
    myCOG = aCenter / aNbSelectableNodes;
  }
  else if (mySelMethod == MeshVS_MSM_PRECISE)
  {
    const TColStd_PackedMapOfInteger& anAllNodesMap = myDataSource->GetAllNodes();
    for (TColStd_MapIteratorOfPackedMapOfInteger aNodesIter(anAllNodesMap); aNodesIter.More();
         aNodesIter.Next())
    {
      const Standard_Integer aNodeIdx = aNodesIter.Key();
      const gp_Pnt           aVertex  = getVertexByIndex(aNodeIdx);
      aCenter += aVertex.XYZ();
      myBndBox.Add(SelectMgr_Vec3(aVertex.X(), aVertex.Y(), aVertex.Z()));
    }
    myCOG = aCenter / anAllNodesMap.Extent();

    const TColStd_PackedMapOfInteger& anAllElementsMap = myDataSource->GetAllElements();
    MeshVS_EntityType                 aType            = MeshVS_ET_NONE;
    for (TColStd_MapIteratorOfPackedMapOfInteger anElemIter(anAllElementsMap); anElemIter.More();
         anElemIter.Next())
    {
      const Standard_Integer anElemIdx = anElemIter.Key();
      if (theParentMesh->IsSelectableElem(anElemIdx)
          && myDataSource->GetGeomType(anElemIdx, Standard_True, aType) && aType == MeshVS_ET_Face)
      {
        myItemIndexes.Append(anElemIdx);
      }
    }
  }
}

//=================================================================================================

MeshVS_CommonSensitiveEntity::MeshVS_CommonSensitiveEntity(
  const MeshVS_CommonSensitiveEntity& theOther)
    : Select3D_SensitiveSet(theOther.myOwnerId),
      myDataSource(theOther.myDataSource),
      myItemIndexes(theOther.myItemIndexes),
      mySelMethod(theOther.mySelMethod),
      myMaxFaceNodes(theOther.myMaxFaceNodes),
      myCOG(theOther.myCOG),
      myBndBox(theOther.myBndBox)
{
  //
}

//=================================================================================================

MeshVS_CommonSensitiveEntity::~MeshVS_CommonSensitiveEntity()
{
  myDataSource.Nullify();
  myItemIndexes.Clear();
}

//=================================================================================================

Standard_Integer MeshVS_CommonSensitiveEntity::NbSubElements() const
{
  return myItemIndexes.Size();
}

//=================================================================================================

Standard_Integer MeshVS_CommonSensitiveEntity::Size() const
{
  return myItemIndexes.Size();
}

//=================================================================================================

gp_Pnt MeshVS_CommonSensitiveEntity::getVertexByIndex(const Standard_Integer theNodeIdx) const
{
  Standard_Real        aCoordsBuf[3] = {};
  TColStd_Array1OfReal aCoords(aCoordsBuf[0], 1, 3);
  Standard_Integer     aNbNodes = 0;
  MeshVS_EntityType    aType    = MeshVS_ET_NONE;
  if (!myDataSource->GetGeom(theNodeIdx, Standard_False, aCoords, aNbNodes, aType))
  {
    return gp_Pnt();
  }
  return gp_Pnt(aCoords.Value(1), aCoords.Value(2), aCoords.Value(3));
}

//=================================================================================================

Select3D_BndBox3d MeshVS_CommonSensitiveEntity::Box(const Standard_Integer theIdx) const
{
  const Standard_Integer anItemIdx = myItemIndexes.Value(theIdx);
  Select3D_BndBox3d      aBox;
  if (mySelMethod == MeshVS_MSM_PRECISE)
  {
    MeshVS_Buffer        aCoordsBuf(3 * myMaxFaceNodes * sizeof(Standard_Real));
    TColStd_Array1OfReal aCoords(aCoordsBuf, 1, 3 * myMaxFaceNodes);
    Standard_Integer     aNbNodes = 0;
    MeshVS_EntityType    aType    = MeshVS_ET_NONE;
    if (!myDataSource->GetGeom(anItemIdx, Standard_True, aCoords, aNbNodes, aType) || aNbNodes == 0)
    {
      return aBox;
    }

    MeshVS_Buffer           aNodesBuf(aNbNodes * sizeof(Standard_Integer));
    TColStd_Array1OfInteger aElemNodes(aNodesBuf, 1, aNbNodes);
    if (!myDataSource->GetNodesByElement(anItemIdx, aElemNodes, aNbNodes))
    {
      return aBox;
    }

    for (Standard_Integer aNodeIdx = 1; aNodeIdx <= aNbNodes; aNodeIdx++)
    {
      const SelectMgr_Vec3 aPnt(aCoords(3 * aNodeIdx - 2),
                                aCoords(3 * aNodeIdx - 1),
                                aCoords(3 * aNodeIdx));
      aBox.Add(aPnt);
    }
  }
  else if (mySelMethod == MeshVS_MSM_NODES)
  {
    const gp_Pnt aVert = getVertexByIndex(anItemIdx);
    aBox.Add(SelectMgr_Vec3(aVert.X(), aVert.Y(), aVert.Z()));
  }

  return aBox;
}

//=================================================================================================

Standard_Real MeshVS_CommonSensitiveEntity::Center(const Standard_Integer theIdx,
                                                   const Standard_Integer theAxis) const
{
  const Select3D_BndBox3d& aBox    = Box(theIdx);
  SelectMgr_Vec3           aCenter = (aBox.CornerMin() + aBox.CornerMax()) * 0.5;

  return theAxis == 0 ? aCenter.x() : (theAxis == 1 ? aCenter.y() : aCenter.z());
}

//=================================================================================================

void MeshVS_CommonSensitiveEntity::Swap(const Standard_Integer theIdx1,
                                        const Standard_Integer theIdx2)
{
  const Standard_Integer anItem1     = myItemIndexes.Value(theIdx1);
  const Standard_Integer anItem2     = myItemIndexes.Value(theIdx2);
  myItemIndexes.ChangeValue(theIdx1) = anItem2;
  myItemIndexes.ChangeValue(theIdx2) = anItem1;
}

//=================================================================================================

Standard_Boolean MeshVS_CommonSensitiveEntity::overlapsElement(
  SelectBasics_PickResult&             thePickResult,
  SelectBasics_SelectingVolumeManager& theMgr,
  Standard_Integer                     theElemIdx,
  Standard_Boolean                     theIsFullInside)
{
  if (theIsFullInside)
  {
    return Standard_True;
  }

  const Standard_Integer anItemIdx = myItemIndexes.Value(theElemIdx);
  if (mySelMethod == MeshVS_MSM_PRECISE)
  {
    MeshVS_Buffer        aCoordsBuf(3 * myMaxFaceNodes * sizeof(Standard_Real));
    TColStd_Array1OfReal aCoords(aCoordsBuf, 1, 3 * myMaxFaceNodes);
    Standard_Integer     aNbNodes = 0;
    MeshVS_EntityType    aType    = MeshVS_ET_NONE;
    if (!myDataSource->GetGeom(anItemIdx, Standard_True, aCoords, aNbNodes, aType) || aNbNodes == 0)
    {
      return Standard_False;
    }

    MeshVS_Buffer           aNodesBuf(aNbNodes * sizeof(Standard_Integer));
    TColStd_Array1OfInteger aElemNodes(aNodesBuf, 1, aNbNodes);
    if (!myDataSource->GetNodesByElement(anItemIdx, aElemNodes, aNbNodes))
    {
      return Standard_False;
    }
    if (aNbNodes == 3)
    {
      return theMgr.OverlapsTriangle(gp_Pnt(aCoords(1), aCoords(2), aCoords(3)),
                                     gp_Pnt(aCoords(4), aCoords(5), aCoords(6)),
                                     gp_Pnt(aCoords(7), aCoords(8), aCoords(9)),
                                     Select3D_TOS_INTERIOR,
                                     thePickResult);
    }

    MeshVS_Buffer      aFacePntsBuf(aNbNodes * 3 * sizeof(Standard_Real));
    TColgp_Array1OfPnt aFacePnts(aFacePntsBuf, 1, aNbNodes);
    for (Standard_Integer aNodeIdx = 1; aNodeIdx <= aNbNodes; aNodeIdx++)
    {
      aFacePnts.SetValue(
        aNodeIdx,
        gp_Pnt(aCoords(3 * aNodeIdx - 2), aCoords(3 * aNodeIdx - 1), aCoords(3 * aNodeIdx)));
    }
    return theMgr.OverlapsPolygon(aFacePnts, Select3D_TOS_INTERIOR, thePickResult);
  }
  else if (mySelMethod == MeshVS_MSM_NODES)
  {
    const gp_Pnt aVert = getVertexByIndex(anItemIdx);
    return theMgr.OverlapsPoint(aVert, thePickResult);
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean MeshVS_CommonSensitiveEntity::elementIsInside(
  SelectBasics_SelectingVolumeManager& theMgr,
  Standard_Integer                     theElemIdx,
  Standard_Boolean                     theIsFullInside)
{
  if (theIsFullInside)
  {
    return Standard_True;
  }

  const Standard_Integer anItemIdx = myItemIndexes.Value(theElemIdx);
  if (mySelMethod == MeshVS_MSM_PRECISE)
  {
    MeshVS_Buffer        aCoordsBuf(3 * myMaxFaceNodes * sizeof(Standard_Real));
    TColStd_Array1OfReal aCoords(aCoordsBuf, 1, 3 * myMaxFaceNodes);
    Standard_Integer     aNbNodes = 0;
    MeshVS_EntityType    aType    = MeshVS_ET_NONE;
    if (!myDataSource->GetGeom(anItemIdx, Standard_True, aCoords, aNbNodes, aType) || aNbNodes == 0)
    {
      return Standard_False;
    }

    MeshVS_Buffer           aNodesBuf(aNbNodes * sizeof(Standard_Integer));
    TColStd_Array1OfInteger aElemNodes(aNodesBuf, 1, aNbNodes);
    if (!myDataSource->GetNodesByElement(anItemIdx, aElemNodes, aNbNodes))
    {
      return Standard_False;
    }

    MeshVS_Buffer      aFacePntsBuf(aNbNodes * 3 * sizeof(Standard_Real));
    TColgp_Array1OfPnt aFacePnts(aFacePntsBuf, 1, aNbNodes);
    for (Standard_Integer aNodeIdx = 1; aNodeIdx <= aNbNodes; ++aNodeIdx)
    {
      const gp_Pnt aPnt(aCoords(3 * aNodeIdx - 2),
                        aCoords(3 * aNodeIdx - 1),
                        aCoords(3 * aNodeIdx));
      if (!theMgr.OverlapsPoint(aPnt))
      {
        return Standard_False;
      }
    }
    return Standard_True;
  }
  else if (mySelMethod == MeshVS_MSM_NODES)
  {
    const gp_Pnt aVert = getVertexByIndex(anItemIdx);
    return theMgr.OverlapsPoint(aVert);
  }
  return Standard_False;
}

//=================================================================================================

Standard_Real MeshVS_CommonSensitiveEntity::distanceToCOG(
  SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter(myCOG);
}

//=================================================================================================

Select3D_BndBox3d MeshVS_CommonSensitiveEntity::BoundingBox()
{
  return myBndBox;
}

//=================================================================================================

gp_Pnt MeshVS_CommonSensitiveEntity::CenterOfGeometry() const
{
  return myCOG;
}
