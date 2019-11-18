// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#ifndef _RWGltf_WriterTrsfFormat_HeaderFile
#define _RWGltf_WriterTrsfFormat_HeaderFile

//! Transformation format.
enum RWGltf_WriterTrsfFormat
{
  RWGltf_WriterTrsfFormat_Compact = 0, //!< automatically choose most compact representation between Mat4 and TRS
  RWGltf_WriterTrsfFormat_Mat4    = 1, //!< 4x4 transformation Matrix
  RWGltf_WriterTrsfFormat_TRS     = 2, //!< transformation decomposed into Translation vector, Rotation quaternion and Scale factor (T * R * S)
};
enum { RWGltf_WriterTrsfFormat_LOWER = 0, RWGltf_WriterTrsfFormat_UPPER = RWGltf_WriterTrsfFormat_TRS }; // aliases

#endif // _RWGltf_WriterTrsfFormat_HeaderFile
