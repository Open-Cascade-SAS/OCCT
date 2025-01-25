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

#ifndef _RWGltf_GltfFace_HeaderFile
#define _RWGltf_GltfFace_HeaderFile

#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <RWGltf_GltfAccessor.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFPrs_Style.hxx>

//! Low-level glTF data structure holding single Face (one primitive array) definition.
class RWGltf_GltfFace : public Standard_Transient
{
public:
  RWGltf_GltfAccessor NodePos;  //!< accessor for nodal positions
  RWGltf_GltfAccessor NodeNorm; //!< accessor for nodal normals
  RWGltf_GltfAccessor NodeUV;   //!< accessor for nodal UV texture coordinates
  RWGltf_GltfAccessor Indices;  //!< accessor for indexes
  TopoDS_Shape        Shape;    //!< original Face or face list
  XCAFPrs_Style       Style;    //!< face style
                                // clang-format off
  Standard_Integer    NbIndexedNodes; //!< transient variable for merging several faces into one while writing Indices
                                // clang-format on

  RWGltf_GltfFace()
      : NbIndexedNodes(0)
  {
  }
};

typedef NCollection_Shared<NCollection_List<Handle(RWGltf_GltfFace)>> RWGltf_GltfFaceList;

#endif // _RWGltf_GltfFace_HeaderFile
