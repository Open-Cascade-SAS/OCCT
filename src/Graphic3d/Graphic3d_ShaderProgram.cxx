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

#include <Graphic3d_ShaderProgram.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <Standard_Atomic.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ShaderProgram,Standard_Transient)

namespace
{
  static volatile Standard_Integer THE_PROGRAM_OBJECT_COUNTER = 0;
}

// =======================================================================
// function : ShadersFolder
// purpose  :
// =======================================================================
const TCollection_AsciiString& Graphic3d_ShaderProgram::ShadersFolder()
{
  static Standard_Boolean        THE_IS_DEFINED = Standard_False;
  static TCollection_AsciiString THE_SHADERS_FOLDER;
  if (!THE_IS_DEFINED)
  {
    THE_IS_DEFINED = Standard_True;
    OSD_Environment aDirEnv ("CSF_ShadersDirectory");
    THE_SHADERS_FOLDER = aDirEnv.Value();
    if (THE_SHADERS_FOLDER.IsEmpty())
    {
      OSD_Environment aCasRootEnv ("CASROOT");
      THE_SHADERS_FOLDER = aCasRootEnv.Value();
      if (!THE_SHADERS_FOLDER.IsEmpty())
      {
        THE_SHADERS_FOLDER += "/src/Shaders";
      }
    }

    if (THE_SHADERS_FOLDER.IsEmpty())
    {
      return THE_SHADERS_FOLDER;
    }

    const OSD_Path aDirPath (THE_SHADERS_FOLDER);
    OSD_Directory aDir (aDirPath);
    const TCollection_AsciiString aProgram = THE_SHADERS_FOLDER + "/Declarations.glsl";
    OSD_File aProgramFile (aProgram);
    if (!aDir.Exists()
     || !aProgramFile.Exists())
    {
      std::cerr << "Standard GLSL programs are not found in: " << THE_SHADERS_FOLDER.ToCString() << std::endl;
      throw Standard_Failure("CSF_ShadersDirectory or CASROOT is set incorrectly");
    }
  }
  return THE_SHADERS_FOLDER;
}

// =======================================================================
// function : Graphic3d_ShaderProgram
// purpose  : Creates new empty program object
// =======================================================================
Graphic3d_ShaderProgram::Graphic3d_ShaderProgram()
{
  myID = TCollection_AsciiString ("Graphic3d_ShaderProgram_")
       + TCollection_AsciiString (Standard_Atomic_Increment (&THE_PROGRAM_OBJECT_COUNTER));
}

// =======================================================================
// function : ~Graphic3d_ShaderProgram
// purpose  : Releases resources of program object
// =======================================================================
Graphic3d_ShaderProgram::~Graphic3d_ShaderProgram()
{
  //
}

// =======================================================================
// function : IsDone
// purpose  : Checks if the program object is valid or not
// =======================================================================
Standard_Boolean Graphic3d_ShaderProgram::IsDone() const
{
  if (myShaderObjects.IsEmpty())
  {
    return Standard_False;
  }

  for (Graphic3d_ShaderObjectList::Iterator anIt (myShaderObjects); anIt.More(); anIt.Next())
  {
    if (!anIt.Value()->IsDone())
      return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : AttachShader
// purpose  : Attaches shader object to the program object
// =======================================================================
Standard_Boolean Graphic3d_ShaderProgram::AttachShader (const Handle(Graphic3d_ShaderObject)& theShader)
{
  if (theShader.IsNull())
  {
    return Standard_False;
  }

  for (Graphic3d_ShaderObjectList::Iterator anIt (myShaderObjects); anIt.More(); anIt.Next())
  {
    if (anIt.Value() == theShader)
      return Standard_False;
  }

  myShaderObjects.Append (theShader);
  return Standard_True;
}

// =======================================================================
// function : DetachShader
// purpose  : Detaches shader object from the program object
// =======================================================================
Standard_Boolean Graphic3d_ShaderProgram::DetachShader (const Handle(Graphic3d_ShaderObject)& theShader)
{
  if (theShader.IsNull())
  {
    return Standard_False;
  }

  for (Graphic3d_ShaderObjectList::Iterator anIt (myShaderObjects); anIt.More(); anIt.Next())
  {
    if (anIt.Value() == theShader)
    {
      myShaderObjects.Remove (anIt);
      return Standard_True;
    }
  }
  
  return Standard_False;
}

// =======================================================================
// function : ClearVariables
// purpose  : Removes all custom uniform variables from the program
// =======================================================================
void Graphic3d_ShaderProgram::ClearVariables()
{
  myVariables.Clear();
}

// =======================================================================
// function : SetAttributes
// purpose  :
// =======================================================================
void Graphic3d_ShaderProgram::SetVertexAttributes (const Graphic3d_ShaderAttributeList& theAttributes)
{
  myAttributes = theAttributes;
}
