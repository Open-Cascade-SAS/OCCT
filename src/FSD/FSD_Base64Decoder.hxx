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

#ifndef _FSD_Base64Decoder_HeaderFile
#define _FSD_Base64Decoder_HeaderFile

#include <NCollection_Buffer.hxx>

//! Tool decoding base64 stream.
class FSD_Base64Decoder
{
public:

  //! Function decoding base64 stream.
  Standard_EXPORT static Handle(NCollection_Buffer) Decode (const Standard_Byte* theStr,
                                                            const Standard_Size  theLen);
};

#endif // _FSD_Base64Decoder_HeaderFile
