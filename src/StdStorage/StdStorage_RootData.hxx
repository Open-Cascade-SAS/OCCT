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

#ifndef _StdStorage_RootData_HeaderFile
#define _StdStorage_RootData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Storage_Error.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <StdStorage_MapOfRoots.hxx>
#include <StdStorage_HSequenceOfRoots.hxx>
class Standard_NoSuchObject;
class Storage_Schema;
class Storage_BaseDriver;
class StdStorage_Root;
class TCollection_AsciiString;
class StdObjMgt_Persistent;

class StdStorage_RootData;
DEFINE_STANDARD_HANDLE(StdStorage_RootData, Standard_Transient)

//! Storage root data section contains root persistent objects
class StdStorage_RootData
  : public Standard_Transient
{
  friend class StdStorage_Data;

public:

  DEFINE_STANDARD_RTTIEXT(StdStorage_RootData, Standard_Transient)

  //! Reads the root data section from the container defined by theDriver. 
  //! Returns Standard_True in case of success. Otherwise, one need to get 
  //! an error code and description using ErrorStatus and ErrorStatusExtension
  //! functions correspondingly.
  Standard_EXPORT Standard_Boolean Read(const Handle(Storage_BaseDriver)& theDriver);

  //! Writes the root data section to the container defined by theDriver. 
  //! Returns Standard_True in case of success. Otherwise, one need to get 
  //! an error code and description using ErrorStatus and ErrorStatusExtension
  //! functions correspondingly.
  Standard_EXPORT Standard_Boolean Write(const Handle(Storage_BaseDriver)& theDriver);

  //! Returns the number of roots.
  Standard_EXPORT Standard_Integer NumberOfRoots() const;

  //! Add a root to <me>. If a root with same name is present, it
  //! will be replaced by <aRoot>.
  Standard_EXPORT void AddRoot(const Handle(StdStorage_Root)& aRoot);

  //! Returns a sequence of all roots
  Standard_EXPORT Handle(StdStorage_HSequenceOfRoots) Roots() const;

  //! Finds a root with name <aName>.
  Standard_EXPORT Handle(StdStorage_Root) Find(const TCollection_AsciiString& aName) const;

  //! Returns Standard_True if <me> contains a root named <aName>
  Standard_EXPORT Standard_Boolean IsRoot(const TCollection_AsciiString& aName) const;

  //! Removes the root named <aName>.
  Standard_EXPORT void RemoveRoot(const TCollection_AsciiString& aName);

  //! Returns a status of the latest call to Read / Write functions
  Standard_EXPORT Storage_Error ErrorStatus() const;

  //! Returns an error message if any of the latest call to Read / Write functions
  Standard_EXPORT TCollection_AsciiString ErrorStatusExtension() const;

  //! Clears error status
  Standard_EXPORT void ClearErrorStatus();

  //! Removes all persistent root objects
  Standard_EXPORT void Clear();

private:

  Standard_EXPORT StdStorage_RootData();

  Standard_EXPORT void SetErrorStatus(const Storage_Error anError);

  Standard_EXPORT void SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt);

  StdStorage_MapOfRoots myObjects;
  Storage_Error myErrorStatus;
  TCollection_AsciiString myErrorStatusExt;

};

#endif // _StdStorage_RootData_HeaderFile
