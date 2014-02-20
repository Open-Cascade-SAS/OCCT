// Created on: 1993-02-03
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Dynamic_InstanceParameter.ixx>

//=======================================================================
//function : Dynamic_InstanceParameter
//purpose  : 
//=======================================================================

Dynamic_InstanceParameter::Dynamic_InstanceParameter
  (const Standard_CString aparameter) : Dynamic_Parameter(aparameter)
{}

//=======================================================================
//function : Dynamic_InstanceParameter
//purpose  : 
//=======================================================================

Dynamic_InstanceParameter::Dynamic_InstanceParameter
  (const Standard_CString aparameter,
   const Handle(Dynamic_DynamicInstance)& avalue)
     : Dynamic_Parameter(aparameter)
{
  thevalue = avalue;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(Dynamic_DynamicInstance) Dynamic_InstanceParameter::Value() const
{
  return thevalue;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Dynamic_InstanceParameter::Value
  (const Handle(Dynamic_DynamicInstance)& avalue)
{
  thevalue = avalue;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_InstanceParameter::Dump(Standard_OStream& astream) const
{
  Dynamic_Parameter::Dump(astream);
}
