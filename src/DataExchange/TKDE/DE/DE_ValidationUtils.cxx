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
#include <NCollection_FlatMap.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>

#include <fstream>
#include <string_view>
#include <utility>

namespace
{
//==================================================================================================

bool reduceStreamName(const TCollection_AsciiString& theInput, TCollection_AsciiString& theOutput)
{
  theOutput.Clear();
  const std::string_view aText = static_cast<std::string_view>(theInput);
  if (aText.empty() || aText.front() == '/' || aText.find_first_of("\\:\0", 0, 3) != aText.npos)
  {
    return false;
  }
  size_t aBegin = 0;
  while (aBegin <= aText.size())
  {
    const size_t           aSlash = aText.find('/', aBegin);
    const size_t           anEnd  = aSlash == aText.npos ? aText.size() : aSlash;
    const std::string_view aPart  = aText.substr(aBegin, anEnd - aBegin);
    if (aPart == "..")
    {
      if (theOutput.IsEmpty())
      {
        return false;
      }
      const int aLastSlash = theOutput.SearchFromEnd("/");
      theOutput.Trunc(aLastSlash > 0 ? aLastSlash - 1 : 0);
    }
    else if (!aPart.empty() && aPart != ".")
    {
      if (!theOutput.IsEmpty())
      {
        theOutput += '/';
      }
      theOutput += aPart;
    }
    if (aSlash == aText.npos)
    {
      break;
    }
    aBegin = aSlash + 1;
  }
  return !theOutput.IsEmpty();
}

} // namespace

//=================================================================================================

bool DE_ValidationUtils::ValidateConfigurationNode(
  const occ::handle<DE_ConfigurationNode>& theNode,
  const occ::handle<Standard_Type>&        theExpectedType,
  const TCollection_AsciiString&           theContext,
  const bool                               theIsVerbose)
{
  if (theNode.IsNull())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Configuration Node is null";
    }
    return false;
  }

  if (!theNode->IsKind(theExpectedType))
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext
                          << ": Configuration Node is not of expected type. Expected: "
                          << theExpectedType->Name() << ", got: " << theNode->DynamicType()->Name();
    }
    return false;
  }

  return true;
}

//=================================================================================================

bool DE_ValidationUtils::ValidateFileForReading(const TCollection_AsciiString& thePath,
                                                const TCollection_AsciiString& theContext,
                                                const bool                     theIsVerbose)
{
  if (thePath.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": File path is empty";
    }
    return false;
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
      return false;
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
      return false;
    }
  }
  catch (const std::exception& anException)
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Cannot access file '" << thePath
                          << "': " << anException.what();
    }
    return false;
  }

  return true;
}

//=================================================================================================

bool DE_ValidationUtils::ValidateFileForWriting(const TCollection_AsciiString& thePath,
                                                const TCollection_AsciiString& theContext,
                                                const bool                     theIsVerbose)
{
  if (thePath.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": File path is empty";
    }
    return false;
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
      return false;
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
    return false;
  }

  return true;
}

//=================================================================================================

bool DE_ValidationUtils::ValidateReadStreamList(const DE_Provider::ReadStreamList& theStreams,
                                                const TCollection_AsciiString&     theContext,
                                                const bool                         theIsVerbose)
{
  return ValidateReadStreamList(theStreams, theContext, theIsVerbose, false);
}

//==================================================================================================

bool DE_ValidationUtils::ValidateReadStreamList(const DE_Provider::ReadStreamList& theStreams,
                                                const TCollection_AsciiString&     theContext,
                                                const bool                         theIsVerbose,
                                                const bool theWithCompanions)
{
  if (theStreams.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Stream list is empty";
    }
    return false;
  }

  if (theStreams.Size() > 1 && !theWithCompanions)
  {
    if (theIsVerbose)
    {
      Message::SendWarning() << "Warning during " << theContext << ": Received "
                             << theStreams.Size() << " streams, using only the first one";
    }
  }

  NCollection_FlatMap<TCollection_AsciiString> aNames;
  if (theWithCompanions)
  {
    aNames.Reserve(theStreams.Size());
  }
  bool isFirst = true;
  for (const DE_Provider::ReadStreamNode& aNode : theStreams)
  {
    if (aNode.Stream.fail())
    {
      if (theIsVerbose)
      {
        TCollection_AsciiString aKeyInfo = aNode.Path.IsEmpty() ? "<empty path>" : aNode.Path;
        Message::SendFail() << "Error during " << theContext << ": Input stream '" << aKeyInfo
                            << "' is in invalid state";
      }
      return false;
    }
    if (theWithCompanions && !(isFirst && aNode.Path.IsEmpty()))
    {
      TCollection_AsciiString aName;
      if (!reduceStreamName(aNode.Path, aName) || !aNames.Add(std::move(aName)))
      {
        if (theIsVerbose)
        {
          Message::SendFail() << "Error during " << theContext
                              << ": Invalid or duplicate input stream name '" << aNode.Path << "'";
        }
        return false;
      }
    }
    isFirst = false;
    if (!theWithCompanions)
    {
      break;
    }
  }

  return true;
}

//=================================================================================================

bool DE_ValidationUtils::ValidateWriteStreamList(DE_Provider::WriteStreamList&  theStreams,
                                                 const TCollection_AsciiString& theContext,
                                                 const bool                     theIsVerbose)
{
  if (theStreams.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Stream list is empty";
    }
    return false;
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
      return false;
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
    return false;
  }

  return true;
}

//=================================================================================================

bool DE_ValidationUtils::ValidateDocument(const occ::handle<TDocStd_Document>& theDocument,
                                          const TCollection_AsciiString&       theContext,
                                          const bool                           theIsVerbose)
{
  if (theDocument.IsNull())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext << ": Document handle is null";
    }
    return false;
  }

  return true;
}

//=================================================================================================

bool DE_ValidationUtils::WarnLengthUnitNotSupported(const double                   theLengthUnit,
                                                    const TCollection_AsciiString& theContext,
                                                    const bool                     theIsVerbose)
{
  if (theIsVerbose && theLengthUnit != 1.0)
  {
    Message::SendWarning() << "Warning during " << theContext
                           << ": Format doesn't support custom length unit scaling (unit: "
                           << theLengthUnit << ")";
  }

  return true;
}

//=================================================================================================

bool DE_ValidationUtils::CreateContentBuffer(const TCollection_AsciiString&   thePath,
                                             occ::handle<NCollection_Buffer>& theBuffer)
{
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream>      aStream =
    aFileSystem->OpenIStream(thePath, std::ios::in | std::ios::binary);

  if (aStream.get() == nullptr)
  {
    theBuffer.Nullify();
    return false;
  }

  return CreateContentBuffer(*aStream, theBuffer);
}

//=================================================================================================

bool DE_ValidationUtils::CreateContentBuffer(std::istream&                    theStream,
                                             occ::handle<NCollection_Buffer>& theBuffer)
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

  return true;
}

//==================================================================================================

bool DE_ValidationUtils::ReadStreamBuffer(std::istream&                    theStream,
                                          occ::handle<NCollection_Buffer>& theBuffer)
{
  theBuffer.Nullify();
  if (theStream.bad())
  {
    return false;
  }
  theStream.clear();
  theStream.seekg(0, std::ios_base::end);
  const std::streampos anEnd = theStream.tellg();
  if (anEnd < std::streampos(0))
  {
    return false;
  }
  const std::streamoff aLength = anEnd - std::streampos(0);
  const size_t         aSize   = static_cast<size_t>(aLength);
  if (static_cast<std::streamoff>(aSize) != aLength)
  {
    return false;
  }
  theStream.seekg(0, std::ios_base::beg);
  if (!theStream.good())
  {
    return false;
  }
  occ::handle<NCollection_Buffer> aBuffer =
    new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(), aSize);
  if (aLength != 0 && aBuffer->IsEmpty())
  {
    return false;
  }
  if (aLength != 0)
  {
    theStream.read(reinterpret_cast<char*>(aBuffer->ChangeData()), aLength);
    if (theStream.gcount() != aLength || !theStream.good())
    {
      return false;
    }
  }
  theBuffer = aBuffer;
  return true;
}

//==================================================================================================

std::istream* DE_ValidationUtils::FindReadStream(const DE_Provider::ReadStreamList& theStreams,
                                                 const TCollection_AsciiString&     theReferrer,
                                                 const TCollection_AsciiString&     theReference,
                                                 TCollection_AsciiString&           theName)
{
  theName.Clear();
  if (theReference.IsEmpty())
  {
    return nullptr;
  }
  const std::string_view aReference = static_cast<std::string_view>(theReference);
  if (aReference.front() == '/' || aReference.find_first_of("\\:\0", 0, 3) != aReference.npos)
  {
    return nullptr;
  }
  TCollection_AsciiString aReferrer;
  if (!theReferrer.IsEmpty() && !reduceStreamName(theReferrer, aReferrer))
  {
    return nullptr;
  }
  TCollection_AsciiString aJoined;
  const std::string_view  aBase  = static_cast<std::string_view>(aReferrer);
  const size_t            aSlash = aBase.rfind('/');
  if (aSlash != aBase.npos)
  {
    aJoined = TCollection_AsciiString(aBase.substr(0, aSlash + 1));
  }
  aJoined += theReference;
  if (!reduceStreamName(aJoined, theName))
  {
    return nullptr;
  }
  TCollection_AsciiString aName;
  for (const DE_Provider::ReadStreamNode& aNode : theStreams)
  {
    if (aNode.Path == theName || (reduceStreamName(aNode.Path, aName) && aName == theName))
    {
      return &aNode.Stream;
    }
  }
  return nullptr;
}
