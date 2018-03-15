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
#include <NCollection_UtfString.hxx>

#if defined(_WIN32) && defined(__GLIBCXX__)
  #include <ext/stdio_filebuf.h> // __gnu_cxx::stdio_filebuf
#endif

//! Function opens the file.
//! @param theName name of file encoded in UTF-16
//! @param theMode opening mode
//! @return file handle of opened file
Standard_EXPORT FILE* OSD_OpenFile (const TCollection_ExtendedString& theName,
                                   const char* theMode);

//! Function retrieves file timestamp.
//! @param theName name of file encoded in UTF-8
//! @return stat.st_ctime value
Standard_EXPORT Standard_Time OSD_FileStatCTime (const char* theName);

//! Open file descriptor for specified UTF-16 file path.
//! @param theName name of file encoded in UTF-16
//! @param theMode opening mode
//! @return file descriptor on success or -1 on error
Standard_EXPORT int OSD_OpenFileDescriptor (const TCollection_ExtendedString& theName,
                                           ::std::ios_base::openmode theMode);

//! Function opens the file buffer.
//! @param theFileBuf file buffer to open
//! @param theName name of file encoded in UTF-16
//! @param theMode opening mode
//! @return true if success, false otherwise
inline bool OSD_OpenStream (::std::filebuf& theFileBuf,
                            const TCollection_ExtendedString& theName,
                            const std::ios_base::openmode theMode)
{
#if defined(_WIN32)
  #if defined(__GLIBCXX__)
  // if file buffer is already open, open() should fail according to C++ standard
  if (theFileBuf.is_open())
    return false;
  // __gnu_cxx::stdio_filebuf is a std::filebuf providing extra constructor taking FILE* or file descriptor;
  // It does not modify virtual methods or add any fields - so we can safely use swap (or move operator) here.
  // MinGW does not provide open() methods taking wchar_t* or file descriptor - thus, creating __gnu_cxx::stdio_filebuf
  // is the only way for opening such files since _wfopen()/_wsopen_s() from C world are available.
  const int aFileDesc = OSD_OpenFileDescriptor (theName.ToWideString(), theMode);
  __gnu_cxx::stdio_filebuf<char> aGccBuf (aFileDesc, theMode);
  if (aGccBuf.is_open())
  {
    theFileBuf.swap (aGccBuf);
    return true;
  }
  return false;
  #else
  return theFileBuf.open (theName.ToWideString(), theMode) != 0;
  #endif
#else
  // conversion to UTF-8 for linux
  NCollection_Utf8String aString (theName.ToExtString());
  return theFileBuf.open (aString.ToCString(), theMode) != 0;
#endif
}

//! Function opens the file stream.
//! @param theStream stream to open
//! @param theName name of file encoded in UTF-16
//! @param theMode opening mode
template <typename T>
inline void OSD_OpenStream (T& theStream,
                            const TCollection_ExtendedString& theName,
                            const std::ios_base::openmode theMode)
{
#if defined(_WIN32)
  #if defined(__GLIBCXX__)
  // Use hackish code for opening wchar_t* file paths on MinGW,
  // which considers implementation details of std::filebuf within std::fstream/std::ifstream/std::ofstream.
  // Should be removed when MinGW will be improved to support wchar_t file paths natively within C++ streams.
  if (! OSD_OpenStream (*theStream.rdbuf(), theName, theMode))
  {
    theStream.setstate (std::ios_base::failbit);
  }
  else
  {
    theStream.clear();
  }
  #else
  theStream.open (theName.ToWideString(), theMode);
  #endif
#else
  // conversion in UTF-8 for linux
  NCollection_Utf8String aString (theName.ToExtString());
  theStream.open (aString.ToCString(), theMode);
#endif
}

//! Function opens the file stream.
//! @param theStream stream to open
//! @param theName name of file encoded in UTF-8
//! @param theMode opening mode
template <typename T>
inline void OSD_OpenStream (T& theStream,
                            const char* theName,
                            const std::ios_base::openmode theMode)
{
#if defined(_WIN32)
  // redirect to method taking UTF-16 string
  const TCollection_ExtendedString aFileNameW (theName, Standard_True);
  OSD_OpenStream (theStream, aFileNameW, theMode);
#else
  theStream.open (theName, theMode);
#endif
}

extern "C" {
#endif // __cplusplus

//! Function opens the file.
//! @param theName name of file encoded in UTF-8
//! @param theMode opening mode
//! @return file handle of opened file
Standard_EXPORT FILE* OSD_OpenFile (const char* theName, const char* theMode);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // _OSD_OpenFile_HeaderFile
