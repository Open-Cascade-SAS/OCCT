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

#ifndef _RWGltf_GltfBufferViewTarget_HeaderFile
#define _RWGltf_GltfBufferViewTarget_HeaderFile

//! Low-level glTF enumeration defining BufferView target.
enum RWGltf_GltfBufferViewTarget
{
  RWGltf_GltfBufferViewTarget_UNKNOWN,                      //!< unknown or invalid type
  RWGltf_GltfBufferViewTarget_ARRAY_BUFFER         = 34962, //!< GL_ARRAY_BUFFER
  RWGltf_GltfBufferViewTarget_ELEMENT_ARRAY_BUFFER = 34963, //!< GL_ELEMENT_ARRAY_BUFFER
};

#endif // _RWGltf_GltfBufferViewTarget_HeaderFile
