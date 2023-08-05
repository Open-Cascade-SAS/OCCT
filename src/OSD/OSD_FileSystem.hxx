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

#ifndef _OSD_FileSystem_HeaderFile
#define _OSD_FileSystem_HeaderFile

#include <OSD_StreamBuffer.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DefineAlloc.hxx>

//! Base interface for a file stream provider.
//! It is intended to be implemented for specific file protocol.
class OSD_FileSystem : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(OSD_FileSystem, Standard_Transient)
public:

  //! Returns a global file system, which a selector between registered file systems (OSD_FileSystemSelector).
  Standard_EXPORT static const Handle(OSD_FileSystem)& DefaultFileSystem();

  //! Registers file system within the global file system selector returned by OSD_FileSystem::DefaultFileSystem().
  //! Note that registering protocols is not thread-safe operation and expected to be done once at application startup.
  //! @param[in] theFileSystem  file system to register
  //! @param[in] theIsPreferred add to the beginning of the list when TRUE, or add to the end otherwise
  Standard_EXPORT static void AddDefaultProtocol (const Handle(OSD_FileSystem)& theFileSystem, bool theIsPreferred = false);

  //! Unregisters file system within the global file system selector returned by OSD_FileSystem::DefaultFileSystem().
  Standard_EXPORT static void RemoveDefaultProtocol (const Handle(OSD_FileSystem)& theFileSystem);

public:

  //! Returns TRUE if URL defines a supported protocol.
  virtual Standard_Boolean IsSupportedPath (const TCollection_AsciiString& theUrl) const = 0;

  //! Returns TRUE if current input stream is opened for reading operations.
  virtual Standard_Boolean IsOpenIStream (const std::shared_ptr<std::istream>& theStream) const = 0;

  //! Returns TRUE if current output stream is opened for writing operations.
  virtual Standard_Boolean IsOpenOStream(const std::shared_ptr<std::ostream>& theStream) const = 0;

  //! Opens stream for specified file URL for reading operations (std::istream).
  //! Default implementation create a stream from file buffer returned by OSD_FileSystem::OpenFileBuffer().
  //! @param theUrl       [in] path to open
  //! @param theMode      [in] flags describing the requested input mode for the stream (std::ios_base::in will be implicitly added)
  //! @param theOffset    [in] expected stream position from the beginning of the file (beginning of the stream by default);
  //!                          -1 would keep seek position undefined (in case of re-using theOldStream)
  //! @param theOldStream [in] a pointer to existing stream pointing to theUrl to be reused (without re-opening)
  //! @return pointer to newly created opened stream, to theOldStream if it can be reused or NULL in case of failure.
  Standard_EXPORT virtual std::shared_ptr<std::istream> OpenIStream
                          (const TCollection_AsciiString& theUrl,
                           const std::ios_base::openmode theMode,
                           const int64_t theOffset = 0,
                           const std::shared_ptr<std::istream>& theOldStream = std::shared_ptr<std::istream>());

  //! Opens stream for specified file URL for writing operations (std::ostream).
  //! Default implementation create a stream from file buffer returned by OSD_FileSystem::OpenFileBuffer().
  //! @param theUrl       [in] path to open
  //! @param theMode      [in] flags describing the requested output mode for the stream (std::ios_base::out will be implicitly added)
  //! @return pointer to newly created opened stream or NULL in case of failure.
  Standard_EXPORT virtual std::shared_ptr<std::ostream> OpenOStream (const TCollection_AsciiString& theUrl,
                                                                     const std::ios_base::openmode theMode);

  //! Opens stream buffer for specified file URL.
  //! @param theUrl        [in]  path to open
  //! @param theMode       [in]  flags describing the requested input mode for the stream
  //! @param theOffset     [in]  expected stream position from the beginning of the buffer (beginning of the stream buffer by default)
  //! @param theOutBufSize [out] total buffer size (only if buffer is opened for read)
  //! @return pointer to newly created opened stream buffer or NULL in case of failure.
  virtual std::shared_ptr<std::streambuf> OpenStreamBuffer (const TCollection_AsciiString& theUrl,
                                                            const std::ios_base::openmode theMode,
                                                            const int64_t theOffset = 0,
                                                            int64_t* theOutBufSize = NULL) = 0;

  //! Constructor.
  Standard_EXPORT OSD_FileSystem();

  //! Destructor.
  Standard_EXPORT virtual ~OSD_FileSystem();
};
#endif // _OSD_FileSystem_HeaderFile
