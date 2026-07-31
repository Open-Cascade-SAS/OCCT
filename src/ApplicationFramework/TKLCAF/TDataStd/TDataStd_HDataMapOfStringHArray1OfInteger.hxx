// Created on: 2007-08-17
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_HDataMapOfStringHArray1OfInteger_HeaderFile
#define _TDataStd_HDataMapOfStringHArray1OfInteger_HeaderFile

#include <Standard.hxx>

#include <TCollection_ExtendedString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Transient.hxx>

//! Extension of NCollection_DataMap<TCollection_ExtendedString,
//! occ::handle<NCollection_HArray1<int>>> class to be manipulated by handle.
class TDataStd_HDataMapOfStringHArray1OfInteger : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(TDataStd_HDataMapOfStringHArray1OfInteger, Standard_Transient)
public:
  Standard_EXPORT TDataStd_HDataMapOfStringHArray1OfInteger(const int NbBuckets = 1);

  Standard_EXPORT TDataStd_HDataMapOfStringHArray1OfInteger(
    const NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>&
      theOther);

  const NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>&
    Map() const
  {
    return myMap;
  }

  NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>&
    ChangeMap()
  {
    return myMap;
  }

private:
  NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>> myMap;
};

#endif // _TDataStd_HDataMapOfStringHArray1OfInteger_HeaderFile
