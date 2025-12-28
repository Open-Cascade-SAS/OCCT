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

#ifndef _StdStorage_TypeData_HeaderFile
#define _StdStorage_TypeData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Storage_Error.hxx>
#include <Standard_Transient.hxx>
#include <StdObjMgt_MapOfInstantiators.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
class Storage_BaseDriver;

//! Storage type data section keeps association between
//! persistent textual types and their numbers
class StdStorage_TypeData : public Standard_Transient
{
  friend class StdStorage_Data;

public:
  DEFINE_STANDARD_RTTIEXT(StdStorage_TypeData, Standard_Transient)

  //! Reads the type data section from the container defined by theDriver.
  //! Returns true in case of success. Otherwise, one need to get
  //! an error code and description using ErrorStatus and ErrorStatusExtension
  //! functions correspondingly.
  Standard_EXPORT bool Read(const occ::handle<Storage_BaseDriver>& theDriver);

  //! Writes the type data section to the container defined by theDriver.
  //! Returns true in case of success. Otherwise, one need to get
  //! an error code and description using ErrorStatus and ErrorStatusExtension
  //! functions correspondingly.
  Standard_EXPORT bool Write(const occ::handle<Storage_BaseDriver>& theDriver);

  //! Returns the number of registered types
  Standard_EXPORT int NumberOfTypes() const;

  //! Add a type to the list in case of reading data
  Standard_EXPORT void AddType(const TCollection_AsciiString& aTypeName, const int aTypeNum);

  //! Add a type of the persistent object in case of writing data
  Standard_EXPORT int AddType(const occ::handle<StdObjMgt_Persistent>& aPObj);

  //! Returns the name of the type with number <aTypeNum>
  Standard_EXPORT TCollection_AsciiString Type(const int aTypeNum) const;

  //! Returns the name of the type with number <aTypeNum>
  Standard_EXPORT int Type(const TCollection_AsciiString& aTypeName) const;

  //! Returns a persistent object instantiator of <aTypeName>
  Standard_EXPORT StdObjMgt_Persistent::Instantiator Instantiator(const int aTypeNum) const;

  //! Checks if <aName> is a registered type
  Standard_EXPORT bool IsType(const TCollection_AsciiString& aName) const;

  //! Returns a sequence of all registered types
  Standard_EXPORT occ::handle<NCollection_HSequence<TCollection_AsciiString>> Types() const;

  //! Returns a status of the latest call to Read / Write functions
  Standard_EXPORT Storage_Error ErrorStatus() const;

  //! Returns an error message if any of the latest call to Read / Write functions
  Standard_EXPORT TCollection_AsciiString ErrorStatusExtension() const;

  //! Clears error status
  Standard_EXPORT void ClearErrorStatus();

  //! Unregisters all types
  Standard_EXPORT void Clear();

private:
  Standard_EXPORT StdStorage_TypeData();

  Standard_EXPORT void SetErrorStatus(const Storage_Error anError);

  Standard_EXPORT void SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt);

  int                                                      myTypeId;
  StdObjMgt_MapOfInstantiators                             myMapOfPInst;
  NCollection_IndexedDataMap<TCollection_AsciiString, int> myPt;
  Storage_Error                                            myErrorStatus;
  TCollection_AsciiString                                  myErrorStatusExt;
};

#endif // _StdStorage_TypeData_HeaderFile
