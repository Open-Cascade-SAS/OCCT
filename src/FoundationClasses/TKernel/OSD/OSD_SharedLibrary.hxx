// Created on: 1994-08-30
// Created by: J.P. TIRAULT
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _OSD_SharedLibrary_HeaderFile
#define _OSD_SharedLibrary_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_PCharacter.hxx>
#include <OSD_LoadMode.hxx>
#include <OSD_Function.hxx>

//! Interface to dynamic library loader.
//! Provides tools to load a shared library
//! and retrieve the address of an entry point.
class OSD_SharedLibrary
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a SharedLibrary object with name NULL.
  Standard_EXPORT OSD_SharedLibrary();

  //! Creates a SharedLibrary object with name aFilename.
  Standard_EXPORT OSD_SharedLibrary(const Standard_CString aFilename);

  //! Sets a name associated to the shared object.
  Standard_EXPORT void SetName(const Standard_CString aName);

  //! Returns the name associated to the shared object.
  Standard_EXPORT Standard_CString Name() const;

  //! The DlOpen method provides an interface to the
  //! dynamic library loader to allow shared libraries
  //! to be loaded and called at runtime.  The DlOpen
  //! function attempts to load Filename, in the address
  //! space of the process, resolving symbols as appropriate.
  //! Any libraries that Filename depends upon are also loaded.
  //! If MODE is RTLD_LAZY, then the runtime loader
  //! does symbol resolution only as needed.
  //! Typically, this means that the first call to a function
  //! in the newly	loaded library will cause the resolution of
  //! the	address	of that	function to occur.
  //! If Mode is RTLD_NOW, then the runtime loader must do all
  //! symbol binding during the DlOpen call.
  //! The DlOpen method returns a	handle that is used by DlSym
  //! or DlClose.
  //! If there is an error, Standard_False is returned,
  //! Standard_True otherwise.
  //! If a NULL Filename is specified, DlOpen returns a handle
  //! for the main	executable, which allows access to dynamic
  //! symbols in the running program.
  Standard_EXPORT Standard_Boolean DlOpen(const OSD_LoadMode Mode);

  //! The dlsym function returns the address of the
  //! symbol name found in the shared library.
  //! If the symbol is not found, a NULL pointer is
  //! returned.
  Standard_EXPORT OSD_Function DlSymb(const Standard_CString Name) const;

  //! Deallocates the address space for the library
  //! corresponding to the shared object.
  //! If any user function continues to call a symbol
  //! resolved in the address space of a library
  //! that has been since been deallocated by DlClose,
  //! the results are undefined.
  Standard_EXPORT void DlClose() const;

  //! The dlerror function returns a string describing
  //! the last error that occurred from
  //! a call to DlOpen, DlClose or DlSym.
  Standard_EXPORT Standard_CString DlError() const;

  //! Frees memory allocated.
  Standard_EXPORT void Destroy();

  ~OSD_SharedLibrary() { Destroy(); }

protected:
private:
  Standard_Address    myHandle;
  Standard_PCharacter myName;
};

#endif // _OSD_SharedLibrary_HeaderFile
