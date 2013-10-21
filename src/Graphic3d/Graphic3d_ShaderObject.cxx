// Created on: 2013-09-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <Standard_Atomic.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <Graphic3d_GraphicDriver.hxx>

namespace
{
  static volatile Standard_Integer THE_SHADER_OBJECT_COUNTER = 0;
};

IMPLEMENT_STANDARD_HANDLE (Graphic3d_ShaderObject, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ShaderObject, Standard_Transient)

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
  aFile.Read (aShader->mySource, aFile.Size());
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
