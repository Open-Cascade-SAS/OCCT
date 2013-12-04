// Created on: 2013-09-26
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _OpenGl_ShaderManager_HeaderFile
#define _OpenGl_ShaderManager_HeaderFile

#include <Graphic3d_ShaderProgram_Handle.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>

#include <Handle_OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_ShaderStates.hxx>

//! List of shader programs.
typedef NCollection_Sequence<Handle(OpenGl_ShaderProgram)> OpenGl_ShaderProgramList;

//! Map to declare per-program states of OCCT materials.
typedef NCollection_DataMap<Handle(OpenGl_ShaderProgram), OpenGl_MaterialState> OpenGl_MaterialStates;

//! This class is responsible for managing shader programs.
class OpenGl_ShaderManager : public Standard_Transient
{
  friend class OpenGl_ShaderProgram;

public:

  //! Creates new empty shader manager.
  Standard_EXPORT OpenGl_ShaderManager (OpenGl_Context* theContext);

  //! Releases resources of shader manager.
  Standard_EXPORT virtual ~OpenGl_ShaderManager();

  //! Creates new shader program or re-use shared instance.
  //! @param theProxy    [IN]  program definition
  //! @param theShareKey [OUT] sharing key
  //! @param theProgram  [OUT] OpenGL program
  Standard_EXPORT void Create (const Handle(Graphic3d_ShaderProgram)& theProxy,
                               TCollection_AsciiString&               theShareKey,
                               Handle(OpenGl_ShaderProgram)&          theProgram);

  //! Unregisters specified shader program.
  Standard_EXPORT void Unregister (TCollection_AsciiString&      theShareKey,
                                   Handle(OpenGl_ShaderProgram)& theProgram);

  //! Returns list of registered shader programs.
  Standard_EXPORT const OpenGl_ShaderProgramList& ShaderPrograms() const;

  //! Returns true if no program objects are registered in the manager.
  Standard_EXPORT Standard_Boolean IsEmpty() const;

  DEFINE_STANDARD_RTTI (OpenGl_ShaderManager)

protected:

  OpenGl_MaterialStates   myMaterialStates;   //!< Per-program state of OCCT material
  OpenGl_ProjectionState  myProjectionState;  //!< State of OCCT projection transformation
  OpenGl_ModelWorldState  myModelWorldState;  //!< State of OCCT model-world transformation
  OpenGl_WorldViewState   myWorldViewState;   //!< State of OCCT world-view transformation
  OpenGl_LightSourceState myClippingState;    //!< State of OCCT clipping planes
  OpenGl_LightSourceState myLightSourceState; //!< State of OCCT light sources

public:

  //! Returns current state of OCCT light sources.
  Standard_EXPORT const OpenGl_LightSourceState& LightSourceState() const;

  //! Updates state of OCCT light sources.
  Standard_EXPORT void UpdateLightSourceStateTo (const OpenGl_ListOfLight* theLights);

  //! Pushes current state of OCCT light sources to specified program.
  Standard_EXPORT void PushLightSourceState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT projection transform.
  Standard_EXPORT const OpenGl_ProjectionState& ProjectionState() const;

  //! Updates state of OCCT projection transform.
  Standard_EXPORT void UpdateProjectionStateTo (const Tmatrix3& theProjectionMatrix);

  //! Reverts state of OCCT projection transform.
  Standard_EXPORT void RevertProjectionStateTo (const Tmatrix3& theProjectionMatrix);

  //! Pushes current state of OCCT projection transform to specified program.
  Standard_EXPORT void PushProjectionState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT model-world transform.
  Standard_EXPORT const OpenGl_ModelWorldState& ModelWorldState() const;

  //! Updates state of OCCT model-world transform.
  Standard_EXPORT void UpdateModelWorldStateTo (const Tmatrix3& theModelWorldMatrix);

  //! Reverts state of OCCT model-world transform.
  Standard_EXPORT void RevertModelWorldStateTo (const Tmatrix3& theModelWorldMatrix);

  //! Pushes current state of OCCT model-world transform to specified program.
  Standard_EXPORT void PushModelWorldState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT world-view transform.
  Standard_EXPORT const OpenGl_WorldViewState& WorldViewState() const;

  //! Updates state of OCCT world-view transform.
  Standard_EXPORT void UpdateWorldViewStateTo (const Tmatrix3& theWorldViewMatrix);

  //! Reverts state of OCCT world-view transform.
  Standard_EXPORT void RevertWorldViewStateTo (const Tmatrix3& theWorldViewMatrix);

  //! Pushes current state of OCCT world-view transform to specified program.
  Standard_EXPORT void PushWorldViewState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Updates state of OCCT clipping planes.
  Standard_EXPORT void UpdateClippingState();

  //! Reverts state of OCCT clipping planes.
  Standard_EXPORT void RevertClippingState();

  //! Pushes current state of OCCT clipping planes to specified program.
  Standard_EXPORT void PushClippingState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Resets state of OCCT material for all programs.
  Standard_EXPORT void ResetMaterialStates();

  //! Updates state of OCCT material for specified program.
  Standard_EXPORT void UpdateMaterialStateTo (const Handle(OpenGl_ShaderProgram)& theProgram,
                                              const OpenGl_Element*               theAspect);

  //! Pushes current state of OCCT material to specified program.
  Standard_EXPORT void PushMaterialState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

  //! Returns current state of OCCT material for specified program.
  Standard_EXPORT const OpenGl_MaterialState* MaterialState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:
  
  //! Pushes current state of OCCT graphics parameters to specified program.
  Standard_EXPORT void PushState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Overwrites context
  void SetContext (OpenGl_Context* theCtx)
  {
    myContext = theCtx;
  }

protected:

  OpenGl_ShaderProgramList myProgramList;  //!< The list of shader programs
  OpenGl_Context*          myContext;      //!< The OpenGL context

private:

  Standard_Boolean         myIsPP;         //!< Is any program object bound (programmable pipeline)?

};

#endif // _OpenGl_ShaderManager_HeaderFile
