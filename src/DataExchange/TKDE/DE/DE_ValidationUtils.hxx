// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _DE_ValidationUtils_HeaderFile
#define _DE_ValidationUtils_HeaderFile

#include <TCollection_AsciiString.hxx>
#include <DE_ConfigurationNode.hxx>
#include <DE_Provider.hxx>

class TDocStd_Document;

//! Utility class providing static methods for common validation operations
//! used across DataExchange providers. Includes validation for configuration nodes,
//! file paths, streams, and other common scenarios with optional verbose error reporting.
class DE_ValidationUtils
{
public:
  //! Validates that configuration node is not null and matches expected type
  //! @param[in] theNode configuration node to validate
  //! @param[in] theExpectedType expected RTTI type
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error messages via Message::SendFail
  //! @return Standard_True if node is valid, Standard_False otherwise
  Standard_EXPORT static Standard_Boolean ValidateConfigurationNode(
    const Handle(DE_ConfigurationNode)& theNode,
    const Handle(Standard_Type)&        theExpectedType,
    const TCollection_AsciiString&      theContext,
    const Standard_Boolean              theIsVerbose = Standard_True);

  //! Checks if file exists and is readable
  //! @param[in] thePath file path to check
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error messages via Message::SendFail
  //! @return Standard_True if file exists and is readable, Standard_False otherwise
  Standard_EXPORT static Standard_Boolean ValidateFileForReading(
    const TCollection_AsciiString& thePath,
    const TCollection_AsciiString& theContext,
    const Standard_Boolean         theIsVerbose = Standard_True);

  //! Checks if file location is writable (file may or may not exist)
  //! @param[in] thePath file path to check
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error messages via Message::SendFail
  //! @return Standard_True if location is writable, Standard_False otherwise
  Standard_EXPORT static Standard_Boolean ValidateFileForWriting(
    const TCollection_AsciiString& thePath,
    const TCollection_AsciiString& theContext,
    const Standard_Boolean         theIsVerbose = Standard_True);

  //! Validates read stream list, warns if multiple streams
  //! @param[in] theStreams read stream list to validate
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error/warning messages
  //! @return Standard_True if stream list is valid, Standard_False otherwise
  Standard_EXPORT static Standard_Boolean ValidateReadStreamList(
    const DE_Provider::ReadStreamList& theStreams,
    const TCollection_AsciiString&     theContext,
    const Standard_Boolean             theIsVerbose = Standard_True);

  //! Validates write stream list, warns if multiple streams
  //! @param[in] theStreams write stream list to validate
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error/warning messages
  //! @return Standard_True if stream list is valid, Standard_False otherwise
  Standard_EXPORT static Standard_Boolean ValidateWriteStreamList(
    DE_Provider::WriteStreamList&  theStreams,
    const TCollection_AsciiString& theContext,
    const Standard_Boolean         theIsVerbose = Standard_True);

  //! Validates that TDocStd_Document handle is not null
  //! @param[in] theDocument document to validate
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error messages via Message::SendFail
  //! @return Standard_True if document is not null, Standard_False otherwise
  Standard_EXPORT static Standard_Boolean ValidateDocument(
    const Handle(TDocStd_Document)& theDocument,
    const TCollection_AsciiString&  theContext,
    const Standard_Boolean          theIsVerbose = Standard_True);

  //! Sends warning when format doesn't support length unit scaling
  //! @param[in] theLengthUnit length unit value to check
  //! @param[in] theContext context string for warning messages
  //! @param[in] theIsVerbose if true, sends warning messages via Message::SendWarning
  //! @return Standard_True always (this is just a warning)
  Standard_EXPORT static Standard_Boolean WarnLengthUnitNotSupported(
    const Standard_Real            theLengthUnit,
    const TCollection_AsciiString& theContext,
    const Standard_Boolean         theIsVerbose = Standard_True);

  //! Creates buffer by reading from file stream for content checking
  //! @param[in] thePath file path for reading
  //! @param[out] theBuffer output buffer with file content
  //! @return Standard_True if successful, Standard_False otherwise
  Standard_EXPORT static Standard_Boolean CreateContentBuffer(
    const TCollection_AsciiString& thePath,
    Handle(NCollection_Buffer)&    theBuffer);

  //! Creates buffer by reading from input stream for content checking
  //! @param[in,out] theStream input stream to read from (position will be restored)
  //! @param[out] theBuffer output buffer with stream content
  //! @return Standard_True if successful, Standard_False otherwise
  Standard_EXPORT static Standard_Boolean CreateContentBuffer(
    std::istream&               theStream,
    Handle(NCollection_Buffer)& theBuffer);
};

#endif // _DE_ValidationUtils_HeaderFile