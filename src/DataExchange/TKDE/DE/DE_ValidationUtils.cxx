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

#include <DE_ValidationUtils.hxx>

#include <Message.hxx>
#include <NCollection_Buffer.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <fstream>

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateConfigurationNode(
  const Handle(DE_ConfigurationNode)& theNode,
  const Handle(Standard_Type)&        theExpectedType,
  const TCollection_AsciiString&      theContext,
  const Standard_Boolean              theIsVerbose)
{
  if (theNode.IsNull())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Configuration Node is null";
    }
    return Standard_False;
  }

  if (!theNode->IsKind(theExpectedType))
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext
                          << ": Configuration Node is not of expected type. Expected: "
                          << theExpectedType->Name() << ", got: " << theNode->DynamicType()->Name();
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateFileForReading(
  const TCollection_AsciiString& thePath,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean         theIsVerbose)
{
  if (thePath.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": File path is empty";
    }
    return Standard_False;
  }

  try
  {
    OSD_Path aOSDPath(thePath);
    OSD_File aFile(aOSDPath);

    // Check if file exists
    if (!aFile.Exists())
    {
      if (theIsVerbose)
      {
        Message::SendFail() << "Error during " << theContext << ": File '" << thePath
                            << "' does not exist";
      }
      return Standard_False;
    }

    // Try to open for reading to verify permissions
    std::ifstream aTestFile(thePath.ToCString());
    if (!aTestFile.is_open() || !aTestFile.good())
    {
      if (theIsVerbose)
      {
        Message::SendFail() << "Error during " << theContext << ": Cannot open file '" << thePath
                            << "' for reading";
      }
      return Standard_False;
    }
  }
  catch (const std::exception& anException)
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Cannot access file '" << thePath
                          << "': " << anException.what();
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateFileForWriting(
  const TCollection_AsciiString& thePath,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean         theIsVerbose)
{
  if (thePath.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": File path is empty";
    }
    return Standard_False;
  }

  try
  {
    // Try to open for writing to verify permissions
    std::ofstream aTestFile(thePath.ToCString(), std::ios::out | std::ios::app);
    if (!aTestFile.is_open() || !aTestFile.good())
    {
      if (theIsVerbose)
      {
        Message::SendFail() << "Error during " << theContext << ": Cannot open file '" << thePath
                            << "' for writing";
      }
      return Standard_False;
    }
    // File will be closed automatically when aTestFile goes out of scope
  }
  catch (const std::exception& anException)
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Cannot access file '" << thePath
                          << "': " << anException.what();
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateReadStreamList(
  const DE_Provider::ReadStreamList& theStreams,
  const TCollection_AsciiString&     theContext,
  const Standard_Boolean             theIsVerbose)
{
  if (theStreams.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Stream list is empty";
    }
    return Standard_False;
  }

  if (theStreams.Size() > 1)
  {
    if (theIsVerbose)
    {
      Message::SendWarning() << "Warning during " << theContext << ": Received "
                             << theStreams.Size() << " streams, using only the first one";
    }
  }

  // Additional validation for input streams
  try
  {
    const DE_Provider::ReadStreamNode& aNode = theStreams.First();
    if (aNode.Stream.fail())
    {
      if (theIsVerbose)
      {
        TCollection_AsciiString aKeyInfo = aNode.Path.IsEmpty() ? "<empty path>" : aNode.Path;
        Message::SendFail() << "Error during " << theContext << ": Input stream '" << aKeyInfo
                            << "' is in invalid state";
      }
      return Standard_False;
    }
  }
  catch (const std::exception&)
  {
    if (theIsVerbose)
    {
      const DE_Provider::ReadStreamNode& aNode = theStreams.First();
      TCollection_AsciiString aKeyInfo         = aNode.Path.IsEmpty() ? "<empty path>" : aNode.Path;
      Message::SendFail() << "Error during " << theContext << ": Cannot access input stream '"
                          << aKeyInfo << "'";
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateWriteStreamList(
  DE_Provider::WriteStreamList&  theStreams,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean         theIsVerbose)
{
  if (theStreams.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Stream list is empty";
    }
    return Standard_False;
  }

  if (theStreams.Size() > 1)
  {
    if (theIsVerbose)
    {
      Message::SendWarning() << "Warning during " << theContext << ": Received "
                             << theStreams.Size() << " streams, using only the first one";
    }
  }

  // Additional validation for output streams
  try
  {
    const DE_Provider::WriteStreamNode& aNode = theStreams.First();
    if (aNode.Stream.fail())
    {
      if (theIsVerbose)
      {
        TCollection_AsciiString aKeyInfo = aNode.Path.IsEmpty() ? "<empty path>" : aNode.Path;
        Message::SendFail() << "Error during " << theContext << ": Output stream '" << aKeyInfo
                            << "' is in invalid state";
      }
      return Standard_False;
    }
  }
  catch (const std::exception&)
  {
    if (theIsVerbose)
    {
      const DE_Provider::WriteStreamNode& aNode = theStreams.First();
      TCollection_AsciiString aKeyInfo = aNode.Path.IsEmpty() ? "<empty path>" : aNode.Path;
      Message::SendFail() << "Error during " << theContext << ": Cannot access output stream '"
                          << aKeyInfo << "'";
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateDocument(const Handle(TDocStd_Document)& theDocument,
                                                      const TCollection_AsciiString&  theContext,
                                                      const Standard_Boolean          theIsVerbose)
{
  if (theDocument.IsNull())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Document handle is null";
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::WarnLengthUnitNotSupported(
  const Standard_Real            theLengthUnit,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean         theIsVerbose)
{
  if (theIsVerbose && theLengthUnit != 1.0)
  {
    Message::SendWarning() << "Warning during " << theContext
                           << ": Format doesn't support custom length unit scaling (unit: "
                           << theLengthUnit << ")";
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::CreateContentBuffer(const TCollection_AsciiString& thePath,
                                                         Handle(NCollection_Buffer)&    theBuffer)
{
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aStream =
    aFileSystem->OpenIStream(thePath, std::ios::in | std::ios::binary);

  if (aStream.get() == nullptr)
  {
    theBuffer.Nullify();
    return Standard_False;
  }

  return CreateContentBuffer(*aStream, theBuffer);
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::CreateContentBuffer(std::istream&               theStream,
                                                         Handle(NCollection_Buffer)& theBuffer)
{
  constexpr std::streamsize aBufferLength = 2048;

  theBuffer =
    new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(), aBufferLength);

  // Save current stream position
  std::streampos aOriginalPos = theStream.tellg();

  theStream.read(reinterpret_cast<char*>(theBuffer->ChangeData()), aBufferLength);
  const std::streamsize aBytesRead = theStream.gcount();
  theBuffer->ChangeData()[aBytesRead < aBufferLength ? aBytesRead : aBufferLength - 1] = '\0';

  // Clear any error flags (including EOF) BEFORE attempting to reset position
  // This is essential because seekg() fails when EOF flag is set
  theStream.clear();

  // Reset stream to original position for subsequent reads
  theStream.seekg(aOriginalPos);

  return Standard_True;
}