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
#include <Standard_Type.hxx>

#include <TDataStd_DataMapOfStringHArray1OfInteger.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Integer.hxx>


class TDataStd_HDataMapOfStringHArray1OfInteger;
DEFINE_STANDARD_HANDLE(TDataStd_HDataMapOfStringHArray1OfInteger, MMgt_TShared)

//! Extension of TDataStd_DataMapOfStringHArray1OfInteger class
//! to be manipulated by handle.
class TDataStd_HDataMapOfStringHArray1OfInteger : public MMgt_TShared
{

public:

  
  Standard_EXPORT TDataStd_HDataMapOfStringHArray1OfInteger(const Standard_Integer NbBuckets = 1);
  
  Standard_EXPORT TDataStd_HDataMapOfStringHArray1OfInteger(const TDataStd_DataMapOfStringHArray1OfInteger& theOther);
  
    const TDataStd_DataMapOfStringHArray1OfInteger& Map() const;
  
    TDataStd_DataMapOfStringHArray1OfInteger& ChangeMap();




  DEFINE_STANDARD_RTTI(TDataStd_HDataMapOfStringHArray1OfInteger,MMgt_TShared)

protected:




private:


  TDataStd_DataMapOfStringHArray1OfInteger myMap;


};


#include <TDataStd_HDataMapOfStringHArray1OfInteger.lxx>





#endif // _TDataStd_HDataMapOfStringHArray1OfInteger_HeaderFile
