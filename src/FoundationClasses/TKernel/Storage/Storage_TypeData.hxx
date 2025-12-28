// Created on: 1997-02-06
// Created by: Kernel
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Storage_TypeData_HeaderFile
#define _Storage_TypeData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Storage_Error.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
class Storage_BaseDriver;

class Storage_TypeData : public Standard_Transient
{

public:
  Standard_EXPORT Storage_TypeData();

  Standard_EXPORT bool Read(const occ::handle<Storage_BaseDriver>& theDriver);

  Standard_EXPORT int NumberOfTypes() const;

  //! add a type to the list
  Standard_EXPORT void AddType(const TCollection_AsciiString& aName,
                               const int         aTypeNum);

  //! returns the name of the type with number <aTypeNum>
  Standard_EXPORT TCollection_AsciiString Type(const int aTypeNum) const;

  //! returns the name of the type with number <aTypeNum>
  Standard_EXPORT int Type(const TCollection_AsciiString& aTypeName) const;

  Standard_EXPORT bool IsType(const TCollection_AsciiString& aName) const;

  Standard_EXPORT occ::handle<NCollection_HSequence<TCollection_AsciiString>> Types() const;

  Standard_EXPORT Storage_Error ErrorStatus() const;

  Standard_EXPORT TCollection_AsciiString ErrorStatusExtension() const;

  Standard_EXPORT void ClearErrorStatus();

  Standard_EXPORT void Clear();

  friend class Storage_Schema;

  DEFINE_STANDARD_RTTIEXT(Storage_TypeData, Standard_Transient)

private:
  Standard_EXPORT void SetErrorStatus(const Storage_Error anError);

  Standard_EXPORT void SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt);

  NCollection_IndexedDataMap<TCollection_AsciiString, int>           myPt;
  Storage_Error           myErrorStatus;
  TCollection_AsciiString myErrorStatusExt;
};

#endif // _Storage_TypeData_HeaderFile
