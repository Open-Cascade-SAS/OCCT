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

#include <OSD_LocalFileSystem.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OSD_LocalFileSystem, OSD_FileSystem)

//=======================================================================
// function : IsSupportedPath
// purpose :
//=======================================================================
Standard_Boolean OSD_LocalFileSystem::IsSupportedPath (const TCollection_AsciiString& theUrl) const
{
  return !OSD_Path::IsRemoteProtocolPath (theUrl.ToCString());
}

//=======================================================================
// function : IsOpenIStream
// purpose :
//=======================================================================
Standard_Boolean OSD_LocalFileSystem::IsOpenIStream (const std::shared_ptr<std::istream>& theStream) const
{
  std::shared_ptr<OSD_IStreamBuffer> aFileStream = std::dynamic_pointer_cast<OSD_IStreamBuffer> (theStream);
  if (aFileStream.get() == NULL)
  {
    return false;
  }
  const std::filebuf* aFileBuf = dynamic_cast<const std::filebuf*> (aFileStream->rdbuf());
  return (aFileBuf != NULL) ? aFileBuf->is_open() : false;
}

//=======================================================================
// function : IsOpenOStream
// purpose :
//=======================================================================
Standard_Boolean OSD_LocalFileSystem::IsOpenOStream (const std::shared_ptr<std::ostream>& theStream) const
{
  std::shared_ptr<OSD_OStreamBuffer> aFileStream = std::dynamic_pointer_cast<OSD_OStreamBuffer> (theStream);
  if (aFileStream.get() == NULL)
  {
    return false;
  }
  const std::filebuf* aFileBuf = dynamic_cast<const std::filebuf*> (aFileStream->rdbuf());
  return (aFileBuf != NULL) ? aFileBuf->is_open() : false;
}

//=======================================================================
// function : OpenStreamBuffer
// purpose :
//=======================================================================
std::shared_ptr<std::streambuf> OSD_LocalFileSystem::OpenStreamBuffer (const TCollection_AsciiString& theUrl,
                                                                       const std::ios_base::openmode theMode,
                                                                       const int64_t theOffset,
                                                                       int64_t* theOutBufSize)
{
  Standard_ASSERT_RAISE (theOffset >= 0, "Incorrect negative stream position during stream buffer opening");
  std::shared_ptr<std::filebuf> aNewBuf(new std::filebuf());
  if (!OSD_OpenStream (*aNewBuf, TCollection_ExtendedString(theUrl), theMode))
  {
    return std::shared_ptr<std::streambuf>();
  }
  // if buffer is opened for read, find the file size
  if (theOutBufSize && ((theMode & std::ios::in) != 0))
  {
    *theOutBufSize = (int64_t )aNewBuf->pubseekoff (0, std::ios_base::end, std::ios_base::in);
    if (aNewBuf->pubseekoff ((std::streamoff )theOffset, std::ios_base::beg, std::ios_base::in) < 0)
    {
      *theOutBufSize = 0;
      return std::shared_ptr<std::streambuf>();
    }
  }
  else if (theOffset > 0 && aNewBuf->pubseekoff ((std::streamoff )theOffset, std::ios_base::beg,
           (theMode & std::ios::in) != 0 ? std::ios_base::in : std::ios_base::out) < 0)
  {
    return std::shared_ptr<std::streambuf>();
  }
  return aNewBuf;
}
