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

#ifndef _RWGltf_GltfBufferView_HeaderFile
#define _RWGltf_GltfBufferView_HeaderFile

#include <RWGltf_GltfBufferViewTarget.hxx>
#include <Standard_TypeDef.hxx>

//! Low-level glTF data structure defining BufferView.
struct RWGltf_GltfBufferView
{
  static const int INVALID_ID = -1;

public:
  int                         Id;         //!< index of bufferView in the array of bufferViews
  int64_t                     ByteOffset; //!< offset to the beginning of the data in buffer
  int64_t                     ByteLength; //!< length of the data
  int32_t                     ByteStride; //!< [0, 255]
  RWGltf_GltfBufferViewTarget Target;

  RWGltf_GltfBufferView()
      : Id(INVALID_ID),
        ByteOffset(0),
        ByteLength(0),
        ByteStride(0),
        Target(RWGltf_GltfBufferViewTarget_UNKNOWN)
  {
  }
};

#endif // _RWGltf_GltfBufferView_HeaderFile
