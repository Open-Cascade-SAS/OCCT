// Created on: 1999-06-15
// Created by: Sergey RUIN
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <PDataStd_RealArray.ixx>

//=======================================================================
//function : PDataStd_RealArray
//purpose  : 
//=======================================================================

PDataStd_RealArray::PDataStd_RealArray () { }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void PDataStd_RealArray::Init(const Standard_Integer lower,
			    const Standard_Integer upper)
{
  myValue = new PColStd_HArray1OfReal(lower, upper);
}


//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void PDataStd_RealArray::SetValue(const Standard_Integer index, const Standard_Real value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real PDataStd_RealArray::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_RealArray::Lower (void) const 
{ return myValue->Lower(); }


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_RealArray::Upper (void) const 
{ return myValue->Upper(); }
