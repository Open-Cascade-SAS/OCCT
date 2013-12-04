// Created on: 2007-08-22
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PDataStd_IntPackedMap.ixx>
#include <PColStd_HArray1OfInteger.hxx>
//=======================================================================
//function : PDataStd_IntPackedMap
//purpose  : Constructor
//=======================================================================
PDataStd_IntPackedMap::PDataStd_IntPackedMap() {}

//=======================================================================
//function : Init
//purpose  : Initializes the internal container
//=======================================================================
void PDataStd_IntPackedMap::Init(const Standard_Integer theLow, 
const Standard_Integer theUp) {
  myIntValues = new PColStd_HArray1OfInteger(theLow, theUp);
}

//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap::GetValue(
				   const Standard_Integer theIndex) const
{ return myIntValues->Value(theIndex); }

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_IntPackedMap::SetValue(const Standard_Integer theIndx, 
				     const Standard_Integer theValue) 
{ 
  myIntValues->SetValue(theIndx, theValue);
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_IntPackedMap::IsEmpty() const
{ 
  if(myIntValues.IsNull()) return Standard_True;
  if(!myIntValues->Upper() && !myIntValues->Lower()) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : Returns an upper bound of the internal container
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap::Upper() const
{ 
  if(myIntValues.IsNull()) return 0;
  return myIntValues->Upper();
}

//=======================================================================
//function : Returns a lower bound of the internal container
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap::Lower() const
{ 
  if(myIntValues.IsNull()) return 0;
  return myIntValues->Lower();
}
