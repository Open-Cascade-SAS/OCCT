// Author: Kirill Gavrilov
// Copyright (c) 2018-2019 OPEN CASCADE SAS
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

#ifndef _RWGltf_GltfPrimArrayData_HeaderFile
#define _RWGltf_GltfPrimArrayData_HeaderFile

#include <NCollection_Buffer.hxx>
#include <RWGltf_GltfAccessor.hxx>
#include <RWGltf_GltfArrayType.hxx>
#include <TCollection_AsciiString.hxx>

//! An element within primitive array - vertex attribute or element indexes.
class RWGltf_GltfPrimArrayData
{
public:
  Handle(NCollection_Buffer) StreamData;
  TCollection_AsciiString    StreamUri;
  int64_t                    StreamOffset;
  int64_t                    StreamLength;

  RWGltf_GltfAccessor  Accessor;
  RWGltf_GltfArrayType Type;

  RWGltf_GltfPrimArrayData()
      : StreamOffset(0),
        StreamLength(0),
        Type(RWGltf_GltfArrayType_UNKNOWN)
  {
  }

  RWGltf_GltfPrimArrayData(RWGltf_GltfArrayType theType)
      : StreamOffset(0),
        StreamLength(0),
        Type(theType)
  {
  }
};

#endif // _RWGltf_GltfPrimArrayData_HeaderFile
