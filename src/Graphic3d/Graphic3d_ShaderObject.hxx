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

#ifndef _Graphic3d_ShaderObject_HeaderFile
#define _Graphic3d_ShaderObject_HeaderFile

#include <Graphic3d_TypeOfShaderObject.hxx>
#include <NCollection_Sequence.hxx>
#include <OSD_Path.hxx>

//! Forward declaration

//! This class is responsible for managing shader objects.
class Graphic3d_ShaderObject : public Standard_Transient
{
public:
  //! Structure defining shader uniform or in/out variable.
  struct ShaderVariable
  {
    TCollection_AsciiString Name;   //!< variable name
    Standard_Integer        Stages; //!< active stages as Graphic3d_TypeOfShaderObject bits;
                                    //!  for in/out variables, intermediate stages will be automatically filled

    //! Create new shader variable.
    ShaderVariable (const TCollection_AsciiString& theVarName, Standard_Integer theShaderStageBits) : Name (theVarName), Stages (theShaderStageBits) {}

    //! Empty constructor.
    ShaderVariable() : Stages (0) {}
  };

  //! List of variable of shader program.
  typedef NCollection_Sequence<ShaderVariable> ShaderVariableList;

public:

  //! Creates new shader object from specified file.
  Standard_EXPORT static Handle(Graphic3d_ShaderObject) CreateFromFile (const Graphic3d_TypeOfShaderObject theType,
                                                                        const TCollection_AsciiString&     thePath);

  //! Creates new shader object from specified source.
  Standard_EXPORT static Handle(Graphic3d_ShaderObject) CreateFromSource (const Graphic3d_TypeOfShaderObject theType,
                                                                          const TCollection_AsciiString&     theSource);

  //! This is a preprocessor for Graphic3d_ShaderObject::CreateFromSource() function.
  //! Creates a new shader object from specified source according to list of uniforms and in/out variables.
  //! @param theSource      shader object source code to modify
  //! @param theType        shader object type to create
  //! @param theUniforms    list of uniform variables
  //! @param theStageInOuts list of stage in/out variables
  //! @param theInName      name of input  variables block;
  //!                       can be empty for accessing each variable without block prefix
  //!                       (mandatory for stages accessing both inputs and outputs)
  //! @param theOutName     name of output variables block;
  //!                       can be empty for accessing each variable without block prefix
  //!                       (mandatory for stages accessing both inputs and outputs)
  //! @param theNbGeomInputVerts number of geometry shader input vertexes
  Standard_EXPORT static Handle(Graphic3d_ShaderObject) CreateFromSource (TCollection_AsciiString& theSource,
                                                                          Graphic3d_TypeOfShaderObject theType,
                                                                          const ShaderVariableList& theUniforms,
                                                                          const ShaderVariableList& theStageInOuts,
                                                                          const TCollection_AsciiString& theInName  = TCollection_AsciiString(),
                                                                          const TCollection_AsciiString& theOutName = TCollection_AsciiString(),
                                                                          Standard_Integer theNbGeomInputVerts = 0);

private:

  //! Creates new shader object of specified type.
  Standard_EXPORT Graphic3d_ShaderObject (const Graphic3d_TypeOfShaderObject theType);

public:

  //! Releases resources of shader object. 
  Standard_EXPORT virtual ~Graphic3d_ShaderObject();
  
  //! Checks if the shader object is valid or not.
  Standard_EXPORT virtual Standard_Boolean IsDone() const;
  
  //! Returns the full path to the shader source.
  const OSD_Path& Path() const { return myPath; }

  //! Returns the source code of the shader object.
  const TCollection_AsciiString& Source() const { return mySource; }

  //! Returns type of the shader object.
  Graphic3d_TypeOfShaderObject Type() const { return myType; }
  
  //! Returns unique ID used to manage resource in graphic driver.
  const TCollection_AsciiString& GetId() const { return myID; }

public:

  DEFINE_STANDARD_RTTIEXT(Graphic3d_ShaderObject,Standard_Transient)

protected:

  TCollection_AsciiString myID;     //!< the ID of shader object
  TCollection_AsciiString mySource; //!< the source code of shader object
  OSD_Path                myPath;   //!< the path to shader source (may be empty)

private:
  
  //! The type of shader object.
  Graphic3d_TypeOfShaderObject myType;
};

DEFINE_STANDARD_HANDLE (Graphic3d_ShaderObject, Standard_Transient)

#endif
