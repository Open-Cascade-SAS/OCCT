// Created on: 2012-02-02
// Created by: Anton POLETAEV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#include <OpenGl_GlCore15.hxx>

#include <BVH_LinearBuilder.hxx>
#include <OpenGl_DepthPeeling.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_LayerList.hxx>
#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShadowMap.hxx>
#include <OpenGl_VertexBuffer.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

namespace
{
//! Auxiliary class extending sequence iterator with index.
class OpenGl_IndexedLayerIterator : public NCollection_List<Handle(Graphic3d_Layer)>::Iterator
{
public:
  //! Main constructor.
  OpenGl_IndexedLayerIterator(const NCollection_List<Handle(Graphic3d_Layer)>& theSeq)
      : NCollection_List<Handle(Graphic3d_Layer)>::Iterator(theSeq),
        myIndex(1)
  {
  }

  //! Return index of current position.
  Standard_Integer Index() const { return myIndex; }

  //! Move to the next position.
  void Next()
  {
    NCollection_List<Handle(Graphic3d_Layer)>::Iterator::Next();
    ++myIndex;
  }

private:
  Standard_Integer myIndex;
};

//! Iterator through layers with filter.
class OpenGl_FilteredIndexedLayerIterator
{
public:
  //! Main constructor.
  OpenGl_FilteredIndexedLayerIterator(const NCollection_List<Handle(Graphic3d_Layer)>& theSeq,
                                      Standard_Boolean   theToDrawImmediate,
                                      OpenGl_LayerFilter theFilterMode,
                                      Graphic3d_ZLayerId theLayersToProcess)
      : myIter(theSeq),
        myFilterMode(theFilterMode),
        myToDrawImmediate(theToDrawImmediate),
        myLayersToProcess(theLayersToProcess)
  {
    next();
  }

  //! Return true if iterator points to the valid value.
  bool More() const { return myIter.More(); }

  //! Return layer at current position.
  const OpenGl_Layer& Value() const { return *myIter.Value(); }

  //! Return index of current position.
  Standard_Integer Index() const { return myIter.Index(); }

  //! Go to the next item.
  void Next()
  {
    myIter.Next();
    next();
  }

private:
  //! Look for the nearest item passing filters.
  void next()
  {
    for (; myIter.More(); myIter.Next())
    {
      const Handle(Graphic3d_Layer)& aLayer = myIter.Value();
      if (aLayer->IsImmediate() != myToDrawImmediate)
      {
        continue;
      }

      switch (myFilterMode)
      {
        case OpenGl_LF_All: {
          if (aLayer->LayerId() >= myLayersToProcess)
          {
            return;
          }
          break;
        }
        case OpenGl_LF_Upper: {
          if (aLayer->LayerId() != Graphic3d_ZLayerId_BotOSD
              && (!aLayer->LayerSettings().IsRaytracable() || aLayer->IsImmediate()))
          {
            return;
          }
          break;
        }
        case OpenGl_LF_Bottom: {
          if (aLayer->LayerId() == Graphic3d_ZLayerId_BotOSD
              && !aLayer->LayerSettings().IsRaytracable())
          {
            return;
          }
          break;
        }
        case OpenGl_LF_Single: {
          if (aLayer->LayerId() == myLayersToProcess)
          {
            return;
          }
          break;
        }
        case OpenGl_LF_RayTracable: {
          if (aLayer->LayerSettings().IsRaytracable() && !aLayer->IsImmediate())
          {
            return;
          }
          break;
        }
      }
    }
  }

private:
  OpenGl_IndexedLayerIterator myIter;
  OpenGl_LayerFilter          myFilterMode;
  Standard_Boolean            myToDrawImmediate;
  Graphic3d_ZLayerId          myLayersToProcess;
};

static const Standard_Integer THE_DRAW_BUFFERS0[]   = {GL_COLOR_ATTACHMENT0};
static const Standard_Integer THE_DRAW_BUFFERS01[]  = {GL_COLOR_ATTACHMENT0,
                                                       GL_COLOR_ATTACHMENT0 + 1};
static const Standard_Integer THE_DRAW_BUFFERS012[] = {GL_COLOR_ATTACHMENT0,
                                                       GL_COLOR_ATTACHMENT0 + 1,
                                                       GL_COLOR_ATTACHMENT0 + 2};
static const float            THE_DEPTH_CLEAR_VALUE = -1e15f;
} // namespace

struct OpenGl_GlobalLayerSettings
{
  GLint     DepthFunc;
  GLboolean DepthMask;
};

//=================================================================================================

OpenGl_LayerList::OpenGl_LayerList()
    : myBVHBuilder(new BVH_LinearBuilder<Standard_Real, 3>(BVH_Constants_LeafNodeSizeSingle,
                                                           BVH_Constants_MaxTreeDepth)),
      myNbStructures(0),
      myImmediateNbStructures(0),
      myModifStateOfRaytraceable(0)
{
  //
}

//=================================================================================================

OpenGl_LayerList::~OpenGl_LayerList() {}

//=================================================================================================

void OpenGl_LayerList::SetFrustumCullingBVHBuilder(const Handle(BVH_Builder3d)& theBuilder)
{
  myBVHBuilder = theBuilder;
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers); aLayerIter.More();
       aLayerIter.Next())
  {
    aLayerIter.ChangeValue()->SetFrustumCullingBVHBuilder(theBuilder);
  }
}

//=================================================================================================

void OpenGl_LayerList::InsertLayerBefore(const Graphic3d_ZLayerId        theNewLayerId,
                                         const Graphic3d_ZLayerSettings& theSettings,
                                         const Graphic3d_ZLayerId        theLayerAfter)
{
  if (myLayerIds.IsBound(theNewLayerId))
  {
    return;
  }

  Handle(Graphic3d_Layer) aNewLayer = new Graphic3d_Layer(theNewLayerId, myBVHBuilder);
  aNewLayer->SetLayerSettings(theSettings);

  Handle(Graphic3d_Layer) anOtherLayer;
  if (theLayerAfter != Graphic3d_ZLayerId_UNKNOWN && myLayerIds.Find(theLayerAfter, anOtherLayer))
  {
    for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers);
         aLayerIter.More();
         aLayerIter.Next())
    {
      if (aLayerIter.Value() == anOtherLayer)
      {
        myLayers.InsertBefore(aNewLayer, aLayerIter);
        break;
      }
    }
  }
  else
  {
    myLayers.Prepend(aNewLayer);
  }

  myLayerIds.Bind(theNewLayerId, aNewLayer);
  myTransparentToProcess.Allocate(myLayers.Size());
}

//=================================================================================================

void OpenGl_LayerList::InsertLayerAfter(const Graphic3d_ZLayerId        theNewLayerId,
                                        const Graphic3d_ZLayerSettings& theSettings,
                                        const Graphic3d_ZLayerId        theLayerBefore)
{
  if (myLayerIds.IsBound(theNewLayerId))
  {
    return;
  }

  Handle(Graphic3d_Layer) aNewLayer = new Graphic3d_Layer(theNewLayerId, myBVHBuilder);
  aNewLayer->SetLayerSettings(theSettings);

  Handle(Graphic3d_Layer) anOtherLayer;
  if (theLayerBefore != Graphic3d_ZLayerId_UNKNOWN && myLayerIds.Find(theLayerBefore, anOtherLayer))
  {
    for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers);
         aLayerIter.More();
         aLayerIter.Next())
    {
      if (aLayerIter.Value() == anOtherLayer)
      {
        myLayers.InsertAfter(aNewLayer, aLayerIter);
        break;
      }
    }
  }
  else
  {
    myLayers.Append(aNewLayer);
  }

  myLayerIds.Bind(theNewLayerId, aNewLayer);
  myTransparentToProcess.Allocate(myLayers.Size());
}

//=================================================================================================

void OpenGl_LayerList::RemoveLayer(const Graphic3d_ZLayerId theLayerId)
{
  Handle(Graphic3d_Layer) aLayerToRemove;
  if (theLayerId <= 0 || !myLayerIds.Find(theLayerId, aLayerToRemove))
  {
    return;
  }

  // move all displayed structures to first layer
  myLayerIds.Find(Graphic3d_ZLayerId_Default)->Append(*aLayerToRemove);

  // remove layer
  myLayers.Remove(aLayerToRemove);
  myLayerIds.UnBind(theLayerId);

  myTransparentToProcess.Allocate(myLayers.Size());
}

//=================================================================================================

void OpenGl_LayerList::AddStructure(const OpenGl_Structure*         theStruct,
                                    const Graphic3d_ZLayerId        theLayerId,
                                    const Graphic3d_DisplayPriority thePriority,
                                    Standard_Boolean                isForChangePriority)
{
  // add structure to associated layer,
  // if layer doesn't exists, display structure in default layer
  const Handle(Graphic3d_Layer)* aLayerPtr = myLayerIds.Seek(theLayerId);
  const Handle(Graphic3d_Layer)& aLayer =
    aLayerPtr != NULL ? *aLayerPtr : myLayerIds.Find(Graphic3d_ZLayerId_Default);
  aLayer->Add(theStruct, thePriority, isForChangePriority);
  ++myNbStructures;
  if (aLayer->IsImmediate())
  {
    ++myImmediateNbStructures;
  }

  // Note: In ray-tracing mode we don't modify modification
  // state here. It is redundant, because the possible changes
  // will be handled in the loop for structures
}

//=================================================================================================

void OpenGl_LayerList::RemoveStructure(const OpenGl_Structure* theStructure)
{
  const Graphic3d_ZLayerId       aLayerId  = theStructure->ZLayer();
  const Handle(Graphic3d_Layer)* aLayerPtr = myLayerIds.Seek(aLayerId);
  const Handle(Graphic3d_Layer)& aLayer =
    aLayerPtr != NULL ? *aLayerPtr : myLayerIds.Find(Graphic3d_ZLayerId_Default);

  Graphic3d_DisplayPriority aPriority = Graphic3d_DisplayPriority_INVALID;

  // remove structure from associated list
  // if the structure is not found there,
  // scan through layers and remove it
  if (aLayer->Remove(theStructure, aPriority))
  {
    --myNbStructures;
    if (aLayer->IsImmediate())
    {
      --myImmediateNbStructures;
    }

    if (aLayer->LayerSettings().IsRaytracable() && theStructure->IsRaytracable())
    {
      ++myModifStateOfRaytraceable;
    }

    return;
  }

  // scan through layers and remove it
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers); aLayerIter.More();
       aLayerIter.Next())
  {
    const Handle(Graphic3d_Layer)& aLayerEx = aLayerIter.ChangeValue();
    if (aLayerEx == aLayer)
    {
      continue;
    }

    if (aLayerEx->Remove(theStructure, aPriority))
    {
      --myNbStructures;
      if (aLayerEx->IsImmediate())
      {
        --myImmediateNbStructures;
      }

      if (aLayerEx->LayerSettings().IsRaytracable() && theStructure->IsRaytracable())
      {
        ++myModifStateOfRaytraceable;
      }
      return;
    }
  }
}

//=================================================================================================

void OpenGl_LayerList::InvalidateBVHData(const Graphic3d_ZLayerId theLayerId)
{
  const Handle(Graphic3d_Layer)* aLayerPtr = myLayerIds.Seek(theLayerId);
  const Handle(Graphic3d_Layer)& aLayer =
    aLayerPtr != NULL ? *aLayerPtr : myLayerIds.Find(Graphic3d_ZLayerId_Default);
  aLayer->InvalidateBVHData();
}

//=================================================================================================

void OpenGl_LayerList::ChangeLayer(const OpenGl_Structure*  theStructure,
                                   const Graphic3d_ZLayerId theOldLayerId,
                                   const Graphic3d_ZLayerId theNewLayerId)
{
  const Handle(Graphic3d_Layer)* aLayerPtr = myLayerIds.Seek(theOldLayerId);
  const Handle(Graphic3d_Layer)& aLayer =
    aLayerPtr != NULL ? *aLayerPtr : myLayerIds.Find(Graphic3d_ZLayerId_Default);

  Graphic3d_DisplayPriority aPriority = Graphic3d_DisplayPriority_INVALID;

  // take priority and remove structure from list found by <theOldLayerId>
  // if the structure is not found there, scan through all other layers
  if (aLayer->Remove(theStructure, aPriority, Standard_False))
  {
    if (aLayer->LayerSettings().IsRaytracable() && !aLayer->LayerSettings().IsImmediate()
        && theStructure->IsRaytracable())
    {
      ++myModifStateOfRaytraceable;
    }

    --myNbStructures;
    if (aLayer->IsImmediate())
    {
      --myImmediateNbStructures;
    }

    // isForChangePriority should be Standard_False below, because we want
    // the BVH tree in the target layer to be updated with theStructure
    AddStructure(theStructure, theNewLayerId, aPriority);
    return;
  }

  // scan through layers and remove it
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers); aLayerIter.More();
       aLayerIter.Next())
  {
    const Handle(OpenGl_Layer)& aLayerEx = aLayerIter.ChangeValue();
    if (aLayerEx == aLayer)
    {
      continue;
    }

    // try to remove structure and get priority value from this layer
    if (aLayerEx->Remove(theStructure, aPriority, Standard_True))
    {
      if (aLayerEx->LayerSettings().IsRaytracable() && !aLayerEx->LayerSettings().IsImmediate()
          && theStructure->IsRaytracable())
      {
        ++myModifStateOfRaytraceable;
      }

      --myNbStructures;
      if (aLayerEx->IsImmediate())
      {
        --myImmediateNbStructures;
      }

      // isForChangePriority should be Standard_False below, because we want
      // the BVH tree in the target layer to be updated with theStructure
      AddStructure(theStructure, theNewLayerId, aPriority);
      return;
    }
  }
}

//=================================================================================================

void OpenGl_LayerList::ChangePriority(const OpenGl_Structure*         theStructure,
                                      const Graphic3d_ZLayerId        theLayerId,
                                      const Graphic3d_DisplayPriority theNewPriority)
{
  const Handle(Graphic3d_Layer)* aLayerPtr = myLayerIds.Seek(theLayerId);
  const Handle(Graphic3d_Layer)& aLayer =
    aLayerPtr != NULL ? *aLayerPtr : myLayerIds.Find(Graphic3d_ZLayerId_Default);

  Graphic3d_DisplayPriority anOldPriority = Graphic3d_DisplayPriority_INVALID;
  if (aLayer->Remove(theStructure, anOldPriority, Standard_True))
  {
    --myNbStructures;
    if (aLayer->IsImmediate())
    {
      --myImmediateNbStructures;
    }

    AddStructure(theStructure, theLayerId, theNewPriority, Standard_True);
    return;
  }

  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers); aLayerIter.More();
       aLayerIter.Next())
  {
    const Handle(OpenGl_Layer)& aLayerEx = aLayerIter.ChangeValue();
    if (aLayerEx == aLayer)
    {
      continue;
    }

    if (aLayerEx->Remove(theStructure, anOldPriority, Standard_True))
    {
      --myNbStructures;
      if (aLayerEx->IsImmediate())
      {
        --myImmediateNbStructures;
      }

      AddStructure(theStructure, theLayerId, theNewPriority, Standard_True);
      return;
    }
  }
}

//=================================================================================================

void OpenGl_LayerList::SetLayerSettings(const Graphic3d_ZLayerId        theLayerId,
                                        const Graphic3d_ZLayerSettings& theSettings)
{
  Graphic3d_Layer& aLayer = Layer(theLayerId);
  if (aLayer.LayerSettings().IsRaytracable() != theSettings.IsRaytracable()
      && aLayer.NbStructures() != 0)
  {
    ++myModifStateOfRaytraceable;
  }
  if (aLayer.LayerSettings().IsImmediate() != theSettings.IsImmediate())
  {
    if (theSettings.IsImmediate())
    {
      myImmediateNbStructures += aLayer.NbStructures();
    }
    else
    {
      myImmediateNbStructures -= aLayer.NbStructures();
    }
  }
  aLayer.SetLayerSettings(theSettings);
}

//=================================================================================================

void OpenGl_LayerList::UpdateCulling(const Handle(OpenGl_Workspace)& theWorkspace,
                                     const Standard_Boolean          theToDrawImmediate)
{
  const Handle(OpenGl_FrameStats)& aStats = theWorkspace->GetGlContext()->FrameStats();
  OSD_Timer& aTimer = aStats->ActiveDataFrame().ChangeTimer(Graphic3d_FrameStatsTimer_CpuCulling);
  aTimer.Start();

  const Standard_Integer       aViewId   = theWorkspace->View()->Identification();
  const Graphic3d_CullingTool& aSelector = theWorkspace->View()->BVHTreeSelector();
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers); aLayerIter.More();
       aLayerIter.Next())
  {
    const Handle(Graphic3d_Layer)& aLayer = aLayerIter.ChangeValue();
    if (aLayer->IsImmediate() != theToDrawImmediate)
    {
      continue;
    }

    aLayer->UpdateCulling(aViewId,
                          aSelector,
                          theWorkspace->View()->RenderingParams().FrustumCullingState);
  }

  aTimer.Stop();
  aStats->ActiveDataFrame()[Graphic3d_FrameStatsTimer_CpuCulling] = aTimer.UserTimeCPU();
}

//=================================================================================================

void OpenGl_LayerList::renderLayer(const Handle(OpenGl_Workspace)&   theWorkspace,
                                   const OpenGl_GlobalLayerSettings& theDefaultSettings,
                                   const Graphic3d_Layer&            theLayer) const
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  const Graphic3d_ZLayerSettings& aLayerSettings = theLayer.LayerSettings();
  // aLayerSettings.ToClearDepth() is handled outside

  // handle depth test
  if (aLayerSettings.ToEnableDepthTest())
  {
    // assuming depth test is enabled by default
    aCtx->core11fwd->glDepthFunc(theDefaultSettings.DepthFunc);
  }
  else
  {
    aCtx->core11fwd->glDepthFunc(GL_ALWAYS);
  }

  // save environment texture
  Handle(OpenGl_TextureSet) anEnvironmentTexture = theWorkspace->EnvironmentTexture();
  if (!aLayerSettings.UseEnvironmentTexture())
  {
    theWorkspace->SetEnvironmentTexture(Handle(OpenGl_TextureSet)());
  }

  // handle depth offset
  const Graphic3d_PolygonOffset anAppliedOffsetParams =
    theWorkspace->SetDefaultPolygonOffset(aLayerSettings.PolygonOffset());

  // handle depth write
  theWorkspace->UseDepthWrite() =
    aLayerSettings.ToEnableDepthWrite() && theDefaultSettings.DepthMask == GL_TRUE;
  aCtx->core11fwd->glDepthMask(theWorkspace->UseDepthWrite() ? GL_TRUE : GL_FALSE);

  const Standard_Boolean              hasLocalCS  = !aLayerSettings.OriginTransformation().IsNull();
  const Handle(OpenGl_ShaderManager)& aManager    = aCtx->ShaderManager();
  Handle(Graphic3d_LightSet)          aLightsBack = aManager->LightSourceState().LightSources();
  Handle(OpenGl_ShadowMapArray)       aShadowMaps = aManager->LightSourceState().ShadowMaps();
  const bool hasOwnLights = aCtx->ColorMask() && !aLayerSettings.Lights().IsNull()
                            && aLayerSettings.Lights() != aLightsBack;
  if (hasOwnLights)
  {
    aLayerSettings.Lights()->UpdateRevision();
    aManager->UpdateLightSourceStateTo(aLayerSettings.Lights(),
                                       theWorkspace->View()->SpecIBLMapLevels(),
                                       Handle(OpenGl_ShadowMapArray)());
  }

  const Handle(Graphic3d_Camera)& aWorldCamera = aCtx->Camera();
  if (hasLocalCS)
  {
    // Apply local camera transformation.
    // The vertex position is computed by the following formula in GLSL program:
    //   gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;
    // where:
    //   occProjectionMatrix - matrix defining orthographic/perspective/stereographic projection
    //   occWorldViewMatrix  - world-view  matrix defining Camera position and orientation
    //   occModelWorldMatrix - model-world matrix defining Object transformation from local
    //   coordinate system to the world coordinate system occVertex           - input vertex
    //   position
    //
    // Since double precision is quite expensive on modern GPUs, and not available on old hardware,
    // all these values are passed with single float precision to the shader.
    // As result, single precision become insufficient for handling objects far from the world
    // origin.
    //
    // Several approaches can be used to solve precision issues:
    //  - [Broute force] migrate to double precision for all matrices and vertex position.
    //    This is too expensive for most hardware.
    //  - Store only translation part with double precision and pass it to GLSL program.
    //    This requires modified GLSL programs for computing transformation
    //    and extra packing mechanism for hardware not supporting double precision natively.
    //    This solution is less expensive then previous one.
    //  - Move translation part of occModelWorldMatrix into occWorldViewMatrix.
    //    The main idea here is that while moving Camera towards the object,
    //    Camera translation part and Object translation part will compensate each other
    //    to fit into single float precision.
    //    But this operation should be performed with double precision - this is why we are moving
    //    translation part of occModelWorldMatrix to occWorldViewMatrix.
    //
    // All approaches might be useful in different scenarios, but for the moment we consider the
    // last one as main scenario. Here we do the trick:
    //  - OpenGl_Layer defines the Local Origin, which is expected to be the center of objects
    //  stored within it.
    //    This Local Origin is included into occWorldViewMatrix during rendering.
    //  - OpenGl_Structure defines Object local transformation occModelWorldMatrix with subtracted
    //  Local Origin of the Layer.
    //    This means that Object itself should be defined within either Local Transformation equal
    //    or near to Local Origin of the Layer.
    theWorkspace->View()->SetLocalOrigin(aLayerSettings.Origin());

    NCollection_Mat4<Standard_Real> aWorldView = aWorldCamera->OrientationMatrix();
    Graphic3d_TransformUtils::Translate(aWorldView,
                                        aLayerSettings.Origin().X(),
                                        aLayerSettings.Origin().Y(),
                                        aLayerSettings.Origin().Z());

    if (!aManager->LightSourceState().ShadowMaps().IsNull())
    {
      // shift shadowmap matrix
      for (OpenGl_ShadowMapArray::Iterator aShadowIter(*aManager->LightSourceState().ShadowMaps());
           aShadowIter.More();
           aShadowIter.Next())
      {
        aShadowIter.Value()->UpdateCamera(*theWorkspace->View(), &aLayerSettings.Origin());
      }
    }

    NCollection_Mat4<Standard_ShortReal> aWorldViewF;
    aWorldViewF.ConvertFrom(aWorldView);
    aCtx->WorldViewState.SetCurrent(aWorldViewF);
    aCtx->ShaderManager()->UpdateClippingState();
    aCtx->ShaderManager()->UpdateLightSourceState();
  }

  // render priority list
  const Standard_Integer aViewId = theWorkspace->View()->Identification();
  for (Standard_Integer aPriorityIter = Graphic3d_DisplayPriority_Bottom;
       aPriorityIter <= Graphic3d_DisplayPriority_Topmost;
       ++aPriorityIter)
  {
    const Graphic3d_IndexedMapOfStructure& aStructures =
      theLayer.Structures((Graphic3d_DisplayPriority)aPriorityIter);
    for (OpenGl_Structure::StructIterator aStructIter(aStructures); aStructIter.More();
         aStructIter.Next())
    {
      const OpenGl_Structure* aStruct = aStructIter.Value();
      if (aStruct->IsCulled() || !aStruct->IsVisible(aViewId))
      {
        continue;
      }

      aStruct->Render(theWorkspace);
    }
  }

  if (hasOwnLights)
  {
    aManager->UpdateLightSourceStateTo(aLightsBack,
                                       theWorkspace->View()->SpecIBLMapLevels(),
                                       aShadowMaps);
  }
  if (hasLocalCS)
  {
    if (!aManager->LightSourceState().ShadowMaps().IsNull())
    {
      // restore shadowmap matrix
      for (OpenGl_ShadowMapArray::Iterator aShadowIter(*aManager->LightSourceState().ShadowMaps());
           aShadowIter.More();
           aShadowIter.Next())
      {
        aShadowIter.Value()->UpdateCamera(*theWorkspace->View(), &gp::Origin().XYZ());
      }
    }

    aCtx->ShaderManager()->RevertClippingState();
    aCtx->ShaderManager()->UpdateLightSourceState();

    aCtx->WorldViewState.SetCurrent(aWorldCamera->OrientationMatrixF());
    theWorkspace->View()->SetLocalOrigin(gp_XYZ(0.0, 0.0, 0.0));
  }

  // always restore polygon offset between layers rendering
  theWorkspace->SetDefaultPolygonOffset(anAppliedOffsetParams);

  // restore environment texture
  if (!aLayerSettings.UseEnvironmentTexture())
  {
    theWorkspace->SetEnvironmentTexture(anEnvironmentTexture);
  }
}

//=================================================================================================

void OpenGl_LayerList::Render(const Handle(OpenGl_Workspace)& theWorkspace,
                              const Standard_Boolean          theToDrawImmediate,
                              const OpenGl_LayerFilter        theFilterMode,
                              const Graphic3d_ZLayerId        theLayersToProcess,
                              OpenGl_FrameBuffer*             theReadDrawFbo,
                              OpenGl_FrameBuffer*             theOitAccumFbo) const
{
  // Remember global settings for glDepth function and write mask.
  OpenGl_GlobalLayerSettings aPrevSettings;

  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  aCtx->core11fwd->glGetIntegerv(GL_DEPTH_FUNC, &aPrevSettings.DepthFunc);
  aCtx->core11fwd->glGetBooleanv(GL_DEPTH_WRITEMASK, &aPrevSettings.DepthMask);
  OpenGl_GlobalLayerSettings aDefaultSettings = aPrevSettings;
  const bool isShadowMapPass = theReadDrawFbo != NULL && !theReadDrawFbo->HasColor();

  // Two render filters are used to support transparency draw. Opaque filter accepts
  // only non-transparent OpenGl elements of a layer and counts number of skipped
  // transparent ones. If the counter has positive value the layer is added into
  // transparency post-processing stack. At the end of drawing or once the depth
  // buffer is to be cleared the layers in the stack should be drawn using
  // blending and depth mask settings and another transparency filter which accepts
  // only transparent OpenGl elements of a layer. The stack <myTransparentToProcess>
  // was preallocated before going into this method and has enough space to keep
  // maximum number of references to layers, therefore it will not increase memory
  // fragmentation during regular rendering.
  const Standard_Integer aPrevFilter =
    theWorkspace->RenderFilter()
    & ~(Standard_Integer)(OpenGl_RenderFilter_OpaqueOnly | OpenGl_RenderFilter_TransparentOnly);
  theWorkspace->SetRenderFilter(aPrevFilter | OpenGl_RenderFilter_OpaqueOnly);

  myTransparentToProcess.Clear();

  OpenGl_LayerStack::iterator aStackIter(myTransparentToProcess.Origin());
  Standard_Integer            aClearDepthLayerPrev = -1, aClearDepthLayer = -1;
  const bool toPerformDepthPrepass = theWorkspace->View()->RenderingParams().ToEnableDepthPrepass
                                     && aPrevSettings.DepthMask == GL_TRUE && !isShadowMapPass;
  const Handle(Graphic3d_LightSet) aLightsBack =
    aCtx->ShaderManager()->LightSourceState().LightSources();
  const Handle(OpenGl_ShadowMapArray) aShadowMaps =
    aCtx->ShaderManager()->LightSourceState().ShadowMaps();
  for (OpenGl_FilteredIndexedLayerIterator aLayerIterStart(myLayers,
                                                           theToDrawImmediate,
                                                           theFilterMode,
                                                           theLayersToProcess);
       aLayerIterStart.More();)
  {
    bool hasSkippedDepthLayers = false;
    for (int aPassIter = toPerformDepthPrepass ? 0 : 2; aPassIter < 3; ++aPassIter)
    {
      if (aPassIter == 0)
      {
        aCtx->SetColorMask(false);
        aCtx->ShaderManager()->UpdateLightSourceStateTo(Handle(Graphic3d_LightSet)(),
                                                        theWorkspace->View()->SpecIBLMapLevels(),
                                                        Handle(OpenGl_ShadowMapArray)());
        aDefaultSettings.DepthFunc = aPrevSettings.DepthFunc;
        aDefaultSettings.DepthMask = GL_TRUE;
      }
      else if (aPassIter == 1)
      {
        if (!hasSkippedDepthLayers)
        {
          continue;
        }
        aCtx->SetColorMask(true);
        aCtx->ShaderManager()->UpdateLightSourceStateTo(aLightsBack,
                                                        theWorkspace->View()->SpecIBLMapLevels(),
                                                        aShadowMaps);
        aDefaultSettings = aPrevSettings;
      }
      else if (aPassIter == 2)
      {
        if (isShadowMapPass)
        {
          aCtx->SetColorMask(false);
          aCtx->ShaderManager()->UpdateLightSourceStateTo(Handle(Graphic3d_LightSet)(),
                                                          theWorkspace->View()->SpecIBLMapLevels(),
                                                          Handle(OpenGl_ShadowMapArray)());
        }
        else
        {
          aCtx->SetColorMask(true);
          aCtx->ShaderManager()->UpdateLightSourceStateTo(aLightsBack,
                                                          theWorkspace->View()->SpecIBLMapLevels(),
                                                          aShadowMaps);
        }
        if (toPerformDepthPrepass)
        {
          aDefaultSettings.DepthFunc = GL_EQUAL;
          aDefaultSettings.DepthMask = GL_FALSE;
        }
      }

      OpenGl_FilteredIndexedLayerIterator aLayerIter(aLayerIterStart);
      for (; aLayerIter.More(); aLayerIter.Next())
      {
        const OpenGl_Layer& aLayer = aLayerIter.Value();

        // make sure to clear depth of previous layers even if layer has no structures
        if (aLayer.LayerSettings().ToClearDepth())
        {
          aClearDepthLayer = aLayerIter.Index();
        }
        if (aLayer.IsCulled())
        {
          continue;
        }
        else if (aClearDepthLayer > aClearDepthLayerPrev)
        {
          // At this point the depth buffer may be set to clear by previous configuration of layers
          // or configuration of the current layer. Additional rendering pass to handle transparent
          // elements of recently drawn layers require use of current depth buffer so we put
          // remaining layers for processing as one bunch before erasing the depth buffer.
          if (aPassIter == 2)
          {
            aLayerIterStart = aLayerIter;
          }
          else
          {
            aClearDepthLayer = -1;
          }
          break;
        }
        else if (aPassIter == 0 && !aLayer.LayerSettings().ToRenderInDepthPrepass())
        {
          hasSkippedDepthLayers = true;
          continue;
        }
        else if (aPassIter == 1 && aLayer.LayerSettings().ToRenderInDepthPrepass())
        {
          continue;
        }

        // Render opaque OpenGl elements of a layer and count the number of skipped.
        // If a layer has skipped (e.g. transparent) elements it should be added into
        // the transparency post-processing stack.
        theWorkspace->ResetSkippedCounter();

        renderLayer(theWorkspace, aDefaultSettings, aLayer);

        if (aPassIter != 0 && theWorkspace->NbSkippedTransparentElements() > 0)
        {
          myTransparentToProcess.Push(&aLayer);
        }
      }
      if (aPassIter == 2 && !aLayerIter.More())
      {
        aLayerIterStart = aLayerIter;
      }
    }

    if (!myTransparentToProcess.IsEmpty())
    {
      renderTransparent(theWorkspace, aStackIter, aPrevSettings, theReadDrawFbo, theOitAccumFbo);
    }
    if (aClearDepthLayer > aClearDepthLayerPrev)
    {
      aClearDepthLayerPrev = aClearDepthLayer;
      aCtx->core11fwd->glDepthMask(GL_TRUE);
      aCtx->core11fwd->glClear(GL_DEPTH_BUFFER_BIT);
    }
  }

  aCtx->ShaderManager()->UpdateLightSourceStateTo(aLightsBack,
                                                  theWorkspace->View()->SpecIBLMapLevels(),
                                                  aShadowMaps);
  aCtx->core11fwd->glDepthMask(aPrevSettings.DepthMask);
  aCtx->core11fwd->glDepthFunc(aPrevSettings.DepthFunc);

  theWorkspace->SetRenderFilter(aPrevFilter);
}

//=======================================================================
// function : renderTransparent
// purpose  : Render transparent objects using blending operator.
//=======================================================================
void OpenGl_LayerList::renderTransparent(const Handle(OpenGl_Workspace)&   theWorkspace,
                                         OpenGl_LayerStack::iterator&      theLayerIter,
                                         const OpenGl_GlobalLayerSettings& theGlobalSettings,
                                         OpenGl_FrameBuffer*               theReadDrawFbo,
                                         OpenGl_FrameBuffer*               theOitAccumFbo) const
{
  // Check if current iterator has already reached the end of the stack.
  // This should happen if no additional layers has been added to
  // the processing stack after last transparency pass.
  if (theLayerIter == myTransparentToProcess.Back())
  {
    return;
  }

  const Handle(OpenGl_Context)&       aCtx       = theWorkspace->GetGlContext();
  const Handle(OpenGl_ShaderManager)& aManager   = aCtx->ShaderManager();
  const OpenGl_LayerStack::iterator   aLayerFrom = theLayerIter;
  OpenGl_View*                        aView      = theWorkspace->View();

  Graphic3d_RenderTransparentMethod anOitMode =
    aView != NULL ? aView->RenderingParams().TransparencyMethod : Graphic3d_RTM_BLEND_UNORDERED;

  const Standard_Integer aPrevFilter =
    theWorkspace->RenderFilter()
    & ~(Standard_Integer)(OpenGl_RenderFilter_OpaqueOnly | OpenGl_RenderFilter_TransparentOnly);
  theWorkspace->SetRenderFilter(aPrevFilter | OpenGl_RenderFilter_TransparentOnly);
  aCtx->core11fwd->glEnable(GL_BLEND);

  const Handle(OpenGl_FrameBuffer)* aGlDepthPeelFBOs =
    aView->DepthPeelingFbos()->DepthPeelFbosOit();
  const Handle(OpenGl_FrameBuffer)* aGlFrontBackColorFBOs =
    aView->DepthPeelingFbos()->FrontBackColorFbosOit();
  const Handle(OpenGl_FrameBuffer)& aGlBlendBackFBO = aView->DepthPeelingFbos()->BlendBackFboOit();

  // Blended order-independent transparency algorithm require several preconditions to be enabled.
  // It should be requested by user, at least two outputs from fragment shader should be supported
  // by GPU, so is the given framebuffer should contain two additional color buffers to handle
  // accumulated color channels, blended alpha channel and weight factors - these accumulation
  // buffers are required to implement commuting blend operator (at least OpenGl 2.0 should be
  // available).
  if (anOitMode == Graphic3d_RTM_BLEND_OIT)
  {
    if (theOitAccumFbo == NULL || theOitAccumFbo->NbColorBuffers() < 2
        || !theOitAccumFbo->ColorTexture(0)->IsValid()
        || !theOitAccumFbo->ColorTexture(1)->IsValid())
    {
      anOitMode = Graphic3d_RTM_BLEND_UNORDERED;
    }
  }
  else if (anOitMode == Graphic3d_RTM_DEPTH_PEELING_OIT)
  {
    if (!aGlBlendBackFBO->IsValid())
    {
      anOitMode = Graphic3d_RTM_BLEND_UNORDERED;
    }
  }
  const bool isMSAA              = theReadDrawFbo && theReadDrawFbo->NbSamples() > 0;
  int        aDepthPeelingDrawId = -1;

  switch (anOitMode)
  {
    case Graphic3d_RTM_BLEND_UNORDERED: {
      aCtx->core11fwd->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;
    }
    case Graphic3d_RTM_BLEND_OIT: {
      const float aDepthFactor = aView->RenderingParams().OitDepthFactor;
      aManager->SetWeighedOitState(aDepthFactor);

      theOitAccumFbo->BindBuffer(aCtx);

      aCtx->SetDrawBuffers(2, THE_DRAW_BUFFERS01);
      // clang-format off
      aCtx->SetColorMaskRGBA (NCollection_Vec4<bool> (true)); // force writes into all components, including alpha
      // clang-format on
      aCtx->core11fwd->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      aCtx->core11fwd->glClear(GL_COLOR_BUFFER_BIT);
      aCtx->core15fwd->glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
      break;
    }
    case Graphic3d_RTM_DEPTH_PEELING_OIT: {
      static const float THE_MIN_DEPTH = 0.0f;
      static const float THE_MAX_DEPTH = 1.0f;

      aView->DepthPeelingFbos()->AttachDepthTexture(aCtx, theReadDrawFbo->DepthStencilTexture());

      // initialize min/max depth buffer
      aGlBlendBackFBO->BindDrawBuffer(aCtx);
      aCtx->SetDrawBuffers(1, THE_DRAW_BUFFERS0);
      // clang-format off
      aCtx->SetColorMaskRGBA (NCollection_Vec4<bool> (true)); // force writes into all components, including alpha
      // clang-format on
      aCtx->core20fwd->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      aCtx->core20fwd->glClear(GL_COLOR_BUFFER_BIT);

      aGlDepthPeelFBOs[1]->BindDrawBuffer(aCtx);
      aCtx->SetDrawBuffers(1, THE_DRAW_BUFFERS0);
      aCtx->core20fwd->glClearColor(-THE_MIN_DEPTH, THE_MAX_DEPTH, 0.0f, 0.0f);
      aCtx->core20fwd->glClear(GL_COLOR_BUFFER_BIT);

      aGlFrontBackColorFBOs[0]->BindDrawBuffer(aCtx);
      aCtx->SetDrawBuffers(2, THE_DRAW_BUFFERS01);
      aCtx->core20fwd->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      aCtx->core20fwd->glClear(GL_COLOR_BUFFER_BIT);

      aGlFrontBackColorFBOs[1]->BindDrawBuffer(aCtx);
      aCtx->SetDrawBuffers(2, THE_DRAW_BUFFERS01);
      aCtx->core20fwd->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      aCtx->core20fwd->glClear(GL_COLOR_BUFFER_BIT);

      // draw depth for first pass to peel
      aGlDepthPeelFBOs[0]->BindBuffer(aCtx);

      aCtx->SetDrawBuffers(1, THE_DRAW_BUFFERS0);
      aCtx->core20fwd->glClearColor(THE_DEPTH_CLEAR_VALUE, THE_DEPTH_CLEAR_VALUE, 0.0f, 0.0f);
      aCtx->core20fwd->glClear(GL_COLOR_BUFFER_BIT);
      aCtx->core20fwd->glBlendEquation(GL_MAX);

      aManager->SetOitState(Graphic3d_RTM_DEPTH_PEELING_OIT);

      aGlDepthPeelFBOs[1]->ColorTexture(0)->Bind(aCtx, aCtx->DepthPeelingDepthTexUnit());
      aGlDepthPeelFBOs[1]->ColorTexture(1)->Bind(aCtx, aCtx->DepthPeelingFrontColorTexUnit());
      break;
    }
  }

  // During blended order-independent transparency pass the depth test
  // should be enabled to discard fragments covered by opaque geometry
  // and depth writing should be disabled, because transparent fragments
  // overall each other with non unitary coverage factor.
  OpenGl_GlobalLayerSettings aGlobalSettings = theGlobalSettings;
  aGlobalSettings.DepthMask                  = GL_FALSE;
  aCtx->core11fwd->glDepthMask(GL_FALSE);

  for (theLayerIter = aLayerFrom; theLayerIter != myTransparentToProcess.Back(); ++theLayerIter)
  {
    renderLayer(theWorkspace, aGlobalSettings, *(*theLayerIter));
  }

  switch (anOitMode)
  {
    case Graphic3d_RTM_BLEND_UNORDERED: {
      break;
    }
    case Graphic3d_RTM_BLEND_OIT: {
      // revert state of rendering
      aManager->ResetOitState();
      theOitAccumFbo->UnbindBuffer(aCtx);
      if (theReadDrawFbo)
      {
        theReadDrawFbo->BindBuffer(aCtx);
      }

      aCtx->SetDrawBuffers(1, THE_DRAW_BUFFERS0);
      aCtx->SetColorMask(true); // update writes into alpha component
      break;
    }
    case Graphic3d_RTM_DEPTH_PEELING_OIT: {
      // Dual Depth Peeling Ping-Pong
      const int            aNbPasses  = aView->RenderingParams().NbOitDepthPeelingLayers;
      OpenGl_VertexBuffer* aQuadVerts = aView->initBlitQuad(false);

      aGlDepthPeelFBOs[1]->ColorTexture(1)->Unbind(aCtx, aCtx->DepthPeelingFrontColorTexUnit());
      aGlDepthPeelFBOs[1]->ColorTexture(0)->Unbind(aCtx, aCtx->DepthPeelingDepthTexUnit());

      for (int aPass = 0; aPass < aNbPasses; ++aPass)
      {
        const int aReadId   = aPass % 2;
        aDepthPeelingDrawId = 1 - aReadId;

        aGlDepthPeelFBOs[aDepthPeelingDrawId]->BindDrawBuffer(aCtx);
        aCtx->SetDrawBuffers(1, THE_DRAW_BUFFERS0);
        aCtx->core20fwd->glClearColor(THE_DEPTH_CLEAR_VALUE, THE_DEPTH_CLEAR_VALUE, 0.0f, 0.0f);
        aCtx->core20fwd->glClear(GL_COLOR_BUFFER_BIT);

        aGlFrontBackColorFBOs[aDepthPeelingDrawId]->BindDrawBuffer(aCtx);
        aCtx->SetDrawBuffers(2, THE_DRAW_BUFFERS01);
        aCtx->core20fwd->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        aCtx->core20fwd->glClear(GL_COLOR_BUFFER_BIT);

        /// aGlDepthPeelFBOs[aDepthPeelingDrawId]->BindDrawBuffer (aCtx);
        aGlDepthPeelFBOs[aDepthPeelingDrawId]->BindBuffer(aCtx);
        aCtx->SetDrawBuffers(3, THE_DRAW_BUFFERS012);
        aCtx->core20fwd->glBlendEquation(GL_MAX);

        aGlDepthPeelFBOs[aReadId]->ColorTexture(0)->Bind(aCtx, aCtx->DepthPeelingDepthTexUnit());
        aGlDepthPeelFBOs[aReadId]->ColorTexture(1)->Bind(aCtx,
                                                         aCtx->DepthPeelingFrontColorTexUnit());

        // draw geometry
        for (theLayerIter = aLayerFrom; theLayerIter != myTransparentToProcess.Back();
             ++theLayerIter)
        {
          renderLayer(theWorkspace, aGlobalSettings, *(*theLayerIter));
        }

        aGlDepthPeelFBOs[aReadId]->ColorTexture(1)->Unbind(aCtx,
                                                           aCtx->DepthPeelingFrontColorTexUnit());
        aGlDepthPeelFBOs[aReadId]->ColorTexture(0)->Unbind(aCtx, aCtx->DepthPeelingDepthTexUnit());

        // blend back color
        aGlBlendBackFBO->BindDrawBuffer(aCtx);
        aCtx->SetDrawBuffers(1, THE_DRAW_BUFFERS0);
        if (aQuadVerts->IsValid() && aManager->BindOitDepthPeelingBlendProgram(isMSAA))
        {
          aCtx->core20fwd->glBlendEquation(GL_FUNC_ADD);
          aCtx->core20fwd->glBlendFuncSeparate(GL_SRC_ALPHA,
                                               GL_ONE_MINUS_SRC_ALPHA,
                                               GL_ONE,
                                               GL_ONE_MINUS_SRC_ALPHA);
          aCtx->core20fwd->glDepthFunc(GL_ALWAYS);

          aQuadVerts->BindVertexAttrib(aCtx, Graphic3d_TOA_POS);

          const Handle(OpenGl_TextureSet) aTextureBack =
            aCtx->BindTextures(Handle(OpenGl_TextureSet)(), Handle(OpenGl_ShaderProgram)());
          aGlDepthPeelFBOs[aDepthPeelingDrawId]->ColorTexture(2)->Bind(aCtx,
                                                                       Graphic3d_TextureUnit_0);

          aCtx->core20fwd->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

          aQuadVerts->UnbindVertexAttrib(aCtx, Graphic3d_TOA_POS);
          aGlDepthPeelFBOs[aDepthPeelingDrawId]->ColorTexture(2)->Unbind(aCtx,
                                                                         Graphic3d_TextureUnit_0);
          aCtx->BindProgram(NULL);

          if (!aTextureBack.IsNull())
          {
            aCtx->BindTextures(aTextureBack, Handle(OpenGl_ShaderProgram)());
          }

          aCtx->core11fwd->glDepthFunc(theGlobalSettings.DepthFunc);
        }
        else
        {
          aCtx->PushMessage(
            GL_DEBUG_SOURCE_APPLICATION,
            GL_DEBUG_TYPE_ERROR,
            0,
            GL_DEBUG_SEVERITY_HIGH,
            "Initialization of OIT compositing pass has failed.\n"
            "  Depth Peeling order-independent transparency will not be available.\n");
          if (aView != NULL)
          {
            Standard_Boolean& aOITFlag = isMSAA ? aView->myToDisableOITMSAA : aView->myToDisableOIT;
            aOITFlag                   = Standard_True;
          }
        }
      }

      aManager->ResetOitState();
      aGlBlendBackFBO->UnbindBuffer(aCtx);
      if (theReadDrawFbo)
      {
        theReadDrawFbo->BindBuffer(aCtx);
      }
      aCtx->SetDrawBuffers(1, THE_DRAW_BUFFERS0);
      aCtx->SetColorMask(true); // update writes into alpha component
      break;
    }
  }

  theWorkspace->SetRenderFilter(aPrevFilter | OpenGl_RenderFilter_OpaqueOnly);
  switch (anOitMode)
  {
    case Graphic3d_RTM_BLEND_UNORDERED: {
      break;
    }
    case Graphic3d_RTM_BLEND_OIT: {
      // draw full screen quad with special shader to compose the buffers
      OpenGl_VertexBuffer* aVerts = aView->initBlitQuad(Standard_False);
      if (aVerts->IsValid() && aManager->BindOitCompositingProgram(isMSAA))
      {
        aCtx->core11fwd->glDepthFunc(GL_ALWAYS);
        aCtx->core11fwd->glDepthMask(GL_FALSE);

        aVerts->BindVertexAttrib(aCtx, Graphic3d_TOA_POS);

        const Handle(OpenGl_TextureSet) aTextureBack =
          aCtx->BindTextures(Handle(OpenGl_TextureSet)(), Handle(OpenGl_ShaderProgram)());
        theOitAccumFbo->ColorTexture(0)->Bind(aCtx, Graphic3d_TextureUnit_0);
        theOitAccumFbo->ColorTexture(1)->Bind(aCtx, Graphic3d_TextureUnit_1);

        aCtx->core11fwd->glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
        aCtx->core11fwd->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        aVerts->UnbindVertexAttrib(aCtx, Graphic3d_TOA_POS);
        theOitAccumFbo->ColorTexture(1)->Unbind(aCtx, Graphic3d_TextureUnit_1);
        theOitAccumFbo->ColorTexture(0)->Unbind(aCtx, Graphic3d_TextureUnit_0);
        aCtx->BindProgram(NULL);

        if (!aTextureBack.IsNull())
        {
          aCtx->BindTextures(aTextureBack, Handle(OpenGl_ShaderProgram)());
        }
      }
      else
      {
        aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                          GL_DEBUG_TYPE_ERROR,
                          0,
                          GL_DEBUG_SEVERITY_HIGH,
                          "Initialization of OIT compositing pass has failed.\n"
                          "  Blended order-independent transparency will not be available.\n");
        if (aView != NULL)
        {
          Standard_Boolean& aOITFlag = isMSAA ? aView->myToDisableOITMSAA : aView->myToDisableOIT;
          aOITFlag                   = Standard_True;
        }
      }
      break;
    }
    case Graphic3d_RTM_DEPTH_PEELING_OIT: {
      // compose depth peeling results into destination FBO
      OpenGl_VertexBuffer* aVerts = aView->initBlitQuad(Standard_False);
      if (aVerts->IsValid() && aManager->BindOitDepthPeelingFlushProgram(isMSAA))
      {
        aCtx->core20fwd->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        aCtx->core20fwd->glDepthFunc(GL_ALWAYS);

        aVerts->BindVertexAttrib(aCtx, Graphic3d_TOA_POS);

        const Handle(OpenGl_TextureSet) aTextureBack =
          aCtx->BindTextures(Handle(OpenGl_TextureSet)(), Handle(OpenGl_ShaderProgram)());
        aGlDepthPeelFBOs[aDepthPeelingDrawId]->ColorTexture(1)->Bind(aCtx, Graphic3d_TextureUnit_0);
        aGlBlendBackFBO->ColorTexture(0)->Bind(aCtx, Graphic3d_TextureUnit_1);

        aCtx->core20fwd->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        aVerts->UnbindVertexAttrib(aCtx, Graphic3d_TOA_POS);
        aGlBlendBackFBO->ColorTexture(0)->Unbind(aCtx, Graphic3d_TextureUnit_1);
        aGlDepthPeelFBOs[aDepthPeelingDrawId]->ColorTexture(1)->Unbind(aCtx,
                                                                       Graphic3d_TextureUnit_0);
        aCtx->BindProgram(NULL);

        if (!aTextureBack.IsNull())
        {
          aCtx->BindTextures(aTextureBack, Handle(OpenGl_ShaderProgram)());
        }

        aCtx->core11fwd->glDepthFunc(theGlobalSettings.DepthFunc);
      }
      else
      {
        aCtx->PushMessage(
          GL_DEBUG_SOURCE_APPLICATION,
          GL_DEBUG_TYPE_ERROR,
          0,
          GL_DEBUG_SEVERITY_HIGH,
          "Initialization of OIT compositing pass has failed.\n"
          "  Depth Peeling order-independent transparency will not be available.\n");
        if (aView != NULL)
        {
          Standard_Boolean& aOITFlag = isMSAA ? aView->myToDisableOITMSAA : aView->myToDisableOIT;
          aOITFlag                   = true;
        }
      }
      aView->DepthPeelingFbos()->DetachDepthTexture(aCtx);

      // Bind the framebuffer for reading depth and writing final color
      // after DetachDepthTexture() because after the function it's unbinded.
      if (theReadDrawFbo)
      {
        theReadDrawFbo->BindBuffer(aCtx);
      }
      break;
    }
  }

  aCtx->core11fwd->glDisable(GL_BLEND);
  aCtx->core11fwd->glBlendFunc(GL_ONE, GL_ZERO);
  aCtx->core11fwd->glDepthMask(theGlobalSettings.DepthMask);
  aCtx->core11fwd->glDepthFunc(theGlobalSettings.DepthFunc);
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_LayerList::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, OpenGl_LayerList)

  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayersIt(myLayers); aLayersIt.More();
       aLayersIt.Next())
  {
    const Handle(Graphic3d_Layer)& aLayerId = aLayersIt.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aLayerId.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNbStructures)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myImmediateNbStructures)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myModifStateOfRaytraceable)
}
