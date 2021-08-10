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

#ifndef _RWGltf_GltfAccessor_HeaderFile
#define _RWGltf_GltfAccessor_HeaderFile

#include <Graphic3d_BndBox3d.hxx>
#include <RWGltf_GltfAccessorCompType.hxx>
#include <RWGltf_GltfAccessorLayout.hxx>
#include <Standard_TypeDef.hxx>

//! Low-level glTF data structure defining Accessor.
struct RWGltf_GltfAccessor
{
  static const int INVALID_ID = -1;
public:

  int                         Id;            //!< identifier
  int64_t                     ByteOffset;    //!< byte offset
  int64_t                     Count;         //!< size
  int32_t                     ByteStride;    //!< [0, 255] for glTF 1.0
  RWGltf_GltfAccessorLayout   Type;          //!< layout type
  RWGltf_GltfAccessorCompType ComponentType; //!< component type
  Graphic3d_BndBox3d          BndBox;        //!< bounding box
  bool                        IsCompressed;  //!< flag indicating KHR_draco_mesh_compression

  //! Empty constructor.
  RWGltf_GltfAccessor()
  : Id (INVALID_ID),
    ByteOffset (0),
    Count (0),
    ByteStride (0),
    Type (RWGltf_GltfAccessorLayout_UNKNOWN),
    ComponentType (RWGltf_GltfAccessorCompType_UNKNOWN),
    IsCompressed (false) {}

};

#endif // _RWGltf_GltfAccessor_HeaderFile
