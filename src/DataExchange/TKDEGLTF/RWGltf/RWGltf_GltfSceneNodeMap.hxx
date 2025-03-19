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

#ifndef _RWGltf_GltfSceneNodeMap_HeaderFile
#define _RWGltf_GltfSceneNodeMap_HeaderFile

#include <NCollection_IndexedMap.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>

//! Indexed map of scene nodes with custom search algorithm.
class RWGltf_GltfSceneNodeMap : public NCollection_IndexedMap<XCAFPrs_DocumentNode>
{
public:
  //! Empty constructor.
  RWGltf_GltfSceneNodeMap() {}

  //! Find index from document node string identifier.
  Standard_Integer FindIndex(const TCollection_AsciiString& theNodeId) const
  {
    if (IsEmpty())
    {
      return 0;
    }
    XCAFPrs_DocumentNode aDocNode;
    aDocNode.Id = theNodeId;
    return NCollection_IndexedMap<XCAFPrs_DocumentNode>::FindIndex(aDocNode);
  }
};

#endif // _RWGltf_GltfSceneNodeMap_HeaderFile
