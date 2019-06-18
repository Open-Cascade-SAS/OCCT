// Author: Kirill Gavrilov
// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _RWGltf_GltfAccessorLayout_HeaderFile
#define _RWGltf_GltfAccessorLayout_HeaderFile

#include <Standard_CString.hxx>

//! Low-level glTF enumeration defining Accessor layout.
//! Similar to Graphic3d_TypeOfData but does not define actual type and includes matrices.
enum RWGltf_GltfAccessorLayout
{
  RWGltf_GltfAccessorLayout_UNKNOWN, //!< unknown or invalid type
  RWGltf_GltfAccessorLayout_Scalar,  //!< "SCALAR"
  RWGltf_GltfAccessorLayout_Vec2,    //!< "VEC2"
  RWGltf_GltfAccessorLayout_Vec3,    //!< "VEC3"
  RWGltf_GltfAccessorLayout_Vec4,    //!< "VEC4"
  RWGltf_GltfAccessorLayout_Mat2,    //!< "MAT2"
  RWGltf_GltfAccessorLayout_Mat3,    //!< "MAT3"
  RWGltf_GltfAccessorLayout_Mat4,    //!< "MAT4"
};

//! Parse GltfAccessorLayout from string.
inline RWGltf_GltfAccessorLayout RWGltf_GltfParseAccessorType (const char* theType)
{
  if (IsEqual ("SCALAR", theType))
  {
    return RWGltf_GltfAccessorLayout_Scalar;
  }
  else if (IsEqual ("VEC2", theType))
  {
    return RWGltf_GltfAccessorLayout_Vec2;
  }
  else if (IsEqual ("VEC3", theType))
  {
    return RWGltf_GltfAccessorLayout_Vec3;
  }
  else if (IsEqual ("VEC4", theType))
  {
    return RWGltf_GltfAccessorLayout_Vec4;
  }
  else if (IsEqual ("MAT2", theType))
  {
    return RWGltf_GltfAccessorLayout_Mat2;
  }
  else if (IsEqual ("MAT3", theType))
  {
    return RWGltf_GltfAccessorLayout_Mat3;
  }
  else if (IsEqual ("MAT4", theType))
  {
    return RWGltf_GltfAccessorLayout_Mat4;
  }
  return RWGltf_GltfAccessorLayout_UNKNOWN;
}

#endif // _RWGltf_GltfAccessorLayout_HeaderFile
