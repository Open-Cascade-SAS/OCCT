// Created on: 2013-01-29
// Created by: Kirill GAVRILOV
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

#ifndef OpenGl_Vec_HeaderFile
#define OpenGl_Vec_HeaderFile

#include <NCollection_Vec2.hxx>

#include <Standard_TypeDef.hxx>

#include <NCollection_Vec3.hxx>

#include <NCollection_Vec4.hxx>

#include <NCollection_Mat4.hxx>

namespace OpenGl
{
//! Tool class for selecting appropriate vector type.
//! \tparam T Numeric data type
template <class T>
struct VectorType
{
  // Not implemented
};

template <>
struct VectorType<double>
{
  typedef NCollection_Vec2<double> Vec2;
  typedef NCollection_Vec3<double> Vec3;
  typedef NCollection_Vec4<double> Vec4;
};

template <>
struct VectorType<float>
{
  typedef NCollection_Vec2<float> Vec2;
  typedef NCollection_Vec3<float> Vec3;
  typedef NCollection_Vec4<float> Vec4;
};

//! Tool class for selecting appropriate matrix type.
//! \tparam T Numeric data type
template <class T>
struct MatrixType
{
  // Not implemented
};

template <>
struct MatrixType<double>
{
  typedef NCollection_Mat4<double> Mat4;
};

template <>
struct MatrixType<float>
{
  typedef NCollection_Mat4<float> Mat4;
};
} // namespace OpenGl

#endif // _OpenGl_Vec_H__
