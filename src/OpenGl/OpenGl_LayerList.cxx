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

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_LayerList.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Workspace.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic.hxx>
#include <Graphic3d_GraphicDriver.hxx>

//=======================================================================
//function : OpenGl_LayerList
//purpose  : Constructor
//=======================================================================

OpenGl_LayerList::OpenGl_LayerList (const Standard_Integer theNbPriorities)
: myNbPriorities (theNbPriorities),
  myNbStructures (0),
  myImmediateNbStructures (0)
{
  // insert default priority layers
  myLayers.Append (OpenGl_Layer (myNbPriorities));
  myLayerIds.Bind (Graphic3d_ZLayerId_BotOSD,  myLayers.Upper());

  myLayers.Append (OpenGl_Layer (myNbPriorities));
  myLayerIds.Bind (Graphic3d_ZLayerId_Default, myLayers.Upper());

  myLayers.Append (OpenGl_Layer (myNbPriorities));
  myLayerIds.Bind (Graphic3d_ZLayerId_Top,     myLayers.Upper());

  myLayers.Append (OpenGl_Layer (myNbPriorities));
  myLayerIds.Bind (Graphic3d_ZLayerId_Topmost, myLayers.Upper());

  myLayers.Append (OpenGl_Layer (myNbPriorities));
  myLayerIds.Bind (Graphic3d_ZLayerId_TopOSD,  myLayers.Upper());
}

//=======================================================================
//function : ~OpenGl_LayerList
//purpose  : Destructor
//=======================================================================

OpenGl_LayerList::~OpenGl_LayerList()
{
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
  myLayers.Append (OpenGl_Layer (myNbPriorities));
  myLayerIds.Bind (theLayerId, myLayers.Length());
}

//=======================================================================
//function : Layer
//purpose  : 
//=======================================================================
OpenGl_Layer& OpenGl_LayerList::Layer (const Graphic3d_ZLayerId theLayerId)
{
  return myLayers.ChangeValue (myLayerIds.Find (theLayerId));
}

//=======================================================================
//function : Layer
//purpose  : 
//=======================================================================
const OpenGl_Layer& OpenGl_LayerList::Layer (const Graphic3d_ZLayerId theLayerId) const
{
  return myLayers.Value (myLayerIds.Find (theLayerId));
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
  const OpenGl_Layer& aLayerToMove = myLayers.Value (aRemovePos);
  myLayers.ChangeFirst().Append (aLayerToMove);

  // remove layer
  myLayers.Remove (aRemovePos);
  myLayerIds.UnBind (theLayerId);

  // updated sequence indexes in map
  OpenGl_LayerSeqIds::Iterator aMapIt (myLayerIds);
  for ( ; aMapIt.More (); aMapIt.Next ())
  {
    Standard_Integer& aSeqIdx = aMapIt.ChangeValue ();
    if (aSeqIdx > aRemovePos)
      aSeqIdx--;
  }
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

  OpenGl_Layer& aLayer = myLayers.ChangeValue (aSeqPos);
  aLayer.Add (theStruct, thePriority, isForChangePriority);
  ++myNbStructures;
  if (aLayer.LayerSettings().IsImmediate)
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

  OpenGl_Layer&    aLayer    = myLayers.ChangeValue (aSeqPos);
  Standard_Integer aPriority = -1;

  // remove structure from associated list
  // if the structure is not found there,
  // scan through layers and remove it
  if (aLayer.Remove (theStructure, aPriority))
  {
    --myNbStructures;
    if (aLayer.LayerSettings().IsImmediate)
    {
      --myImmediateNbStructures;
    }

    if (theStructure->IsRaytracable())
    {
      ++myModificationState;
    }

    return;
  }

  // scan through layers and remove it
  Standard_Integer aSeqId = 1;
  for (OpenGl_SequenceOfLayers::Iterator anIts (myLayers); anIts.More(); anIts.Next(), ++aSeqId)
  {
    OpenGl_Layer& aLayerEx = anIts.ChangeValue();
    if (aSeqPos == aSeqId)
    {
      continue;
    }

    if (aLayerEx.Remove (theStructure, aPriority))
    {
      --myNbStructures;
      if (aLayerEx.LayerSettings().IsImmediate)
      {
        --myImmediateNbStructures;
      }

      if (theStructure->IsRaytracable())
      {
        ++myModificationState;
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
  OpenGl_Layer& aLayer = myLayers.ChangeValue (aSeqPos);
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
  OpenGl_Layer&    aLayer    = myLayers.ChangeValue (aSeqPos);
  Standard_Integer aPriority = -1;

  // take priority and remove structure from list found by <theOldLayerId>
  // if the structure is not found there, scan through all other layers
  if (aLayer.Remove (theStructure, aPriority, Standard_True))
  {
    --myNbStructures;
    if (aLayer.LayerSettings().IsImmediate)
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
    OpenGl_Layer& aLayerEx = anIts.ChangeValue();
    if (aLayerEx.Remove (theStructure, aPriority, Standard_True))
    {
      --myNbStructures;
      if (aLayerEx.LayerSettings().IsImmediate)
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
  OpenGl_Layer&    aLayer        = myLayers.ChangeValue (aSeqPos);
  Standard_Integer anOldPriority = -1;

  if (aLayer.Remove (theStructure, anOldPriority, Standard_True))
  {
    --myNbStructures;
    if (aLayer.LayerSettings().IsImmediate)
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

    OpenGl_Layer& aLayerEx = anIts.ChangeValue();
    if (aLayerEx.Remove (theStructure, anOldPriority, Standard_True))
    {
      --myNbStructures;
      if (aLayerEx.LayerSettings().IsImmediate)
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
  if (aLayer.LayerSettings().IsImmediate != theSettings.IsImmediate)
  {
    if (theSettings.IsImmediate)
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
                               const OpenGl_LayerFilter        theLayersToProcess) const
{
  OpenGl_GlobalLayerSettings aDefaultSettings;

  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  aCtx->core11fwd->glGetIntegerv (GL_DEPTH_FUNC,      &aDefaultSettings.DepthFunc);
  aCtx->core11fwd->glGetBooleanv (GL_DEPTH_WRITEMASK, &aDefaultSettings.DepthMask);

  Standard_Integer aSeqId = myLayers.Lower(), aMainId = myLayerIds.Find (Graphic3d_ZLayerId_Default);
  for (OpenGl_SequenceOfLayers::Iterator anIts (myLayers); anIts.More(); anIts.Next(), ++aSeqId)
  {
    if (theLayersToProcess == OpenGl_LF_Bottom)
    {
      if (aSeqId >= aMainId) continue;
    }
    else if (theLayersToProcess == OpenGl_LF_Upper)
    {
      if (aSeqId <= aMainId) continue;
    }
    else if (theLayersToProcess == OpenGl_LF_Default)
    {
      if (aSeqId != aMainId) continue;
    }

    const OpenGl_Layer& aLayer = anIts.Value();
    if (aLayer.NbStructures() < 1)
    {
      continue;
    }
    else if (theToDrawImmediate)
    {
      if (!aLayer.LayerSettings().IsImmediate)
      {
        continue;
      }
    }
    else
    {
      if (aLayer.LayerSettings().IsImmediate)
      {
        continue;
      }
    }

    // render layer
    aLayer.Render (theWorkspace, aDefaultSettings);
  }

  aCtx->core11fwd->glDepthMask (aDefaultSettings.DepthMask);
  aCtx->core11fwd->glDepthFunc (aDefaultSettings.DepthFunc);
}
