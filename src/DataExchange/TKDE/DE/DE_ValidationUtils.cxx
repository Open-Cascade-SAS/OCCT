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
#include <filesystem>
#include <fstream>

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateConfigurationNode(
  const Handle(DE_ConfigurationNode)& theNode,
  const Handle(Standard_Type)& theExpectedType,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean theIsVerbose)
{
  if (theNode.IsNull())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext 
                          << ": Configuration Node is null";
    }
    return Standard_False;
  }

  if (!theNode->IsKind(theExpectedType))
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext 
                          << ": Configuration Node is not of expected type. Expected: "
                          << theExpectedType->Name() 
                          << ", got: " << theNode->DynamicType()->Name();
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateFileForReading(
  const TCollection_AsciiString& thePath,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean theIsVerbose)
{
  if (thePath.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext 
                          << ": File path is empty";
    }
    return Standard_False;
  }

  try
  {
    std::filesystem::path aFilePath(thePath.ToCString());
    
    // Check if file exists
    if (!std::filesystem::exists(aFilePath))
    {
      if (theIsVerbose)
      {
        Message::SendFail() << "Error during " << theContext 
                            << ": File '" << thePath << "' does not exist";
      }
      return Standard_False;
    }

    // Check if it's a regular file
    if (!std::filesystem::is_regular_file(aFilePath))
    {
      if (theIsVerbose)
      {
        Message::SendFail() << "Error during " << theContext 
                            << ": Path '" << thePath << "' is not a regular file";
      }
      return Standard_False;
    }

    // Try to open for reading to verify permissions
    std::ifstream aTestFile(aFilePath);
    if (!aTestFile.is_open() || !aTestFile.good())
    {
      if (theIsVerbose)
      {
        Message::SendFail() << "Error during " << theContext 
                            << ": Cannot open file '" << thePath << "' for reading";
      }
      return Standard_False;
    }
  }
  catch (const std::exception& anException)
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext 
                          << ": Cannot access file '" << thePath << "': " << anException.what();
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateFileForWriting(
  const TCollection_AsciiString& thePath,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean theIsVerbose)
{
  if (thePath.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext 
                          << ": File path is empty";
    }
    return Standard_False;
  }

  try
  {
    std::filesystem::path aFilePath(thePath.ToCString());
    
    // If file exists, check if it's writable
    if (std::filesystem::exists(aFilePath))
    {
      // Check if it's a regular file
      if (!std::filesystem::is_regular_file(aFilePath))
      {
        if (theIsVerbose)
        {
          Message::SendFail() << "Error during " << theContext 
                              << ": Path '" << thePath << "' exists but is not a regular file";
        }
        return Standard_False;
      }
    }
    else
    {
      // File doesn't exist, check if parent directory exists and is writable
      std::filesystem::path aParentPath = aFilePath.parent_path();
      if (!aParentPath.empty() && !std::filesystem::exists(aParentPath))
      {
        if (theIsVerbose)
        {
          Message::SendFail() << "Error during " << theContext 
                              << ": Parent directory '" << aParentPath.string().c_str() << "' does not exist";
        }
        return Standard_False;
      }
    }

    // Try to open for writing to verify permissions
    std::ofstream aTestFile(aFilePath, std::ios::out | std::ios::app);
    if (!aTestFile.is_open() || !aTestFile.good())
    {
      if (theIsVerbose)
      {
        Message::SendFail() << "Error during " << theContext 
                            << ": Cannot open file '" << thePath << "' for writing";
      }
      return Standard_False;
    }
    // File will be closed automatically when aTestFile goes out of scope
  }
  catch (const std::exception& anException)
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext 
                          << ": Cannot access file '" << thePath << "': " << anException.what();
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateReadStreamMap(
  const DE_Provider::ReadStreamMap& theStreams,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean theIsVerbose)
{
  if (theStreams.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext 
                          << ": Stream map is empty";
    }
    return Standard_False;
  }

  if (theStreams.Size() > 1)
  {
    if (theIsVerbose)
    {
      Message::SendWarning() << "Warning during " << theContext 
                             << ": Received " << theStreams.Size()
                             << " streams, using only the first one";
    }
  }

  // Additional validation for input streams
  try
  {
    const Standard_IStream& aStream = theStreams(1);
    if (aStream.fail() || aStream.bad())
    {
      if (theIsVerbose)
      {
        TCollection_AsciiString aFirstKey = theStreams.FindKey(1);
        TCollection_AsciiString aKeyInfo = aFirstKey.IsEmpty() ? "<empty key>" : aFirstKey;
        Message::SendFail() << "Error during " << theContext 
                            << ": Input stream '" << aKeyInfo << "' is in invalid state";
      }
      return Standard_False;
    }
  }
  catch (const std::exception&)
  {
    if (theIsVerbose)
    {
      TCollection_AsciiString aFirstKey = theStreams.FindKey(1);
      TCollection_AsciiString aKeyInfo = aFirstKey.IsEmpty() ? "<empty key>" : aFirstKey;
      Message::SendFail() << "Error during " << theContext 
                          << ": Cannot access input stream '" << aKeyInfo << "'";
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateWriteStreamMap(
  DE_Provider::WriteStreamMap& theStreams,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean theIsVerbose)
{
  if (theStreams.IsEmpty())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext 
                          << ": Stream map is empty";
    }
    return Standard_False;
  }

  if (theStreams.Size() > 1)
  {
    if (theIsVerbose)
    {
      Message::SendWarning() << "Warning during " << theContext 
                             << ": Received " << theStreams.Size()
                             << " streams, using only the first one";
    }
  }

  // Additional validation for output streams
  try
  {
    Standard_OStream& aStream = theStreams.ChangeFromIndex(1);
    if (aStream.fail() || aStream.bad())
    {
      if (theIsVerbose)
      {
        TCollection_AsciiString aFirstKey = theStreams.FindKey(1);
        TCollection_AsciiString aKeyInfo = aFirstKey.IsEmpty() ? "<empty key>" : aFirstKey;
        Message::SendFail() << "Error during " << theContext 
                            << ": Output stream '" << aKeyInfo << "' is in invalid state";
      }
      return Standard_False;
    }
  }
  catch (const std::exception&)
  {
    if (theIsVerbose)
    {
      TCollection_AsciiString aFirstKey = theStreams.FindKey(1);
      TCollection_AsciiString aKeyInfo = aFirstKey.IsEmpty() ? "<empty key>" : aFirstKey;
      Message::SendFail() << "Error during " << theContext 
                          << ": Cannot access output stream '" << aKeyInfo << "'";
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DE_ValidationUtils::ValidateDocument(
  const Handle(TDocStd_Document)& theDocument,
  const TCollection_AsciiString& theContext,
  const Standard_Boolean theIsVerbose)
{
  if (theDocument.IsNull())
  {
    if (theIsVerbose)
    {
      Message::SendFail() << "Error during " << theContext 
                          << ": Document handle is null";
    }
    return Standard_False;
  }

  return Standard_True;
}