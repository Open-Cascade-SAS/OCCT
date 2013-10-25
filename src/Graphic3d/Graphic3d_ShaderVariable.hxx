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

#ifndef _Graphic3d_ShaderVariable_HeaderFile
#define _Graphic3d_ShaderVariable_HeaderFile

#include <Graphic3d_Vec.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

#include <Graphic3d_ShaderVariable_Handle.hxx>

//! Interface for generic variable value.
struct Graphic3d_ValueInterface
{
  //! Releases memory resources of variable value.
  Standard_EXPORT virtual ~Graphic3d_ValueInterface();

  //! Returns unique identifier of value type.
  virtual Standard_Size TypeID() const = 0;

  //! Returns variable value casted to specified type.
  template <class T> T& As();

  //! Returns variable value casted to specified type.
  template <class T> const T& As() const;
};

//! Generates unique type identifier for variable value.
template<class T>
struct Graphic3d_UniformValueTypeID {
  /* Not implemented */
};

template<>
struct Graphic3d_UniformValueTypeID<Standard_Integer> {
  static const Standard_Size ID = __LINE__;
};

template<>
struct Graphic3d_UniformValueTypeID<Standard_ShortReal> {
  static const Standard_Size ID = __LINE__;
};

template<>
struct Graphic3d_UniformValueTypeID<Graphic3d_Vec2> {
  static const Standard_Size ID = __LINE__;
};

template<>
struct Graphic3d_UniformValueTypeID<Graphic3d_Vec3> {
  static const Standard_Size ID = __LINE__;
};

template<>
struct Graphic3d_UniformValueTypeID<Graphic3d_Vec4> {
  static const Standard_Size ID = __LINE__;
};

template<>
struct Graphic3d_UniformValueTypeID<Graphic3d_Vec2i> {
  static const Standard_Size ID = __LINE__;
};

template<>
struct Graphic3d_UniformValueTypeID<Graphic3d_Vec3i> {
  static const Standard_Size ID = __LINE__;
};

template<>
struct Graphic3d_UniformValueTypeID<Graphic3d_Vec4i> {
  static const Standard_Size ID = __LINE__;
};

//! Describes specific value of custom uniform variable.
template <class T>
struct Graphic3d_UniformValue : public Graphic3d_ValueInterface
{
  //! Creates new variable value.
  Graphic3d_UniformValue (const T& theValue) : Value (theValue) { }

  //! Returns unique identifier of value type.
  virtual Standard_Size TypeID() const;

  //! Value of custom uniform variable.
  T Value;
};

//! Integer uniform value.
typedef Graphic3d_UniformValue<Standard_Integer> Graphic3d_UniformInt;

//! Integer uniform 2D vector.
typedef Graphic3d_UniformValue<Graphic3d_Vec2i> Graphic3d_UniformVec2i;

//! Integer uniform 3D vector.
typedef Graphic3d_UniformValue<Graphic3d_Vec3i> Graphic3d_UniformVec3i;

//! Integer uniform 4D vector.
typedef Graphic3d_UniformValue<Graphic3d_Vec4i> Graphic3d_UniformVec4i;

//! Floating-point uniform value.
typedef Graphic3d_UniformValue<Standard_ShortReal> Graphic3d_UniformFloat;

//! Floating-point uniform 2D vector.
typedef Graphic3d_UniformValue<Graphic3d_Vec2> Graphic3d_UniformVec2;

//! Floating-point uniform 3D vector.
typedef Graphic3d_UniformValue<Graphic3d_Vec3> Graphic3d_UniformVec3;

//! Floating-point uniform 4D vector.
typedef Graphic3d_UniformValue<Graphic3d_Vec4> Graphic3d_UniformVec4;

//! Describes custom uniform shader variable.
class Graphic3d_ShaderVariable : public Standard_Transient
{
public:

  //! Releases resources of shader variable.
  Standard_EXPORT virtual ~Graphic3d_ShaderVariable();
  
  //! Returns name of shader variable.
  Standard_EXPORT const TCollection_AsciiString& Name() const;

  //! Checks if the shader variable is valid or not.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns interface of shader variable value.
  Standard_EXPORT Graphic3d_ValueInterface* Value();

  //! Creates new initialized shader variable.
  template<class T>
  static Graphic3d_ShaderVariable* Create (const TCollection_AsciiString& theName,
                                           const T&                       theValue);

public:

  DEFINE_STANDARD_RTTI (Graphic3d_ShaderVariable)

protected:

  //! Creates new uninitialized shader variable.
  Standard_EXPORT Graphic3d_ShaderVariable (const TCollection_AsciiString& theName);

protected:

  //! The name of uniform shader variable.
  TCollection_AsciiString myName;

  //! The generic value of shader variable.
  Graphic3d_ValueInterface* myValue;
};

#include <Graphic3d_ShaderVariable.lxx>

#endif // _Graphic3d_ShaderVariable_HeaderFile
