// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <OSD_FileSystemSelector.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OSD_FileSystemSelector, OSD_FileSystem)

//=================================================================================================

void OSD_FileSystemSelector::AddProtocol(const Handle(OSD_FileSystem)& theFileSystem,
                                         bool                          theIsPreferred)
{
  myProtocols.Remove(theFileSystem); // avoid duplicates
  if (theIsPreferred)
  {
    myProtocols.Prepend(theFileSystem);
  }
  else
  {
    myProtocols.Append(theFileSystem);
  }
}

//=================================================================================================

void OSD_FileSystemSelector::RemoveProtocol(const Handle(OSD_FileSystem)& theFileSystem)
{
  myProtocols.Remove(theFileSystem);
}

//=================================================================================================

Standard_Boolean OSD_FileSystemSelector::IsSupportedPath(
  const TCollection_AsciiString& theUrl) const
{
  for (NCollection_List<Handle(OSD_FileSystem)>::Iterator aProtIter(myProtocols); aProtIter.More();
       aProtIter.Next())
  {
    if (aProtIter.Value()->IsSupportedPath(theUrl))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

Standard_Boolean OSD_FileSystemSelector::IsOpenIStream(
  const std::shared_ptr<std::istream>& theStream) const
{
  std::shared_ptr<OSD_IStreamBuffer> aFileStream =
    std::dynamic_pointer_cast<OSD_IStreamBuffer>(theStream);
  if (aFileStream.get() == NULL)
  {
    return false;
  }
  for (NCollection_List<Handle(OSD_FileSystem)>::Iterator aProtIter(myProtocols); aProtIter.More();
       aProtIter.Next())
  {
    const Handle(OSD_FileSystem)& aFileSystem = aProtIter.Value();
    if (aFileSystem->IsSupportedPath(TCollection_AsciiString(aFileStream->Url().c_str())))
    {
      if (aFileSystem->IsOpenIStream(theStream))
      {
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

Standard_Boolean OSD_FileSystemSelector::IsOpenOStream(
  const std::shared_ptr<std::ostream>& theStream) const
{
  std::shared_ptr<OSD_OStreamBuffer> aFileStream =
    std::dynamic_pointer_cast<OSD_OStreamBuffer>(theStream);
  if (aFileStream.get() == NULL)
  {
    return false;
  }
  for (NCollection_List<Handle(OSD_FileSystem)>::Iterator aProtIter(myProtocols); aProtIter.More();
       aProtIter.Next())
  {
    const Handle(OSD_FileSystem)& aFileSystem = aProtIter.Value();
    if (aFileSystem->IsSupportedPath(TCollection_AsciiString(aFileStream->Url().c_str())))
    {
      if (aFileSystem->IsOpenOStream(theStream))
      {
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

std::shared_ptr<std::istream> OSD_FileSystemSelector::OpenIStream(
  const TCollection_AsciiString&       theUrl,
  const std::ios_base::openmode        theMode,
  const int64_t                        theOffset,
  const std::shared_ptr<std::istream>& theOldStream)
{
  for (NCollection_List<Handle(OSD_FileSystem)>::Iterator aProtIter(myProtocols); aProtIter.More();
       aProtIter.Next())
  {
    const Handle(OSD_FileSystem)& aFileSystem = aProtIter.Value();
    if (aFileSystem->IsSupportedPath(theUrl))
    {
      std::shared_ptr<std::istream> aStream =
        aFileSystem->OpenIStream(theUrl, theMode, theOffset, theOldStream);
      if (aStream.get() != NULL)
      {
        return aStream;
      }
    }
  }
  return std::shared_ptr<std::istream>();
}

//=================================================================================================

std::shared_ptr<std::ostream> OSD_FileSystemSelector::OpenOStream(
  const TCollection_AsciiString& theUrl,
  const std::ios_base::openmode  theMode)
{
  for (NCollection_List<Handle(OSD_FileSystem)>::Iterator aProtIter(myProtocols); aProtIter.More();
       aProtIter.Next())
  {
    const Handle(OSD_FileSystem)& aFileSystem = aProtIter.Value();
    if (aFileSystem->IsSupportedPath(theUrl))
    {
      std::shared_ptr<std::ostream> aStream = aFileSystem->OpenOStream(theUrl, theMode);
      if (aStream.get() != NULL)
      {
        return aStream;
      }
    }
  }
  return std::shared_ptr<std::ostream>();
}

//=================================================================================================

std::shared_ptr<std::streambuf> OSD_FileSystemSelector::OpenStreamBuffer(
  const TCollection_AsciiString& theUrl,
  const std::ios_base::openmode  theMode,
  const int64_t                  theOffset,
  int64_t*                       theOutBufSize)
{
  for (NCollection_List<Handle(OSD_FileSystem)>::Iterator aProtIter(myProtocols); aProtIter.More();
       aProtIter.Next())
  {
    const Handle(OSD_FileSystem)& aFileSystem = aProtIter.Value();
    if (aFileSystem->IsSupportedPath(theUrl))
    {
      std::shared_ptr<std::streambuf> aBuf =
        aFileSystem->OpenStreamBuffer(theUrl, theMode, theOffset, theOutBufSize);
      if (aBuf.get() != NULL)
      {
        return aBuf;
      }
    }
  }
  return std::shared_ptr<std::streambuf>();
}
