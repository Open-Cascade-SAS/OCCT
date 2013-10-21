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

#include <Standard_Atomic.hxx>
#include <Standard_Assert.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <Graphic3d_ShaderProgram.hxx>

namespace
{
  static volatile Standard_Integer THE_PROGRAM_OBJECT_COUNTER = 0;
};

IMPLEMENT_STANDARD_HANDLE (Graphic3d_ShaderProgram, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ShaderProgram, Standard_Transient)

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
  Destroy();
}

// =======================================================================
// function : Destroy
// purpose  : Releases resources of program object
// =======================================================================
void Graphic3d_ShaderProgram::Destroy() const
{ }

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
