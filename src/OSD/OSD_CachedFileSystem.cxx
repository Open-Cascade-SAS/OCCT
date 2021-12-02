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

#include <OSD_CachedFileSystem.hxx>
#include <OSD_OpenFile.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OSD_CachedFileSystem, OSD_FileSystem)

//=======================================================================
// function : OSD_CachedFileSystem
// purpose :
//=======================================================================
OSD_CachedFileSystem::OSD_CachedFileSystem (const Handle(OSD_FileSystem)& theLinkedFileSystem)
: myLinkedFS (!theLinkedFileSystem.IsNull() ? theLinkedFileSystem : OSD_FileSystem::DefaultFileSystem())
{
  //
}

//=======================================================================
// function : IsSupportedPath
// purpose :
//=======================================================================
Standard_Boolean OSD_CachedFileSystem::IsSupportedPath (const TCollection_AsciiString& theUrl) const
{
  return myLinkedFS->IsSupportedPath (theUrl);
}

//=======================================================================
// function : IsOpenIStream
// purpose :
//=======================================================================
Standard_Boolean OSD_CachedFileSystem::IsOpenIStream (const std::shared_ptr<std::istream>& theStream) const
{
  return myLinkedFS->IsOpenIStream (theStream);
}

//=======================================================================
// function : IsOpenOStream
// purpose :
//=======================================================================
Standard_Boolean OSD_CachedFileSystem::IsOpenOStream (const std::shared_ptr<std::ostream>& theStream) const
{
  return myLinkedFS->IsOpenOStream (theStream);
}

//=======================================================================
// function : OpenIStream
// purpose :
//=======================================================================
std::shared_ptr<std::istream> OSD_CachedFileSystem::OpenIStream (const TCollection_AsciiString& theUrl,
                                                                 const std::ios_base::openmode theParams,
                                                                 const int64_t theOffset,
                                                                 const std::shared_ptr<std::istream>& /*theOldStream*/)
{
  if (myStream.Url != theUrl)
  {
    myStream.Url = theUrl;
    myStream.Reset();
  }
  myStream.Stream = myLinkedFS->OpenIStream (theUrl, theParams, theOffset, myStream.Stream);
  return myStream.Stream;
}

//=======================================================================
// function : OpenOStream
// purpose :
//=======================================================================
std::shared_ptr<std::ostream> OSD_CachedFileSystem::OpenOStream (const TCollection_AsciiString& theUrl,
                                                                 const std::ios_base::openmode theMode)
{
  return myLinkedFS->OpenOStream (theUrl, theMode);
}

//=======================================================================
// function : OpenStreamBuffer
// purpose :
//=======================================================================
std::shared_ptr<std::streambuf> OSD_CachedFileSystem::OpenStreamBuffer (const TCollection_AsciiString& theUrl,
                                                                        const std::ios_base::openmode theMode,
                                                                        const int64_t theOffset,
                                                                        int64_t* theOutBufSize)
{
  if ((theMode & std::ios::out) == std::ios::out)
  {
    return myLinkedFS->OpenStreamBuffer (theUrl, theMode, theOffset, theOutBufSize);
  }
  if (myStream.Url != theUrl)
  {
    myStream.Url = theUrl;
    myStream.Reset();
  }
  myStream.StreamBuf = myLinkedFS->OpenStreamBuffer (theUrl, theMode, theOffset, theOutBufSize);
  return myStream.StreamBuf;
}
