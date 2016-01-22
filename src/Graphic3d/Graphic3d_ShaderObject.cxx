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

#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <Standard_Atomic.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <Graphic3d_GraphicDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ShaderObject,Standard_Transient)

namespace
{
  static volatile Standard_Integer THE_SHADER_OBJECT_COUNTER = 0;
}


// =======================================================================
// function : Graphic3d_ShaderObject
// purpose  : Creates a shader object from specified file
// =======================================================================
Graphic3d_ShaderObject::Graphic3d_ShaderObject (const Graphic3d_TypeOfShaderObject theType)
: myType (theType)
{
  myID = TCollection_AsciiString ("Graphic3d_ShaderObject_")
       + TCollection_AsciiString (Standard_Atomic_Increment (&THE_SHADER_OBJECT_COUNTER));
}

// =======================================================================
// function : CreatFromFile
// purpose  : Creates new shader object from specified file
// =======================================================================
Handle(Graphic3d_ShaderObject) Graphic3d_ShaderObject::CreateFromFile (const Graphic3d_TypeOfShaderObject theType,
                                                                       const TCollection_AsciiString&     thePath)
{
  Handle(Graphic3d_ShaderObject) aShader = new Graphic3d_ShaderObject (theType);
  aShader->myPath = thePath;

  OSD_File aFile (thePath);
  if (!aFile.Exists())
  {
    return NULL;
  }

  aFile.Open (OSD_ReadOnly, OSD_Protection());
  aFile.Read (aShader->mySource, (int)aFile.Size());
  aFile.Close();

  return aShader;
}

// =======================================================================
// function : CreatFromSource
// purpose  : Creates new shader object from specified source
// =======================================================================
Handle(Graphic3d_ShaderObject) Graphic3d_ShaderObject::CreateFromSource (const Graphic3d_TypeOfShaderObject theType,
                                                                         const TCollection_AsciiString&     theSource)
{
  Handle(Graphic3d_ShaderObject) aShader = new Graphic3d_ShaderObject (theType);
  aShader->mySource = theSource;
  return aShader;
}

// =======================================================================
// function : ~Graphic3d_ShaderObject
// purpose  : Releases resources of shader object
// =======================================================================
Graphic3d_ShaderObject::~Graphic3d_ShaderObject()
{
  //
}

// =======================================================================
// function : IsDone
// purpose  : Checks if the shader object is valid or not
// =======================================================================
Standard_Boolean Graphic3d_ShaderObject::IsDone() const
{
  return !mySource.IsEmpty();
}
