// Created on: 2013-09-20
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

#ifndef _Graphic3d_ShaderProgram_HeaderFile
#define _Graphic3d_ShaderProgram_HeaderFile

#include <Graphic3d_ShaderAttribute.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <Graphic3d_ShaderVariable.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <NCollection_Sequence.hxx>

//! List of shader objects.
typedef NCollection_Sequence<Handle(Graphic3d_ShaderObject)> Graphic3d_ShaderObjectList;

//! List of custom uniform shader variables.
typedef NCollection_Sequence<Handle(Graphic3d_ShaderVariable)> Graphic3d_ShaderVariableList;

//! List of custom vertex shader attrubures
typedef NCollection_Sequence<Handle(Graphic3d_ShaderAttribute)> Graphic3d_ShaderAttributeList;

//! This class is responsible for managing shader programs.
class Graphic3d_ShaderProgram : public Standard_Transient
{

public:

  //! Creates new empty program object.
  Standard_EXPORT Graphic3d_ShaderProgram();

  //! Releases resources of program object.
  Standard_EXPORT virtual ~Graphic3d_ShaderProgram();

  //! Checks if the program object is valid or not.
  Standard_EXPORT virtual Standard_Boolean IsDone() const;

  //! Returns unique ID used to manage resource in graphic driver.
  const TCollection_AsciiString& GetId() const { return myID; }

  //! Returns GLSL header (version code and extensions).
  const TCollection_AsciiString& Header() const { return myHeader; }

  //! Setup GLSL header containing language version code and used extensions.
  //! Will be prepended to the very beginning of the source code.
  //! Example:
  //! @code
  //!   #version 300 es
  //!   #extension GL_ARB_bindless_texture : require
  //! @endcode
  void SetHeader (const TCollection_AsciiString& theHeader) { myHeader = theHeader; }

  //! Attaches shader object to the program object.
  Standard_EXPORT Standard_Boolean AttachShader (const Handle(Graphic3d_ShaderObject)& theShader);

  //! Detaches shader object from the program object.
  Standard_EXPORT Standard_Boolean DetachShader (const Handle(Graphic3d_ShaderObject)& theShader);

  //! Returns list of attached shader objects.
  const Graphic3d_ShaderObjectList& ShaderObjects() const { return myShaderObjects; }

  //! The list of currently pushed but not applied custom uniform variables.
  //! This list is automatically cleared after applying to GLSL program.
  const Graphic3d_ShaderVariableList& Variables() const { return myVariables; }

  //! Return the list of custom vertex attributes.
  const Graphic3d_ShaderAttributeList& VertexAttributes() const { return myAttributes; }

  //! Assign the list of custom vertex attributes.
  //! Should be done before GLSL program initialization.
  Standard_EXPORT void SetVertexAttributes (const Graphic3d_ShaderAttributeList& theAttributes);

  //! Pushes custom uniform variable to the program.
  //! The list of pushed variables is automatically cleared after applying to GLSL program.
  //! Thus after program recreation even unchanged uniforms should be pushed anew.
  template<class T>
  Standard_Boolean PushVariable (const TCollection_AsciiString& theName,
                                 const T&                       theValue);

  //! Removes all custom uniform variables from the program.
  Standard_EXPORT void ClearVariables();

  //! Pushes float uniform.
  Standard_Boolean PushVariableFloat (const TCollection_AsciiString& theName, const float theValue)            { return PushVariable (theName, theValue); }

  //! Pushes vec2 uniform.
  Standard_Boolean PushVariableVec2  (const TCollection_AsciiString& theName, const Graphic3d_Vec2& theValue)  { return PushVariable (theName, theValue); }

  //! Pushes vec3 uniform.
  Standard_Boolean PushVariableVec3  (const TCollection_AsciiString& theName, const Graphic3d_Vec3& theValue)  { return PushVariable (theName, theValue); }

  //! Pushes vec4 uniform.
  Standard_Boolean PushVariableVec4  (const TCollection_AsciiString& theName, const Graphic3d_Vec4& theValue)  { return PushVariable (theName, theValue); }

  //! Pushes int uniform.
  Standard_Boolean PushVariableInt   (const TCollection_AsciiString& theName, const int theValue)              { return PushVariable (theName, theValue); }

  //! Pushes vec2i uniform.
  Standard_Boolean PushVariableVec2i (const TCollection_AsciiString& theName, const Graphic3d_Vec2i& theValue) { return PushVariable (theName, theValue); }

  //! Pushes vec3i uniform.
  Standard_Boolean PushVariableVec3i (const TCollection_AsciiString& theName, const Graphic3d_Vec3i& theValue) { return PushVariable (theName, theValue); }

  //! Pushes vec4i uniform.
  Standard_Boolean PushVariableVec4i (const TCollection_AsciiString& theName, const Graphic3d_Vec4i& theValue) { return PushVariable (theName, theValue); }

public:

  //! The path to GLSL programs determined from CSF_ShadersDirectory or CASROOT environment variables.
  //! @return the root folder with default GLSL programs.
  Standard_EXPORT static const TCollection_AsciiString& ShadersFolder();

public:

  DEFINE_STANDARD_RTTIEXT(Graphic3d_ShaderProgram,Standard_Transient)

private:

  TCollection_AsciiString       myID;            //!< the unique identifier of program object
  Graphic3d_ShaderObjectList    myShaderObjects; //!< the list of attached shader objects
  Graphic3d_ShaderVariableList  myVariables;     //!< the list of custom uniform variables
  Graphic3d_ShaderAttributeList myAttributes;    //!< the list of custom vertex attributes
  TCollection_AsciiString       myHeader;        //!< GLSL header with version code and used extensions

};

DEFINE_STANDARD_HANDLE (Graphic3d_ShaderProgram, Standard_Transient)

// =======================================================================
// function : PushVariable
// purpose  : Pushes custom uniform variable to the program
// =======================================================================
template<class T> inline
Standard_Boolean Graphic3d_ShaderProgram::PushVariable (const TCollection_AsciiString& theName,
                                                        const T& theValue)
{
  Handle(Graphic3d_ShaderVariable) aVariable = Graphic3d_ShaderVariable::Create (theName, theValue);
  if (aVariable.IsNull() || !aVariable->IsDone())
  {
    return Standard_False;
  }

  myVariables.Append (aVariable);
  return Standard_True;
}

#endif
