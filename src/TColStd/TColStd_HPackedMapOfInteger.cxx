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


#include <Standard_Type.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TColStd_HPackedMapOfInteger,Standard_Transient)

//! Constructor of empty map
TColStd_HPackedMapOfInteger::TColStd_HPackedMapOfInteger (const Standard_Integer NbBuckets)
{
  myMap.ReSize(NbBuckets);
}

//! Constructor from already existing map; performs copying
TColStd_HPackedMapOfInteger::TColStd_HPackedMapOfInteger (const TColStd_PackedMapOfInteger &theOther)
{ 
  myMap.Assign ( theOther ); 
}


