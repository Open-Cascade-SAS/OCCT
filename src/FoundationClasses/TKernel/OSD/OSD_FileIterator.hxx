// Created on: 1992-03-18
// Created by: Stephan GARNAUD
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _OSD_FileIterator_HeaderFile
#define _OSD_FileIterator_HeaderFile

#include <OSD_File.hxx>
#include <OSD_Error.hxx>
#include <TCollection_AsciiString.hxx>

class OSD_Path;

//! Manages a breadth-only search for files in the specified Path.
//! There is no specific order of results.
class OSD_FileIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Instantiates Object as empty Iterator;
  Standard_EXPORT OSD_FileIterator();

  //! Instantiates Object as Iterator;
  //! Wild-card "*" can be used in Mask the same way it
  //! is used by unix shell for file names
  Standard_EXPORT OSD_FileIterator(const OSD_Path& where, const TCollection_AsciiString& Mask);

  Standard_EXPORT void Destroy();

  ~OSD_FileIterator() { Destroy(); }

  //! Initializes the current File Iterator
  Standard_EXPORT void Initialize(const OSD_Path& where, const TCollection_AsciiString& Mask);

  //! Returns TRUE if there are other items using the 'Tree'
  //! method.
  Standard_EXPORT Standard_Boolean More();

  //! Sets the iterator to the next item.
  //! Returns the item value corresponding to the current
  //! position of the iterator.
  Standard_EXPORT void Next();

  //! Returns the next file found .
  Standard_EXPORT OSD_File Values();

  //! Returns TRUE if an error occurs
  Standard_EXPORT Standard_Boolean Failed() const;

  //! Resets error counter to zero
  Standard_EXPORT void Reset();

  //! Raises OSD_Error
  Standard_EXPORT void Perror();

  //! Returns error number if 'Failed' is TRUE.
  Standard_EXPORT Standard_Integer Error() const;

private:
  OSD_File                TheIterator;
  Standard_Boolean        myFlag;
  TCollection_AsciiString myMask;
  TCollection_AsciiString myPlace;
  OSD_Error               myError;

  // platform-specific fields
#ifdef _WIN32
  Standard_Address myHandle;
  Standard_Address myData;
  Standard_Boolean myFirstCall;
#else
  Standard_Address myDescr;
  Standard_Address myEntry;
  Standard_Integer myInit;
#endif
};

#endif // _OSD_FileIterator_HeaderFile
