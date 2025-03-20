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

#ifndef _FSD_Base64_HeaderFile
#define _FSD_Base64_HeaderFile

#include <NCollection_Buffer.hxx>
#include <TCollection_AsciiString.hxx>

//! Tool for encoding/decoding base64 stream.
class FSD_Base64
{
public:
  //! Function encoding a buffer to base64 string.
  //! @param[out] theEncodedStr the place for encoded string. Terminating null is not put.
  //!                           If it is NULL just return the needed size.
  //! @param[in] theStrLen  the length of the buffer theEncodedStr in bytes.
  //!                       This value must not be less than value returned when theEncodedStr is
  //!                       NULL.
  //! @param[in] theData    the input binary data.
  //! @param[in] theDataLen the length of input data in bytes.
  //! @return the length of the encoded string not including terminating null.
  //! If theStrLen is not enough for storing all data nothing is written and 0 is returned.
  Standard_EXPORT static Standard_Size Encode(char*                theEncodedStr,
                                              const Standard_Size  theStrLen,
                                              const Standard_Byte* theData,
                                              const Standard_Size  theDataLen);

  //! Function encoding a buffer to base64 string.
  //! @param[in] theData the input binary data
  //! @param[in] theDataLen the length of input data in bytes
  //! @return Base64 encoded string.
  Standard_EXPORT static TCollection_AsciiString Encode(const Standard_Byte* theData,
                                                        const Standard_Size  theDataLen);

  //! Function decoding base64 string.
  //! @param[out] theDecodedData the place for decoded data.
  //!                            If it is NULL just return the needed size.
  //! @param[in] theDataLen the length of the buffer theDecodedData in bytes.
  //!                       This value must not be less than value returned when theDecodedData is
  //!                       NULL.
  //! @param[in] theEncodedStr the input encoded string.
  //! @param[in] theStrLen     the length of input encoded string.
  //! @return the length of the decoded data in bytes. If theDataLen is not enough
  //!         for storing all data nothing is written and 0 is returned.
  Standard_EXPORT static Standard_Size Decode(Standard_Byte*      theDecodedData,
                                              const Standard_Size theDataLen,
                                              Standard_CString    theEncodedStr,
                                              const Standard_Size theStrLen);

  //! Function decoding base64 string.
  //! @param[in] theStr the input encoded string
  //! @param[in] theLen the length of input encoded string
  //! @return null handle in case of out of memory condition
  Standard_EXPORT static Handle(NCollection_Buffer) Decode(Standard_CString    theStr,
                                                           const Standard_Size theLen);
};

#endif // _FSD_Base64_HeaderFile
