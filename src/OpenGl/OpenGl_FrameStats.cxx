// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <OpenGl_FrameStats.hxx>

#include <OpenGl_GlCore20.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_FrameStats, Graphic3d_FrameStats)

namespace
{
  //! Return estimated data size.
  static Standard_Size estimatedDataSize (const Handle(OpenGl_Resource)& theRes)
  {
    return !theRes.IsNull() ? theRes->EstimatedDataSize() : 0;
  }
}

// =======================================================================
// function : OpenGl_FrameStats
// purpose  :
// =======================================================================
OpenGl_FrameStats::OpenGl_FrameStats()
{
  //
}

// =======================================================================
// function : ~OpenGl_FrameStats
// purpose  :
// =======================================================================
OpenGl_FrameStats::~OpenGl_FrameStats()
{
  //
}

// =======================================================================
// function : IsFrameUpdated
// purpose  :
// =======================================================================
bool OpenGl_FrameStats::IsFrameUpdated (Handle(OpenGl_FrameStats)& thePrev) const
{
  const Graphic3d_FrameStatsData& aFrame = LastDataFrame();
  if (thePrev.IsNull())
  {
    thePrev = new OpenGl_FrameStats();
  }
  // check just a couple of major counters
  else if (myLastFrameIndex == thePrev->myLastFrameIndex
        && Abs (aFrame.FrameRate()    - thePrev->myCountersTmp.FrameRate())    <= 0.001
        && Abs (aFrame.FrameRateCpu() - thePrev->myCountersTmp.FrameRateCpu()) <= 0.001
        && Abs (aFrame.ImmediateFrameRate()    - thePrev->myCountersTmp.ImmediateFrameRate())    <= 0.001
        && Abs (aFrame.ImmediateFrameRateCpu() - thePrev->myCountersTmp.ImmediateFrameRateCpu()) <= 0.001
        && aFrame[Graphic3d_FrameStatsCounter_NbLayers]           == thePrev->myCountersTmp[Graphic3d_FrameStatsCounter_NbLayers]
        && aFrame[Graphic3d_FrameStatsCounter_NbLayersNotCulled]  == thePrev->myCountersTmp[Graphic3d_FrameStatsCounter_NbLayersNotCulled]
        && aFrame[Graphic3d_FrameStatsCounter_NbStructs]          == thePrev->myCountersTmp[Graphic3d_FrameStatsCounter_NbStructs]
        && aFrame[Graphic3d_FrameStatsCounter_NbStructsNotCulled] == thePrev->myCountersTmp[Graphic3d_FrameStatsCounter_NbStructsNotCulled])
  {
    return false;
  }

  thePrev->myLastFrameIndex = myLastFrameIndex;
  thePrev->myCountersTmp = aFrame;
  return true;
}

// =======================================================================
// function : updateStatistics
// purpose  :
// =======================================================================
void OpenGl_FrameStats::updateStatistics (const Handle(Graphic3d_CView)& theView,
                                          bool theIsImmediateOnly)
{
  const OpenGl_View* aView = dynamic_cast<const OpenGl_View*> (theView.get());
  if (aView == NULL)
  {
    myCounters.SetValue (myLastFrameIndex, myCountersTmp);
    myCountersTmp.Reset();
    return;
  }

  const Graphic3d_RenderingParams::PerfCounters aBits = theView->RenderingParams().CollectedStats;
  const Standard_Boolean toCountMem     = (aBits & Graphic3d_RenderingParams::PerfCounters_EstimMem)  != 0;
  const Standard_Boolean toCountTris    = (aBits & Graphic3d_RenderingParams::PerfCounters_Triangles) != 0
                                       || (aBits & Graphic3d_RenderingParams::PerfCounters_Lines)     != 0
                                       || (aBits & Graphic3d_RenderingParams::PerfCounters_Points)    != 0;
  const Standard_Boolean toCountElems   = (aBits & Graphic3d_RenderingParams::PerfCounters_GroupArrays) != 0 || toCountTris || toCountMem;
  const Standard_Boolean toCountGroups  = (aBits & Graphic3d_RenderingParams::PerfCounters_Groups)      != 0 || toCountElems;
  const Standard_Boolean toCountStructs = (aBits & Graphic3d_RenderingParams::PerfCounters_Structures)  != 0
                                       || (aBits & Graphic3d_RenderingParams::PerfCounters_Layers)      != 0 || toCountGroups;

  myCountersTmp[Graphic3d_FrameStatsCounter_NbLayers] = aView->LayerList().Layers().Size();
  if (toCountStructs
   || (aBits & Graphic3d_RenderingParams::PerfCounters_Layers)    != 0)
  {
    const Standard_Integer aViewId = aView->Identification();
    for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter (aView->LayerList().Layers()); aLayerIter.More(); aLayerIter.Next())
    {
      const Handle(OpenGl_Layer)& aLayer = aLayerIter.Value();
      myCountersTmp[Graphic3d_FrameStatsCounter_NbStructs] += aLayer->NbStructures();
      if (theIsImmediateOnly && !aLayer->LayerSettings().IsImmediate())
      {
        continue;
      }

      if (!aLayer->IsCulled())
      {
        ++myCountersTmp[Graphic3d_FrameStatsCounter_NbLayersNotCulled];
      }
      myCountersTmp[Graphic3d_FrameStatsCounter_NbStructsNotCulled] += aLayer->NbStructuresNotCulled();
      if (toCountGroups)
      {
        updateStructures (aViewId, aLayer->CullableStructuresBVH().Structures(), toCountElems, toCountTris, toCountMem);
        updateStructures (aViewId, aLayer->CullableTrsfPersStructuresBVH().Structures(), toCountElems, toCountTris, toCountMem);
        updateStructures (aViewId, aLayer->NonCullableStructures(), toCountElems, toCountTris, toCountMem);
      }
    }
  }
  if (toCountMem)
  {
    for (OpenGl_Context::OpenGl_ResourcesMap::Iterator aResIter (aView->GlWindow()->GetGlContext()->SharedResources());
         aResIter.More(); aResIter.Next())
    {
      myCountersTmp[Graphic3d_FrameStatsCounter_EstimatedBytesTextures] += aResIter.Value()->EstimatedDataSize();
    }

    {
      Standard_Size& aMemFbos = myCountersTmp[Graphic3d_FrameStatsCounter_EstimatedBytesFbos];
      // main FBOs
      aMemFbos += estimatedDataSize (aView->myMainSceneFbos[0]);
      aMemFbos += estimatedDataSize (aView->myMainSceneFbos[1]);
      aMemFbos += estimatedDataSize (aView->myImmediateSceneFbos[0]);
      aMemFbos += estimatedDataSize (aView->myImmediateSceneFbos[1]);
      // OIT FBOs
      aMemFbos += estimatedDataSize (aView->myMainSceneFbosOit[0]);
      aMemFbos += estimatedDataSize (aView->myMainSceneFbosOit[1]);
      aMemFbos += estimatedDataSize (aView->myImmediateSceneFbosOit[0]);
      aMemFbos += estimatedDataSize (aView->myImmediateSceneFbosOit[1]);
      // dump FBO
      aMemFbos += estimatedDataSize (aView->myFBO);
      // RayTracing FBO
      aMemFbos += estimatedDataSize (aView->myOpenGlFBO);
      aMemFbos += estimatedDataSize (aView->myOpenGlFBO2);
      aMemFbos += estimatedDataSize (aView->myRaytraceFBO1[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceFBO1[1]);
      aMemFbos += estimatedDataSize (aView->myRaytraceFBO2[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceFBO2[1]);
      // also RayTracing
      aMemFbos += estimatedDataSize (aView->myRaytraceOutputTexture[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceOutputTexture[1]);
      aMemFbos += estimatedDataSize (aView->myRaytraceVisualErrorTexture[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceVisualErrorTexture[1]);
      aMemFbos += estimatedDataSize (aView->myRaytraceTileOffsetsTexture[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceTileOffsetsTexture[1]);
      aMemFbos += estimatedDataSize (aView->myRaytraceTileSamplesTexture[0]);
      aMemFbos += estimatedDataSize (aView->myRaytraceTileSamplesTexture[1]);
    }
    {
      // Ray Tracing geometry
      Standard_Size& aMemGeom = myCountersTmp[Graphic3d_FrameStatsCounter_EstimatedBytesGeom];
      aMemGeom += estimatedDataSize (aView->mySceneNodeInfoTexture);
      aMemGeom += estimatedDataSize (aView->mySceneMinPointTexture);
      aMemGeom += estimatedDataSize (aView->mySceneMaxPointTexture);
      aMemGeom += estimatedDataSize (aView->mySceneTransformTexture);
      aMemGeom += estimatedDataSize (aView->myGeometryVertexTexture);
      aMemGeom += estimatedDataSize (aView->myGeometryNormalTexture);
      aMemGeom += estimatedDataSize (aView->myGeometryTexCrdTexture);
      aMemGeom += estimatedDataSize (aView->myGeometryTriangTexture);
      aMemGeom += estimatedDataSize (aView->myRaytraceMaterialTexture);
      aMemGeom += estimatedDataSize (aView->myRaytraceLightSrcTexture);
    }
  }
}

// =======================================================================
// function : updateStructures
// purpose  :
// =======================================================================
void OpenGl_FrameStats::updateStructures (Standard_Integer theViewId,
                                          const NCollection_IndexedMap<const Graphic3d_CStructure*>& theStructures,
                                          Standard_Boolean theToCountElems,
                                          Standard_Boolean theToCountTris,
                                          Standard_Boolean theToCountMem)
{
  for (OpenGl_Structure::StructIterator aStructIter (theStructures); aStructIter.More(); aStructIter.Next())
  {
    const OpenGl_Structure* aStruct = aStructIter.Value();
    const bool isStructHidden = aStruct->IsCulled()
                            || !aStruct->IsVisible (theViewId);
    for (; aStruct != NULL; aStruct = aStruct->InstancedStructure())
    {
      if (isStructHidden)
      {
        if (theToCountMem)
        {
          for (OpenGl_Structure::GroupIterator aGroupIter (aStruct->Groups()); aGroupIter.More(); aGroupIter.Next())
          {
            const OpenGl_Group* aGroup = aGroupIter.Value();
            for (const OpenGl_ElementNode* aNodeIter = aGroup->FirstNode(); aNodeIter != NULL; aNodeIter = aNodeIter->next)
            {
              aNodeIter->elem->UpdateMemStats (myCountersTmp);
            }
          }
        }
        continue;
      }

      myCountersTmp[Graphic3d_FrameStatsCounter_NbGroupsNotCulled] += aStruct->Groups().Size();
      if (!theToCountElems)
      {
        continue;
      }

      for (OpenGl_Structure::GroupIterator aGroupIter (aStruct->Groups()); aGroupIter.More(); aGroupIter.Next())
      {
        const OpenGl_Group* aGroup = aGroupIter.Value();
        for (const OpenGl_ElementNode* aNodeIter = aGroup->FirstNode(); aNodeIter != NULL; aNodeIter = aNodeIter->next)
        {
          if (theToCountMem)
          {
            aNodeIter->elem->UpdateMemStats (myCountersTmp);
          }
          aNodeIter->elem->UpdateDrawStats (myCountersTmp, theToCountTris);
        }
      }
    }
  }
}
