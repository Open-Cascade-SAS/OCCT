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

#ifndef _RWGltf_GltfRootElement_HeaderFile
#define _RWGltf_GltfRootElement_HeaderFile

//! Root elements within glTF JSON document.
enum RWGltf_GltfRootElement
{
  RWGltf_GltfRootElement_Asset,        //!< "asset"       element, mandatory
  RWGltf_GltfRootElement_Scenes,       //!< "scenes"      element, mandatory
  RWGltf_GltfRootElement_Scene,        //!< "scene"       element, optional
  RWGltf_GltfRootElement_Nodes,        //!< "nodes"       element, mandatory
  RWGltf_GltfRootElement_Meshes,       //!< "meshes"      element, mandatory
  RWGltf_GltfRootElement_Accessors,    //!< "accessors"   element, mandatory
  RWGltf_GltfRootElement_BufferViews,  //!< "bufferViews" element, mandatory
  RWGltf_GltfRootElement_Buffers,      //!< "buffers"     element, mandatory
  RWGltf_GltfRootElement_NB_MANDATORY, //!< number of mandatory elements
  // optional elements
  RWGltf_GltfRootElement_Animations = RWGltf_GltfRootElement_NB_MANDATORY, //!< "animations" element
  RWGltf_GltfRootElement_Materials,          //!< "materials"  element,
  RWGltf_GltfRootElement_Programs,           //!< "programs"   element,
  RWGltf_GltfRootElement_Samplers,           //!< "samplers"   element,
  RWGltf_GltfRootElement_Shaders,            //!< "shaders"    element,
  RWGltf_GltfRootElement_Skins,              //!< "skins"      element,
  RWGltf_GltfRootElement_Techniques,         //!< "techniques" element,
  RWGltf_GltfRootElement_Textures,           //!< "textures"   element,
  RWGltf_GltfRootElement_Images,             //!< "images"     element,
  RWGltf_GltfRootElement_ExtensionsUsed,     //!< "extensionsUsed"     element,
  RWGltf_GltfRootElement_ExtensionsRequired, //!< "extensionsRequired" element,
  RWGltf_GltfRootElement_NB                  //!< overall number of elements
};

//! Root elements within glTF JSON document - names array.
inline const char* RWGltf_GltfRootElementName(RWGltf_GltfRootElement theElem)
{
  static const char* THE_ROOT_NAMES[RWGltf_GltfRootElement_NB] = {"asset",
                                                                  "scenes",
                                                                  "scene",
                                                                  "nodes",
                                                                  "meshes",
                                                                  "accessors",
                                                                  "bufferViews",
                                                                  "buffers",
                                                                  "animations",
                                                                  "materials",
                                                                  "programs",
                                                                  "samplers",
                                                                  "shaders",
                                                                  "skins",
                                                                  "techniques",
                                                                  "textures",
                                                                  "images",
                                                                  "extensionsUsed",
                                                                  "extensionsRequired"};
  return THE_ROOT_NAMES[theElem];
}

#endif // _RWGltf_GltfRootElement_HeaderFile
