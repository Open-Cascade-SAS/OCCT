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

#ifndef _OSD_FileSystemSelector_HeaderFile
#define _OSD_FileSystemSelector_HeaderFile

#include <OSD_FileSystem.hxx>

#include <NCollection_List.hxx>

//! File system implementation which tried to open stream using registered list of file systems.
class OSD_FileSystemSelector : public OSD_FileSystem
{
  DEFINE_STANDARD_RTTIEXT(OSD_FileSystemSelector, OSD_FileSystem)
public:

  //! Constructor.
  OSD_FileSystemSelector() {}

  //! Registers file system within this selector.
  //! @param theFileSystem  [in] file system to register
  //! @param theIsPreferred [in] add to the beginning of the list when TRUE, or add to the end otherwise
  Standard_EXPORT void AddProtocol (const Handle(OSD_FileSystem)& theFileSystem, bool theIsPreferred = false);

  //! Unregisters file system within this selector.
  Standard_EXPORT void RemoveProtocol (const Handle(OSD_FileSystem)& theFileSystem);

public:

  //! Returns TRUE if URL defines a supported protocol.
  Standard_EXPORT virtual bool IsSupportedPath (const TCollection_AsciiString& theUrl) const Standard_OVERRIDE;

  //! Returns TRUE if current input stream is opened for reading operations.
  Standard_EXPORT virtual Standard_Boolean IsOpenIStream (const std::shared_ptr<std::istream>& theStream) const Standard_OVERRIDE;

  //! Returns TRUE if current output stream is opened for writing operations.
  Standard_EXPORT virtual Standard_Boolean IsOpenOStream (const std::shared_ptr<std::ostream>& theStream) const Standard_OVERRIDE;

  //! Opens input stream using one of registered protocols.
  Standard_EXPORT virtual std::shared_ptr<std::istream> OpenIStream
                          (const TCollection_AsciiString& theUrl,
                           const std::ios_base::openmode theMode,
                           const int64_t theOffset = 0,
                           const std::shared_ptr<std::istream>& theOldStream = std::shared_ptr<std::istream>()) Standard_OVERRIDE;

  //! Opens output stream using one of registered protocols.
  Standard_EXPORT virtual std::shared_ptr<std::ostream> OpenOStream (const TCollection_AsciiString& theUrl,
                                                                     const std::ios_base::openmode theMode) Standard_OVERRIDE;

  //! Opens stream buffer using one of registered protocols.
  Standard_EXPORT virtual std::shared_ptr<std::streambuf> OpenStreamBuffer
                          (const TCollection_AsciiString& theUrl,
                           const std::ios_base::openmode theMode,
                           const int64_t theOffset = 0,
                           int64_t* theOutBufSize = NULL) Standard_OVERRIDE;

protected:

  NCollection_List<Handle(OSD_FileSystem)> myProtocols;

};

#endif // _OSD_FileSystemSelector_HeaderFile
