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

#ifndef OpenGl_ShaderStates_HeaderFile
#define OpenGl_ShaderStates_HeaderFile

#include <Graphic3d_RenderTransparentMethod.hxx>
#include <Graphic3d_LightSet.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_Vec.hxx>

class OpenGl_ShadowMapArray;

//! Defines interface for OpenGL state.
class OpenGl_StateInterface
{
public:
  //! Creates new state.
  Standard_EXPORT OpenGl_StateInterface();

  //! Returns current state index.
  size_t Index() const { return myIndex; }

  //! Increment current state.
  void Update() { ++myIndex; }

protected:
  size_t myIndex; //!< current state index
};

//! Defines state of OCCT projection transformation.
class OpenGl_ProjectionState : public OpenGl_StateInterface
{
public:
  //! Creates uninitialized projection state.
  Standard_EXPORT OpenGl_ProjectionState();

  //! Sets new projection matrix.
  Standard_EXPORT void Set(const NCollection_Mat4<float>& theProjectionMatrix);

  //! Returns current projection matrix.
  const NCollection_Mat4<float>& ProjectionMatrix() const { return myProjectionMatrix; }

  //! Returns inverse of current projection matrix.
  Standard_EXPORT const NCollection_Mat4<float>& ProjectionMatrixInverse() const;

private:
  NCollection_Mat4<float>         myProjectionMatrix;        //!< OCCT projection matrix
  mutable NCollection_Mat4<float> myProjectionMatrixInverse; //!< Inverse of OCCT projection matrix
  mutable bool                    myInverseNeedUpdate;       //!< Is inversed matrix outdated?
};

//! Defines state of OCCT model-world transformation.
class OpenGl_ModelWorldState : public OpenGl_StateInterface
{
public:
  //! Creates uninitialized model-world state.
  Standard_EXPORT OpenGl_ModelWorldState();

  //! Sets new model-world matrix.
  Standard_EXPORT void Set(const NCollection_Mat4<float>& theModelWorldMatrix);

  //! Returns current model-world matrix.
  const NCollection_Mat4<float>& ModelWorldMatrix() const { return myModelWorldMatrix; }

  //! Returns inverse of current model-world matrix.
  Standard_EXPORT const NCollection_Mat4<float>& ModelWorldMatrixInverse() const;

private:
  NCollection_Mat4<float>         myModelWorldMatrix;        //!< OCCT model-world matrix
  mutable NCollection_Mat4<float> myModelWorldMatrixInverse; //!< Inverse of OCCT model-world matrix
  mutable bool                    myInverseNeedUpdate;       //!< Is inversed matrix outdated?
};

//! Defines state of OCCT world-view transformation.
class OpenGl_WorldViewState : public OpenGl_StateInterface
{
public:
  //! Creates uninitialized world-view state.
  Standard_EXPORT OpenGl_WorldViewState();

  //! Sets new world-view matrix.
  Standard_EXPORT void Set(const NCollection_Mat4<float>& theWorldViewMatrix);

  //! Returns current world-view matrix.
  const NCollection_Mat4<float>& WorldViewMatrix() const { return myWorldViewMatrix; }

  //! Returns inverse of current world-view matrix.
  Standard_EXPORT const NCollection_Mat4<float>& WorldViewMatrixInverse() const;

private:
  NCollection_Mat4<float>         myWorldViewMatrix;        //!< OCCT world-view matrix
  mutable NCollection_Mat4<float> myWorldViewMatrixInverse; //!< Inverse of OCCT world-view matrix
  mutable bool                    myInverseNeedUpdate;      //!< Is inversed matrix outdated?
};

//! Defines state of OCCT light sources.
class OpenGl_LightSourceState : public OpenGl_StateInterface
{
public:
  //! Creates uninitialized state of light sources.
  OpenGl_LightSourceState()
      : mySpecIBLMapLevels(0),
        myToCastShadows(true)
  {
  }

  //! Sets new light sources.
  void Set(const occ::handle<Graphic3d_LightSet>& theLightSources)
  {
    myLightSources = theLightSources;
  }

  //! Returns current list of light sources.
  const occ::handle<Graphic3d_LightSet>& LightSources() const { return myLightSources; }

  //! Returns number of mipmap levels used in specular IBL map.
  //! 0 by default or in case of using non-PBR shading model.
  int SpecIBLMapLevels() const { return mySpecIBLMapLevels; }

  //! Sets number of mipmap levels used in specular IBL map.
  void SetSpecIBLMapLevels(int theSpecIBLMapLevels) { mySpecIBLMapLevels = theSpecIBLMapLevels; }

  //! Returns TRUE if shadowmap is set.
  bool HasShadowMaps() const { return myToCastShadows && !myShadowMaps.IsNull(); }

  //! Returns shadowmap.
  const occ::handle<OpenGl_ShadowMapArray>& ShadowMaps() const { return myShadowMaps; }

  //! Sets shadowmap.
  void SetShadowMaps(const occ::handle<OpenGl_ShadowMapArray>& theMap) { myShadowMaps = theMap; }

  //! Returns TRUE if shadowmap should be enabled when available; TRUE by default.
  bool ToCastShadows() const { return myToCastShadows; }

  //! Set if shadowmap should be enabled when available.
  void SetCastShadows(bool theToCast) { myToCastShadows = theToCast; }

private:
  occ::handle<Graphic3d_LightSet> myLightSources;     //!< List of OCCT light sources
                                                      // clang-format off
  int           mySpecIBLMapLevels; //!< Number of mipmap levels used in specular IBL map (0 by default or in case of using non-PBR shading model)
                                                      // clang-format on
  occ::handle<OpenGl_ShadowMapArray> myShadowMaps;    //!< active shadowmap
  bool                               myToCastShadows; //!< enable/disable shadowmap
};

//! Defines generic state of OCCT clipping state.
class OpenGl_ClippingState
{
public:
  //! Creates new clipping state.
  Standard_EXPORT OpenGl_ClippingState();

  //! Returns current state index.
  size_t Index() const { return myIndex; }

  //! Updates current state.
  Standard_EXPORT void Update();

  //! Reverts current state.
  Standard_EXPORT void Revert();

protected:
  size_t                   myIndex;      //!< Current state index
  size_t                   myNextIndex;  //!< Next    state index
  NCollection_List<size_t> myStateStack; //!< Stack of previous states
};

//! Defines generic state of order-independent transparency rendering properties.
class OpenGl_OitState : public OpenGl_StateInterface
{
public:
  //! Creates new uniform state.
  OpenGl_OitState()
      : myOitMode(Graphic3d_RTM_BLEND_UNORDERED),
        myDepthFactor(0.5f)
  {
  }

  //! Sets the uniform values.
  //! @param[in] theToEnableWrite  flag indicating whether color and coverage
  //!  values for OIT processing should be written by shader program.
  //! @param[in] theDepthFactor  scalar factor [0-1] defining influence of depth
  //!  component of a fragment to its final coverage coefficient.
  void Set(Graphic3d_RenderTransparentMethod theMode, const float theDepthFactor)
  {
    myOitMode     = theMode;
    myDepthFactor = static_cast<float>((std::max)(0.f, (std::min)(1.f, theDepthFactor)));
  }

  //! Returns flag indicating whether writing of output for OIT processing
  //! should be enabled/disabled.
  Graphic3d_RenderTransparentMethod ActiveMode() const { return myOitMode; }

  //! Returns factor defining influence of depth component of a fragment
  //! to its final coverage coefficient.
  float DepthFactor() const { return myDepthFactor; }

private:
  // clang-format off
  Graphic3d_RenderTransparentMethod myOitMode;     //!< active OIT method for the main GLSL program
  // clang-format on
  float myDepthFactor; //!< factor of depth influence to coverage
};

#endif // _OpenGl_State_HeaderFile
