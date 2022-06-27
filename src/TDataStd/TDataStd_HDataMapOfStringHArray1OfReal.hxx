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

#ifndef _TDataStd_HDataMapOfStringHArray1OfReal_HeaderFile
#define _TDataStd_HDataMapOfStringHArray1OfReal_HeaderFile

#include <Standard.hxx>

#include <TDataStd_DataMapOfStringHArray1OfReal.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>

//! Extension of TDataStd_DataMapOfStringHArray1OfReal class
//! to be manipulated by handle.
class TDataStd_HDataMapOfStringHArray1OfReal : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(TDataStd_HDataMapOfStringHArray1OfReal, Standard_Transient)
public:

  Standard_EXPORT TDataStd_HDataMapOfStringHArray1OfReal(const Standard_Integer NbBuckets = 1);

  Standard_EXPORT TDataStd_HDataMapOfStringHArray1OfReal(const TDataStd_DataMapOfStringHArray1OfReal& theOther);

  const TDataStd_DataMapOfStringHArray1OfReal& Map() const { return myMap; }

  TDataStd_DataMapOfStringHArray1OfReal& ChangeMap() { return myMap; }

private:

  TDataStd_DataMapOfStringHArray1OfReal myMap;

};

DEFINE_STANDARD_HANDLE(TDataStd_HDataMapOfStringHArray1OfReal, Standard_Transient)

#endif // _TDataStd_HDataMapOfStringHArray1OfReal_HeaderFile
