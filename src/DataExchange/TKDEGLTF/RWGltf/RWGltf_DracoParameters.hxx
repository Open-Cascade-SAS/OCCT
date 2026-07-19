// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _RWGltf_DracoParameters_HeaderFile
#define _RWGltf_DracoParameters_HeaderFile

//! Draco compression parameters
struct RWGltf_DracoParameters
{
  RWGltf_DracoParameters()
      : DracoCompression(false),
        CompressionLevel(7),
        QuantizePositionBits(14),
        QuantizeNormalBits(10),
        QuantizeTexcoordBits(12),
        QuantizeColorBits(8),
        QuantizeGenericBits(12),
        UnifiedQuantization(false)
  {
  }

  // clang-format off
  bool DracoCompression;    //!< flag to use Draco compression (FALSE by default). If it is TRUE, compression is used
  int CompressionLevel;     //!< Draco compression level [0-10] (7 by default)
  int QuantizePositionBits; //!< quantization bits for position attribute (14 by default)
  int QuantizeNormalBits;   //!< quantization bits for normal attribute (10 by default)
  int QuantizeTexcoordBits; //!< quantization bits for texture coordinate attribute (12 by default)
  int QuantizeColorBits;    //!< quantization bits for color attributes (8 by default)
  int QuantizeGenericBits;  //!< quantization bits for skinning and custom attributes (12 by default)
  bool UnifiedQuantization; //!< quantize positions of all primitives using the same quantization grid (FALSE by default)
  // clang-format on
};

#endif
