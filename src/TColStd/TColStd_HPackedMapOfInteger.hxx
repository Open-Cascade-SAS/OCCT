// Created on: 2006-12-05
// Created by: Sergey  KOCHETKOV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef _TColStd_HPackedMapOfInteger_HeaderFile
#define _TColStd_HPackedMapOfInteger_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_PackedMapOfInteger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>


class TColStd_HPackedMapOfInteger;
DEFINE_STANDARD_HANDLE(TColStd_HPackedMapOfInteger, Standard_Transient)

//! Extension of TColStd_PackedMapOfInteger class to be manipulated by handle.
class TColStd_HPackedMapOfInteger : public Standard_Transient
{

public:

  
  Standard_EXPORT TColStd_HPackedMapOfInteger(const Standard_Integer NbBuckets = 1);
  
  Standard_EXPORT TColStd_HPackedMapOfInteger(const TColStd_PackedMapOfInteger& theOther);
  
    const TColStd_PackedMapOfInteger& Map() const;
  
    TColStd_PackedMapOfInteger& ChangeMap();




  DEFINE_STANDARD_RTTIEXT(TColStd_HPackedMapOfInteger,Standard_Transient)

protected:




private:


  TColStd_PackedMapOfInteger myMap;


};


#include <TColStd_HPackedMapOfInteger.lxx>





#endif // _TColStd_HPackedMapOfInteger_HeaderFile
