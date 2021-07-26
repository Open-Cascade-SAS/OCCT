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

#ifndef _OSD_LocalFileSystem_HeaderFile
#define _OSD_LocalFileSystem_HeaderFile

#include <OSD_FileSystem.hxx>

//! A file system opening local files (or files from mount systems).
class OSD_LocalFileSystem : public OSD_FileSystem
{
  DEFINE_STANDARD_RTTIEXT(OSD_LocalFileSystem, OSD_FileSystem)
public:

  //! Constructor.
  OSD_LocalFileSystem() {}

  //! Returns TRUE if URL defines a supported protocol.
  Standard_EXPORT virtual Standard_Boolean IsSupportedPath (const TCollection_AsciiString& theUrl) const Standard_OVERRIDE;

  //! Returns TRUE if current input stream is opened for reading operations.
  Standard_EXPORT virtual Standard_Boolean IsOpenIStream (const opencascade::std::shared_ptr<std::istream>& theStream) const Standard_OVERRIDE;

  //! Returns TRUE if current output stream is opened for writing operations.
  Standard_EXPORT virtual Standard_Boolean IsOpenOStream (const opencascade::std::shared_ptr<std::ostream>& theStream) const Standard_OVERRIDE;

  //! Opens stream buffer for specified file URL.
  Standard_EXPORT virtual opencascade::std::shared_ptr<std::streambuf> OpenStreamBuffer
                          (const TCollection_AsciiString& theUrl,
                           const std::ios_base::openmode theMode,
                           const int64_t theOffset = 0,
                           int64_t* theOutBufSize = NULL) Standard_OVERRIDE;
};
#endif // _OSD_LocalFileSystem_HeaderFile
