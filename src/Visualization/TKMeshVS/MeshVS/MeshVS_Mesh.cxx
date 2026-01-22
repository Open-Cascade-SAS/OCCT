// Created on: 2003-09-09
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <AIS_InteractiveContext.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <MeshVS_Buffer.hxx>
#include <MeshVS_CommonSensitiveEntity.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <NCollection_DataMap.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_DummySensitiveEntity.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshEntityOwner.hxx>
#include <MeshVS_MeshOwner.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <MeshVS_SelectionModeFlags.hxx>
#include <MeshVS_SensitiveFace.hxx>
#include <MeshVS_SensitiveMesh.hxx>
#include <MeshVS_SensitivePolyhedron.hxx>
#include <MeshVS_SensitiveSegment.hxx>
#include <MeshVS_SensitiveQuad.hxx>
#include <OSD_Timer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_Selection.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <NCollection_Array1.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <NCollection_PackedMapAlgo.hxx>
class SelectMgr_EntityOwner;

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_Mesh, AIS_InteractiveObject)

//=================================================================================================

MeshVS_Mesh::MeshVS_Mesh(const bool theIsAllowOverlapped)
{
  myDataSource.Nullify();
  myHilighter.Nullify();
  myWholeMeshOwner.Nullify();
  mySelectionMethod = MeshVS_MSM_NODES;

  SetAutoHilight(false);

  SetDisplayMode(MeshVS_DMF_WireFrame); // Mode as default
  SetHilightMode(MeshVS_DMF_WireFrame); // Wireframe as default hilight mode

  SetColor(Quantity_NOC_WHITE);
  SetMaterial(Graphic3d_NameOfMaterial_Plastified);

  myCurrentDrawer = new MeshVS_Drawer();
  myCurrentDrawer->SetColor(MeshVS_DA_InteriorColor, Quantity_NOC_BLUE4);
  myCurrentDrawer->SetInteger(MeshVS_DA_InteriorStyle, Aspect_IS_SOLID);
  myCurrentDrawer->SetInteger(MeshVS_DA_MaxFaceNodes, 10);
  myCurrentDrawer->SetBoolean(MeshVS_DA_IsAllowOverlapped, theIsAllowOverlapped);
  myCurrentDrawer->SetBoolean(MeshVS_DA_Reflection, true);
  myCurrentDrawer->SetDouble(MeshVS_DA_ShrinkCoeff, 0.8);
  myCurrentDrawer->SetBoolean(MeshVS_DA_ComputeTime, false);
  myCurrentDrawer->SetBoolean(MeshVS_DA_ComputeSelectionTime, false);
  myCurrentDrawer->SetBoolean(MeshVS_DA_DisplayNodes, true);
  myCurrentDrawer->SetDouble(MeshVS_DA_EdgeWidth, 1.0);
  myCurrentDrawer->SetInteger(MeshVS_DA_EdgeType, Aspect_TOL_SOLID);
  myCurrentDrawer->SetInteger(MeshVS_DA_MarkerType, Aspect_TOM_O);
  myCurrentDrawer->SetColor(MeshVS_DA_MarkerColor, Quantity_NOC_WHITE);
  myCurrentDrawer->SetDouble(MeshVS_DA_MarkerScale, 1.0);
  myCurrentDrawer->SetInteger(MeshVS_DA_BeamType, Aspect_TOL_SOLID);
  myCurrentDrawer->SetDouble(MeshVS_DA_BeamWidth, 1.0);
  myCurrentDrawer->SetBoolean(MeshVS_DA_SmoothShading, false);
  myCurrentDrawer->SetBoolean(MeshVS_DA_SupressBackFaces, false);

  mySelectionDrawer = new MeshVS_Drawer();
  mySelectionDrawer->Assign(myCurrentDrawer);
  mySelectionDrawer->SetInteger(MeshVS_DA_MarkerType, Aspect_TOM_STAR);
  mySelectionDrawer->SetColor(MeshVS_DA_MarkerColor, Quantity_NOC_GRAY80);
  mySelectionDrawer->SetDouble(MeshVS_DA_MarkerScale, 2.0);

  mySelectionDrawer->SetColor(MeshVS_DA_BeamColor, Quantity_NOC_GRAY80);
  mySelectionDrawer->SetInteger(MeshVS_DA_BeamType, Aspect_TOL_SOLID);
  mySelectionDrawer->SetDouble(MeshVS_DA_BeamWidth, 3.0);

  myHilightDrawer = new MeshVS_Drawer();
  myHilightDrawer->Assign(myCurrentDrawer);
  myHilightDrawer->SetDouble(MeshVS_DA_ShrinkCoeff, 0.7);
  myHilightDrawer->SetInteger(MeshVS_DA_InteriorStyle, Aspect_IS_SOLID);
  myHilightDrawer->SetColor(MeshVS_DA_InteriorColor, Quantity_NOC_YELLOW);
  myHilightDrawer->SetColor(MeshVS_DA_BackInteriorColor, Quantity_NOC_YELLOW);
  myHilightDrawer->SetColor(MeshVS_DA_EdgeColor, Quantity_NOC_GREEN);
  myHilightDrawer->SetInteger(MeshVS_DA_EdgeType, Aspect_TOL_SOLID);
  myHilightDrawer->SetDouble(MeshVS_DA_EdgeWidth, 1.0);
  myHilightDrawer->SetMaterial(MeshVS_DA_FrontMaterial, Graphic3d_NameOfMaterial_Plastified);
  myHilightDrawer->SetMaterial(MeshVS_DA_BackMaterial, Graphic3d_NameOfMaterial_Plastified);

  myHilightDrawer->SetColor(MeshVS_DA_BeamColor, Quantity_NOC_GRAY80);
  myHilightDrawer->SetInteger(MeshVS_DA_BeamType, Aspect_TOL_SOLID);
  myHilightDrawer->SetDouble(MeshVS_DA_BeamWidth, 3.0);

  myHilightDrawer->SetInteger(MeshVS_DA_MarkerType, Aspect_TOM_STAR);
  myHilightDrawer->SetColor(MeshVS_DA_MarkerColor, Quantity_NOC_GRAY80);
  myHilightDrawer->SetDouble(MeshVS_DA_MarkerScale, 2.0);
}

//=================================================================================================

bool MeshVS_Mesh::AcceptDisplayMode(const int theMode) const
{
  if (theMode <= 0)
  {
    return false;
  }
  else if (myBuilders.IsEmpty())
  {
    return true;
  }

  for (NCollection_Sequence<occ::handle<MeshVS_PrsBuilder>>::Iterator aBuilderIter(myBuilders);
       aBuilderIter.More();
       aBuilderIter.Next())
  {
    const occ::handle<MeshVS_PrsBuilder>& aBuilder = aBuilderIter.Value();
    if (!aBuilder.IsNull() && aBuilder->TestFlags(theMode))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

void MeshVS_Mesh::Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                          const occ::handle<Prs3d_Presentation>&         thePresentation,
                          const int                                      theMode)
{
  bool toShowComputeTime = true;
  myCurrentDrawer->GetBoolean(MeshVS_DA_ComputeTime, toShowComputeTime);
  OSD_Timer aTimer;
  if (toShowComputeTime)
  {
    aTimer.Reset();
    aTimer.Start();
  }

  // Repair Ids in map if necessary
  occ::handle<MeshVS_DataSource> aDS = GetDataSource();
  if (aDS.IsNull() || theMode <= 0)
  {
    return;
  }

  const TColStd_PackedMapOfInteger& aNodes      = aDS->GetAllNodes();
  const TColStd_PackedMapOfInteger& aElems      = aDS->GetAllElements();
  const bool                        hasNodes    = !aNodes.IsEmpty();
  const bool                        hasElements = !aElems.IsEmpty();

  TColStd_PackedMapOfInteger aNodesToExclude, aElemsToExclude;
  for (NCollection_Sequence<occ::handle<MeshVS_PrsBuilder>>::Iterator aBuilderIter(myBuilders);
       aBuilderIter.More();
       aBuilderIter.Next())
  {
    const occ::handle<MeshVS_PrsBuilder>& aBuilder = aBuilderIter.Value();
    if (!aBuilder.IsNull() && aBuilder->TestFlags(theMode))
    {
      aBuilder->SetPresentationManager(thePrsMgr);
      if (hasNodes)
      {
        aBuilder->Build(thePresentation, aNodes, aNodesToExclude, false, theMode);
      }
      if (hasElements)
      {
        aBuilder->Build(thePresentation, aElems, aElemsToExclude, true, theMode);
      }
    }
  }

  if (toShowComputeTime)
  {
    double aSec, aCpu;
    int    aMin, anHour;
    aTimer.Show(aSec, aMin, anHour, aCpu);
    std::cout << "DisplayMode : " << theMode << "\n";
    std::cout << "Compute : " << aSec << " sec\n";
    std::cout << "Compute CPU : " << aCpu << " sec\n\n";
  }
}

//=================================================================================================

void MeshVS_Mesh::scanFacesForSharedNodes(const TColStd_PackedMapOfInteger& theAllElements,
                                          const int                         theNbMaxFaceNodes,
                                          TColStd_PackedMapOfInteger&       theSharedNodes) const
{
  theSharedNodes.Clear();
  MeshVS_EntityType          aType;
  int                        aNbNodes;
  MeshVS_Buffer              aCoordsBuf(3 * theNbMaxFaceNodes * sizeof(double));
  NCollection_Array1<double> aCoords(aCoordsBuf, 1, 3 * theNbMaxFaceNodes);
  for (TColStd_PackedMapOfInteger::Iterator aFaceIter(theAllElements); aFaceIter.More();
       aFaceIter.Next())
  {
    const int aFaceIdx = aFaceIter.Key();

    if (IsSelectableElem(aFaceIdx) && myDataSource->GetGeomType(aFaceIdx, true, aType)
        && aType == MeshVS_ET_Face)
    {
      myDataSource->GetGeom(aFaceIdx, true, aCoords, aNbNodes, aType);
      if (aNbNodes == 0)
        continue;

      MeshVS_Buffer           aNodesBuf(aNbNodes * sizeof(int));
      NCollection_Array1<int> aElemNodes(aNodesBuf, 1, aNbNodes);
      if (!myDataSource->GetNodesByElement(aFaceIdx, aElemNodes, aNbNodes))
        continue;

      MeshVS_Buffer              aFacePntsBuf(aNbNodes * 3 * sizeof(double));
      NCollection_Array1<gp_Pnt> aFacePnts(aFacePntsBuf, 1, aNbNodes);
      for (int aNodeIdx = 1; aNodeIdx <= aNbNodes; ++aNodeIdx)
      {
        theSharedNodes.Add(aElemNodes(aNodeIdx));
      }
    }
  }
}

//=================================================================================================

void MeshVS_Mesh::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                   const int                               theMode)
{
  OSD_Timer gTimer;
  bool      toShowComputeSelectionTime = true;
  myCurrentDrawer->GetBoolean(MeshVS_DA_ComputeSelectionTime, toShowComputeSelectionTime);
  if (toShowComputeSelectionTime)
  {
    gTimer.Reset();
    gTimer.Start();
  }

  int                            aMaxFaceNodes = 0;
  occ::handle<MeshVS_DataSource> aSource       = GetDataSource();
  if (aSource.IsNull() || myCurrentDrawer.IsNull()
      || !myCurrentDrawer->GetInteger(MeshVS_DA_MaxFaceNodes, aMaxFaceNodes) || aMaxFaceNodes <= 0)
  {
    return;
  }

  const int aMode = HasDisplayMode() ? DisplayMode() : DefaultDisplayMode();
  if (myHilighter.IsNull() || (aMode & MeshVS_DMF_OCCMask) == 0)
  {
    return;
  }

  // Make two array aliases pointing to the same memory:
  // - NCollection_Array1<double> for getting values from MeshVS_DataSource interface
  // - array of gp_Pnt for convenient work with array of points
  MeshVS_Buffer              aCoordsBuf(3 * aMaxFaceNodes * sizeof(double));
  NCollection_Array1<gp_Pnt> aPntArray(aCoordsBuf, 1, aMaxFaceNodes);
  NCollection_Array1<double> aPntArrayAsCoordArray(aCoordsBuf, 1, 3 * aMaxFaceNodes);

  const TColStd_PackedMapOfInteger& anAllNodesMap    = aSource->GetAllNodes();
  const TColStd_PackedMapOfInteger& anAllElementsMap = aSource->GetAllElements();
  if (aSource->IsAdvancedSelectionEnabled())
  {
    occ::handle<MeshVS_MeshOwner> anOwner;
    for (NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>>::Iterator anIt(
           GetOwnerMaps(false));
         anIt.More();
         anIt.Next())
    {
      anOwner = occ::down_cast<MeshVS_MeshOwner>(anIt.Value());
      if (!anOwner.IsNull())
      {
        // get the owner if it is already created
        break;
      }
    }
    if (anOwner.IsNull())
    {
      // create one owner for the whole mesh and for all selection modes
      anOwner = new MeshVS_MeshOwner(this, aSource, 5);
    }

    // Save the owner. It will be available via GetOwnerMaps method
    if (!myMeshOwners.IsBound(1))
    {
      myMeshOwners.Bind(1, anOwner);
    }
    // Create one sensitive entity. It should detect mesh entities correspondingly to selection mode
    occ::handle<MeshVS_SensitiveMesh> aSensMesh = new MeshVS_SensitiveMesh(anOwner, theMode);
    theSelection->Add(aSensMesh);
  }
  else
  {
    switch (theMode)
    {
      case MeshVS_SMF_Node: {
        myNodeOwners.Clear();
        for (TColStd_PackedMapOfInteger::Iterator anIter(anAllNodesMap); anIter.More();
             anIter.Next())
        {
          const int         aKey     = anIter.Key();
          int               aNbNodes = 0;
          MeshVS_EntityType aType    = MeshVS_ET_NONE;
          if (!myDataSource->GetGeom(aKey, false, aPntArrayAsCoordArray, aNbNodes, aType))
          {
            continue;
          }

          void*                               anAddr = myDataSource->GetAddr(aKey, false);
          occ::handle<MeshVS_MeshEntityOwner> anOwner =
            new MeshVS_MeshEntityOwner(this, aKey, anAddr, aType, 5);
          myNodeOwners.Bind(aKey, anOwner);
          if (IsSelectableNode(aKey))
          {
            occ::handle<Select3D_SensitivePoint> aPoint =
              new Select3D_SensitivePoint(anOwner, aPntArray.First());
            theSelection->Add(aPoint);
          }
          else
          {
            theSelection->Add(new MeshVS_DummySensitiveEntity(anOwner));
          }
        }
        break;
      }
      case MeshVS_SMF_Mesh: {
        if (myWholeMeshOwner.IsNull())
        {
          myWholeMeshOwner = new SelectMgr_EntityOwner(this);
        }

        switch (mySelectionMethod)
        {
          case MeshVS_MSM_BOX: {
            Bnd_Box aBndBox;
            BoundingBox(aBndBox);
            if (!aBndBox.IsVoid())
            {
              theSelection->Add(new Select3D_SensitiveBox(myWholeMeshOwner, aBndBox));
            }
            break;
          }
          case MeshVS_MSM_NODES: {
            theSelection->Add(
              new MeshVS_CommonSensitiveEntity(myWholeMeshOwner, this, MeshVS_MSM_NODES));
            break;
          }
          case MeshVS_MSM_PRECISE: {
            theSelection->Add(
              new MeshVS_CommonSensitiveEntity(myWholeMeshOwner, this, MeshVS_MSM_PRECISE));

            // since MeshVS_Mesh objects can contain free edges and vertices, it is necessary to
            // create separate sensitive entity for each of them
            TColStd_PackedMapOfInteger aSharedNodes;
            scanFacesForSharedNodes(anAllElementsMap, aMaxFaceNodes, aSharedNodes);

            // create sensitive entities for free edges, if there are any
            int               aNbNodes = 0;
            MeshVS_EntityType aType    = MeshVS_ET_NONE;
            for (TColStd_PackedMapOfInteger::Iterator anElemIter(anAllElementsMap);
                 anElemIter.More();
                 anElemIter.Next())
            {
              const int anElemIdx = anElemIter.Key();
              if (IsSelectableElem(anElemIdx) && myDataSource->GetGeomType(anElemIdx, true, aType)
                  && aType == MeshVS_ET_Link)
              {
                myDataSource->GetGeom(anElemIdx, true, aPntArrayAsCoordArray, aNbNodes, aType);
                if (aNbNodes == 0)
                {
                  continue;
                }

                MeshVS_Buffer           aNodesBuf(aNbNodes * sizeof(int));
                NCollection_Array1<int> aElemNodes(aNodesBuf, 1, aNbNodes);
                if (!myDataSource->GetNodesByElement(anElemIdx, aElemNodes, aNbNodes))
                {
                  continue;
                }

                MeshVS_Buffer              aPntsBuf(aNbNodes * 3 * sizeof(double));
                NCollection_Array1<gp_Pnt> aLinkPnts(aPntsBuf, 1, aNbNodes);
                bool                       isVertsShared = true;
                for (int aPntIdx = 1; aPntIdx <= aNbNodes; ++aPntIdx)
                {
                  aLinkPnts(aPntIdx) = aPntArray.Value(aPntIdx);
                  isVertsShared      = isVertsShared && aSharedNodes.Contains(aElemNodes(aPntIdx));
                  aSharedNodes.Add(aElemNodes(aPntIdx));
                }

                if (!isVertsShared)
                {
                  occ::handle<Select3D_SensitiveEntity> aLinkEnt =
                    new Select3D_SensitiveSegment(myWholeMeshOwner,
                                                  aLinkPnts.Value(1),
                                                  aLinkPnts.Value(2));
                  theSelection->Add(aLinkEnt);
                }
              }
            }

            // create sensitive entities for free nodes, if there are any
            for (TColStd_PackedMapOfInteger::Iterator aNodesIter(anAllNodesMap); aNodesIter.More();
                 aNodesIter.Next())
            {
              const int aNodeIdx = aNodesIter.Key();
              if (IsSelectableNode(aNodeIdx)
                  && myDataSource->GetGeom(aNodeIdx, false, aPntArrayAsCoordArray, aNbNodes, aType)
                  && !aSharedNodes.Contains(aNodeIdx))
              {
                occ::handle<Select3D_SensitiveEntity> aNodeEnt =
                  new Select3D_SensitivePoint(myWholeMeshOwner, aPntArray.First());
                theSelection->Add(aNodeEnt);
              }
            }
          }
          break;
        }
        break;
      }
      case MeshVS_SMF_Group: {
        myGroupOwners.Clear();

        TColStd_PackedMapOfInteger anAllGroupsMap;
        aSource->GetAllGroups(anAllGroupsMap);

        occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> aTopo;
        for (TColStd_PackedMapOfInteger::Iterator anIter(anAllGroupsMap); anIter.More();
             anIter.Next())
        {
          const int                  aKeyGroup  = anIter.Key();
          MeshVS_EntityType          aGroupType = MeshVS_ET_NONE;
          TColStd_PackedMapOfInteger aGroupMap;
          if (!myDataSource->GetGroup(aKeyGroup, aGroupType, aGroupMap))
          {
            continue;
          }

          void* anAddr = myDataSource->GetGroupAddr(aKeyGroup);
          int   aPrior = 0;
          switch (aGroupType)
          {
            case MeshVS_ET_Volume:
              aPrior = 1;
              break;
            case MeshVS_ET_Face:
              aPrior = 2;
              break;
            case MeshVS_ET_Link:
              aPrior = 3;
              break;
            case MeshVS_ET_0D:
              aPrior = 4;
              break;
            case MeshVS_ET_Node:
              aPrior = 5;
              break;
            default:
              break;
          }

          occ::handle<MeshVS_MeshEntityOwner> anOwner =
            new MeshVS_MeshEntityOwner(this, aKeyGroup, anAddr, aGroupType, aPrior, true);
          myGroupOwners.Bind(aKeyGroup, anOwner);

          bool              added    = false;
          int               aNbNodes = 0;
          MeshVS_EntityType aType    = MeshVS_ET_NONE;
          for (TColStd_PackedMapOfInteger::Iterator anIterMG(aGroupMap); anIterMG.More();
               anIterMG.Next())
          {
            int aKey = anIterMG.Key();
            if (aGroupType == MeshVS_ET_Node)
            {
              if (myDataSource->GetGeom(aKey, false, aPntArrayAsCoordArray, aNbNodes, aType)
                  && IsSelectableNode /*!IsHiddenNode*/ (aKey))
              {
                theSelection->Add(new Select3D_SensitivePoint(anOwner, aPntArray.First()));
                added = true;
              }
            }
            else if (myDataSource->GetGeomType(aKey, true, aType)
                     && IsSelectableElem /*!IsHiddenElem*/ (aKey))
            {
              myDataSource->GetGeom(aKey, true, aPntArrayAsCoordArray, aNbNodes, aType);
              if (aType == MeshVS_ET_Face && aNbNodes > 0) // Faces: 2D-elements
              {
                occ::handle<Select3D_SensitiveEntity> aSensFace;
                if (aNbNodes == 3)
                {
                  aSensFace = new Select3D_SensitiveTriangle(anOwner,
                                                             aPntArray.Value(1),
                                                             aPntArray.Value(2),
                                                             aPntArray.Value(3),
                                                             Select3D_TOS_INTERIOR);
                }
                else if (aNbNodes == 4)
                {
                  aSensFace = new MeshVS_SensitiveQuad(anOwner, aPntArray);
                }
                else
                {
                  aSensFace = new MeshVS_SensitiveFace(anOwner, aPntArray);
                }
                theSelection->Add(aSensFace);
                added = true;
              }
              else if (aType == MeshVS_ET_Link && aNbNodes > 0) // Links: 1D-elements
              {
                occ::handle<MeshVS_SensitiveSegment> aSeg =
                  new MeshVS_SensitiveSegment(anOwner, aPntArray(1), aPntArray(2));
                theSelection->Add(aSeg);
                added = true;
              }
              else if (aType == MeshVS_ET_Volume && aSource->Get3DGeom(aKey, aNbNodes, aTopo))
              {
                occ::handle<MeshVS_SensitivePolyhedron> aPolyhedron =
                  new MeshVS_SensitivePolyhedron(anOwner, aPntArray, aTopo);
                theSelection->Add(aPolyhedron);
                added = true;
              }
              else // if ( aType == MeshVS_ET_0D )   // Custom : not only 0D-elements !!!
              {
                occ::handle<Select3D_SensitiveEntity> anEnt =
                  myHilighter->CustomSensitiveEntity(anOwner, aKey);
                if (!anEnt.IsNull())
                {
                  theSelection->Add(anEnt);
                  added = true;
                }
              }
            }
          }
          if (!added)
          {
            theSelection->Add(new MeshVS_DummySensitiveEntity(anOwner));
          }
        }
        break;
      }
      default: // all residuary modes
      {
        occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> aTopo;
        myElementOwners.Clear();

        NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>>* aCurMap = &my0DOwners;
        if (theMode == MeshVS_ET_Link)
        {
          aCurMap = &myLinkOwners;
        }
        else if (theMode == MeshVS_ET_Face)
        {
          aCurMap = &myFaceOwners;
        }
        else if (theMode == MeshVS_ET_Volume)
        {
          aCurMap = &myVolumeOwners;
        }
        aCurMap->Clear();

        int               aNbNodes = 0;
        MeshVS_EntityType aType    = MeshVS_ET_NONE;
        for (TColStd_PackedMapOfInteger::Iterator anIterMV(anAllElementsMap); anIterMV.More();
             anIterMV.Next())
        {
          int aKey = anIterMV.Key();
          if (myDataSource->GetGeomType(aKey, true, aType) && theMode == aType)
          {
            myDataSource->GetGeom(aKey, true, aPntArrayAsCoordArray, aNbNodes, aType);
            void* anAddr = myDataSource->GetAddr(aKey, true);

            int aPrior = 0;
            switch (aType)
            {
              case MeshVS_ET_Volume:
                aPrior = 1;
                break;
              case MeshVS_ET_Face:
                aPrior = 2;
                break;
              case MeshVS_ET_Link:
                aPrior = 3;
                break;
              case MeshVS_ET_0D:
                aPrior = 4;
                break;
              default:
                break;
            }

            occ::handle<MeshVS_MeshEntityOwner> anOwner =
              new MeshVS_MeshEntityOwner(this, aKey, anAddr, aType, aPrior);
            aCurMap->Bind(aKey, anOwner);
            if (IsSelectableElem(aKey)) // The element is selectable
            {
              if (aType == MeshVS_ET_Face && aNbNodes > 0) // Faces: 2D-elements
              {
                occ::handle<Select3D_SensitiveEntity> aSensFace;
                if (aNbNodes == 3)
                {
                  aSensFace = new Select3D_SensitiveTriangle(anOwner,
                                                             aPntArray.Value(1),
                                                             aPntArray.Value(2),
                                                             aPntArray.Value(3),
                                                             Select3D_TOS_INTERIOR);
                }
                else if (aNbNodes == 4)
                {
                  aSensFace = new MeshVS_SensitiveQuad(anOwner, aPntArray);
                }
                else
                {
                  aSensFace = new MeshVS_SensitiveFace(anOwner, aPntArray);
                }
                theSelection->Add(aSensFace);
              }
              else if (aType == MeshVS_ET_Link && aNbNodes > 0) // Links: 1D-elements
              {
                occ::handle<MeshVS_SensitiveSegment> aSeg =
                  new MeshVS_SensitiveSegment(anOwner, aPntArray(1), aPntArray(2));
                theSelection->Add(aSeg);
              }
              else if (aType == MeshVS_ET_Volume && aSource->Get3DGeom(aKey, aNbNodes, aTopo))
              {
                occ::handle<MeshVS_SensitivePolyhedron> aPolyhedron =
                  new MeshVS_SensitivePolyhedron(anOwner, aPntArray, aTopo);
                theSelection->Add(aPolyhedron);
              }
              else // if ( aType == MeshVS_ET_0D )   // Custom : not only 0D-elements !!!
              {
                occ::handle<Select3D_SensitiveEntity> anEnt =
                  myHilighter->CustomSensitiveEntity(anOwner, aKey);
                if (!anEnt.IsNull())
                {
                  theSelection->Add(anEnt);
                }
              }
            }
            else
            {
              theSelection->Add(new MeshVS_DummySensitiveEntity(anOwner));
            }
          }
        }
        break;
      }
    }
  }

  if (toShowComputeSelectionTime)
  {
    double sec, cpu;
    int    min, hour;
    gTimer.Show(sec, min, hour, cpu);
    std::cout << "SelectionMode : " << theMode << "\n";
    std::cout << "Compute selection: " << sec << " sec\n";
    std::cout << "Compute selection CPU : " << cpu << " sec\n\n";
    gTimer.Stop();
  }
}

//=================================================================================================

int MeshVS_Mesh::GetBuildersCount() const
{
  return myBuilders.Length();
}

//=================================================================================================

int MeshVS_Mesh::GetFreeId() const
{
  TColStd_PackedMapOfInteger Ids;
  int                        i, len = myBuilders.Length(), curId;

  for (i = 1; i <= len; i++)
    Ids.Add(myBuilders.Value(i)->GetId());

  curId = 0;
  while (Ids.Contains(curId))
    curId++;

  return curId;
}

//=================================================================================================

occ::handle<MeshVS_PrsBuilder> MeshVS_Mesh::GetBuilder(const int Index) const
{
  if (Index >= 1 && Index <= myBuilders.Length())
    return myBuilders.Value(Index);
  else
    return nullptr;
}

//=================================================================================================

occ::handle<MeshVS_PrsBuilder> MeshVS_Mesh::GetBuilderById(const int Id) const
{
  occ::handle<MeshVS_PrsBuilder> Result;

  int i, len = myBuilders.Length();
  for (i = 1; i <= len; i++)
    if (myBuilders.Value(i)->GetId() == Id)
    {
      Result = myBuilders.Value(i);
      break;
    }
  return Result;
}

//=================================================================================================

void MeshVS_Mesh::AddBuilder(const occ::handle<MeshVS_PrsBuilder>& theBuilder,
                             const bool                            TreatAsHilighter)
{
  if (theBuilder.IsNull())
    return;

  int i, n = myBuilders.Length();
  for (i = 1; i <= n; i++)
    if (myBuilders(i)->GetPriority() < theBuilder->GetPriority())
      break;

  if (i > n)
    myBuilders.Append(theBuilder);
  else
    myBuilders.InsertBefore(i, theBuilder);

  if (TreatAsHilighter)
    myHilighter = theBuilder;
}

//=================================================================================================

void MeshVS_Mesh::RemoveBuilder(const int theIndex)
{
  occ::handle<MeshVS_PrsBuilder> aBuild = GetBuilder(theIndex);
  if (!aBuild.IsNull())
  {
    if (aBuild == myHilighter)
      myHilighter.Nullify();
    myBuilders.Remove(theIndex);
  }
}

//=================================================================================================

void MeshVS_Mesh::RemoveBuilderById(const int Id)
{
  int i, n = myBuilders.Length();
  for (i = 1; i <= n; i++)
  {
    occ::handle<MeshVS_PrsBuilder> aCur = myBuilders(i);
    if (!aCur.IsNull() && aCur->GetId() == Id)
      break;
  }
  if (i >= 1 && i <= n)
  {
    if (GetBuilder(i) == myHilighter)
      myHilighter.Nullify();
    RemoveBuilder(i);
  }
}

//=================================================================================================

void MeshVS_Mesh::SetHiddenElems(const occ::handle<TColStd_HPackedMapOfInteger>& theMap)
{
  myHiddenElements = theMap;

  // Note: update of list of selectable nodes -- this is not optimal!
  bool AutoSelUpdate = false;
  if (!GetDrawer().IsNull() && GetDrawer()->GetBoolean(MeshVS_DA_SelectableAuto, AutoSelUpdate)
      && AutoSelUpdate)
    UpdateSelectableNodes();
}

//=================================================================================================

void MeshVS_Mesh::SetHiddenNodes(const occ::handle<TColStd_HPackedMapOfInteger>& theMap)
{
  myHiddenNodes = theMap;

  // Note: update of list of selectable nodes -- this is not optimal!
  bool AutoSelUpdate = false;
  if (!GetDrawer().IsNull() && GetDrawer()->GetBoolean(MeshVS_DA_SelectableAuto, AutoSelUpdate)
      && AutoSelUpdate)
    UpdateSelectableNodes();
}

//=================================================================================================

const occ::handle<TColStd_HPackedMapOfInteger>& MeshVS_Mesh::GetHiddenElems() const
{
  return myHiddenElements;
}

//=================================================================================================

const occ::handle<TColStd_HPackedMapOfInteger>& MeshVS_Mesh::GetHiddenNodes() const
{
  return myHiddenNodes;
}

//=================================================================================================

void AddToMap(NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>>&       Result,
              const NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>>& Addition)
{
  NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>>::Iterator anIt(Addition);
  for (; anIt.More(); anIt.Next())
    if (Result.IsBound(anIt.Key()))
      Result.ChangeFind(anIt.Key()) = anIt.Value();
    else
      Result.Bind(anIt.Key(), anIt.Value());
}

//=================================================================================================

const NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>>& MeshVS_Mesh::GetOwnerMaps(
  const bool IsElements)
{
  occ::handle<MeshVS_DataSource> aDS = GetDataSource();
  if (!aDS.IsNull() && aDS->IsAdvancedSelectionEnabled())
    return myMeshOwners;
  if (IsElements)
  {
    if (myElementOwners.IsEmpty())
    {
      AddToMap(myElementOwners, my0DOwners);
      AddToMap(myElementOwners, myLinkOwners);
      AddToMap(myElementOwners, myFaceOwners);
      AddToMap(myElementOwners, myVolumeOwners);
    }
    return myElementOwners;
  }
  else
    return myNodeOwners;
}

//=================================================================================================

bool MeshVS_Mesh::IsHiddenElem(const int theID) const
{
  return !myHiddenElements.IsNull() && myHiddenElements->Map().Contains(theID);
}

//=================================================================================================

bool MeshVS_Mesh::IsHiddenNode(const int theID) const
{
  // note that by default all nodes are hidden
  return myHiddenNodes.IsNull() || myHiddenNodes->Map().Contains(theID);
}

//=================================================================================================

occ::handle<MeshVS_Drawer> MeshVS_Mesh::GetDrawer() const
{
  return myCurrentDrawer;
}

//=================================================================================================

void MeshVS_Mesh::SetDrawer(const occ::handle<MeshVS_Drawer>& aDrawer)
{
  myCurrentDrawer = aDrawer;
}

//=================================================================================================

occ::handle<MeshVS_DataSource> MeshVS_Mesh::GetDataSource() const
{
  return myDataSource;
}

//=================================================================================================

void MeshVS_Mesh::SetDataSource(const occ::handle<MeshVS_DataSource>& theDataSource)
{
  myDataSource = theDataSource;
}

//=================================================================================================

void MeshVS_Mesh::HilightSelected(
  const occ::handle<PrsMgr_PresentationManager>&                  thePM,
  const NCollection_Sequence<occ::handle<SelectMgr_EntityOwner>>& theOwners)
{
  if (myHilighter.IsNull())
    return;

  //  if ( mySelectionPrs.IsNull() )
  //    mySelectionPrs = new Prs3d_Presentation ( thePM->StructureManager() );

  // new functionality

  occ::handle<Prs3d_Presentation> aSelectionPrs;

  aSelectionPrs = GetSelectPresentation(thePM);

  if (HasPresentation())
    aSelectionPrs->SetTransformPersistence(Presentation()->TransformPersistence());
  //----------------

  //   It is very important to call this parent method, because it check whether
  // mySelectionPrs is created and if not, create it.

#ifdef OCCT_DEBUG
  OSD_Timer gTimer;
  gTimer.Reset();
  gTimer.Start();
#endif

  int len = theOwners.Length(), i;

  occ::handle<MeshVS_MeshEntityOwner> anOwner;
  TColStd_PackedMapOfInteger          aSelNodes, aSelElements;

  for (i = 1; i <= len; i++)
  {
    if (theOwners.Value(i) == GlobalSelOwner())
    {
      const int                        aHiMode = HasHilightMode() ? HilightMode() : 0;
      const occ::handle<Prs3d_Drawer>& aSelStyle =
        !HilightAttributes().IsNull() ? HilightAttributes() : GetContext()->SelectionStyle();
      thePM->Color(this, aSelStyle, aHiMode);
      continue;
    }
    anOwner = occ::down_cast<MeshVS_MeshEntityOwner>(theOwners.Value(i));
    if (!anOwner.IsNull())
    {
      // nkv: add support of mesh groups
      if (anOwner->IsGroup())
      {
        MeshVS_EntityType          aGroupType;
        TColStd_PackedMapOfInteger aGroupMap;
        if (GetDataSource()->GetGroup(anOwner->ID(), aGroupType, aGroupMap))
        {
          if (aGroupType == MeshVS_ET_Node)
          {
            for (TColStd_PackedMapOfInteger::Iterator anIt(aGroupMap); anIt.More(); anIt.Next())
              if (IsSelectableNode /*!IsHiddenNode*/ (anIt.Key()))
                aSelNodes.Add(anIt.Key());
          }
          else
          {
            for (TColStd_PackedMapOfInteger::Iterator anIt(aGroupMap); anIt.More(); anIt.Next())
              if (IsSelectableElem /*!IsHiddenElem*/ (anIt.Key()))
                aSelElements.Add(anIt.Key());
          }
        }
      }
      else
      {
        if (anOwner->Type() == MeshVS_ET_Node)
          aSelNodes.Add(anOwner->ID());
        else
          aSelElements.Add(anOwner->ID());
      }
    }
    else if (GetDataSource()->IsAdvancedSelectionEnabled())
    {
      occ::handle<MeshVS_MeshOwner> aMeshOwner =
        occ::down_cast<MeshVS_MeshOwner>(theOwners.Value(i));
      if (!aMeshOwner.IsNull())
      {
        occ::handle<TColStd_HPackedMapOfInteger> aNodes = aMeshOwner->GetSelectedNodes();
        occ::handle<TColStd_HPackedMapOfInteger> aElems = aMeshOwner->GetSelectedElements();
        if (!aNodes.IsNull())
          aSelNodes.Assign(aNodes->Map());
        if (!aElems.IsNull())
          aSelElements.Assign(aElems->Map());
      }
    }
    // agv    else if( theOwners.Value ( i )==myWholeMeshOwner )
    else if (IsWholeMeshOwner(theOwners.Value(i)))
    {
      TColStd_PackedMapOfInteger::Iterator anIt(GetDataSource()->GetAllNodes());
      for (; anIt.More(); anIt.Next())
        if (!IsHiddenNode(anIt.Key()))
          aSelNodes.Add(anIt.Key());

      anIt = TColStd_PackedMapOfInteger::Iterator(GetDataSource()->GetAllElements());
      for (; anIt.More(); anIt.Next())
        if (!IsHiddenElem(anIt.Key()))
          aSelElements.Add(anIt.Key());

      break;
    }
  }

  bool IsNeedToRedisplay = false;

  aSelectionPrs->Clear();

  myHilighter->SetDrawer(mySelectionDrawer);

  if (aSelNodes.Extent() > 0)
  {
    TColStd_PackedMapOfInteger tmp;
    myHilighter->Build(aSelectionPrs, aSelNodes, tmp, false, MeshVS_DMF_SelectionPrs);
  }
  if (aSelElements.Extent() > 0)
  {
    TColStd_PackedMapOfInteger tmp;
    myHilighter->Build(aSelectionPrs, aSelElements, tmp, true, MeshVS_DMF_SelectionPrs);
  }

  myHilighter->SetDrawer(nullptr);

  IsNeedToRedisplay = true;

  aSelectionPrs->SetZLayer(Graphic3d_ZLayerId_Top);

  if (IsNeedToRedisplay)
  {
    aSelectionPrs->SetDisplayPriority(Graphic3d_DisplayPriority_Highlight);
    aSelectionPrs->Display();
  }

#ifdef OCCT_DEBUG
  double sec, cpu;
  int    min, hour;

  gTimer.Show(sec, min, hour, cpu);
  std::cout << "HilightSelected : " << std::endl;
  std::cout << aSelNodes.Extent() << " nodes " << std::endl;
  std::cout << aSelElements.Extent() << " elements " << std::endl;
  std::cout << "Time : " << sec << " sec" << std::endl;
  std::cout << "CPU time : " << cpu << " sec" << std::endl << std::endl;
  gTimer.Stop();
#endif
}

//=================================================================================================

void MeshVS_Mesh::HilightOwnerWithColor(const occ::handle<PrsMgr_PresentationManager>& thePM,
                                        const occ::handle<Prs3d_Drawer>&               theStyle,
                                        const occ::handle<SelectMgr_EntityOwner>&      theOwner)
{
  if (theOwner.IsNull())
    return;

  const Quantity_Color& aColor = theStyle->Color();
  if (theOwner == GlobalSelOwner())
  {
    int aHiMode = HasHilightMode() ? HilightMode() : 0;
    thePM->Color(this, theStyle, aHiMode, nullptr, Graphic3d_ZLayerId_Top);
    return;
  }

  if (myHilighter.IsNull())
    return;

  occ::handle<Prs3d_Presentation> aHilightPrs;
  aHilightPrs = GetHilightPresentation(thePM);

  aHilightPrs->Clear();

  // new functionality
  if (HasPresentation())
    aHilightPrs->SetTransformPersistence(Presentation()->TransformPersistence());
  //----------------

  const bool isMeshEntityOwner = theOwner->IsKind(STANDARD_TYPE(MeshVS_MeshEntityOwner));
  const bool isWholeMeshOwner =
    // agv    !Owner.IsNull() && Owner==myWholeMeshOwner;
    IsWholeMeshOwner(theOwner);

  int aDispMode = MeshVS_DMF_Shading;
  if (HasDisplayMode() && (DisplayMode() & MeshVS_DMF_OCCMask) > MeshVS_DMF_WireFrame)
    aDispMode = (DisplayMode() & MeshVS_DMF_OCCMask);
  // It because we draw hilighted owners only in shading or shrink (not in wireframe)

  myHilightDrawer->SetColor(MeshVS_DA_InteriorColor, aColor);
  myHilightDrawer->SetColor(MeshVS_DA_BackInteriorColor, aColor);
  myHilightDrawer->SetColor(MeshVS_DA_EdgeColor, aColor);
  myHilightDrawer->SetColor(MeshVS_DA_BeamColor, aColor);
  myHilightDrawer->SetColor(MeshVS_DA_MarkerColor, aColor);
  myHilighter->SetDrawer(myHilightDrawer);

  if (isMeshEntityOwner)
  {
    occ::handle<MeshVS_MeshEntityOwner> theAISOwner =
      occ::down_cast<MeshVS_MeshEntityOwner>(theOwner);
    MeshVS_EntityType aType = theAISOwner->Type();
    int               anID  = theAISOwner->ID();

    if (theAISOwner->IsGroup())
    {
      MeshVS_EntityType          aGroupType;
      TColStd_PackedMapOfInteger aGroupMap;
      if (myDataSource->GetGroup(anID, aGroupType, aGroupMap))
      {
        TColStd_PackedMapOfInteger tmp;
        myHilighter->Build(aHilightPrs,
                           aGroupMap,
                           tmp,
                           aType != MeshVS_ET_Node,
                           aDispMode | MeshVS_DMF_HilightPrs);
      }
    }
    else
    {
      TColStd_PackedMapOfInteger anOne, tmp;
      anOne.Add(anID);
      myHilighter->Build(aHilightPrs,
                         anOne,
                         tmp,
                         aType != MeshVS_ET_Node,
                         aDispMode | MeshVS_DMF_HilightPrs);
    }
  }
  else if (isWholeMeshOwner)
  {
    if (!GetDataSource().IsNull())
    {
      TColStd_PackedMapOfInteger tmp;
      myHilighter->Build(aHilightPrs,
                         GetDataSource()->GetAllElements(),
                         tmp,
                         true,
                         MeshVS_DMF_WireFrame);
    }
  }
  else
  {
    occ::handle<MeshVS_MeshOwner> aMeshOwner = occ::down_cast<MeshVS_MeshOwner>(theOwner);
    if (!aMeshOwner.IsNull())
    {
      occ::handle<TColStd_HPackedMapOfInteger> aNodes = aMeshOwner->GetDetectedNodes();
      occ::handle<TColStd_HPackedMapOfInteger> aElems = aMeshOwner->GetDetectedElements();
      // hilight detected entities
      if (!aNodes.IsNull())
      {
        TColStd_PackedMapOfInteger tmp;
        myHilighter->Build(aHilightPrs,
                           aNodes->Map(),
                           tmp,
                           false,
                           aDispMode | MeshVS_DMF_HilightPrs);
      }
      if (!aElems.IsNull())
      {
        TColStd_PackedMapOfInteger tmp;
        myHilighter->Build(aHilightPrs,
                           aElems->Map(),
                           tmp,
                           true,
                           aDispMode | MeshVS_DMF_HilightPrs);
      }
    }
  }

  aHilightPrs->SetZLayer(Graphic3d_ZLayerId_Topmost);

  if (thePM->IsImmediateModeOn())
  {
    thePM->AddToImmediateList(aHilightPrs);
  }
  myHilighter->SetDrawer(nullptr);
}

//=================================================================================================

void MeshVS_Mesh::ClearSelected()
{
  occ::handle<Prs3d_Presentation> aSelectionPrs = GetSelectPresentation(nullptr);
  if (!aSelectionPrs.IsNull())
    aSelectionPrs->Clear();
}

//=================================================================================================

occ::handle<MeshVS_PrsBuilder> MeshVS_Mesh::FindBuilder(const char* const theTypeName) const
{
  for (const occ::handle<MeshVS_PrsBuilder>& aBuilder : myBuilders)
  {
    if (aBuilder->IsKind(theTypeName))
    {
      return aBuilder;
    }
  }
  return nullptr;
}

occ::handle<MeshVS_PrsBuilder> MeshVS_Mesh::FindBuilder(
  const occ::handle<Standard_Type>& theType) const
{
  for (const occ::handle<MeshVS_PrsBuilder>& aBuilder : myBuilders)
  {
    if (aBuilder->IsKind(theType))
    {
      return aBuilder;
    }
  }
  return nullptr;
}

//=================================================================================================

void MeshVS_Mesh::SetHilighter(const occ::handle<MeshVS_PrsBuilder>& Builder)
{
  myHilighter = Builder;
}

//=================================================================================================

bool MeshVS_Mesh::SetHilighter(const int Index)
{
  occ::handle<MeshVS_PrsBuilder> aBuild = GetBuilder(Index);
  bool                           aRes   = (!aBuild.IsNull());
  if (aRes)
    myHilighter = aBuild;
  return aRes;
}

//=================================================================================================

bool MeshVS_Mesh::SetHilighterById(const int Id)
{
  occ::handle<MeshVS_PrsBuilder> aBuild = GetBuilderById(Id);
  bool                           aRes   = (!aBuild.IsNull());
  if (aRes)
    myHilighter = aBuild;
  return aRes;
}

//=================================================================================================

occ::handle<MeshVS_PrsBuilder> MeshVS_Mesh::GetHilighter() const
{
  return myHilighter;
}

//=================================================================================================

bool MeshVS_Mesh::IsSelectableElem(const int ID) const
{
  return !IsHiddenElem(ID);
}

//=================================================================================================

bool MeshVS_Mesh::IsSelectableNode(const int ID) const
{
  return mySelectableNodes.IsNull() ? !IsHiddenNode(ID) : mySelectableNodes->Map().Contains(ID);
}

//=================================================================================================

const occ::handle<TColStd_HPackedMapOfInteger>& MeshVS_Mesh::GetSelectableNodes() const
{
  return mySelectableNodes;
}

//=================================================================================================

void MeshVS_Mesh::SetSelectableNodes(const occ::handle<TColStd_HPackedMapOfInteger>& Ids)
{
  mySelectableNodes = Ids;
}

//=================================================================================================

void MeshVS_Mesh::UpdateSelectableNodes()
{
  mySelectableNodes = new TColStd_HPackedMapOfInteger;

  int                            aMaxFaceNodes;
  occ::handle<MeshVS_DataSource> aSource = GetDataSource();
  if (aSource.IsNull() || myCurrentDrawer.IsNull()
      || !myCurrentDrawer->GetInteger(MeshVS_DA_MaxFaceNodes, aMaxFaceNodes) || aMaxFaceNodes <= 0)
    return;

  // all non-hidden nodes are selectable;
  // by default (i.e. if myHiddenNodes.IsNull()) all nodes are hidden
  if (!myHiddenNodes.IsNull())
  {
    NCollection_PackedMapAlgo::Subtraction(mySelectableNodes->ChangeMap(),
                                           aSource->GetAllNodes(),
                                           myHiddenNodes->Map());
  }

  // add all nodes belonging to non-hidden elements
  TColStd_PackedMapOfInteger::Iterator anIter(aSource->GetAllElements());
  for (; anIter.More(); anIter.Next())
  {
    int aKey = anIter.Key();
    if (IsHiddenElem(aKey))
      continue;

    MeshVS_Buffer           aNodesBuf(aMaxFaceNodes * sizeof(int));
    NCollection_Array1<int> aNodes(aNodesBuf, 1, aMaxFaceNodes);
    int                     NbNodes;
    if (!aSource->GetNodesByElement(aKey, aNodes, NbNodes))
      continue;
    for (int i = 1; i <= NbNodes; i++)
      mySelectableNodes->ChangeMap().Add(aNodes(i));
  }
}

//=================================================================================================

MeshVS_MeshSelectionMethod MeshVS_Mesh::GetMeshSelMethod() const
{
  return mySelectionMethod;
}

//=================================================================================================

void MeshVS_Mesh::SetMeshSelMethod(const MeshVS_MeshSelectionMethod M)
{
  mySelectionMethod = M;
}

//=================================================================================================

bool MeshVS_Mesh::IsWholeMeshOwner(const occ::handle<SelectMgr_EntityOwner>& theOwn) const
{
  return theOwn.IsNull() ? false : (theOwn == myWholeMeshOwner);
}
