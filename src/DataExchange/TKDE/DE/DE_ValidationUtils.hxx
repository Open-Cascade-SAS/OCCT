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

#include <iosfwd>

class NCollection_Buffer;
class TDocStd_Document;

//! Shared validation and stream utilities for DE providers.
//!
//! Checks configuration nodes, documents, file access and input/output stream lists.
//! Validation methods return a status and can send diagnostics with the operation context.
//!
//! Named companion streams are resolved relative to the referring model or resource.
//! Lookup uses the supplied stream list without filesystem fallback. Streams remain owned
//! by the caller; a returned stream pointer is valid only while that stream is alive.
//!
//! Content buffers hold an initial part of the input for format detection. Complete stream
//! buffers retain all input bytes independently of the source stream's lifetime.
class DE_ValidationUtils
{
public:
  //! Validates that configuration node is not null and matches expected type
  //! @param[in] theNode configuration node to validate
  //! @param[in] theExpectedType expected RTTI type
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error messages via Message::SendFail
  //! @return true if node is valid, false otherwise
  Standard_EXPORT static bool ValidateConfigurationNode(
    const occ::handle<DE_ConfigurationNode>& theNode,
    const occ::handle<Standard_Type>&        theExpectedType,
    const TCollection_AsciiString&           theContext,
    const bool                               theIsVerbose = true);

  //! Checks if file exists and is readable
  //! @param[in] thePath file path to check
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error messages via Message::SendFail
  //! @return true if file exists and is readable, false otherwise
  Standard_EXPORT static bool ValidateFileForReading(const TCollection_AsciiString& thePath,
                                                     const TCollection_AsciiString& theContext,
                                                     const bool theIsVerbose = true);

  //! Checks if file location is writable (file may or may not exist)
  //! @param[in] thePath file path to check
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error messages via Message::SendFail
  //! @return true if location is writable, false otherwise
  Standard_EXPORT static bool ValidateFileForWriting(const TCollection_AsciiString& thePath,
                                                     const TCollection_AsciiString& theContext,
                                                     const bool theIsVerbose = true);

  //! Validate the first input stream and warn about additional streams.
  //! @param[in] theStreams read stream list to validate
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error/warning messages
  //! @return true if stream list is valid, false otherwise
  Standard_EXPORT static bool ValidateReadStreamList(const DE_Provider::ReadStreamList& theStreams,
                                                     const TCollection_AsciiString&     theContext,
                                                     const bool theIsVerbose = true);

  //! Validate input streams and optional companion names.
  //! Companion names must be relative and unique after reducing dot segments.
  //! Only the first stream can have an empty name.
  //! @param[in] theStreams input streams
  //! @param[in] theContext context string for diagnostics
  //! @param[in] theIsVerbose send error and warning messages
  //! @param[in] theWithCompanions check all streams and their companion names
  //! @return false for empty input, failed streams, invalid or duplicate companion names
  Standard_EXPORT static bool ValidateReadStreamList(const DE_Provider::ReadStreamList& theStreams,
                                                     const TCollection_AsciiString&     theContext,
                                                     bool theIsVerbose,
                                                     bool theWithCompanions);

  //! Find a companion by relative name in a validated stream list.
  //! Names are case-sensitive, use '/' separators, and stay within the transfer root.
  //! @param[in] theStreams list validated with companion checks enabled
  //! @param[in] theReferrer name of the referring model or material
  //! @param[in] theReference relative companion name, URI-decoded when needed
  //! @param[out] theName resolved transfer-root name; empty for an invalid reference
  //! @return borrowed stream, or null if the reference cannot be resolved
  Standard_EXPORT static std::istream* FindReadStream(const DE_Provider::ReadStreamList& theStreams,
                                                      const TCollection_AsciiString& theReferrer,
                                                      const TCollection_AsciiString& theReference,
                                                      TCollection_AsciiString&       theName);

  //! Copy a complete seekable stream into an owned buffer.
  //! @param[in,out] theStream input read from the beginning; left at the end on success
  //! @param[out] theBuffer resulting bytes; null on failure
  //! @return true if the complete stream was read
  //! @throw std::ios_base::failure according to the stream's exception settings
  Standard_EXPORT static bool ReadStreamBuffer(std::istream&                    theStream,
                                               occ::handle<NCollection_Buffer>& theBuffer);

  //! Validates write stream list, warns if multiple streams
  //! @param[in] theStreams write stream list to validate
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error/warning messages
  //! @return true if stream list is valid, false otherwise
  Standard_EXPORT static bool ValidateWriteStreamList(DE_Provider::WriteStreamList&  theStreams,
                                                      const TCollection_AsciiString& theContext,
                                                      const bool theIsVerbose = true);

  //! Validates that TDocStd_Document handle is not null
  //! @param[in] theDocument document to validate
  //! @param[in] theContext context string for error messages
  //! @param[in] theIsVerbose if true, sends detailed error messages via Message::SendFail
  //! @return true if document is not null, false otherwise
  Standard_EXPORT static bool ValidateDocument(const occ::handle<TDocStd_Document>& theDocument,
                                               const TCollection_AsciiString&       theContext,
                                               const bool theIsVerbose = true);

  //! Sends warning when format doesn't support length unit scaling
  //! @param[in] theLengthUnit length unit value to check
  //! @param[in] theContext context string for warning messages
  //! @param[in] theIsVerbose if true, sends warning messages via Message::SendWarning
  //! @return true always (this is just a warning)
  Standard_EXPORT static bool WarnLengthUnitNotSupported(const double theLengthUnit,
                                                         const TCollection_AsciiString& theContext,
                                                         const bool theIsVerbose = true);

  //! Creates buffer by reading from file stream for content checking
  //! @param[in] thePath file path for reading
  //! @param[out] theBuffer output buffer with file content
  //! @return true if successful, false otherwise
  Standard_EXPORT static bool CreateContentBuffer(const TCollection_AsciiString&   thePath,
                                                  occ::handle<NCollection_Buffer>& theBuffer);

  //! Creates buffer by reading from input stream for content checking
  //! @param[in,out] theStream input stream to read from (position will be restored)
  //! @param[out] theBuffer output buffer with stream content
  //! @return true if successful, false otherwise
  Standard_EXPORT static bool CreateContentBuffer(std::istream&                    theStream,
                                                  occ::handle<NCollection_Buffer>& theBuffer);
};

#endif // _DE_ValidationUtils_HeaderFile
