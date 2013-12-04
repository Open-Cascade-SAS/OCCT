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

#include <PDataStd_ExtStringList.ixx>

//=======================================================================
//function : PDataStd_ExtStringList
//purpose  : 
//=======================================================================
PDataStd_ExtStringList::PDataStd_ExtStringList() 
{ 

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void PDataStd_ExtStringList::Init(const Standard_Integer lower,
				  const Standard_Integer upper)
{
  if (upper >= lower)
    myValue = new PColStd_HArray1OfExtendedString(lower, upper);
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_ExtStringList::SetValue(const Standard_Integer index, const Handle(PCollection_HExtendedString)& value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Handle(PCollection_HExtendedString) PDataStd_ExtStringList::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringList::Lower (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Lower(); 
  return 0;
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringList::Upper (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Upper(); 
  return -1;
}
