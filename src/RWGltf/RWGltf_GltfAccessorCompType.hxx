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

#ifndef _RWGltf_GltfAccessorCompType_HeaderFile
#define _RWGltf_GltfAccessorCompType_HeaderFile

//! Low-level glTF enumeration defining Accessor component type.
enum RWGltf_GltfAccessorCompType
{
  RWGltf_GltfAccessorCompType_UNKNOWN,        //!< unknown or invalid type
  RWGltf_GltfAccessorCompType_Int8    = 5120, //!< GL_BYTE
  RWGltf_GltfAccessorCompType_UInt8   = 5121, //!< GL_UNSIGNED_BYTE
  RWGltf_GltfAccessorCompType_Int16   = 5122, //!< GL_SHORT
  RWGltf_GltfAccessorCompType_UInt16  = 5123, //!< GL_UNSIGNED_SHORT
  RWGltf_GltfAccessorCompType_UInt32  = 5125, //!< GL_UNSIGNED_INT
  RWGltf_GltfAccessorCompType_Float32 = 5126, //!< GL_FLOAT
};

#endif // _RWGltf_GltfAccessorCompType_HeaderFile
