// Created on: 2013-10-02
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_ShaderStates.hxx>

// =======================================================================
// function : OpenGl_StateInterface
// purpose  :
// =======================================================================
OpenGl_StateInterface::OpenGl_StateInterface()
: myIndex (0)
{
  //
}

// =======================================================================
// function : OpenGl_ProjectionState
// purpose  : Creates uninitialized projection state
// =======================================================================
OpenGl_ProjectionState::OpenGl_ProjectionState()
: myInverseNeedUpdate (false)
{
  //
}

// =======================================================================
// function : Set
// purpose  : Sets new OCCT projection state
// =======================================================================
void OpenGl_ProjectionState::Set (const OpenGl_Mat4& theProjectionMatrix)
{
  myProjectionMatrix = theProjectionMatrix;
  myInverseNeedUpdate = true;
}

// =======================================================================
// function : ProjectionMatrix
// purpose  : Returns current projection matrix
// =======================================================================
const OpenGl_Mat4& OpenGl_ProjectionState::ProjectionMatrix() const
{
  return myProjectionMatrix;
}

// =======================================================================
// function : ProjectionMatrixInverse
// purpose  : Returns inverse of current projection matrix
// =======================================================================
const OpenGl_Mat4& OpenGl_ProjectionState::ProjectionMatrixInverse() const
{
  if (!myInverseNeedUpdate)
  {
    return myProjectionMatrixInverse;
  }

  myProjectionMatrix.Inverted (myProjectionMatrixInverse);

  return myProjectionMatrixInverse;
}

// =======================================================================
// function : OpenGl_ModelWorldState
// purpose  : Creates uninitialized model-world state
// =======================================================================
OpenGl_ModelWorldState::OpenGl_ModelWorldState()
: myInverseNeedUpdate (false)
{
  //
}

// =======================================================================
// function : Set
// purpose  : Sets new model-world matrix
// =======================================================================
void OpenGl_ModelWorldState::Set (const OpenGl_Mat4& theModelWorldMatrix)
{
  myModelWorldMatrix = theModelWorldMatrix;
  myInverseNeedUpdate = true;
}

// =======================================================================
// function : ModelWorldMatrix
// purpose  : Returns current model-world matrix
// =======================================================================
const OpenGl_Mat4& OpenGl_ModelWorldState::ModelWorldMatrix() const
{
  return myModelWorldMatrix;
}

// =======================================================================
// function : ModelWorldMatrixInverse
// purpose  : Returns inverse of current model-world matrix
// =======================================================================
const OpenGl_Mat4& OpenGl_ModelWorldState::ModelWorldMatrixInverse() const
{
  if (!myInverseNeedUpdate)
  {
    return myModelWorldMatrix;
  }

  myModelWorldMatrix.Inverted (myModelWorldMatrixInverse);

  return myModelWorldMatrixInverse;
}

// =======================================================================
// function : OpenGl_WorldViewState
// purpose  : Creates uninitialized world-view state
// =======================================================================
OpenGl_WorldViewState::OpenGl_WorldViewState()
: myInverseNeedUpdate (false)
{
  //
}

// =======================================================================
// function : Set
// purpose  : Sets new world-view matrix
// =======================================================================
void OpenGl_WorldViewState::Set (const OpenGl_Mat4& theWorldViewMatrix)
{
  myWorldViewMatrix = theWorldViewMatrix;
  myInverseNeedUpdate = true;
}

// =======================================================================
// function : WorldViewMatrix
// purpose  : Returns current world-view matrix
// =======================================================================
const OpenGl_Mat4& OpenGl_WorldViewState::WorldViewMatrix() const
{
  return myWorldViewMatrix;
}

// =======================================================================
// function : WorldViewMatrixInverse
// purpose  : Returns inverse of current world-view matrix
// =======================================================================
const OpenGl_Mat4& OpenGl_WorldViewState::WorldViewMatrixInverse() const
{
  if (!myInverseNeedUpdate)
  {
    return myWorldViewMatrix;
  }

  myWorldViewMatrix.Inverted (myWorldViewMatrixInverse);

  return myWorldViewMatrixInverse;
}

// =======================================================================
// function : OpenGl_LightSourceState
// purpose  : Creates uninitialized state of light sources
// =======================================================================
OpenGl_LightSourceState::OpenGl_LightSourceState()
: myLightSources (NULL)
{
  //
}

// =======================================================================
// function : Set
// purpose  : Sets new light sources
// =======================================================================
void OpenGl_LightSourceState::Set (const OpenGl_ListOfLight* theLightSources)
{
  myLightSources = theLightSources;
}

// =======================================================================
// function : LightSources
// purpose  : Returns current list of light sources
// =======================================================================
const OpenGl_ListOfLight* OpenGl_LightSourceState::LightSources() const
{
  return myLightSources;
}

// =======================================================================
// function : OpenGl_ClippingState
// purpose  : Creates new clipping state
// =======================================================================
OpenGl_ClippingState::OpenGl_ClippingState()
: myIndex (0),
  myNextIndex (1)
{
  //
}

// =======================================================================
// function : Update
// purpose  : Updates current state
// =======================================================================
void OpenGl_ClippingState::Update()
{
  myStateStack.Prepend (myIndex);
  myIndex = myNextIndex; // use myNextIndex here to handle properly Update() after Revert()
  ++myNextIndex;
}

// =======================================================================
// function : Revert
// purpose  : Reverts current state
// =======================================================================
void OpenGl_ClippingState::Revert()
{
  if (!myStateStack.IsEmpty())
  {
    myIndex = myStateStack.First();
    myStateStack.RemoveFirst();
  }
  else
  {
    myIndex = 0;
  }
}
