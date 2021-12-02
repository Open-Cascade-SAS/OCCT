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

#ifndef _OSD_CachedFileSystem_HeaderFile
#define _OSD_CachedFileSystem_HeaderFile

#include <OSD_FileSystem.hxx>

//! File system keeping last stream created by linked file system (OSD_FileSystem::DefaultFileSystem() by default) to be reused for opening a stream with the same URL.
//! Note that as file is kept in opened state, application will need destroying this object to ensure all files being closed.
//! This interface could be handy in context of reading numerous objects pointing to the same file (at different offset).
//! Make sure to create a dedicated OSD_CachedFileSystem for each working thread to avoid data races.
class OSD_CachedFileSystem : public OSD_FileSystem
{
  DEFINE_STANDARD_RTTIEXT(OSD_CachedFileSystem, OSD_FileSystem)
public:

  //! Constructor.
  Standard_EXPORT OSD_CachedFileSystem (const Handle(OSD_FileSystem)& theLinkedFileSystem = Handle(OSD_FileSystem)());

  //! Return linked file system; initialized with OSD_FileSystem::DefaultFileSystem() by default.
  const Handle(OSD_FileSystem)& LinkedFileSystem() const { return myLinkedFS; }

  //! Sets linked file system.
  void SetLinkedFileSystem (const Handle(OSD_FileSystem)& theLinkedFileSystem) { myLinkedFS = theLinkedFileSystem; }

  //! Returns TRUE if URL defines a supported protocol.
  Standard_EXPORT virtual Standard_Boolean IsSupportedPath (const TCollection_AsciiString& theUrl) const Standard_OVERRIDE;

  //! Returns TRUE if current input stream is opened for reading operations.
  Standard_EXPORT virtual Standard_Boolean IsOpenIStream (const std::shared_ptr<std::istream>& theStream) const Standard_OVERRIDE;

  //! Returns TRUE if current output stream is opened for writing operations.
  Standard_EXPORT virtual Standard_Boolean IsOpenOStream (const std::shared_ptr<std::ostream>& theStream) const Standard_OVERRIDE;

  //! Opens stream for specified file URL for reading operations or returns previously created stream pointing to the same URL.
  Standard_EXPORT virtual std::shared_ptr<std::istream> OpenIStream
                          (const TCollection_AsciiString& theUrl,
                           const std::ios_base::openmode theParams,
                           const int64_t theOffset,
                           const std::shared_ptr<std::istream>& theOldStream) Standard_OVERRIDE;

  //! Opens stream for specified file URL for writing operations (std::ostream) by calling parent's method.
  Standard_EXPORT virtual std::shared_ptr<std::ostream> OpenOStream (const TCollection_AsciiString& theUrl,
                                                                     const std::ios_base::openmode theMode) Standard_OVERRIDE;

  //! Opens stream buffer for specified file URL.
  Standard_EXPORT virtual std::shared_ptr<std::streambuf> OpenStreamBuffer
                          (const TCollection_AsciiString& theUrl,
                           const std::ios_base::openmode theMode,
                           const int64_t theOffset = 0,
                           int64_t* theOutBufSize = NULL) Standard_OVERRIDE;

protected:

  // Auxiliary structure to save shared stream with path to it.
  struct OSD_CachedStream
  {
    TCollection_AsciiString         Url;
    std::shared_ptr<std::istream>   Stream;
    std::shared_ptr<std::streambuf> StreamBuf;

    void Reset()
    {
      Stream.reset();
      StreamBuf.reset();
    }
  };

protected:

  OSD_CachedStream       myStream;   //!< active cached stream
  Handle(OSD_FileSystem) myLinkedFS; //!< linked file system to open files

};

#endif // _OSD_CachedFileSystem_HeaderFile
