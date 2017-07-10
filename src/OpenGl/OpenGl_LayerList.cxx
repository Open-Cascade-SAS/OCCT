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
  myModifStateOfRaytraceable (0),
  myRenderOpaqueFilter (new OpenGl_OpaqueFilter()),
  myRenderTranspFilter (new OpenGl_TransparentFilter())
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
  Standard_Integer aSeqId = 1;
  for (OpenGl_SequenceOfLayers::Iterator anIts (myLayers); anIts.More(); anIts.Next(), ++aSeqId)
  {
    OpenGl_Layer& aLayerEx = *anIts.ChangeValue();
    if (aSeqPos == aSeqId)
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

      if (aSeqId == myDefaultLayerIndex
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
  Standard_Integer aSeqId = 1;
  for (OpenGl_SequenceOfLayers::Iterator anIts (myLayers); anIts.More(); anIts.Next(), ++aSeqId)
  {
    if (aSeqPos == aSeqId)
    {
      continue;
    }
  
    // try to remove structure and get priority value from this layer
    OpenGl_Layer& aLayerEx = *anIts.ChangeValue();
    if (aLayerEx.Remove (theStructure, aPriority, Standard_True))
    {
      if (aSeqId == myDefaultLayerIndex
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

  Standard_Integer aSeqId = 1;
  for (OpenGl_SequenceOfLayers::Iterator anIts (myLayers); anIts.More(); anIts.Next(), ++aSeqId)
  {
    if (aSeqPos == aSeqId)
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
  OpenGl_GlobalLayerSettings aDefaultSettings;

  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  aCtx->core11fwd->glGetIntegerv (GL_DEPTH_FUNC,      &aDefaultSettings.DepthFunc);
  aCtx->core11fwd->glGetBooleanv (GL_DEPTH_WRITEMASK, &aDefaultSettings.DepthMask);

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
  const Handle(OpenGl_RenderFilter) aPrevFilter = theWorkspace->GetRenderFilter();
  myRenderOpaqueFilter->SetPreviousFilter (aPrevFilter);
  myRenderTranspFilter->SetPreviousFilter (aPrevFilter);
  theWorkspace->SetRenderFilter (myRenderOpaqueFilter);

  myTransparentToProcess.Clear();

  OpenGl_LayerStack::iterator aStackIter (myTransparentToProcess.Origin());
  Standard_Integer aSeqId = myLayers.Lower();
  bool toClearDepth = false;
  for (OpenGl_SequenceOfLayers::Iterator aLayerIter (myLayers); aLayerIter.More(); aLayerIter.Next(), ++aSeqId)
  {
    if (theLayersToProcess == OpenGl_LF_Bottom)
    {
      if (aSeqId >= myDefaultLayerIndex) continue;
    }
    else if (theLayersToProcess == OpenGl_LF_Upper)
    {
      if (aSeqId <= myDefaultLayerIndex) continue;
    }
    else if (theLayersToProcess == OpenGl_LF_Default)
    {
      if (aSeqId != myDefaultLayerIndex) continue;
    }

    const OpenGl_Layer& aLayer = *aLayerIter.Value();
    if (aLayer.IsImmediate() != theToDrawImmediate)
    {
      continue;
    }
    else if (aLayer.NbStructures() < 1)
    {
      // Make sure to clear depth of previous layers even if layer has no structures.
      toClearDepth = toClearDepth || aLayer.LayerSettings().ToClearDepth();
      continue;
    }

    // At this point the depth buffer may be set to clear by
    // previous configuration of layers or configuration of the
    // current layer. Additional rendering pass to handle transparent
    // elements of recently drawn layers require use of current depth
    // buffer so we put remaining layers for processing as one bunch before
    // erasing the depth buffer.
    if (toClearDepth
     || aLayer.LayerSettings().ToClearDepth())
    {
      if (!myTransparentToProcess.IsEmpty())
      {
        renderTransparent (theWorkspace, aStackIter, aDefaultSettings, theReadDrawFbo, theOitAccumFbo);
      }

      toClearDepth = false;
      glDepthMask (GL_TRUE);
      glClear (GL_DEPTH_BUFFER_BIT);
    }

    // Render opaque OpenGl elements of a layer and count the number of skipped.
    // If a layer has skipped (e.g. transparent) elements it should be added into
    // the transparency post-processing stack.
    myRenderOpaqueFilter->SetSkippedCounter (0);

    aLayer.Render (theWorkspace, aDefaultSettings);

    if (myRenderOpaqueFilter->NbSkipped() > 0)
    {
      myTransparentToProcess.Push (&aLayer);
    }
  }

  // Before finishing process the remaining collected layers with transparency.
  if (!myTransparentToProcess.IsEmpty())
  {
    renderTransparent (theWorkspace, aStackIter, aDefaultSettings, theReadDrawFbo, theOitAccumFbo);
  }

  if (toClearDepth)
  {
    glDepthMask (GL_TRUE);
    glClear (GL_DEPTH_BUFFER_BIT);
  }

  aCtx->core11fwd->glDepthMask (aDefaultSettings.DepthMask);
  aCtx->core11fwd->glDepthFunc (aDefaultSettings.DepthFunc);

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

  theWorkspace->SetRenderFilter (myRenderTranspFilter);

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
    (*theLayerIter)->Render (theWorkspace, aGlobalSettings);
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

  theWorkspace->SetRenderFilter (myRenderOpaqueFilter);
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
      theOitAccumFbo->ColorTexture (0)->Unbind (aCtx, Graphic3d_TextureUnit_0);
      theOitAccumFbo->ColorTexture (1)->Unbind (aCtx, Graphic3d_TextureUnit_1);
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

//=======================================================================
//class    : OpenGl_OpaqueFilter
//function : ShouldRender
//purpose  : Checks whether the element should be rendered or skipped.
//=======================================================================
Standard_Boolean OpenGl_LayerList::OpenGl_OpaqueFilter::ShouldRender (const Handle(OpenGl_Workspace)& theWorkspace,
                                                                      const OpenGl_Element*           theGlElement)
{
  if (!myFilter.IsNull()
   && !myFilter->ShouldRender (theWorkspace, theGlElement))
  {
    return Standard_False;
  }

  const OpenGl_PrimitiveArray* aPArray = dynamic_cast<const OpenGl_PrimitiveArray*> (theGlElement);
  if (aPArray == NULL
   || aPArray->DrawMode() < OpenGl_PrimitiveArray::THE_FILLPRIM_FROM
   || aPArray->DrawMode() > OpenGl_PrimitiveArray::THE_FILLPRIM_TO)
  {
    return Standard_True;
  }

  if (OpenGl_Context::CheckIsTransparent (theWorkspace->AspectFace(),
                                          theWorkspace->HighlightStyle()))
  {
    ++mySkippedCounter;
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//class    : OpenGl_TransparentFilter
//function : ShouldRender
//purpose  : Checks whether the element should be rendered or skipped.
//=======================================================================
Standard_Boolean OpenGl_LayerList::OpenGl_TransparentFilter::ShouldRender (const Handle(OpenGl_Workspace)& theWorkspace,
                                                                           const OpenGl_Element*           theGlElement)
{
  if (!myFilter.IsNull()
   && !myFilter->ShouldRender (theWorkspace, theGlElement))
  {
    return Standard_False;
  }

  const OpenGl_PrimitiveArray* aPArray = dynamic_cast<const OpenGl_PrimitiveArray*> (theGlElement);
  if (aPArray == NULL
   || aPArray->DrawMode() < OpenGl_PrimitiveArray::THE_FILLPRIM_FROM
   || aPArray->DrawMode() > OpenGl_PrimitiveArray::THE_FILLPRIM_TO)
  {
    return dynamic_cast<const OpenGl_AspectFace*> (theGlElement) != NULL;
  }

  return OpenGl_Context::CheckIsTransparent (theWorkspace->AspectFace(),
                                             theWorkspace->HighlightStyle());
}
