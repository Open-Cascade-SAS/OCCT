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

#include <OSD_FileSystem.hxx>
#include <OSD_FileSystemSelector.hxx>
#include <OSD_LocalFileSystem.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OSD_FileSystem, Standard_Transient)

//=================================================================================================

static Handle(OSD_FileSystem) createDefaultFileSystem()
{
  Handle(OSD_FileSystemSelector) aSystem = new OSD_FileSystemSelector();
  aSystem->AddProtocol(new OSD_LocalFileSystem());
  return aSystem;
}

//=================================================================================================

OSD_FileSystem::OSD_FileSystem() {}

//=================================================================================================

OSD_FileSystem::~OSD_FileSystem() {}

//=================================================================================================

const Handle(OSD_FileSystem)& OSD_FileSystem::DefaultFileSystem()
{
  static const Handle(OSD_FileSystem) aDefSystem = createDefaultFileSystem();
  return aDefSystem;
}

//=================================================================================================

void OSD_FileSystem::AddDefaultProtocol(const Handle(OSD_FileSystem)& theFileSystem,
                                        bool                          theIsPreferred)
{
  Handle(OSD_FileSystemSelector) aFileSelector =
    Handle(OSD_FileSystemSelector)::DownCast(DefaultFileSystem());
  aFileSelector->AddProtocol(theFileSystem, theIsPreferred);
}

//=================================================================================================

void OSD_FileSystem::RemoveDefaultProtocol(const Handle(OSD_FileSystem)& theFileSystem)
{
  Handle(OSD_FileSystemSelector) aFileSelector =
    Handle(OSD_FileSystemSelector)::DownCast(DefaultFileSystem());
  aFileSelector->RemoveProtocol(theFileSystem);
}

//=================================================================================================

std::shared_ptr<std::istream> OSD_FileSystem::OpenIStream(
  const TCollection_AsciiString&       theUrl,
  const std::ios_base::openmode        theMode,
  const int64_t                        theOffset,
  const std::shared_ptr<std::istream>& theOldStream)
{
  Standard_ASSERT_RAISE(theOffset >= -1,
                        "Incorrect negative stream position during stream opening");

  std::shared_ptr<std::istream>      aNewStream;
  std::shared_ptr<OSD_IStreamBuffer> anOldStream =
    std::dynamic_pointer_cast<OSD_IStreamBuffer>(theOldStream);
  if (anOldStream.get() != NULL && theUrl.IsEqual(anOldStream->Url().c_str())
      && IsOpenIStream(anOldStream))
  {
    if (!anOldStream->good())
    {
      // Reset flags without re-opening
      anOldStream->clear();
    }
    aNewStream = anOldStream;
    if (theOffset >= 0)
    {
      aNewStream->seekg((std::streamoff)theOffset, std::ios_base::beg);
    }
  }
  if (aNewStream.get() == NULL)
  {
    std::shared_ptr<std::streambuf> aFileBuf =
      OpenStreamBuffer(theUrl, theMode | std::ios_base::in);
    if (aFileBuf.get() == NULL)
    {
      return std::shared_ptr<std::istream>();
    }

    aNewStream.reset(new OSD_IStreamBuffer(theUrl.ToCString(), aFileBuf));
    if (theOffset > 0)
    {
      aNewStream->seekg((std::streamoff)theOffset, std::ios_base::beg);
    }
  }
  return aNewStream;
}

//=================================================================================================

std::shared_ptr<std::ostream> OSD_FileSystem::OpenOStream(const TCollection_AsciiString& theUrl,
                                                          const std::ios_base::openmode  theMode)
{
  std::shared_ptr<std::ostream>   aNewStream;
  std::shared_ptr<std::streambuf> aFileBuf = OpenStreamBuffer(theUrl, theMode | std::ios_base::out);
  if (aFileBuf.get() == NULL)
  {
    return std::shared_ptr<std::ostream>();
  }

  aNewStream.reset(new OSD_OStreamBuffer(theUrl.ToCString(), aFileBuf));
  return aNewStream;
}
