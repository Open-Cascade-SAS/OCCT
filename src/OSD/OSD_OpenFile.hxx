// Copyright (c) 2014 OPEN CASCADE SAS
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

//! Auxiulary file to provide Unicode compatibility for file open functionality
//! Names of files are encoded as UTF-16 strings

#ifndef _OSD_OpenFile_HeaderFile
#define _OSD_OpenFile_HeaderFile

#include <Standard_Macro.hxx>

#if defined(__cplusplus)

#include <fstream>
#include <TCollection_ExtendedString.hxx>

//! Function opens the file stream.
//! @param theStream stream to open
//! @param theName name of file encoded in UTF-8
//! @param theMode opening mode
__Standard_API void OSD_OpenStream (std::ofstream& theStream,
                                    const char* theName,
                                    const std::ios_base::openmode theMode);

//! Function opens the file stream.
//! @param theStream stream to open
//! @param theName name of file encoded in UTF-16
//! @param theMode opening mode
__Standard_API void OSD_OpenStream (std::ofstream& theStream,
                                    const TCollection_ExtendedString& theName,
                                    const std::ios_base::openmode theMode);

//! Function opens the file buffer.
//! @param theBuff file buffer to open
//! @param theName name of file encoded in UTF-8
//! @param theMode opening mode
__Standard_API void OSD_OpenFileBuf (std::filebuf& theBuff,
                                     const char* theName,
                                     const std::ios_base::openmode theMode);

//! Function opens the file buffer.
//! @param theBuff file buffer to open
//! @param theName name of file encoded in UTF-16
//! @param theMode opening mode
__Standard_API void OSD_OpenFileBuf (std::filebuf& theBuff,
                                     const TCollection_ExtendedString& theName,
                                     const std::ios_base::openmode theMode);


//! Function opens the file.
//! @param theName name of file encoded in UTF-16
//! @param theMode opening mode
//! @return file handle of opened file
__Standard_API FILE* OSD_OpenFile (const TCollection_ExtendedString& theName,
                                   const char* theMode);

extern "C" {
#endif // __cplusplus

//! Function opens the file.
//! @param theName name of file encoded in UTF-8
//! @param theMode opening mode
//! @return file handle of opened file
__Standard_API FILE* OSD_OpenFile (const char* theName, const char* theMode);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // _OSD_OpenFile_HeaderFile
