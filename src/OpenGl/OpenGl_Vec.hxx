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

#include <Graphic3d_Vec.hxx>

typedef Graphic3d_Vec2i  OpenGl_Vec2i;
typedef Graphic3d_Vec3i  OpenGl_Vec3i;
typedef Graphic3d_Vec4i  OpenGl_Vec4i;

typedef Graphic3d_Vec2b  OpenGl_Vec2b;
typedef Graphic3d_Vec3b  OpenGl_Vec3b;
typedef Graphic3d_Vec4b  OpenGl_Vec4b;

typedef Graphic3d_Vec2u  OpenGl_Vec2u;
typedef Graphic3d_Vec3u  OpenGl_Vec3u;
typedef Graphic3d_Vec4u  OpenGl_Vec4u;

typedef Graphic3d_Vec2ub OpenGl_Vec2ub;
typedef Graphic3d_Vec3ub OpenGl_Vec3ub;
typedef Graphic3d_Vec4ub OpenGl_Vec4ub;

typedef Graphic3d_Vec2   OpenGl_Vec2;
typedef Graphic3d_Vec3   OpenGl_Vec3;
typedef Graphic3d_Vec4   OpenGl_Vec4;

typedef Graphic3d_Vec2d  OpenGl_Vec2d;
typedef Graphic3d_Vec3d  OpenGl_Vec3d;
typedef Graphic3d_Vec4d  OpenGl_Vec4d;

typedef Graphic3d_Mat4   OpenGl_Mat4;
typedef Graphic3d_Mat4d  OpenGl_Mat4d;

namespace OpenGl
{
  //! Tool class for selecting appropriate vector type.
  //! \tparam T Numeric data type
  template<class T> struct VectorType
  {
    // Not implemented
  };

  template<> struct VectorType<Standard_Real>
  {
    typedef OpenGl_Vec2d Vec2;
    typedef OpenGl_Vec3d Vec3;
    typedef OpenGl_Vec4d Vec4;
  };

  template<> struct VectorType<Standard_ShortReal>
  {
    typedef OpenGl_Vec2 Vec2;
    typedef OpenGl_Vec3 Vec3;
    typedef OpenGl_Vec4 Vec4;
  };

  //! Tool class for selecting appropriate matrix type.
  //! \tparam T Numeric data type
  template<class T> struct MatrixType
  {
    // Not implemented
  };

  template<> struct MatrixType<Standard_Real>
  {
    typedef OpenGl_Mat4d Mat4;
  };

  template<> struct MatrixType<Standard_ShortReal>
  {
    typedef OpenGl_Mat4 Mat4;
  };
}

#endif // _OpenGl_Vec_H__
