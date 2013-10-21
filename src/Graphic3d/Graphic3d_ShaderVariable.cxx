// Created on: 2013-09-25
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

#include <Graphic3d_ShaderVariable.hxx>

IMPLEMENT_STANDARD_HANDLE (Graphic3d_ShaderVariable, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ShaderVariable, Standard_Transient)

//
// Specific instantiations of struct templates to avoid compilation warnings
//
template struct Graphic3d_UniformValue<int>;
template struct Graphic3d_UniformValue<float>;
template struct Graphic3d_UniformValue<OpenGl_Vec2>;
template struct Graphic3d_UniformValue<OpenGl_Vec3>;
template struct Graphic3d_UniformValue<OpenGl_Vec4>;
template struct Graphic3d_UniformValue<OpenGl_Vec2i>;
template struct Graphic3d_UniformValue<OpenGl_Vec3i>;
template struct Graphic3d_UniformValue<OpenGl_Vec4i>;

// =======================================================================
// function : ~Graphic3d_ValueInterface
// purpose  : Releases memory resources of variable value
// =======================================================================
Graphic3d_ValueInterface::~Graphic3d_ValueInterface()
{
  //
}

// =======================================================================
// function : Graphic3d_ShaderVariable
// purpose  : Creates new abstract shader variable
// =======================================================================
Graphic3d_ShaderVariable::Graphic3d_ShaderVariable (const TCollection_AsciiString& theName)
: myName (theName),
  myValue (NULL)
{
  //
}

// =======================================================================
// function : ~Graphic3d_ShaderVariableBase
// purpose  : Releases resources of shader variable
// =======================================================================
Graphic3d_ShaderVariable::~Graphic3d_ShaderVariable()
{
  delete myValue;
}

// =======================================================================
// function : IsDone
// purpose  : Checks if the shader variable is valid or not
// =======================================================================
Standard_Boolean Graphic3d_ShaderVariable::IsDone() const
{
  return !myName.IsEmpty() && (myValue != NULL);
}

// =======================================================================
// function : Name
// purpose  : Returns name of shader variable
// =======================================================================
const TCollection_AsciiString& Graphic3d_ShaderVariable::Name() const
{
  return myName;
}

// =======================================================================
// function : Value
// purpose  : Returns interface of shader variable value
// =======================================================================
Graphic3d_ValueInterface* Graphic3d_ShaderVariable::Value()
{
  return myValue;
}
