// Created on: 2007-05-29
// Created by: Vlad Romashko
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

#include <PDataStd_RealList.ixx>

//=======================================================================
//function : PDataStd_RealList
//purpose  : 
//=======================================================================
PDataStd_RealList::PDataStd_RealList() 
{ 

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void PDataStd_RealList::Init(const Standard_Integer lower,
			     const Standard_Integer upper)
{
  if (upper >= lower)
    myValue = new PColStd_HArray1OfReal(lower, upper);
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_RealList::SetValue(const Standard_Integer index, const Standard_Real value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Real PDataStd_RealList::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_RealList::Lower (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Lower(); 
  return 0;
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_RealList::Upper (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Upper(); 
  return -1;
}
