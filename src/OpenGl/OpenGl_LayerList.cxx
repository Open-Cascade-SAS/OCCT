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
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_LayerList.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_VertexBuffer.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_GraphicDriver.hxx>

namespace
{
  //! Auxiliary class extending sequence iterator with index.
  class OpenGl_IndexedLayerIterator : public OpenGl_SequenceOfLayers::Iterator
  {
  public:
    //! Main constructor.
    OpenGl_IndexedLayerIterator (const OpenGl_SequenceOfLayers& theSeq)
    : OpenGl_SequenceOfLayers::Iterator (theSeq),
      myIndex (theSeq.Lower()) {}

    //! Return index of current position.
    Standard_Integer Index() const { return myIndex; }

    //! Move to the next position.
    void Next()
    {
      OpenGl_SequenceOfLayers::Iterator::Next();
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
    OpenGl_FilteredIndexedLayerIterator (const OpenGl_SequenceOfLayers& theSeq,
                                         Standard_Integer theDefaultLayerIndex,
                                         Standard_Boolean theToDrawImmediate,
                                         OpenGl_LayerFilter theLayersToProcess)
    : myIter (theSeq),
      myDefaultLayerIndex (theDefaultLayerIndex),
      myLayersToProcess (theLayersToProcess),
      myToDrawImmediate (theToDrawImmediate)
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
        if (myIter.Value()->IsImmediate() != myToDrawImmediate)
        {
          continue;
        }

        switch (myLayersToProcess)
        {
          case OpenGl_LF_All:
          {
            break;
          }
          case OpenGl_LF_Upper:
          {
            if (myIter.Index() <= myDefaultLayerIndex)
            {
              continue;
            }
            break;
          }
          case OpenGl_LF_Bottom:
          {
            if (myIter.Index() >= myDefaultLayerIndex)
            {
              continue;
            }
            break;
          }
          case OpenGl_LF_Default:
          {
            if (myIter.Index() != myDefaultLayerIndex)
            {
              continue;
            }
            break;
          }
        }
        return;
      }
    }
  private:
    OpenGl_IndexedLayerIterator myIter;
    Standard_Integer            myDefaultLayerIndex;
    OpenGl_LayerFilter          myLayersToProcess;
    Standard_Boolean            myToDrawImmediate;
  };
}

struct OpenGl_GlobalLayerSettings
{
  GLint DepthFunc;
  GLboolean DepthMask;
};

//=======================================================================
//function : OpenGl_LayerList
//purpose  : Constructor
//=======================================================================

OpenGl_LayerList::OpenGl_LayerList (const Standard_Integer theNbPriorities)
: myBVHBuilder (new BVH_LinearBuilder<Standard_Real, 3> (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth)),
  myDefaultLayerIndex (0),
  myNbPriorities (theNbPriorities),
  myNbStructures (0),
  myImmediateNbStructures (0),
  myModifStateOfRaytraceable (0)
{
  // insert default priority layers
  myLayers.Append (new OpenGl_Layer (myNbPriorities, myBVHBuilder));
  myLayerIds.Bind (Graphic3d_ZLayerId_BotOSD,  myLayers.Upper());

  myLayers.Append (new OpenGl_Layer (myNbPriorities, myBVHBuilder));
  myLayerIds.Bind (Graphic3d_ZLayerId_Default, myLayers.Upper());

  myLayers.Append (new OpenGl_Layer (myNbPriorities, myBVHBuilder));
  myLayerIds.Bind (Graphic3d_ZLayerId_Top,     myLayers.Upper());

  myLayers.Append (new OpenGl_Layer (myNbPriorities, myBVHBuilder));
  myLayerIds.Bind (Graphic3d_ZLayerId_Topmost, myLayers.Upper());

  myLayers.Append (new OpenGl_Layer (myNbPriorities, myBVHBuilder));
  myLayerIds.Bind (Graphic3d_ZLayerId_TopOSD,  myLayers.Upper());

  myDefaultLayerIndex = myLayerIds.Find (Graphic3d_ZLayerId_Default);

  myTransparentToProcess.Allocate (myLayers.Length());
}

//=======================================================================
//function : ~OpenGl_LayerList
//purpose  : Destructor
//=======================================================================

OpenGl_LayerList::~OpenGl_LayerList()
{
}

//=======================================================================
//function : SetFrustumCullingBVHBuilder
//purpose  :
//=======================================================================
void OpenGl_LayerList::SetFrustumCullingBVHBuilder (const Handle(Select3D_BVHBuilder3d)& theBuilder)
{
  myBVHBuilder = theBuilder;
  for (OpenGl_SequenceOfLayers::Iterator anIts (myLayers); anIts.More(); anIts.Next())
  {
    anIts.ChangeValue()->SetFrustumCullingBVHBuilder (theBuilder);
  }
}

//=======================================================================
//function : AddLayer
//purpose  : 
//=======================================================================

void OpenGl_LayerList::AddLayer (const Graphic3d_ZLayerId theLayerId)
{
  if (myLayerIds.IsBound (theLayerId))
  {
    return;
  }

  // add the new layer
  myLayers.Append (new OpenGl_Layer (myNbPriorities, myBVHBuilder));
  myLayerIds.Bind (theLayerId, myLayers.Length());

  myTransparentToProcess.Allocate (myLayers.Length());
}

//=======================================================================
//function : Layer
//purpose  : 
//=======================================================================
OpenGl_Layer& OpenGl_LayerList::Layer (const Graphic3d_ZLayerId theLayerId)
{
  return *myLayers.ChangeValue (myLayerIds.Find (theLayerId));
}

//=======================================================================
//function : Layer
//purpose  : 
//=======================================================================
const OpenGl_Layer& OpenGl_LayerList::Layer (const Graphic3d_ZLayerId theLayerId) const
{
  return *myLayers.Value (myLayerIds.Find (theLayerId));
}

//=======================================================================
//function : RemoveLayer
//purpose  :
//=======================================================================

void OpenGl_LayerList::RemoveLayer (const Graphic3d_ZLayerId theLayerId)
{
  if (!myLayerIds.IsBound (theLayerId)
    || theLayerId <= 0)
  {
    return;
  }

  const Standard_Integer aRemovePos = myLayerIds.Find (theLayerId);
  
  // move all displayed structures to first layer
  {
    const OpenGl_Layer& aLayerToMove = *myLayers.Value (aRemovePos);
    myLayers.ChangeFirst()->Append (aLayerToMove);
  }

  // remove layer
  myLayers.Remove (aRemovePos);
  myLayerIds.UnBind (theLayerId);

  // updated sequence indexes in map
  for (OpenGl_LayerSeqIds::Iterator aMapIt (myLayerIds); aMapIt.More(); aMapIt.Next())
  {
    Standard_Integer& aSeqIdx = aMapIt.ChangeValue();
    if (aSeqIdx > aRemovePos)
      aSeqIdx--;
  }

  myDefaultLayerIndex = myLayerIds.Find (Graphic3d_ZLayerId_Default);

  myTransparentToProcess.Allocate (myLayers.Length());
}

//=======================================================================
//function : AddStructure
//purpose  :
//=======================================================================

void OpenGl_LayerList::AddStructure (const OpenGl_Structure*  theStruct,
                                     const Graphic3d_ZLayerId theLayerId,
                                     const Standard_Integer   thePriority,
                                     Standard_Boolean         isForChangePriority)
{
  // add structure to associated layer,
  // if layer doesn't exists, display structure in default layer
  Standard_Integer aSeqPos = myLayers.Lower();
  myLayerIds.Find (theLayerId, aSeqPos);

  OpenGl_Layer& aLayer = *myLayers.ChangeValue (aSeqPos);
  aLayer.Add (theStruct, thePriority, isForChangePriority);
  ++myNbStructures;
  if (aLayer.IsImmediate())
  {
    ++myImmediateNbStructures;
  }

  // Note: In ray-tracing mode we don't modify modification
  // state here. It is redundant, because the possible changes
  // will be handled in the loop for structures
}

//=======================================================================
//function : RemoveStructure
//purpose  :
//=======================================================================

void OpenGl_LayerList::RemoveStructure (const OpenGl_Structure* theStructure)
{
  const Graphic3d_ZLayerId aLayerId = theStructure->ZLayer();

  Standard_Integer aSeqPos = myLayers.Lower();
  myLayerIds.Find (aLayerId, aSeqPos);

  OpenGl_Layer&    aLayer    = *myLayers.ChangeValue (aSeqPos);
  Standard_Integer aPriority = -1;

  // remove structure from associated list
  // if the structure is not found there,
  // scan through layers and remove it
  if (aLayer.Remove (theStructure, aPriority))
  {
    --myNbStructures;
    if (aLayer.IsImmediate())
    {
      --myImmediateNbStructures;
    }

    if (aLayerId == Graphic3d_ZLayerId_Default
     && theStructure->IsRaytracable())
    {
      ++myModifStateOfRaytraceable;
    }

    return;
  }

  // scan through layers and remove it
  for (OpenGl_IndexedLayerIterator anIts (myLayers); anIts.More(); anIts.Next())
  {
    OpenGl_Layer& aLayerEx = *anIts.ChangeValue();
    if (aSeqPos == anIts.Index())
    {
      continue;
    }

    if (aLayerEx.Remove (theStructure, aPriority))
    {
      --myNbStructures;
      if (aLayerEx.IsImmediate())
      {
        --myImmediateNbStructures;
      }

      if (anIts.Index() == myDefaultLayerIndex
       && theStructure->IsRaytracable())
      {
        ++myModifStateOfRaytraceable;
      }
      return;
    }
  }
}

//=======================================================================
//function : InvalidateBVHData
//purpose  :
//=======================================================================
void OpenGl_LayerList::InvalidateBVHData (const Graphic3d_ZLayerId theLayerId)
{
  Standard_Integer aSeqPos = myLayers.Lower();
  myLayerIds.Find (theLayerId, aSeqPos);
  OpenGl_Layer& aLayer = *myLayers.ChangeValue (aSeqPos);
  aLayer.InvalidateBVHData();
}

//=======================================================================
//function : ChangeLayer
//purpose  :
//=======================================================================

void OpenGl_LayerList::ChangeLayer (const OpenGl_Structure*  theStructure,
                                    const Graphic3d_ZLayerId theOldLayerId,
                                    const Graphic3d_ZLayerId theNewLayerId)
{
  Standard_Integer aSeqPos = myLayers.Lower();
  myLayerIds.Find (theOldLayerId, aSeqPos);
  OpenGl_Layer&    aLayer    = *myLayers.ChangeValue (aSeqPos);
  Standard_Integer aPriority = -1;

  // take priority and remove structure from list found by <theOldLayerId>
  // if the structure is not found there, scan through all other layers
  if (aLayer.Remove (theStructure, aPriority, Standard_False))
  {
    if (theOldLayerId == Graphic3d_ZLayerId_Default
     && theStructure->IsRaytracable())
    {
      ++myModifStateOfRaytraceable;
    }

    --myNbStructures;
    if (aLayer.IsImmediate())
    {
      --myImmediateNbStructures;
    }

    // isForChangePriority should be Standard_False below, because we want
    // the BVH tree in the target layer to be updated with theStructure
    AddStructure (theStructure, theNewLayerId, aPriority);
    return;
  }

  // scan through layers and remove it
  for (OpenGl_IndexedLayerIterator anIts (myLayers); anIts.More(); anIts.Next())
  {
    if (aSeqPos == anIts.Index())
    {
      continue;
    }
  
    // try to remove structure and get priority value from this layer
    OpenGl_Layer& aLayerEx = *anIts.ChangeValue();
    if (aLayerEx.Remove (theStructure, aPriority, Standard_True))
    {
      if (anIts.Index() == myDefaultLayerIndex
       && theStructure->IsRaytracable())
      {
        ++myModifStateOfRaytraceable;
      }

      --myNbStructures;
      if (aLayerEx.IsImmediate())
      {
        --myImmediateNbStructures;
      }

      // isForChangePriority should be Standard_False below, because we want
      // the BVH tree in the target layer to be updated with theStructure
      AddStructure (theStructure, theNewLayerId, aPriority);
      return;
    }
  }
}

//=======================================================================
//function : ChangePriority
//purpose  :
//=======================================================================
void OpenGl_LayerList::ChangePriority (const OpenGl_Structure*  theStructure,
                                       const Graphic3d_ZLayerId theLayerId,
                                       const Standard_Integer   theNewPriority)
{
  Standard_Integer aSeqPos = myLayers.Lower();
  myLayerIds.Find (theLayerId, aSeqPos);
  OpenGl_Layer&    aLayer        = *myLayers.ChangeValue (aSeqPos);
  Standard_Integer anOldPriority = -1;

  if (aLayer.Remove (theStructure, anOldPriority, Standard_True))
  {
    --myNbStructures;
    if (aLayer.IsImmediate())
    {
      --myImmediateNbStructures;
    }

    AddStructure (theStructure, theLayerId, theNewPriority, Standard_True);
    return;
  }

  for (OpenGl_IndexedLayerIterator anIts (myLayers); anIts.More(); anIts.Next())
  {
    if (aSeqPos == anIts.Index())
    {
      continue;
    }

    OpenGl_Layer& aLayerEx = *anIts.ChangeValue();
    if (aLayerEx.Remove (theStructure, anOldPriority, Standard_True))
    {
      --myNbStructures;
      if (aLayerEx.IsImmediate())
      {
        --myImmediateNbStructures;
      }

      AddStructure (theStructure, theLayerId, theNewPriority, Standard_True);
      return;
    }
  }
}

//=======================================================================
//function : SetLayerSettings
//purpose  :
//=======================================================================
void OpenGl_LayerList::SetLayerSettings (const Graphic3d_ZLayerId        theLayerId,
                                         const Graphic3d_ZLayerSettings& theSettings)
{
  OpenGl_Layer& aLayer = Layer (theLayerId);
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
  aLayer.SetLayerSettings (theSettings);
}

//=======================================================================
//function : UpdateCulling
//purpose  :
//=======================================================================
void OpenGl_LayerList::UpdateCulling (const Handle(OpenGl_Workspace)& theWorkspace,
                                      const Standard_Boolean theToDrawImmediate)
{
  const Handle(OpenGl_FrameStats)& aStats = theWorkspace->GetGlContext()->FrameStats();
  OSD_Timer& aTimer = aStats->ActiveDataFrame().ChangeTimer (Graphic3d_FrameStatsTimer_CpuCulling);
  aTimer.Start();

  const Standard_Integer aViewId = theWorkspace->View()->Identification();
  const Graphic3d_CullingTool& aSelector = theWorkspace->View()->BVHTreeSelector();
  for (OpenGl_IndexedLayerIterator anIts (myLayers); anIts.More(); anIts.Next())
  {
    OpenGl_Layer& aLayer = *anIts.ChangeValue();
    if (aLayer.IsImmediate() != theToDrawImmediate)
    {
      continue;
    }

    aLayer.UpdateCulling (aViewId, aSelector, theWorkspace->View()->RenderingParams().FrustumCullingState);
  }

  aTimer.Stop();
  aStats->ActiveDataFrame()[Graphic3d_FrameStatsTimer_CpuCulling] = aTimer.UserTimeCPU();
}

//=======================================================================
//function : renderLayer
//purpose  :
//=======================================================================
void OpenGl_LayerList::renderLayer (const Handle(OpenGl_Workspace)& theWorkspace,
                                    const OpenGl_GlobalLayerSettings& theDefaultSettings,
                                    const Graphic3d_Layer& theLayer) const
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  const Graphic3d_ZLayerSettings& aLayerSettings = theLayer.LayerSettings();
  // aLayerSettings.ToClearDepth() is handled outside

  // handle depth test
  if (aLayerSettings.ToEnableDepthTest())
  {
    // assuming depth test is enabled by default
    glDepthFunc (theDefaultSettings.DepthFunc);
  }
  else
  {
    glDepthFunc (GL_ALWAYS);
  }

  // save environment texture
  Handle(OpenGl_TextureSet) anEnvironmentTexture = theWorkspace->EnvironmentTexture();
  if (!aLayerSettings.UseEnvironmentTexture())
  {
    theWorkspace->SetEnvironmentTexture (Handle(OpenGl_TextureSet)());
  }

  // handle depth offset
  const Graphic3d_PolygonOffset anAppliedOffsetParams = theWorkspace->SetDefaultPolygonOffset (aLayerSettings.PolygonOffset());

  // handle depth write
  theWorkspace->UseDepthWrite() = aLayerSettings.ToEnableDepthWrite() && theDefaultSettings.DepthMask == GL_TRUE;
  glDepthMask (theWorkspace->UseDepthWrite() ? GL_TRUE : GL_FALSE);

  const Standard_Boolean hasLocalCS = !aLayerSettings.OriginTransformation().IsNull();
  const Handle(OpenGl_ShaderManager)& aManager = aCtx->ShaderManager();
  Handle(Graphic3d_LightSet) aLightsBack = aManager->LightSourceState().LightSources();
  const bool hasOwnLights = aCtx->ColorMask() && !aLayerSettings.Lights().IsNull() && aLayerSettings.Lights() != aLightsBack;
  if (hasOwnLights)
  {
    aLayerSettings.Lights()->UpdateRevision();
    aManager->UpdateLightSourceStateTo (aLayerSettings.Lights());
  }

  const Handle(Graphic3d_Camera)& aWorldCamera = theWorkspace->View()->Camera();
  if (hasLocalCS)
  {
    // Apply local camera transformation.
    // The vertex position is computed by the following formula in GLSL program:
    //   gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;
    // where:
    //   occProjectionMatrix - matrix defining orthographic/perspective/stereographic projection
    //   occWorldViewMatrix  - world-view  matrix defining Camera position and orientation
    //   occModelWorldMatrix - model-world matrix defining Object transformation from local coordinate system to the world coordinate system
    //   occVertex           - input vertex position
    //
    // Since double precision is quite expensive on modern GPUs, and not available on old hardware,
    // all these values are passed with single float precision to the shader.
    // As result, single precision become insufficient for handling objects far from the world origin.
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
    // All approaches might be useful in different scenarios, but for the moment we consider the last one as main scenario.
    // Here we do the trick:
    //  - OpenGl_Layer defines the Local Origin, which is expected to be the center of objects stored within it.
    //    This Local Origin is included into occWorldViewMatrix during rendering.
    //  - OpenGl_Structure defines Object local transformation occModelWorldMatrix with subtracted Local Origin of the Layer.
    //    This means that Object itself should be defined within either Local Transformation equal or near to Local Origin of the Layer.
    theWorkspace->View()->SetLocalOrigin (aLayerSettings.Origin());

    NCollection_Mat4<Standard_Real> aWorldView = aWorldCamera->OrientationMatrix();
    Graphic3d_TransformUtils::Translate (aWorldView, aLayerSettings.Origin().X(), aLayerSettings.Origin().Y(), aLayerSettings.Origin().Z());

    NCollection_Mat4<Standard_ShortReal> aWorldViewF;
    aWorldViewF.ConvertFrom (aWorldView);
    aCtx->WorldViewState.SetCurrent (aWorldViewF);
    aCtx->ShaderManager()->UpdateClippingState();
    aCtx->ShaderManager()->UpdateLightSourceState();
  }

  // render priority list
  const Standard_Integer aViewId = theWorkspace->View()->Identification();
  for (Graphic3d_ArrayOfIndexedMapOfStructure::Iterator aMapIter (theLayer.ArrayOfStructures()); aMapIter.More(); aMapIter.Next())
  {
    const Graphic3d_IndexedMapOfStructure& aStructures = aMapIter.Value();
    for (OpenGl_Structure::StructIterator aStructIter (aStructures); aStructIter.More(); aStructIter.Next())
    {
      const OpenGl_Structure* aStruct = aStructIter.Value();
      if (aStruct->IsCulled()
      || !aStruct->IsVisible (aViewId))
      {
        continue;
      }

      aStruct->Render (theWorkspace);
    }
  }

  if (hasOwnLights)
  {
    aManager->UpdateLightSourceStateTo (aLightsBack);
  }
  if (hasLocalCS)
  {
    aCtx->ShaderManager()->RevertClippingState();
    aCtx->ShaderManager()->UpdateLightSourceState();

    aCtx->WorldViewState.SetCurrent (aWorldCamera->OrientationMatrixF());
    theWorkspace->View() ->SetLocalOrigin (gp_XYZ (0.0, 0.0, 0.0));
  }

  // always restore polygon offset between layers rendering
  theWorkspace->SetDefaultPolygonOffset (anAppliedOffsetParams);

  // restore environment texture
  if (!aLayerSettings.UseEnvironmentTexture())
  {
    theWorkspace->SetEnvironmentTexture (anEnvironmentTexture);
  }
}

//=======================================================================
//function : Render
//purpose  :
//=======================================================================
void OpenGl_LayerList::Render (const Handle(OpenGl_Workspace)& theWorkspace,
                               const Standard_Boolean          theToDrawImmediate,
                               const OpenGl_LayerFilter        theLayersToProcess,
                               OpenGl_FrameBuffer*             theReadDrawFbo,
                               OpenGl_FrameBuffer*             theOitAccumFbo) const
{
  // Remember global settings for glDepth function and write mask.
  OpenGl_GlobalLayerSettings aPrevSettings;

  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  aCtx->core11fwd->glGetIntegerv (GL_DEPTH_FUNC,      &aPrevSettings.DepthFunc);
  aCtx->core11fwd->glGetBooleanv (GL_DEPTH_WRITEMASK, &aPrevSettings.DepthMask);
  OpenGl_GlobalLayerSettings aDefaultSettings = aPrevSettings;

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
  const Standard_Integer aPrevFilter = theWorkspace->RenderFilter() & ~(Standard_Integer )(OpenGl_RenderFilter_OpaqueOnly | OpenGl_RenderFilter_TransparentOnly);
  theWorkspace->SetRenderFilter (aPrevFilter | OpenGl_RenderFilter_OpaqueOnly);

  myTransparentToProcess.Clear();

  OpenGl_LayerStack::iterator aStackIter (myTransparentToProcess.Origin());
  Standard_Integer aClearDepthLayerPrev = -1, aClearDepthLayer = -1;
  const bool toPerformDepthPrepass = theWorkspace->View()->RenderingParams().ToEnableDepthPrepass
                                  && aPrevSettings.DepthMask == GL_TRUE;
  const Handle(Graphic3d_LightSet) aLightsBack = aCtx->ShaderManager()->LightSourceState().LightSources();
  for (OpenGl_FilteredIndexedLayerIterator aLayerIterStart (myLayers, myDefaultLayerIndex, theToDrawImmediate, theLayersToProcess); aLayerIterStart.More();)
  {
    bool hasSkippedDepthLayers = false;
    for (int aPassIter = toPerformDepthPrepass ? 0 : 2; aPassIter < 3; ++aPassIter)
    {
      if (aPassIter == 0)
      {
        aCtx->SetColorMask (false);
        aCtx->ShaderManager()->UpdateLightSourceStateTo (Handle(Graphic3d_LightSet)());
        aDefaultSettings.DepthFunc = aPrevSettings.DepthFunc;
        aDefaultSettings.DepthMask = GL_TRUE;
      }
      else if (aPassIter == 1)
      {
        if (!hasSkippedDepthLayers)
        {
          continue;
        }
        aCtx->SetColorMask (true);
        aCtx->ShaderManager()->UpdateLightSourceStateTo (aLightsBack);
        aDefaultSettings = aPrevSettings;
      }
      else if (aPassIter == 2)
      {
        aCtx->SetColorMask (true);
        aCtx->ShaderManager()->UpdateLightSourceStateTo (aLightsBack);
        if (toPerformDepthPrepass)
        {
          aDefaultSettings.DepthFunc = GL_EQUAL;
          aDefaultSettings.DepthMask = GL_FALSE;
        }
      }

      OpenGl_FilteredIndexedLayerIterator aLayerIter (aLayerIterStart);
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
          // At this point the depth buffer may be set to clear by previous configuration of layers or configuration of the current layer.
          // Additional rendering pass to handle transparent elements of recently drawn layers require use of current depth
          // buffer so we put remaining layers for processing as one bunch before erasing the depth buffer.
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
        else if (aPassIter == 0
             && !aLayer.LayerSettings().ToRenderInDepthPrepass())
        {
          hasSkippedDepthLayers = true;
          continue;
        }
        else if (aPassIter == 1
              && aLayer.LayerSettings().ToRenderInDepthPrepass())
        {
          continue;
        }

        // Render opaque OpenGl elements of a layer and count the number of skipped.
        // If a layer has skipped (e.g. transparent) elements it should be added into
        // the transparency post-processing stack.
        theWorkspace->ResetSkippedCounter();

        renderLayer (theWorkspace, aDefaultSettings, aLayer);

        if (aPassIter != 0
         && theWorkspace->NbSkippedTransparentElements() > 0)
        {
          myTransparentToProcess.Push (&aLayer);
        }
      }
      if (aPassIter == 2
      && !aLayerIter.More())
      {
        aLayerIterStart = aLayerIter;
      }
    }

    if (!myTransparentToProcess.IsEmpty())
    {
      renderTransparent (theWorkspace, aStackIter, aPrevSettings, theReadDrawFbo, theOitAccumFbo);
    }
    if (aClearDepthLayer > aClearDepthLayerPrev)
    {
      aClearDepthLayerPrev = aClearDepthLayer;
      glDepthMask (GL_TRUE);
      glClear (GL_DEPTH_BUFFER_BIT);
    }
  }

  aCtx->core11fwd->glDepthMask (aPrevSettings.DepthMask);
  aCtx->core11fwd->glDepthFunc (aPrevSettings.DepthFunc);

  theWorkspace->SetRenderFilter (aPrevFilter);
}

//=======================================================================
//function : renderTransparent
//purpose  : Render transparent objects using blending operator.
//=======================================================================
void OpenGl_LayerList::renderTransparent (const Handle(OpenGl_Workspace)&   theWorkspace,
                                          OpenGl_LayerStack::iterator&      theLayerIter,
                                          const OpenGl_GlobalLayerSettings& theGlobalSettings,
                                          OpenGl_FrameBuffer*               theReadDrawFbo,
                                          OpenGl_FrameBuffer*               theOitAccumFbo) const
{
  // Blended order-independent transparency algorithm require several preconditions
  // to be enabled. It should be requested by user, at least two outputs from
  // fragment shader should be supported by GPU, so is the given framebuffer
  // should contain two additional color buffers to handle accumulated color channels,
  // blended alpha channel and weight factors - these accumulation buffers are required
  // to implement commuting blend operator (at least OpenGl 2.0 should be available).
  const bool isEnabledOit = theOitAccumFbo != NULL
                         && theOitAccumFbo->NbColorBuffers() >= 2
                         && theOitAccumFbo->ColorTexture (0)->IsValid()
                         && theOitAccumFbo->ColorTexture (1)->IsValid();

  // Check if current iterator has already reached the end of the stack.
  // This should happen if no additional layers has been added to
  // the processing stack after last transparency pass.
  if (theLayerIter == myTransparentToProcess.Back())
  {
    return;
  }

  const Handle(OpenGl_Context) aCtx            = theWorkspace->GetGlContext();
  const Handle(OpenGl_ShaderManager)& aManager = aCtx->ShaderManager();
  OpenGl_View* aView = theWorkspace->View();
  const float aDepthFactor =  aView != NULL ? aView->RenderingParams().OitDepthFactor : 0.0f;

  const Standard_Integer aPrevFilter = theWorkspace->RenderFilter() & ~(Standard_Integer )(OpenGl_RenderFilter_OpaqueOnly | OpenGl_RenderFilter_TransparentOnly);
  theWorkspace->SetRenderFilter (aPrevFilter | OpenGl_RenderFilter_TransparentOnly);

  aCtx->core11fwd->glEnable (GL_BLEND);

  if (isEnabledOit)
  {
    aManager->SetOitState (true, aDepthFactor);

    theOitAccumFbo->BindBuffer (aCtx);

    static const Standard_Integer aDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0 + 1 };
    aCtx->SetDrawBuffers (2, aDrawBuffers);
    aCtx->core11fwd->glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
    aCtx->core11fwd->glClear (GL_COLOR_BUFFER_BIT);
    aCtx->core15fwd->glBlendFuncSeparate (GL_ONE, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
  }
  else
  {
    aCtx->core11fwd->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  // During blended order-independent transparency pass the depth test
  // should be enabled to discard fragments covered by opaque geometry
  // and depth writing should be disabled, because transparent fragments
  // overal each other with non unitary coverage factor.
  OpenGl_GlobalLayerSettings aGlobalSettings = theGlobalSettings;
  aGlobalSettings.DepthMask   = GL_FALSE;
  aCtx->core11fwd->glDepthMask (GL_FALSE);

  for (; theLayerIter != myTransparentToProcess.Back(); ++theLayerIter)
  {
    renderLayer (theWorkspace, aGlobalSettings, *(*theLayerIter));
  }

  // Revert state of rendering.
  if (isEnabledOit)
  {
    aManager->SetOitState (false, aDepthFactor);
    theOitAccumFbo->UnbindBuffer (aCtx);
    if (theReadDrawFbo)
    {
      theReadDrawFbo->BindBuffer (aCtx);
    }

    static const Standard_Integer aDrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    aCtx->SetDrawBuffers (1, aDrawBuffers);
  }

  theWorkspace->SetRenderFilter (aPrevFilter | OpenGl_RenderFilter_OpaqueOnly);
  if (isEnabledOit)
  {
    const Standard_Boolean isMSAA = theReadDrawFbo && theReadDrawFbo->NbSamples() > 0;
    OpenGl_VertexBuffer*   aVerts = theWorkspace->View()->initBlitQuad (Standard_False);
    if (aVerts->IsValid() && aManager->BindOitCompositingProgram (isMSAA))
    {
      aCtx->core11fwd->glDepthFunc (GL_ALWAYS);
      aCtx->core11fwd->glDepthMask (GL_FALSE);

      // Bind full screen quad buffer and framebuffer resources.
      aVerts->BindVertexAttrib (aCtx, Graphic3d_TOA_POS);

      const Handle(OpenGl_TextureSet) aTextureBack = aCtx->BindTextures (Handle(OpenGl_TextureSet)());

      theOitAccumFbo->ColorTexture (0)->Bind (aCtx, Graphic3d_TextureUnit_0);
      theOitAccumFbo->ColorTexture (1)->Bind (aCtx, Graphic3d_TextureUnit_1);

      // Draw full screen quad with special shader to compose the buffers.
      aCtx->core11fwd->glBlendFunc (GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
      aCtx->core11fwd->glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);

      // Unbind OpenGL texture objects and shader program.
      aVerts->UnbindVertexAttrib (aCtx, Graphic3d_TOA_POS);
      theOitAccumFbo->ColorTexture (1)->Unbind (aCtx, Graphic3d_TextureUnit_1);
      theOitAccumFbo->ColorTexture (0)->Unbind (aCtx, Graphic3d_TextureUnit_0);
      aCtx->BindProgram (NULL);

      if (!aTextureBack.IsNull())
      {
        aCtx->BindTextures (aTextureBack);
      }
    }
    else
    {
      aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "Initialization of OIT compositing pass has failed.\n"
                         "  Blended order-independent transparency will not be available.\n");
      if (aView != NULL)
      {
        Standard_Boolean& aOITFlag = isMSAA ? aView->myToDisableOITMSAA : aView->myToDisableOIT;
        aOITFlag = Standard_True;
      }
    }
  }

  aCtx->core11fwd->glDisable (GL_BLEND);
  aCtx->core11fwd->glBlendFunc (GL_ONE, GL_ZERO);
  aCtx->core11fwd->glDepthMask (theGlobalSettings.DepthMask);
  aCtx->core11fwd->glDepthFunc (theGlobalSettings.DepthFunc);
}
