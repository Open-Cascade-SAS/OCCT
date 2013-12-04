// Created on: 2008-03-27
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <PDataStd_IntPackedMap_1.ixx>
#include <PColStd_HArray1OfInteger.hxx>

//=======================================================================
//function : PDataStd_IntPackedMap_1
//purpose  : Constructor
//=======================================================================
PDataStd_IntPackedMap_1::PDataStd_IntPackedMap_1() : myDelta(Standard_False) {}

//=======================================================================
//function : Init
//purpose  : Initializes the internal container
//=======================================================================
void PDataStd_IntPackedMap_1::Init(const Standard_Integer theLow, 
const Standard_Integer theUp) {
  myIntValues = new PColStd_HArray1OfInteger(theLow, theUp);
}

//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap_1::GetValue(
				   const Standard_Integer theIndex) const
{ return myIntValues->Value(theIndex); }

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_IntPackedMap_1::SetValue(const Standard_Integer theIndx, 
				     const Standard_Integer theValue) 
{ 
  myIntValues->SetValue(theIndx, theValue);
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_IntPackedMap_1::IsEmpty() const
{ 
  if(myIntValues.IsNull()) return Standard_True;
  if(!myIntValues->Upper() && !myIntValues->Lower()) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : Returns an upper bound of the internal container
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap_1::Upper() const
{ 
  if(myIntValues.IsNull()) return 0;
  return myIntValues->Upper();
}

//=======================================================================
//function : Returns a lower bound of the internal container
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap_1::Lower() const
{ 
  if(myIntValues.IsNull()) return 0;
  return myIntValues->Lower();
}

//=======================================================================
//function : SetDelta
//purpose  : 
//=======================================================================
void PDataStd_IntPackedMap_1::SetDelta(const Standard_Boolean delta)
{
  myDelta = delta;
}

//=======================================================================
//function : GetDelta
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_IntPackedMap_1::GetDelta() const
{
  return myDelta;
}
