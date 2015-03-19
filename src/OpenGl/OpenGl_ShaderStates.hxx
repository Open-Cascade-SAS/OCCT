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

#ifndef _OpenGl_State_HeaderFile
#define _OpenGl_State_HeaderFile

#include <InterfaceGraphic_tgl_all.hxx>
#include <NCollection_List.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_Light.hxx>
#include <OpenGl_Vec.hxx>
#include <Visual3d_TypeOfSurfaceDetail.hxx>

//! Defines interface for OpenGL state.
class OpenGl_StateInterface
{
public:

  //! Creates new state.
  OpenGl_StateInterface();

  //! Returns current state index.
  Standard_Size Index() const { return myIndex; }

  //! Increment current state.
  void Update() { ++myIndex; }

protected:

  Standard_Size myIndex; //!< current state index

};

//! Defines state of OCCT projection transformation.
class OpenGl_ProjectionState : public OpenGl_StateInterface
{
public:

  //! Creates uninitialized projection state.
  OpenGl_ProjectionState();

  //! Sets new projection matrix.
  void Set (const OpenGl_Mat4& theProjectionMatrix);

  //! Returns current projection matrix.
  const OpenGl_Mat4& ProjectionMatrix() const;

  //! Returns inverse of current projection matrix.
  const OpenGl_Mat4& ProjectionMatrixInverse() const;

private:

  OpenGl_Mat4         myProjectionMatrix;        //!< OCCT projection matrix
  mutable OpenGl_Mat4 myProjectionMatrixInverse; //!< Inverse of OCCT projection matrix
  bool                myInverseNeedUpdate;       //!< Is inversed matrix outdated?

};

//! Defines state of OCCT model-world transformation.
class OpenGl_ModelWorldState : public OpenGl_StateInterface
{
public:

  //! Creates uninitialized model-world state.
  OpenGl_ModelWorldState();

  //! Sets new model-world matrix.
  void Set (const OpenGl_Mat4& theModelWorldMatrix);

  //! Returns current model-world matrix.
  const OpenGl_Mat4& ModelWorldMatrix() const;

  //! Returns inverse of current model-world matrix.
  const OpenGl_Mat4& ModelWorldMatrixInverse() const;

private:

  OpenGl_Mat4         myModelWorldMatrix;        //!< OCCT model-world matrix
  mutable OpenGl_Mat4 myModelWorldMatrixInverse; //!< Inverse of OCCT model-world matrix
  bool                myInverseNeedUpdate;       //!< Is inversed matrix outdated?
  
};

//! Defines state of OCCT world-view transformation.
class OpenGl_WorldViewState : public OpenGl_StateInterface
{
public:

  //! Creates uninitialized world-view state.
  OpenGl_WorldViewState();

  //! Sets new world-view matrix.
  void Set (const OpenGl_Mat4& theWorldViewMatrix);

  //! Returns current world-view matrix.
  const OpenGl_Mat4& WorldViewMatrix() const;

  //! Returns inverse of current world-view matrix.
  const OpenGl_Mat4& WorldViewMatrixInverse() const;

private:

  OpenGl_Mat4         myWorldViewMatrix;        //!< OCCT world-view matrix
  mutable OpenGl_Mat4 myWorldViewMatrixInverse; //!< Inverse of OCCT world-view matrix
  bool                myInverseNeedUpdate;      //!< Is inversed matrix outdated?

};

//! Defines state of OCCT light sources.
class OpenGl_LightSourceState : public OpenGl_StateInterface
{
public:

  //! Creates uninitialized state of light sources.
  OpenGl_LightSourceState();

  //! Sets new light sources.
  void Set (const OpenGl_ListOfLight* theLightSources);

  //! Returns current list of light sources.
  const OpenGl_ListOfLight* LightSources() const;

private:

  const OpenGl_ListOfLight* myLightSources; //!< List of OCCT light sources

};

//! Defines generic state of OCCT material properties.
class OpenGl_MaterialState : public OpenGl_StateInterface
{
public:

  //! Creates new material state.
  OpenGl_MaterialState (const OpenGl_Element* theAspect = NULL);

  //! Sets new material aspect.
  void Set (const OpenGl_Element* theAspect);

  //! Returns material aspect.
  const OpenGl_Element* Aspect() const;

private:

  const OpenGl_Element* myAspect; //!< OCCT material aspect

};

//! Defines generic state of OCCT clipping state.
class OpenGl_ClippingState
{
public:

  //! Creates new clipping state.
  OpenGl_ClippingState();

  //! Returns current state index.
  Standard_Size Index() const { return myIndex; }

  //! Updates current state.
  void Update();

  //! Reverts current state.
  void Revert();

protected:

  Standard_Size                   myIndex;      //!< Current state index
  Standard_Size                   myNextIndex;  //!< Next    state index
  NCollection_List<Standard_Size> myStateStack; //!< Stack of previous states

};

//! Defines generic state of OCCT surface detail.
class OpenGl_SurfaceDetailState : public OpenGl_StateInterface
{
public:

  //! Creates new surface detail state.
  OpenGl_SurfaceDetailState (const Visual3d_TypeOfSurfaceDetail theDetail = Visual3d_TOD_NONE)
  : myDetail (theDetail)
  {
    //
  }

  //! Sets new surface detail.
  void Set (const Visual3d_TypeOfSurfaceDetail theDetail) { myDetail = theDetail; }

  //! Returns surface detail.
  const Visual3d_TypeOfSurfaceDetail Detail() const { return myDetail; }

private:

  Visual3d_TypeOfSurfaceDetail myDetail; //!< OCCT surface detail

};

#endif // _OpenGl_State_HeaderFile
