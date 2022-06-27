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

#ifndef _TDataStd_HDataMapOfStringByte_HeaderFile
#define _TDataStd_HDataMapOfStringByte_HeaderFile

#include <Standard.hxx>

#include <TDataStd_DataMapOfStringByte.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>

//! Extension of TDataStd_DataMapOfStringByte class
//! to be manipulated by handle.
class TDataStd_HDataMapOfStringByte : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(TDataStd_HDataMapOfStringByte, Standard_Transient)
public:

  Standard_EXPORT TDataStd_HDataMapOfStringByte(const Standard_Integer NbBuckets = 1);

  Standard_EXPORT TDataStd_HDataMapOfStringByte(const TDataStd_DataMapOfStringByte& theOther);

  const TDataStd_DataMapOfStringByte& Map() const { return myMap; }

  TDataStd_DataMapOfStringByte& ChangeMap() { return myMap; }

private:

  TDataStd_DataMapOfStringByte myMap;

};

DEFINE_STANDARD_HANDLE(TDataStd_HDataMapOfStringByte, Standard_Transient)

#endif // _TDataStd_HDataMapOfStringByte_HeaderFile
