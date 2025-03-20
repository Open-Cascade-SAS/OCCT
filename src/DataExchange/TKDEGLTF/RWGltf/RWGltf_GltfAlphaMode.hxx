// Author: Kirill Gavrilov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _RWGltf_GltfAlphaMode_HeaderFile
#define _RWGltf_GltfAlphaMode_HeaderFile

#include <Standard_CString.hxx>

//! Low-level glTF enumeration defining Alpha Mode.
enum RWGltf_GltfAlphaMode
{
  RWGltf_GltfAlphaMode_Opaque, //!< alpha value is ignored and the rendered output is fully opaque
  RWGltf_GltfAlphaMode_Mask,   //!< rendered output is either fully opaque or fully transparent
                             //!< depending on the alpha value and the specified alpha cutoff value
  RWGltf_GltfAlphaMode_Blend, //!< alpha value is used to composite the source and destination areas
};

//! Parse RWGltf_GltfAlphaMode from string.
inline RWGltf_GltfAlphaMode RWGltf_GltfParseAlphaMode(const char* theType)
{
  if (IsEqual("OPAQUE", theType))
  {
    return RWGltf_GltfAlphaMode_Opaque;
  }
  else if (IsEqual("MASK", theType))
  {
    return RWGltf_GltfAlphaMode_Mask;
  }
  else if (IsEqual("BLEND", theType))
  {
    return RWGltf_GltfAlphaMode_Blend;
  }
  return RWGltf_GltfAlphaMode_Opaque;
}

#endif // _RWGltf_GltfAlphaMode_HeaderFile
