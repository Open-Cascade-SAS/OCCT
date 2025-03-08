// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _StdStorage_Data_HeaderFile
#define _StdStorage_Data_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Handle.hxx>

class StdStorage_HeaderData;
class StdStorage_TypeData;
class StdStorage_RootData;

//! A picture memorizing the stored in a
//! container (for example, in a file).
//! A StdStorage_Data object represents either:
//! -   persistent data to be written into a container,
//! or
//! -   persistent data which are read from a container.
//! A StdStorage_Data object is used in both the
//! storage and retrieval operations:
//! -   Storage mechanism: create an empty
//! StdStorage_Data object, then add successively
//! persistent objects (roots) to be stored using
//! the StdStorage_RootData's function AddRoot. When the set of
//! data is complete, write it to a container using the
//! function Write in your StdStorage algorithm.
//! -   Retrieval mechanism: a StdStorage_Data
//! object is returned by the Read function from
//! your StdStorage algorithm. Use the StdStorage_RootData's
//! functions NumberOfRoots and Roots to find the roots which
//! were stored in the read container.
//! The roots of a StdStorage_Data object may share
//! references on objects. The shared internal
//! references of a StdStorage_Data object are
//! maintained by the storage/retrieval mechanism.
//! Note: References shared by objects which are
//! contained in two distinct StdStorage_Data objects
//! are not maintained by the storage/retrieval
//! mechanism: external references are not
//! supported by Storage_Schema algorithm
class StdStorage_Data : public Standard_Transient
{

public:
  //! Creates an empty set of data.
  //! You explicitly create a StdStorage_Data object
  //! when preparing the set of objects to be stored
  //! together in a container (for example, in a file).
  //! Then use the function StdStorage_RootData's AddRoot
  //! to add persistent objects to the set of data.
  //! A StdStorage_Data object is also returned by the
  //! Read function of a StdStorage algorithm. Use the
  //! StdStorage_RootData's functions NumberOfRoots and
  //! Roots to find the roots which were stored in the
  //! read container.
  Standard_EXPORT StdStorage_Data();

  //! Makes the container empty
  Standard_EXPORT void Clear();

  //! Returns the header data section
  Handle(StdStorage_HeaderData) HeaderData() { return myHeaderData; }

  //! Returns the type data section
  Handle(StdStorage_TypeData) TypeData() { return myTypeData; }

  //! Returns the root data section
  Handle(StdStorage_RootData) RootData() { return myRootData; }

private:
  Handle(StdStorage_HeaderData) myHeaderData;
  Handle(StdStorage_TypeData)   myTypeData;
  Handle(StdStorage_RootData)   myRootData;
};

#endif // _StdStorage_Data_HeaderFile
