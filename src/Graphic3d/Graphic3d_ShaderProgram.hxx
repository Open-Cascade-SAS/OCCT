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

#include <Graphic3d_ShaderObject.hxx>
#include <Graphic3d_ShaderVariable.hxx>
#include <NCollection_Sequence.hxx>

//! List of shader objects.
typedef NCollection_Sequence<Handle(Graphic3d_ShaderObject)> Graphic3d_ShaderObjectList;

//! List of custom uniform shader variables.
typedef NCollection_Sequence<Handle(Graphic3d_ShaderVariable)> Graphic3d_ShaderVariableList;

//! This class is responsible for managing shader programs.
class Graphic3d_ShaderProgram : public Standard_Transient
{

public:

  //! The list of built-in GLSL programs
  enum ShaderName
  {
    ShaderName_UNKNOWN, //!< undefined program
    ShaderName_Phong    //!< per-pixel lighting (Phong shading)
  };

public:

  //! Creates new empty program object.
  Standard_EXPORT Graphic3d_ShaderProgram();

  //! Creates program object from pre-defined shaders.
  //! Raises Standard_Failure exception if shader resources are unavailable.
  Standard_EXPORT Graphic3d_ShaderProgram (const Graphic3d_ShaderProgram::ShaderName theName);

  //! Releases resources of program object.
  Standard_EXPORT virtual ~Graphic3d_ShaderProgram();

  //! Releases resources of program object.
  Standard_EXPORT void Destroy() const;

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

  //! Returns list of custom uniform variables.
  const Graphic3d_ShaderVariableList& Variables() const { return myVariables; }

  //! Pushes custom uniform variable to the program.
  template<class T>
  Standard_Boolean PushVariable (const TCollection_AsciiString& theName,
                                 const T&                       theValue);

  //! Removes all custom uniform variables from the program.
  Standard_EXPORT void ClearVariables();

public:

  //! The path to GLSL programs determined from CSF_ShadersDirectory or CASROOT environment variables.
  //! @return the root folder with default GLSL programs.
  Standard_EXPORT static const TCollection_AsciiString& ShadersFolder();

public:

  DEFINE_STANDARD_RTTI (Graphic3d_ShaderProgram)

private:

  TCollection_AsciiString      myID;            //!< The unique identifier of program object.
  Graphic3d_ShaderObjectList   myShaderObjects; //!< the list of attached shader objects.
  Graphic3d_ShaderVariableList myVariables;     //!< the list of custom uniform variables.
  TCollection_AsciiString      myHeader;        //!< GLSL header with version code and used extensions

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
