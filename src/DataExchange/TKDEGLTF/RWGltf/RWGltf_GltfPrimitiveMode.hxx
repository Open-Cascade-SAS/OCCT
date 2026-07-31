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

#ifndef _RWGltf_GltfPrimitiveMode_HeaderFile
#define _RWGltf_GltfPrimitiveMode_HeaderFile

//! Low-level glTF enumeration defining Primitive type.
//! Similar to Graphic3d_TypeOfData but does not define actual type and includes matrices.
enum RWGltf_GltfPrimitiveMode
{
  RWGltf_GltfPrimitiveMode_UNKNOWN       = -1, //!< unknown or invalid type
  RWGltf_GltfPrimitiveMode_Points        = 0,  //!< GL_POINTS
  RWGltf_GltfPrimitiveMode_Lines         = 1,  //!< GL_LINES
  RWGltf_GltfPrimitiveMode_LineLoop      = 2,  //!< GL_LINE_LOOP
  RWGltf_GltfPrimitiveMode_LineStrip     = 3,  //!< GL_LINE_STRIP
  RWGltf_GltfPrimitiveMode_Triangles     = 4,  //!< GL_TRIANGLES
  RWGltf_GltfPrimitiveMode_TriangleStrip = 5,  //!< GL_TRIANGLE_STRIP
  RWGltf_GltfPrimitiveMode_TriangleFan   = 6,  //!< GL_TRIANGLE_FAN
};

#endif // _RWGltf_GltfPrimitiveMode_HeaderFile
